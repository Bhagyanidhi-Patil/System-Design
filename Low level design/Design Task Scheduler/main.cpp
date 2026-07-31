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
    chrono::milliseconds interval;
    bool recurring;

    Schedule(int delaySec,bool recurring = false,int intervalSec = 0){
        nextRun = chrono::steady_clock::now()+chrono::seconds(delaySec);
        this->recurring = recurring;
        interval = chrono::seconds(intervalSec);
    }
};

class ScheduledTask{
public:
    shared_ptr<Task>task;
    Schedule schedule;

    ScheduledTask(shared_ptr<Task> t,Schedule s):task(t),schedule(s){}
};

struct Compare{
    bool operator()(ScheduledTask &a,ScheduledTask &b){
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

    scheduler.addTask({make_shared<PrintTask>("Task 1"),Schedule(2)});
    scheduler.addTask({make_shared<PrintTask>("Task 2"),Schedule(5)});
    scheduler.addTask({make_shared<PrintTask>("Recurring"),Schedule(1,true,3)});

    this_thread::sleep_for(chrono::seconds(12));
    scheduler.stop();
}