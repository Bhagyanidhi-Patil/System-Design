#include <iostream>
#include <functional>
#include <vector>
#include <mutex>
using namespace std;

class EventManager{
private:
    vector<function<void()>>callback;
    mutex mtx;
public:
    void register_callback(function<void()>cb){
        lock_guard<mutex>lock(mtx);
        callback.push_back(cb);
    }

    void eventfired(){
        vector<function<void()>>currcallbacks;
        {
            lock_guard<mutex>lock(mtx);
            currcallbacks = callback;
        }
        for(auto &cb:currcallbacks){
            cb();
        }
    }
};

int main() {

    EventManager eventManager;

    eventManager.register_callback([]() {
        cout << "Callback 1 executed" << endl;
    });

    eventManager.register_callback([]() {
        cout << "Callback 2 executed" << endl;
    });

    cout << "Event fired!" << endl;

    eventManager.eventfired();

    return 0;
}

/*
First: what is the mutex doing?

The mutex is like a lock on the callbacks vector.
lock_guard<mutex> lock(mtx);

means:
"I am accessing callbacks. Other threads, please wait."

Suppose we write this ❌
void event_fired() {

    lock_guard<mutex> lock(mtx);

    for (auto &cb : callbacks) {
        cb();
    }
}

The mutex is locked while cb() is executing.

Now imagine:
cb1() {
    eventManager.register_callback(cb2);
}

So the sequence becomes:
event_fired()
     ↓
🔒 mutex locked
     ↓
cb1()
     ↓
cb1 wants to register cb2
     ↓
register_callback()
     ↓
tries to lock mutex 🔒
     ↓
WAIT!!!

Why does it wait?
Because the same mutex is already locked by event_fired().
And event_fired() is waiting for cb1() to finish.

So:
event_fired()
    ↓
waiting for cb1()
       ↑
       |
      cb1()
       ↓
waiting for mutex
       ↑
       |
event_fired() holds mutex

Nobody can proceed.
This is a deadlock.
*/


/*
handles several concurrency problems.
---
1. Multiple threads registering callbacks

Without a mutex:

Thread 1 → push_back(cb1)
Thread 2 → push_back(cb2)

Both modify the same vector simultaneously → data race / undefined behavior.

With the mutex:

Thread 1 → 🔒 → add cb1 → 🔓
Thread 2 →      🔒 → add cb2 → 🔓

✅ Safe.

---
2. register_callback() while event_fired() is reading

Without synchronization:

Thread 1 → event_fired() → reading callbacks
Thread 2 → register_callback() → modifying callbacks

One thread reads while another modifies the vector.

This can cause data races and vector reallocation problems.

With the mutex:

🔒 copy callbacks
🔓

Then registration can safely happen.

✅ Safe.

---
3. Callback registers another callback

Suppose:

cb1() {
    register_callback(cb3);
}

If we directly iterate over the original vector:

for (auto &cb : callbacks)
    cb();

then cb1() modifies callbacks while we're iterating over it.

That can cause iterator/reference invalidation and undefined behavior.

Our snapshot solves this:

callbacks = [cb1, cb2]

       ↓ copy

snapshot = [cb1, cb2]

       ↓

execute snapshot
       ↓
cb1() → registers cb3

callbacks = [cb1, cb2, cb3]
snapshot  = [cb1, cb2]

✅ cb3 executes on the next event.

---
4. We avoid holding the mutex while executing callbacks

This is very important.

We do:

🔒 lock
   ↓
copy callbacks
   ↓
🔓 unlock
   ↓
execute callbacks

instead of:

🔒 lock
   ↓
execute callback
   ↓
callback tries to register
   ↓
tries to acquire same lock
   ↓
❌ deadlock

So the snapshot approach helps prevent this type of deadlock.

---
5. Multiple threads can register while callbacks are executing

Suppose:

Thread 1 → event_fired()
Thread 2 → register cb3
Thread 3 → register cb4

Once Thread 1 has taken the snapshot:

snapshot = [cb1, cb2]

Threads 2 and 3 can modify the original vector:

callbacks = [cb1, cb2, cb3, cb4]

while Thread 1 executes:

snapshot = [cb1, cb2]

✅ They don't interfere with each other.

---
6. Two event_fired() calls

Here we need to distinguish the semantics.

If multiple events are allowed:

Thread 1 → event_fired()
Thread 2 → event_fired()

both can take a snapshot and execute callbacks.

That's okay if each call represents a separate event.

If the event is supposed to happen only once, we need an additional flag:

bool eventFired = false;

protected by the same mutex.

Then:

Thread 1 → 🔒 → eventFired = false → set true → 🔓
Thread 2 → 🔒 → eventFired = true → return → 🔓

✅ Only one thread processes the one-time event.
*/