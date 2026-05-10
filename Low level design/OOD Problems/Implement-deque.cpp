/*
Implement Deque: Double Ended Queue.

It is a linear data structure where insertion and deletion can happen from both ends:
Front
Rear (Back)

So it combines features of:
Queue → insert rear, delete front
Stack → insert/delete at same end

In a normal Queue:

Insertion happens at the rear
Deletion happens from the front

This follows the FIFO principle: First In First Out
*/

#include <iostream>
using namespace std;

class Deque{
private:
    int *arr;
    int front;
    int rear;
    int size;

public:
    Deque(int s){
        size = s;
        arr = new int[size];
        front = -1;
        rear = -1;
    }

    bool isFull(){
        return ((front == 0 && rear == size-1)||(front == rear+1));
    }

    bool isEmpty(){
        return (front == -1);
    }

    void pushFront(int x){
        if(isFull()){
            cout<<"Deque overflow"<<endl;
            return;
        }
        //1st element
        if(front == -1){
            front = rear = 0;
        }
        //circular move
        else if(front == 0){
            front = size-1;
        }
        else{
            front --;
        }
        arr[front] = x;
    }

    void pushBack(int x){
        if(isFull()){
            cout<<"Deque overflow"<<endl;
            return;
        }
        //1st element
        if(front == -1){
            front = rear = 0;
        }
        // Circular move
        else if (rear == size - 1) {
            rear = 0;
        }

        else {
            rear++;
        }

        arr[rear] = x;
    }

    // Delete from front
    void popFront() {
        if (isEmpty()) {
            cout << "Deque Underflow\n";
            return;
        }

        // Only one element
        if (front == rear) {
            front = rear = -1;
        }

        else if (front == size - 1) {
            front = 0;
        }

        else {
            front++;
        }
    }

    // Delete from rear
    void popBack() {
        if (isEmpty()) {
            cout << "Deque Underflow\n";
            return;
        }

        // Only one element
        if (front == rear) {
            front = rear = -1;
        }

        else if (rear == 0) {
            rear = size - 1;
        }

        else {
            rear--;
        }
    }
        // Get front element
    int getFront() {
        if (isEmpty()) {
            cout << "Deque is Empty\n";
            return -1;
        }
        return arr[front];
    }

    // Get rear element
    int getRear() {
        if (isEmpty()) {
            cout << "Deque is Empty\n";
            return -1;
        }
        return arr[rear];
    }

    // Display deque
    void display() {
        if (isEmpty()) {
            cout << "Deque is Empty\n";
            return;
        }
        cout << "Deque elements: ";
        int i = front;
        while (true) {
            cout << arr[i] << " ";
            if (i == rear)
                break;
            i = (i + 1) % size;
        }
        cout << endl;
    }

    // Destructor
    ~Deque() {
        delete[] arr;
    }
};


int main() {

    Deque dq(5);

    dq.pushBack(10);
    dq.pushBack(20);
    dq.pushFront(5);
    dq.pushFront(2);

    dq.display();

    cout << "Front: " << dq.getFront() << endl;
    cout << "Rear : " << dq.getRear() << endl;

    dq.popFront();
    dq.popBack();

    dq.display();

    return 0;
}

/*
===============================
DEQUE VISUAL REPRESENTATION
(Circular Array Implementation)
===============================

SIZE = 5

Indexes:
0   1   2   3   4


--------------------------------
1. INITIAL STATE
--------------------------------

_   _   _   _   _

front = -1
rear  = -1

Deque is EMPTY.


--------------------------------
2. pushBack(10)
--------------------------------

10  _   _   _   _

F,R

front = 0
rear  = 0


Logical Deque:
10


--------------------------------
3. pushBack(20)
--------------------------------

10  20  _   _   _

F    R

front = 0
rear  = 1


Logical Deque:
10 20


--------------------------------
4. pushBack(30)
--------------------------------

10  20  30  _   _

F        R

front = 0
rear  = 2


Logical Deque:
10 20 30


--------------------------------
5. pushFront(5)
--------------------------------

front was at 0
so it wraps to size-1

front = 4


10  20  30  _   5

        R       F
        


Logical Traversal:
4 -> 0 -> 1 -> 2


Logical Deque:
5 10 20 30


--------------------------------
6. pushFront(2)
--------------------------------

front moves backward

front = 3
rear = 2

10  20  30  2   5

        R    F       


Logical Traversal:
3 -> 4 -> 0 -> 1 -> 2


Logical Deque:
2 5 10 20 30


--------------------------------
7. DEQUE IS NOW FULL
--------------------------------

Condition:

(front == rear + 1)

3 == 2 + 1

TRUE
No more insertion possible.

--------------------------------
8. popFront()
--------------------------------

front element removed

front moves forward


10  20  30  2   5

        R   F
        


front = 4
rear  = 2


Logical Traversal:
4 -> 0 -> 1 -> 2


Logical Deque:
5 10 20 30



--------------------------------
9. popBack()
--------------------------------

rear element removed

rear moves backward


10  20  30  2   5

        R       F
    


front = 4
rear  = 1


Logical Traversal:
4 -> 0 -> 1


Logical Deque:
5 10 20


--------------------------------
10. popFront()
--------------------------------

front moves forward


10  20  30  2   5

F    R


front = 0
rear  = 1


Logical Deque:
10 20


--------------------------------
11. pushBack(40)
--------------------------------

rear moves forward


10  20  40  2   5

F       R


front = 0
rear  = 2


Logical Deque:
10 20 40


--------------------------------
12. pushBack(50)
--------------------------------

rear moves forward


10  20  40  50  5

F            R


front = 0
rear  = 3


Logical Deque:
10 20 40 50


--------------------------------
13. pushBack(60)
--------------------------------

rear moves forward

10  20  40  50  60

F               R


front = 0
rear  = 4

Logical Deque:
10 20 40 50 60

Now deque becomes FULL.

================================
IMPORTANT CIRCULAR MOVEMENTS
================================

Moving FRONT backward:

if(front == 0)
    front = size-1;
else
    front--;



Moving REAR forward:

if(rear == size-1)
    rear = 0;
else
    rear++;



================================
FULL CONDITION
================================

(front == 0 && rear == size-1)
OR
(front == rear + 1)


================================
EMPTY CONDITION
================================

front == -1
rear == -1

================================
TRAVERSAL FORMULA
================================

i = (i + 1) % size

Example:
(4 + 1) % 5 = 0

Wraps around to beginning.
*/
