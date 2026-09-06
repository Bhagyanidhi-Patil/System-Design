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
*/