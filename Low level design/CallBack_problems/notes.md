# Thread safety for callback registration and event firing

## What if multiple threads call `register_callback()` simultaneously?

If multiple threads call `register_callback()` at the same time, the current code is not thread-safe because all threads are modifying the same vector.

For example:

- Thread 1 → `register_callback(cb1)`
- Thread 2 → `register_callback(cb2)`
- Thread 3 → `register_callback(cb3)`

All three may call:

```cpp
callbacks.push_back(cb);
```

at the same time. This can cause a data race and undefined behavior.

### Solution: use a mutex

```cpp
class EventManager {
private:
    vector<function<void()>> callbacks;
    mutex mtx;

public:
    void register_callback(function<void()> cb) {
        lock_guard<mutex> lock(mtx);
        callbacks.push_back(cb);
    }
};
```

Now:

- Thread 1 → 🔒 → add `cb1` → 🔓
- Thread 2 → 🔒 → add `cb2` → 🔓
- Thread 3 → 🔒 → add `cb3` → 🔓

Only one thread modifies `callbacks` at a time.

---

## What if `event_fired()` occurs while a callback is being registered?

Suppose these happen at the same time:

- Thread 1: `register_callback(cb2)`
- Thread 2: `event_fired()`

Both access the same `callbacks` vector.

We need to decide what should happen to `cb2`.

### Option 1: `cb2` should run in the current event

If `register_callback(cb2)` happens before `event_fired()` takes the callback list, then:

```text
register cb1
      ↓
register cb2  ← happens
      ↓
event_fired()
      ↓
cb1()
cb2()
```

### Option 2: `cb2` should wait for the next event

If `event_fired()` has already started processing callbacks:

```text
event_fired()
   ↓
cb1()       ← event already started
   ↓
register cb2
   ↓
cb2 waits for next event
```

You need to define this behavior in the design.

### Thread-safe approach

A good approach is to protect the vector with a mutex and take a snapshot:

```cpp
void event_fired() {
    vector<function<void()>> currentCallbacks;

    {
        lock_guard<mutex> lock(mtx);
        currentCallbacks = callbacks;
    }

    // Mutex is released here

    for (auto &cb : currentCallbacks) {
        cb();
    }
}
```

Now imagine:

```text
callbacks = [cb1]
```

`event_fired()` gets:

```text
currentCallbacks = [cb1]
```

and releases the lock.

Then another thread does:

```cpp
register_callback(cb2);
```

Now:

```text
callbacks        = [cb1, cb2]
currentCallbacks = [cb1]
```

So the current event executes only `cb1`, while `cb2` will participate in the next event.

---

## What if two threads call `event_fired()` simultaneously?

Suppose:

- Thread 1 → `event_fired()`
- Thread 2 → `event_fired()`

at exactly the same time.

If both access the same `callbacks` vector without synchronization, both threads may execute the callbacks, potentially causing unexpected behavior.

### Example

Suppose:

```text
callbacks = [cb1, cb2]
```

Two threads call:

- Thread 1: `event_fired()`
- Thread 2: `event_fired()`

You could get:

```text
Thread 1 → cb1()
Thread 2 → cb1()
Thread 1 → cb2()
Thread 2 → cb2()
```

If the requirement is "each `event_fired()` represents a separate event", this may actually be correct: both events should trigger all registered callbacks.

But if the requirement is "the event should be processed only once", then we need synchronization.

### For a one-time event

For example:

```cpp
event_fired()
```

should happen only once, even if two threads call it.

Use a mutex:

```cpp
void event_fired() {
    vector<function<void()>> currentCallbacks;

    {
        lock_guard<mutex> lock(mtx);

        if (eventFired)
            return;

        eventFired = true;

        currentCallbacks = callbacks;
    }

    for (auto &cb : currentCallbacks) {
        cb();
    }
}
```

Now:

```text
Thread 1 → 🔒 → eventFired = false
                    ↓
                 set true
                    ↓
                 copy callbacks
              → 🔓
              → execute callbacks

Thread 2 → 🔒 → eventFired = true
              → return
              → 🔓
```

So only one thread processes the event.

## Can the same callback execute twice?

It can, depending on the semantics of your event system.

### In your current design

You have:

```cpp
void event_fired() {
    for (auto &cb : callbacks) {
        cb();
    }
}
```

Suppose:

```text
callbacks = [cb1]
```

If `event_fired()` is called twice:

```text
event_fired()
→ cb1()

event_fired()
→ cb1()
```

So the same callback executes twice because it is still stored in `callbacks`.

