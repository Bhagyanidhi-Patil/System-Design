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
                            return stop||!tasks.empty();   //is stop true or is queue not empty?
                        });
                        if(stop && tasks.empty())
                            return ;
                        task = move(tasks.front());
                        tasks.pop();
                    }
                    task();
                }
            });
        }
    }

    template<typename F>
    void submit(F task){
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

