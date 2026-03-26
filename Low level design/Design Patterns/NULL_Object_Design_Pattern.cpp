/*
The Null Object Design Pattern is a behavioral design pattern that helps eliminate null checks 
in your code by providing a default “do-nothing” object instead of using nullptr.

In C++, you often see code like:
    if (obj != nullptr) {
        obj->doSomething();
    }

    This leads to:
        Repeated null checks everywhere
        Risk of segmentation faults if forgotten
        Messy and less readable code
    Instead of returning nullptr, return a special object that:
        Implements the same interface
        Does nothing (or default behavior)

🔹 Structure
You typically have:
    Abstract base class (interface)
    Real object class
    Null object class (does nothing)

                    UML-Diagram
                
                +----------------------+
                |       Logger         |   <<interface>>
                +----------------------+
                | + log(message) = 0   |
                +----------------------+
                         ▲
                         │
        ┌────────────────┴────────────────┐
        │                                 │
+----------------------+        +----------------------+
|   ConsoleLogger      |        |     NullLogger       |
+----------------------+        +----------------------+
| + log(message)       |        | + log(message)       |
|   (prints message)   |        |   (does nothing)     |
+----------------------+        +----------------------+

*/
#include<iostream>
using namespace std;

class Logger {
public:
    virtual void log(const std::string& message) = 0;
    virtual ~Logger() {}
};

class ConsoleLogger : public Logger {
public:
    void log(const std::string& message) override {
        std::cout << "Log: " << message << std::endl;
    }
};

class NullLogger : public Logger {
public:
    void log(const std::string& message) override {
        // Do nothing
    }
};

void process(Logger* logger) {
    logger->log("Processing started");
}

int main() {
    Logger* logger;

    bool enableLogging = false;

    if (enableLogging)
        logger = new ConsoleLogger();
    else
        logger = new NullLogger();  // instead of nullptr

    process(logger);

    delete logger;
    return 0;
}