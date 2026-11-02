#include <iostream>
#include <thread>
#include <vector>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>

using namespace std;

class ThreadPool{
private:
    vector<thread>workers;
    queue<function<void()>>tasks;
    mutex mtx;
    condition_variable cv;
    bool stop;

public:
    ThreadPool(int n):stop(false)
    {
        for(int i=0;i<n;i++){
            workers.emplace_back([this]()
            {
                while(true){
                    function<void()>task;
                    {
                        unique_lock<mutex>lock(mtx);
                        cv.wait(lock,[this](){
                            return stop||!tasks.empty();   
                        });
                        if(stop == true && tasks.empty())
                            return ;
                        task = move(tasks.front());
                        tasks.pop();
                    }
                    task();
                }
            });
        }
    }

    void submit(function<void()>task){
        {
            lock_guard<mutex> lock(mtx);
            tasks.emplace(task);
        }
        cv.notify_one();
    }

    ~ThreadPool(){
        {
            lock_guard<mutex> lock(mtx);
            stop = true;
        }
        cv.notify_all();
        for(auto &t:workers)
            t.join();
    }
};

int main(){
    ThreadPool pool(3);

    for(int i=1;i<=10;i++){
        pool.submit([i](){
            cout<<"Task "<<i<<" executed by "<<this_thread::get_id()<<endl;
        });
    }
    return 0;
}

/*
- A thread waits while the condition is false.
- It wakes up when the condition becomes true.So, if stop means if stop is true it will come out of wait, 
else if queue is not empty it will come out of waiting.


- ThreadPool pool(3) creates and starts the three worker threads immediately.
- They do not start when submit() is called. They are already running inside while(true), but are blocked inside cv.wait().
- submit() doesn't create or start threads—it simply adds work to the queue and wakes one sleeping worker to process it.


| Instead of           | Write        |
| -------------------- | ------------ |
| `stop == true`       | `stop`       |
| `stop == false`      | `!stop`      |
| `if (stop == true)`  | `if (stop)`  |
| `if (stop == false)` | `if (!stop)` |


+--------+---------------+----------------+-----------------------------------+-------------------------------------------------------------+
| stop   | tasks.empty() | !tasks.empty() | Predicate (stop || !tasks.empty())| Worker Action                                               |
+--------+---------------+----------------+-----------------------------------+-------------------------------------------------------------+
| false  | true          | false          | false || false = false            | Waits (no tasks, pool is running)                           |
| false  | false         | true           | false || true = true              | Wakes up and executes a task                                |
| true   | true          | false          | true || false = true              | Wakes up and exits (return)                                 |
| true   | false         | true           | true || true = true               | Wakes up, executes remaining tasks, then exits when         |
|        |               |                |                                   | the queue becomes empty                                     |
+--------+---------------+----------------+-----------------------------------+-------------------------------------------------------------+

Main thread: Constructor execution
1. Iteration 1: i = 0
   
   Main thread creates Thread 1.
   Thread 1 created
   Thread 1 starts executing its lambda and reaches cv.wait().
   
   Thread 1 goes to sleep because the queue is empty.
2. Iteration 2: i = 1
   
   Main thread continues the loop without waiting for Thread 1.
   
   It creates Thread 2.
   Thread 2 created
   Thread 2 also reaches cv.wait() and sleeps.
3. Iteration 3: i = 2
   
   Main thread creates Thread 3.
   Thread 3 created
   Thread 3 also reaches cv.wait() and sleeps.

Constructor finishes. All 3 worker threads are waiting for tasks.

---
There are 4 threads in total (assuming the usual single-threaded main() execution):
1. 1 Main thread – Executes main() and creates the ThreadPool.
2. 3 Worker threads – Created by the ThreadPool constructor to execute tasks.

Execution flow
1. Main thread: Starts executing main().
2. Object creation:
   ThreadPool pool(3);
   
   
   Constructor is called.
3. Constructor creates 3 worker threads:
   - Worker 1 starts and waits for tasks.
   - Worker 2 starts and waits for tasks.
   - Worker 3 starts and waits for tasks.
4. Constructor finishes.
5. Main thread resumes execution:
   for (int i = 1; i <= 10; i++)
   
   
   It starts submitting 10 tasks.
6. Worker threads: Whenever tasks become available, workers wake up and execute them.
7. Main thread: Once all 10 tasks are submitted, it reaches return 0.
8. Destructor: Sets stop = true, notifies all workers and joins them. Workers finish any remaining tasks before exiting.

----
function<void()>task;
{
    unique_lock<mutex>lock(mtx);
    cv.wait(lock,[this](){
        return stop||!tasks.empty();   
    });
    if(stop == true && tasks.empty())
        return ;
    task = move(tasks.front());
    tasks.pop();
}
task();


Step-by-step explanation
Step	What happens?
1	Creates a local variable task of type function<void()>.
2	Acquires the mutex to safely access shared resources.
3	Worker waits until stop is true OR the queue is not empty.
4	If shutdown has started and the queue is empty, the worker exits.
5	Moves the front task from the queue into the local task variable.
6	Removes that task from the queue.
7	Exits the inner scope, automatically releasing the mutex.
8	Executes the task using task().

Why do we use the extra { }?
This is called a scope block.

Its purpose is to release the mutex as soon as we finish accessing the shared queue.

*/