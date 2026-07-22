//Problem - Producer adds items, consumer removes items safely.

#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
using namespace std;

queue<int>q;
mutex mtx;
condition_variable cvProducer;
condition_variable cvConsumer;
const int MAX_SIZE = 5;

void producer(){
    for(int i=0;i<10;i++){
        unique_lock<mutex>lock(mtx);
        cvProducer.wait(lock,[]{return q.size()<MAX_SIZE;});
        q.push(i);
        cout<<"Produced "<<i<<" "<<endl;
         // Unlock before notifying
        lock.unlock();
        cvConsumer.notify_one();
    }
}

void consumer(){
    for(int i=0;i<10;i++){
        unique_lock<mutex>lock(mtx);
        cvConsumer.wait(lock,[]{return !q.empty();});
        cout<<"Consumed "<<q.front()<<" "<<endl;
        q.pop();
         // Unlock before notifying - release mutex early so other thread can acquire it immediately after notify (avoids unnecessary blocking)
        lock.unlock();
        cvProducer.notify_one();
    }
}

int main() {
    thread t1(producer), t2(consumer);
    t1.join(); t2.join();
    return 0;
}

/*
cv.wait(lock, condition) sleeps when condition is FALSE, and continues when condition is TRUE.
*/

/*
----
Dry Run

Initially:
Buffer = { }

Producer starts.

Step 1
Producer locks mutex
Checks:
buffer.size() < BUFFER_SIZE
0 < 5
True.
Producer inserts
1
Buffer becomes
[1]

Producer unlocks mutex and calls

cvConsumer.notify_one();

Consumer wakes.

Checks
!buffer.empty()
!false
true

Consumer removes
1
Buffer becomes
{ }
Consumer calls
cvProducer.notify_one();
Producer wakes again.

What Happens if Buffer Becomes Full?

Suppose
Buffer
1
2
3
4
5

Producer executes
cvProducer.wait(lock, [] {
    return buffer.size() < BUFFER_SIZE;
});

Predicate
5 < 5
False.

Producer:
Releases mutex
Goes to sleep

Consumer removes one item.

Buffer
2
3
4
5

Consumer calls
cvProducer.notify_one();
Producer wakes.

Now
4 < 5
True.

Producer continues.

What Happens if Buffer Becomes Empty?
Suppose
Buffer
{ }

Consumer executes
cvConsumer.wait(lock, [] {
    return !buffer.empty();
});

Predicate
!true
False.

Consumer sleeps.
Producer inserts
10
Calls
cvConsumer.notify_one();
Consumer wakes and consumes the item.

---

Why Are Two Condition Variables Used?

condition_variable cvProducer;
condition_variable cvConsumer;
cvProducer

Used by the producer to wait when the buffer is full.

cvProducer.wait(lock, [] {
    return buffer.size() < BUFFER_SIZE;
});

The consumer notifies it after removing an item.

cvProducer.notify_one();
cvConsumer

Used by the consumer to wait when the buffer is empty.

cvConsumer.wait(lock, [] {
    return !buffer.empty();
});

The producer notifies it after inserting an item.

cvConsumer.notify_one();

*/