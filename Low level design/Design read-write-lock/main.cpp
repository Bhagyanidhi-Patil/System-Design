#include<iostream>
#include<mutex>
#include<condition_variable>
#include<thread>
#include<chrono>
#include<vector>
using namespace std;

class ReadWriteLock{
private:
    mutex mtx;
    condition_variable cv;
    int activeReaders;
    bool writerActive;
    int waitingWriters;

public:
    ReadWriteLock(){
        activeReaders = 0;
        writerActive = false;
        waitingWriters = 0;
    }

    void lockRead(){
        unique_lock<mutex> lock(mtx);
        cv.wait(lock,[&](){                                  //"If a writer is active (writerActive == true), sleep. Once the writer finishes (writerActive == false), wake up and continue."
            return !writerActive && waitingWriters==0 ;      // "Wait until there is no active writer."
        });
        activeReaders++;
    }

    void unlockRead(){
        unique_lock<mutex> lock(mtx);
        activeReaders--;
        if(activeReaders==0){
            cv.notify_all();
        }
    }

    void lockWrite()
    {
        unique_lock<mutex> lock(mtx);
        waitingWriters++;
        cv.wait(lock, [&]()
        {
            return !writerActive && activeReaders == 0;
        });
        waitingWriters--;
        writerActive = true;
    }

    void unlockWrite()
    {
        unique_lock<mutex> lock(mtx);

        writerActive = false;

        cv.notify_all();
    }

};

ReadWriteLock rw;

void reader(int id)
{
    rw.lockRead();

    cout << "Reader " << id << " reading\n";

    this_thread::sleep_for(chrono::seconds(2));

    cout << "Reader " << id << " finished\n";

    rw.unlockRead();
}

void writer(int id)
{
    rw.lockWrite();

    cout << "Writer " << id << " writing\n";

    this_thread::sleep_for(chrono::seconds(3));

    cout << "Writer " << id << " finished\n";

    rw.unlockWrite();
}

int main()
{
    thread t1(reader, 1);
    thread t2(reader, 2);
    thread t3(reader, 3);

    this_thread::sleep_for(chrono::milliseconds(500));

    thread t4(writer, 1);

    thread t5(reader, 4);

    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
}





/*
Case 1: Initially

writerActive = false;

The lambda checks:
return !writerActive;

becomes
return !false;

which is
return true;

Since it returned true, cv.wait() says: "Okay, the condition is satisfied. Continue." No waiting happens.

----

Case 2: A writer starts writing

writerActive = true;

Now the lambda becomes:
return !true;

which is
return false;

cv.wait() says: "Condition is not satisfied. Go to sleep." The thread waits.
*/