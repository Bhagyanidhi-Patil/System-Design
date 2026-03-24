/*
The Chain of Responsibility is a behavioral design pattern in C++ that lets you pass a request along a chain of handlers.
🧱 Structure: 
    Handler (abstract class) - Defines interface and holds next handler
    Concrete Handlers - Implement handling logic
    Client - Builds the chain and sends request

                   UML-Diagram

            +----------------------+
            |      Handler        |  <<abstract>>
            +----------------------+
            | - next: Handler*    |
            +----------------------+
            | + setNext(h:Handler*)|
            | + handleRequest()   |
            +----------+----------+
                       |
        ---------------------------------
        |               |               |
+----------------+ +----------------+ +----------------+
| Level1Handler  | | Level2Handler  | | Level3Handler  |
+----------------+ +----------------+ +----------------+
|                | |                | |                |
+----------------+ +----------------+ +----------------+
| +handleRequest()| | +handleRequest()| | +handleRequest()|
+----------------+ +----------------+ +----------------+


*/
#include <iostream>
using namespace std;

class Handler {
protected:
    Handler* next;

public:
    Handler() : next(nullptr) {}

    void setNext(Handler* n) {
        next = n;
    }

    virtual void handleRequest(int request) {
        if (next) {
            next->handleRequest(request);
        }
    }

    virtual ~Handler() {}
};

class Level1Support : public Handler {
public:
    void handleRequest(int request) override {
        if (request <= 10) {
            cout << "Level 1 handled request: " << request << endl;
        } else if (next) {
            next->handleRequest(request);
        }
    }
};

class Level2Support : public Handler {
public:
    void handleRequest(int request) override {
        if (request <= 20) {
            cout << "Level 2 handled request: " << request << endl;
        } else if (next) {
            next->handleRequest(request);
        }
    }
};

class Level3Support : public Handler {
public:
    void handleRequest(int request) override {
        cout << "Level 3 handled request: " << request << endl;
    }
};

int main() {
    Handler* l1 = new Level1Support();
    Handler* l2 = new Level2Support();
    Handler* l3 = new Level3Support();

    l1->setNext(l2);
    l2->setNext(l3);

    l1->handleRequest(5);   // handled by Level 1
    l1->handleRequest(15);  // handled by Level 2
    l1->handleRequest(30);  // handled by Level 3

    delete l1;
    delete l2;
    delete l3;

    return 0;
}