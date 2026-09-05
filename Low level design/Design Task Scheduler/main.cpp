#include <iostream>
#include <mutex>
#include <thread>
#include <chrono>
#include <memory>
#include <queue>
#include <vector>
#include <condition_variable>
using namespace std;

class Task{
public:
    virtual void execute()=0;
    virtual ~Task() = default;
};

class PrintTask:public Task{
    string message;
public:
    PrintTask(string msg):message(msg){}
    void execute() override{
        cout<<"Executing "<<message<<endl;
    }
};

class Schedule{
public:
    chrono::steady_clock::time_point nextRun;
    chrono::seconds interval;
    bool recurring;

    Schedule(int delaySec,bool recurring = false,int intervalSec = 0){
        nextRun = chrono::steady_clock::now()+chrono::seconds(delaySec);
        this->recurring = recurring;
        interval = chrono::seconds(intervalSec);
    }
};

class ScheduledTask{
public:
    Task* task;
    Schedule schedule;

    ScheduledTask(Task *t,Schedule s):task(t),schedule(s){}
};

struct Compare{
    bool operator()(const ScheduledTask &a,const ScheduledTask &b){
        return a.schedule.nextRun>b.schedule.nextRun;
    }
};

class Scheduler{
private:
    priority_queue<ScheduledTask,vector<ScheduledTask>,Compare>pq;
    mutex mtx;
    condition_variable cv;
    bool running = false;
    thread worker;
public:
    void addTask(ScheduledTask task){
        lock_guard<mutex>lock(mtx);
        pq.push(task);
        cv.notify_one();
    }

    void start(){
        running = true;
        worker = thread(&Scheduler::run,this);
    }

    void stop(){
        running = false;
        cv.notify_all();
        if(worker.joinable()){
            worker.join();
        }
    }

private:
    void run(){
        while(running){
            unique_lock<mutex>lock(mtx);
            if(pq.empty()){
                cv.wait(lock);
                continue;
            }
            auto schedule_task = pq.top();
            auto now = chrono::steady_clock::now();
            if(now>=schedule_task.schedule.nextRun){
                pq.pop();
                lock.unlock();
                schedule_task.task->execute();
                if(schedule_task.schedule.recurring){
                    schedule_task.schedule.nextRun = now + schedule_task.schedule.interval;
                    addTask(schedule_task);
                }
            }
            else{
                cv.wait_until(lock,schedule_task.schedule.nextRun);
            }
        }
    }
};

int main(){
    Scheduler scheduler;
    scheduler.start();
    PrintTask t1("Task 1");
    PrintTask t2("Task 2");
    PrintTask t3("Recurring");
    scheduler.addTask({&t1,Schedule(2)});
    scheduler.addTask({&t2,Schedule(5)});
    scheduler.addTask({&t3,Schedule(1,true,3)});

    this_thread::sleep_for(chrono::seconds(12));
    scheduler.stop();
}

/*
In this current implementation, there are only 2 threads.
***********************************
Thread 1: Main Thread
***********************************
This is created automatically when the program starts.

It executes:

int main() {

    Scheduler scheduler;

    scheduler.start();

    scheduler.addTask(...);

    scheduler.addTask(...);

    scheduler.addTask(...);

    this_thread::sleep_for(...);

    scheduler.stop();
}

So the main thread is responsible for:

Creating the scheduler
Starting the worker thread
Adding tasks
Sleeping
Stopping the scheduler

***********************************
Thread 2: Worker Thread
***********************************
When you call

scheduler.start();

inside

void start() {

    running = true;

    worker = thread(&Scheduler::run, this);
}

this line worker = thread(&Scheduler::run, this); creates a new thread.

This new thread immediately starts executing
Scheduler::run()

So now there are two threads running simultaneously.

----
In Scheduler::run() function :

If the priority queue is empty, cv.wait(lock) releases the mutex and waits. 
After being notified, the continue statement starts the next iteration of the while loop, 
so pq.top() is not executed in that iteration. 
It is executed only when the priority queue is not empty.

continue is not executed immediately after cv.wait(lock) is called, cv.wait(lock) is a blocking call. Once cv.notify is called
this thread wakesup and runs next line that is continue.

The worker thread sees that the queue is empty, calls cv.wait(lock), releases the mutex, and goes to sleep. 
The main thread later adds a task and calls notify_one(). The worker thread wakes up, reacquires the mutex, 
executes continue, starts the next iteration, sees that the queue is no longer empty, and then executes pq.top(), 
pq.pop(), and execute().

Now, suppose execute() takes 5 seconds. During those 5 seconds...Main Thread Calls 'scheduler.addTask(newTask);'

Inside addTask():
    lock_guard<mutex> lock(mtx);
    pq.push(newTask);
    cv.notify_one();

Since the worker already called lock.unlock();
the main thread immediately acquires the mutex. Push new task to queue and notify one.

The worker is still here:
schedule_task.task->execute();

It does not stop executing.It continues until execute() finishes.
After execute() finishes. The worker reaches the end of the loop.
Then executes while loop , since it is infinte loop,
while(running)

Next iteration:
unique_lock<mutex> lock(mtx);

It locks the mutex again.Now the priority queue already contains the new task added by the main thread.So it processes that task next.
*/