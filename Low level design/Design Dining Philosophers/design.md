# Dining Philosophers Design

## Overall Design

We have three main classes:

- `DiningTable`
- `Fork`
- `Philosopher`

```text
           DiningTable
          /           \
         /             \
      Forks       Philosophers
```

- `DiningTable` owns all forks and philosophers.
- Each `Philosopher` knows its left and right fork.
- Every philosopher runs in a separate thread.

## Fork Class

### Why do we need a `Fork` class?

Each fork is a shared resource.

```text
P0 ----- Fork0 ----- P1
```

Only one philosopher can hold a fork at a time.

- Every fork has a unique number.
- The fork is protected by a mutex.

Whenever a philosopher wants the fork:

```cpp
lock(mtx);
```

Only one thread can own the mutex at a time, ensuring exclusive access to the fork.

## Philosopher Class

Each philosopher has:

- `ID`
- `leftFork`
- `rightFork`

When we create philosopher 2:

```cpp
Philosopher(2, Fork2, Fork3);
```

Then:

- `id = 2`
- `leftFork` → `Fork2`
- `rightFork` → `Fork3`

> Notice: We are storing pointers. We do not copy forks. All philosophers share the same fork objects.

### think()

This simply simulates thinking.

```text
Think
↓
Sleep 1 second
↓
Return
```

### eat()

Suppose:

- `Left = Fork4`
- `Right = Fork0`

Then:

- `first = Fork0` because `0 < 4`
- `second = Fork4`

So the locking order is always:

```text
Smaller Fork
↓
Larger Fork
```

This removes circular waiting.

### Locking

```cpp
unique_lock<mutex> lock1(first->mtx);
unique_lock<mutex> lock2(second->mtx);
```

Immediately after the first lock:

- `Fork0` LOCKED

After the second lock:

- `Fork4` LOCKED

The philosopher owns both forks.

### Why `unique_lock`?

Instead of manually using:

```cpp
mtx.lock();
...
mtx.unlock();
```

we use `unique_lock<mutex>` because it follows RAII.

When the function exits, the destructor calls `unlock()` automatically.

Even if an exception occurs, the mutex is released safely.

### Eating

```cpp
cout << "starts eating";
// sleep 1 second
cout << "finished eating";
```

Then the function ends.

Locks are automatically released.

## DiningTable Class

This class manages the whole simulation and owns all resources.

### Data members

- `int n;`
  - Number of philosophers.
- `vector<unique_ptr<Fork>> forks;`
  - Stores all forks.
- `vector<Philosopher> philosophers;`
  - Stores philosopher objects.
- `vector<thread> threads;`
  - Stores all threads.
  - One thread per philosopher.

### Why `unique_ptr<Fork>`?

`std::vector` may need to relocate its elements when it grows. Since `Fork` contains a `std::mutex`, which is neither copyable nor movable, `Fork` cannot be stored directly in a `std::vector` on implementations that require movable elements.

By storing `std::unique_ptr<Fork>`, the vector only moves the smart pointers, while the `Fork` objects themselves remain at fixed memory addresses.

- ❌ `vector<Fork>` → may need to move `Fork` objects, which is not allowed because of `std::mutex`.
- ✅ `vector<unique_ptr<Fork>>` → vector only moves `unique_ptr` objects; the `Fork` objects stay in place on the heap.

### Memory layout

```text
vector
↓
unique_ptr
↓
Fork0
↓
Fork1
↓
Fork2
↓
Fork3
↓
Fork4
```

## Constructor

```cpp
DiningTable(int n) : n(n) {}
```

Suppose `n = 5`.

### Create forks

```cpp
for (int i = 0; i < n; i++) {
    forks.push_back(make_unique<Fork>(i));
}
```

This creates:

- `Fork0`
- `Fork1`
- `Fork2`
- `Fork3`
- `Fork4`

### Create philosophers

```cpp
Fork* left = forks[i].get();
```

`.get()` returns the raw pointer stored inside the `unique_ptr`.

Example:

```text
forks[2]
↓
unique_ptr
↓
Fork2

.get() gives
Fork*
```

For philosopher `i = 2`:

- `Left = Fork2`
- `Right = Fork3`

For philosopher `i = 4`:

- `Left = Fork4`
- `Right = Fork0`

because:

```cpp
(i + 1) % n
```

For `n = 5`, `(4 + 1) % 5 = 0`.

So the table forms a circle.

## startDinner()

```cpp
for (int i = 0; i < n; i++) {
    threads.emplace_back(
        &Philosopher::dine,
        &philosophers[i]
    );
}
```

This starts `n` threads.

Equivalent to:

```cpp
thread t0(&Philosopher::dine, &philosophers[0]);
thread t1(&Philosopher::dine, &philosophers[1]);
thread t2(&Philosopher::dine, &philosophers[2]);
thread t3(&Philosopher::dine, &philosophers[3]);
thread t4(&Philosopher::dine, &philosophers[4]);
```

Each thread executes:

```text
think()
↓
eat()
↓
think()
↓
eat()
↓
think()
↓
eat()
```

independently.

## join()

```cpp
for (auto &t : threads) {
    t.join();
}
```

`join()` waits for a thread to finish.

Without it:

```text
main()
↓
returns
↓
Program exits
↓
Threads terminate abruptly
```

With `join()`:

```text
Main
↓
Waits
↓
All philosophers finish
↓
Program exits
```
