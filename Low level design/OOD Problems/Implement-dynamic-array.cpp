//Implement dynamic array (like vector) 

#include <iostream>
using namespace std;

template<typename T>

class DynamicArray{
private:
    T* arr;
    int size;
    int cap;

    void resize(){
        cap = cap*2;
        T* newarr = new T[cap];

        for(int i=0;i<size;i++){
            newarr[i] = arr[i];
        }
        delete[] arr;
        arr = newarr;
    }
public:
    DynamicArray(){
        cap = 1;
        size = 0;
        arr = new T[cap];
    }

    ~DynamicArray(){
        delete[] arr;
    }

    void push_back(T value){
        if(size == cap){
            resize();
        }
        arr[size++] = value;
    }

    void pop_back(){
        if(size > 0){
            size--;
        }
    }

    int size_(){
        return size;
    }

    void printValue(){
        for(int i=0;i<size;i++){
            cout<<arr[i]<<" ";
        }
        cout<<endl;
    }
};

int main(){
    DynamicArray<int>a;
    a.push_back(10);
    a.push_back(15);
    a.push_back(30);
    a.push_back(25);
    cout<<a.size_()<<endl;
    a.printValue();
    a.pop_back();
    a.printValue();
    a.push_back(40);
    cout<<a.size_()<<endl;
    a.printValue();
    return 0;
}

/*
T* arr means:- arr is a pointer variable that can store the address of a T , not to first address of array.

What does T* arr mean here?
In this code, T* arr is: a pointer that will point to the first element of a dynamically allocated array of type T.

Why we can say it's an array here:
Because later you do: T* newArr = new T[cap];
This creates a dynamic array of size cap.

T arr[fixed_size]; → static/stack array (fixed, short-lived)
T* arr = new T[cap]; → dynamic array (flexible, required for containers)
You use new T[cap] not just because it's dynamic—but because it's the only correct way to implement a resizable array like this in standard C++.

Why resize() is private ?
Because the user of your class should NOT control how resizing happens.
Resizing is an internal detail of how your dynamic array works.

--------------------------------------------------------------------------------------------------------

🧠 Why we write:
DynamicArray<int> a;
and NOT: DynamicArray a;   // ❌ wrong
Reason: Template needs a type parameter to know what type of elements the dynamic array will hold.
Here, T is unknown until you specify it. 

DynamicArray a;
👉 Compiler doesn’t know: Should T be int? float? char?
➡️ So it gives an error.So the compiler asks: “What type should I use for T?”

DynamicArray<int> a;
👉 Now compiler knows: T = int
So it becomes: int* arr;
*/