#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <chrono>
#include <memory>
using namespace std;

class Fork{
public:
    int id;
    mutex mtx;

    Fork(int Id):id(Id){}
};

class Philosopher{
private:
    int id;
    Fork* leftFork;
    Fork* rightFork;
public:
    Philosopher(int id,Fork* left,Fork* right)
    :id(id),leftFork(left),rightFork(right){}

    void think(){
        cout<<"Philosopher "<<id<<" is thinking "<<endl;
        this_thread::sleep_for(chrono::milliseconds(1000));
    }

    void eat(){
        Fork* first = (leftFork->id<rightFork->id)?leftFork:rightFork;
        Fork* second = (leftFork->id<rightFork->id)?rightFork:leftFork;

        unique_lock<mutex>lock1(first->mtx);
        unique_lock<mutex>lock2(second->mtx);

        cout<<"Philosopher "<<id <<" starts eating "<<endl;
        this_thread::sleep_for(chrono::milliseconds(1000));
        cout<<"Philosopher "<<id<<" finished eating "<<endl;
    }

    void dine(){
        for(int i=0;i<3;i++){
            think();
            eat();
        }
        cout<<"Philospher "<<id<<" leaves the table"<<endl;
    }
};

class DiningTable{
private:
    int n;
    vector<unique_ptr<Fork>> forks;
    vector<Philosopher>philosphers;
    vector<thread>threads;

public:
    DiningTable(int n):n(n){
        // Create forks
        for (int i = 0; i < n; i++)
        {
            forks.push_back(make_unique<Fork>(i));
        }

        // Create philosophers
        for (int i = 0; i < n; i++)
        {
            Fork* left = forks[i].get();
            Fork* right = forks[(i + 1) % n].get();

            philosphers.emplace_back(i, left, right);
        }
    }

    void startDinner(){
        for(int i=0;i<n;i++){
            threads.emplace_back(&Philosopher::dine,&philosphers[i]);
        }
        for(auto &t:threads){
            t.join();
        }
    }
};

int main() {

    const int NUM_PHILOSOPHERS = 5;

    DiningTable table(NUM_PHILOSOPHERS);

    table.startDinner();

    return 0;
}

/*
We create a DiningTable with N philosophers. 
The constructor creates all the shared Fork objects and all the Philosopher objects. 
When startDinner() is called, it creates one thread per philosopher. 
Each thread executes that philosopher's dine() method, where the philosopher repeatedly thinks → eats three times before finishing.

----

threads.emplace_back(&Philosopher::dine, &philosphers[i]);

std::thread constructor has the following form:
thread(Function, Arguments...);

It starts a new thread that executes:
Function(Arguments...);

&Philosopher::dine -> is a pointer to the member function dine().
&philosphers[i] -> is the object on which dine() should be called.
*/