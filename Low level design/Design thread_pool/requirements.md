### Design a Thread Pool in C++.

#### Requirements:

- Fixed number of worker threads
- Submit tasks dynamically
- Tasks execute asynchronously
- Multiple tasks can wait in a queue
- Workers continuously pick tasks
- Graceful shutdown
- Thread-safe

---

#### Why do we need a thread pool ?

A Thread Pool is used to efficiently manage and reuse a fixed number of threads to execute multiple tasks concurrently.

`Problem: Creating a new thread for every task`
- Suppose your application receives 10,000 tasks and if we create a new thread for every task.
#### What problems can occur?
1. Thread creation overhead: Creating and destroying threads consumes time and system resources.
2. Memory consumption: Every thread requires its own stack memory and other resources.
3. Context switching: Too many threads can cause excessive context switching, reducing performance.
4. Resource exhaustion: Creating thousands of threads can exhaust system resources.
So, creating a new thread for every task is not efficient.

`Solution: Thread Pool`
- Instead of creating a new thread for every task, we create a fixed number of worker threads and reuse them.

`With a thread pool:`
- A fixed number of worker threads are created.
- Incoming requests are placed in a task queue.
- Available workers pick up requests and execute them.
- When a worker finishes, it takes another request.
---
#### High Level Design
```
                 submit(task)

                   |
                   v

          +-------------------+
          |     Task Queue    |
          +-------------------+
                    ^
                    |
            mutex + condition_variable
                    |
            --------------------------
            |      |      |      |
            v      v      v      v
        Worker1 Worker2 Worker3 Worker4
            |      |      |      |
            -------- Execute -------
```

---

#### Classes
```
ThreadPool
    |
    |---- vector<thread> workers
    |
    |---- queue<function<void()> > tasks
    |
    |---- mutex
    |
    |---- condition_variable
    |
    |---- bool stop

Only one class is generally enough.
```

---

#### Data Members
```
class ThreadPool
{
private:

    vector<thread> workers;

    queue<function<void()>> tasks;

    mutex mtx;

    condition_variable cv;

    bool stop;
};
```

---

#### Why function<void()>?

- Every submitted task may be different. 
- It is not a function.
- It is an object that can store any callable.

For example, it can store:

- A lambda
- A normal function
- A functor
- A member function (using bind)

```
Task 1

print()

Task 2

calculate()

Task 3

downloadFile()
```
We need one common datatype.

`function<void()>`

means
Store any callable that takes no parameters and returns nothing.

---

### Worker Thread Logic

Every worker continuously performs
```
while(true)
{

    Wait for task

    Pick task

    Execute task

}
```

---

### Where does it get its value?

Suppose in main() you wrote:
```
pool.submit([]()
{
    cout << "Hello\n";
});
```
`Inside submit():`

**tasks.emplace(task);**

The queue now contains:

`Queue`
```
Front
-------------------------
Lambda: cout<<"Hello"
-------------------------
```
---

### What does task(); do?

Remember,

task contains
```
[]()
{
    cout << "Hello\n";
}
```
So

`task();`

is exactly the same as writing
```
[]()
{
    cout << "Hello\n";
}();
```
or equivalently

`cout << "Hello\n";`

Therefore it prints

`Hello`

### Another Example

Suppose
```
pool.submit([]()
{
    cout << "Task 1\n";
});
```
```
pool.submit([]()
{
    cout << "Task 2\n";
});
```
Queue
```
Front

Task1 Lambda

Task2 Lambda
```
Worker executes

`task = move(tasks.front());`

Now
```
task

↓

Task1 Lambda
```
Queue becomes
```
Task2 Lambda
```
Now

`task();`

executes

`Task 1`

The worker loops again.

Now it picks
```
Task2 Lambda
```
Again

`task();`

prints

`Task 2`

---