### If two threads call `event_fired()` simultaneously

It can also happen:

```text
Thread 1 → event_fired() → cb1()
Thread 2 → event_fired() → cb1()
```

So `cb1` executes twice.

### If you want each callback to execute only once

You need to define that behavior and remove or mark the callback after execution.

For example:

```text
register cb1
event_fired()
→ cb1()

event_fired()
→ nothing
```

Or, for concurrent calls, you need synchronization so only one thread gets to execute the callback.

---

## What if the same thread registers twice?

In your current implementation, if the same callback is registered twice, it will be stored twice.

For example:

```cpp
eventManager.register_callback(cb1);
eventManager.register_callback(cb1);
```

The vector becomes:

```text
callbacks = [cb1, cb1]
```

Then:

```cpp
eventManager.eventfired();
```

will produce:

```text
cb1()
cb1()
```

So the same callback executes twice.

### Is that correct?

It depends on the requirement.

1. Duplicate registration is allowed

   Then your current implementation is fine:

   ```text
   register cb1
   register cb1

   event_fired()
   → cb1()
   → cb1()
   ```

2. Duplicate registration is NOT allowed

   Then you need to detect whether the callback is already registered.

   But there's an important C++ detail: `std::function` does not directly support equality comparison, so you cannot simply do:

   ```cpp
   if (cb == existingCallback)   // ❌
   ```

   For a real callback system, you'd usually give each registration an ID/token or use a wrapper object to identify callbacks.

---

## What if a callback registers another callback?

This is an important edge case.

Suppose:

```cpp
cb1() {
    register_callback(cb2);
}
```

and currently:

```text
callbacks = [cb1]
```

Then `event_fired()` starts.

### If you directly iterate over `callbacks`

```cpp
void event_fired() {
    for (auto &cb : callbacks) {
        cb();
    }
}
```

Execution:

```text
event_fired()
    ↓
cb1()
    ↓
register cb2
    ↓
callbacks = [cb1, cb2]
```

Now you're modifying the vector while iterating over it.

That can cause problems, especially if `push_back()` causes the vector to reallocate. It can lead to iterator/reference invalidation and undefined behavior.

### Better approach: take a snapshot

This is one reason the snapshot approach we discussed earlier is useful:

```cpp
void event_fired() {
    vector<function<void()>> currentCallbacks;

    {
        lock_guard<mutex> lock(mtx);
        currentCallbacks = callbacks;
    }

    for (auto &cb : currentCallbacks) {
        cb();
    }
}
```

Suppose:

```text
callbacks = [cb1]
```

We copy:

```text
currentCallbacks = [cb1]
```

Then execute:

```text
cb1()
 ↓
register cb2
```

Now:

```text
callbacks        = [cb1, cb2]
currentCallbacks = [cb1]
```

So `cb2` does not execute during the current event.

It will execute on the next event:

```text
event_fired()
→ cb1()
→ cb2()
```

---

## What if registration and event firing happen at the same time?

If `event_fired()` and `register_callback()` access the shared callback list without synchronization, the program can have a data race. Protect access to the list with a mutex, and take a snapshot while holding the lock.

```cpp
vector<function<void()>> callbacks;
mutex mtx;
```

Assume `event_fired()` swaps the registered callbacks into a local snapshot while holding `mtx`. It then releases the mutex before executing the callbacks.

### Case 1: `event_fired()` acquires the lock first

```text
T1: event_fired()
        ↓
    lock mtx
        ↓
    swap callbacks into a local snapshot
        ↓
    unlock mtx
        ↓
    execute the snapshot

T2: register_callback(newCallback)
        ↓
    lock mtx
        ↓
    add newCallback to callbacks
        ↓
    unlock mtx
```

The newly registered callback is not in the snapshot already being executed. It remains in `callbacks` for a future event firing.

### Case 2: `register_callback()` acquires the lock first

```text
T2: register_callback(newCallback)
        ↓
    lock mtx
        ↓
    add newCallback to callbacks
        ↓
    unlock mtx

T1: event_fired()
        ↓
    lock mtx
        ↓
    swap callbacks into a local snapshot
        ↓
    unlock mtx
        ↓
    execute the snapshot
```

In this case, `newCallback` is included in the snapshot and runs during this event firing.

### Key idea: the synchronization point

The mutex makes the ordering unambiguous. The callback is included in the event if registration completes before `event_fired()` takes its snapshot. If the snapshot is taken first, the callback is left for a future event. Callbacks should be executed after releasing the mutex so user callback code does not run while the callback list is locked.