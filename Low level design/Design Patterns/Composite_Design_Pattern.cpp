/*
The Composite Design Pattern is a structural pattern that lets you treat individual objects and groups of objects uniformly. 
It’s especially useful when you’re working with tree-like structures (like file systems, UI elements, or organizational hierarchies).

🧱 Components of Composite Pattern
1.Component (Base Class)
    Declares the common interface
2.Leaf
    Represents individual objects (no children)
3.Composite
    Represents a group of objects (can contain children)

            UML-Diagram
                +------------------+
                |    Graphic       |   <<interface>>
                +------------------+
                | + draw() = 0     |
                +------------------+
                         ▲
            ┌────────────┼────────────┐
            │            │            │
   +---------------+  +---------------+  +------------------+
   |    Circle     |  |  Rectangle    |  |      Group       |
   +---------------+  +---------------+  +------------------+
   | + draw()      |  | + draw()      |  | - children       |
   +---------------+  +---------------+  | : List<Graphic>  |
                                         +------------------+
                                         | + add(Graphic*)  |
                                         | + draw()         |
                                         +------------------+
                                                 ▲
                                                 │
                                      (contains / aggregates)
                                                 │
                                                 ▼
                                           Graphic
*/
#include <iostream>
#include <vector>
using namespace std;

//Component Interface
class Graphic {
public:
    virtual void draw() = 0;
    virtual ~Graphic() {}
};

//Leaf
class Circle:public Graphic{
public:
    void draw() override{
        cout<<"Drawing a circle"<<endl;
    }
};
class Rectangle : public Graphic {
public:
    void draw() override {
        cout << "Drawing Rectangle\n";
    }
};

//Composite
class Group:public Graphic{
    vector<Graphic*>children;
public:
    void add(Graphic* g){
        children.push_back(g);
    }
    void draw() override{
        cout<<"Drawing Circle from Group:"<<endl;
        for(auto child:children){
            child->draw();
        }
    }
};

int main() {
    // Leaf objects
    Graphic* circle1 = new Circle();
    Graphic* circle2 = new Circle();
    Graphic* rect1 = new Rectangle();

    // First group
    Group* group1 = new Group();
    group1->add(circle1);
    group1->add(rect1);

    // Main group (nested composite)
    Group* mainGroup = new Group();
    mainGroup->add(group1);
    mainGroup->add(circle2);

    // Uniform call
    mainGroup->draw();

    return 0;
}
/*
Code explaination:
1. Component → Graphic
    This is the common interface
    Both leaf and composite classes inherit from it
    The client only knows about Graphic, not concrete types
2.Leaf → Circle , Reactangle
    A basic object
    Cannot contain other objects
    Implements draw() directly
3.Composite → Group
    It stores a collection of Graphic objects
    Important: it stores the base type, not Circle
        So it can hold: Circle (leaf) or Group (another composite)
*/