#include <iostream>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <thread>
using namespace std;

class BlockingQueue{
private:
    queue<int>q;
    int capacity;
    mutex mtx;
    condition_variable cv_notfull;
    condition_variable cv_notempty;
public:
    BlockingQueue(int cap){
        capacity = cap;
    }

    void push(int val){
        unique_lock<mutex>lock(mtx);
        cv_notfull.wait(lock,[this](){    //[this] → capture the current object so the lambda can access its member variables/functions.
            return q.size()<capacity;
        });
        q.push(val);
        cout << "Produced: " << val << endl;
        cv_notempty.notify_one();
    }

    int pop(){
        unique_lock<mutex>lock(mtx);
        cv_notempty.wait(lock,[this](){
            return q.size()>0;
        });
        int val = q.front();
        q.pop();
        cout << "Consumed: " << val << endl;
        cv_notfull.notify_one();
        return val;
    }
};

int main(){
    BlockingQueue bq(3);
    thread t1([&](){
        for(int i=0;i<=10;i++){
            bq.push(i);
        }
    });
    thread t2([&](){
        for(int i=0;i<=10;i++){
            cout<<bq.pop();
        }
    });
    t1.join();
    t2.join();
    return 0;
}


/*

Capture	    Meaning
[this]	    Capture the current object
[&]	        Capture surrounding variables by reference
[=]	        Capture surrounding variables by value
[&bq]	    Capture only bq by reference
[bq]	    Capture bq by value

For example:    
    int x = 10;

    thread t([&]() {
        cout << x << endl;
    });

Here [&] tells the lambda:
"If I use a variable from outside this lambda, capture it by reference."
So x is accessed by reference.

*/