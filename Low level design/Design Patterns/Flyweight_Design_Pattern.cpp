/*
The Flyweight pattern is a structural design pattern used to reduce memory usage when you have a large number of similar objects.
👉 Instead of creating many duplicate objects, you share common data between them.

Split object data into two parts:
1. Intrinsic State (shared)
    Common data shared across many objects
    Stored once and reused
    Example: font style, color, texture
2. Extrinsic State (unique)
    Data unique to each object
    Passed from outside when needed
    Example: position, size

🧩 Real-Life Analogy:
    Think of characters in a text editor:
        Each letter 'A' doesn't store font/style repeatedly
        All 'A's share the same font object
        Only position differs

🏗️ Structure:
    1.Flyweight (interface/class)
    2.ConcreteFlyweight (shared objects)
    3.FlyweightFactory (manages and reuses objects)
    4.Client (uses flyweights)

                        UML-Diagram
               
                +-----------------------------+
                |        TreeType             | <<Flyweight>>
                +-----------------------------+
                | - name : string             |
                | - color : string            |
                +-----------------------------+
                | + TreeType(name, color)     |
                | + display(x:int, y:int):void|
                +-----------------------------+

                            ▲
                            | (shared)
                            |
                +-----------------------------+
                |           Tree              | <<Context>>
                +-----------------------------+
                | - x : int                   |
                | - y : int                   |
                | - type : TreeType*          |
                +-----------------------------+
                | + Tree(x, y, type)          |
                | + display(): void           |
                +-----------------------------+

                            ^
                            |
                            | uses
                            |
            +--------------------------------------+
            |          TreeFactory                 | <<FlyweightFactory>>
            +--------------------------------------+
            | - treeTypes : map<string, TreeType*> |
            +--------------------------------------+
            | + getTreeType(name, color):TreeType* |
            +--------------------------------------+
*/

#include<iostream>
#include<unordered_map>
using namespace std;

// Flyweight
class TreeType {
private:
    string name;
    string color;

public:
    TreeType(string name, string color)
        : name(name), color(color) {}

    void display(int x, int y) {
        cout << "Tree: " << name
             << ", Color: " << color
             << ", Position: (" << x << ", " << y << ")\n";
    }
};

// Flyweight Factory
class TreeFactory {
private:
    unordered_map<string, TreeType*> treeTypes;

public:
    TreeType* getTreeType(string name, string color) {
        string key = name + "_" + color;

        if (treeTypes.find(key) == treeTypes.end()) {
            treeTypes[key] = new TreeType(name, color);
        }
        return treeTypes[key];
    }
};

// Context (Extrinsic State)
class Tree {
private:
    int x, y;
    TreeType* type;

public:
    Tree(int x, int y, TreeType* type)
        : x(x), y(y), type(type) {}

    void display() {
        type->display(x, y);
    }
};

int main() {
    TreeFactory factory;

    Tree* t1 = new Tree(10, 20, factory.getTreeType("Oak", "Green"));
    Tree* t2 = new Tree(30, 40, factory.getTreeType("Oak", "Green"));
    Tree* t3 = new Tree(50, 60, factory.getTreeType("Pine", "Dark Green"));

    t1->display();
    t2->display();
    t3->display();

    return 0;
}

/*
🔍 What’s Happening?
    "Oak + Green" tree type is created once
    Reused for multiple trees (t1, t2)
    Saves memory when scaling to thousands/millions
*/