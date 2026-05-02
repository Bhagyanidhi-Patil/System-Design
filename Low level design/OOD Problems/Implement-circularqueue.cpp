/*
A circular queue is a linear data structure that follows FIFO (First In, First Out) 
but the last position is connected back to the first, forming a circle.
✅ Insertion (enqueue) → happens at the rear
✅ Deletion (dequeue) → happens from the front
*/

#include <iostream>
using namespace std;

class CircularQueue{
    int *arr;
    int front, rear, size;
public:
    CircularQueue(int n){
        size = n;
        arr = new int[size];
        front = rear =-1;
    }

    bool isFull(){
        return (rear+1)%size == front;
    }

    bool isEmpty(){
        return front == -1;
    }

    void enqueue(int val){
        if(isFull()){
            cout<<"Queue is full"<<endl;
            return ;
        }
        if(isEmpty()){
            front = rear = 0;
        }
        else{
            rear = (rear+1)%size;
        }
        arr[rear] = val;
    }

    void dequeue(){
        if(isEmpty()){
            cout<<"Queue is empty"<<endl;
            return ;
        }
        if(front == rear){
            front = rear = -1;
        }
        else{
            front = (front+1)%size;
        }
    }

     int getFront() {
        if (isEmpty()) {
            cout << "Queue is Empty\n";
            return -1;
        }
        return arr[front];
    }

    void display(){
        int i = front;
        while (true) {
            cout << arr[i] << " ";
            if (i == rear) break;
            i = (i + 1) % size;
        }
        cout << endl;
    }

    ~CircularQueue(){
        delete []arr;
    }
};

int main() {
    CircularQueue q(5);

    q.enqueue(10);
    q.enqueue(20);
    q.enqueue(30);
    q.enqueue(40);
    q.enqueue(50);

    q.display();

    q.dequeue();
    q.dequeue();

    q.enqueue(60);
    q.enqueue(70);

    q.display();

    return 0;
}

/*
q.enqueue(10);
q.enqueue(20);
q.enqueue(30);
q.enqueue(40);
q.enqueue(50);

Index:   0   1   2   3   4
Value:  10  20  30  40  50
         ↑               ↑
       front           rear

front = 0 ,rear = 4 ,Queue is FULL

q.dequeue(); // removes 10
q.dequeue(); // removes 20

Index:   0   1   2   3   4
Value:   _   _  30  40  50
                 ↑       ↑
               front    rear
front = 2, rear = 4

q.enqueue(60);
Since (rear + 1) % size = (4 + 1) % 5 = 0, it wraps around:

Index:   0   1   2   3   4
Value:  60   _  30  40  50
        ↑       ↑       ↑
       rear    front  (old rear)
front = 2, rear = 0
*/