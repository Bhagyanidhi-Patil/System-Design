/*
The Prototype Design Pattern is a creational design pattern used when you want to create new objects by copying
an existing object (prototype) instead of building them from scratch.

Prototype design pattern is used
    ○ Used when you need multiple instances of an object but want to avoid the cost of creating them from scratch.
	○ Instead of creating a new object every time, you clone (copy) an existing object.
	○ Useful when object creation is expensive.

    Instead of:
        Object* obj = new Object(); // costly initialization
    You do:
        Object* obj = existingObj->clone(); // fast copy
    
🔹 Structure
	○ Prototype Interface: An interface or abstract class that declares a clone method.
	○ Concrete Prototype: Implements the clone method to return a copy of itself.
    ○ Client: Uses the prototype to create new objects by invoking the clone method.
    
                    UML-Diagram
                
                +----------------------+
                |        Shape         |   <<abstract>>
                +----------------------+
                | + clone(): Shape*    |
                | + draw(): void       |
                +----------------------+
                         ▲
                         │
                +----------------------+
                |        Circle        |
                +----------------------+
                | - radius: int        |
                +----------------------+
                | + Circle(r:int)      |
                | + clone(): Shape*    |
                | + draw(): void       |
                +----------------------+

                +----------------------+
                |        Client        |
                +----------------------+
                | uses Shape           |
                +----------------------+
*/

#include<iostream>
using namespace std;

class Shape {
public:
    virtual Shape* clone() = 0;
    virtual void draw() = 0;
    virtual ~Shape() {}
};

class Circle : public Shape {
private:
    int radius;

public:
    Circle(int r) : radius(r) {}

    // Clone method
    Shape* clone() override {
        return new Circle(radius); // copy constructor
    }

    void draw() override {
        cout << "Circle with radius: " << radius << endl;
    }
};

int main() {
    Shape* original = new Circle(10);

    // Clone the object
    Shape* copy = original->clone();

    original->draw();
    copy->draw();

    delete original;
    delete copy;

    return 0;
}