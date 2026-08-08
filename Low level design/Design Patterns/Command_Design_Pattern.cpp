/*
The Command Design Pattern is a behavioral design pattern that turns a request into a stand-alone object 
containing all information about the request.
Command Pattern encapsulates a request or action as an object, allowing the sender to execute it without 
knowing how the action is performed.
----
Imagine a Restaurant 🍕

There are 4 people:

Customer  --->  Waiter  --->  Chef
     (Request)     (Command)    (Receiver)
Customer wants a pizza.
Waiter takes the order.
Chef actually cooks the pizza.

The customer doesn't know how to cook, and the chef doesn't talk directly to the customer.
The waiter acts as the Command object, carrying the request.
----
🧠 Core Idea
👉 In the Command Pattern, the client does not directly call the function on the receiver.
    Instead, it:
        Creates a command object (which contains the request)
        Passes that command to an invoker
        The invoker calls execute() on the command
        The command then calls the actual function on the receiver
----
🔁 Flow in simple terms
    Without Command Pattern:
        Client → directly calls → Receiver.function()

    With Command Pattern:
        Client → Command → Invoker → Command.execute() → Receiver.function()
----
Why do we wrap the action into an object?

    Suppose you want to:

    Save the action for later
    Queue it
    Undo it
    Log it

    Instead of directly calling:
    light.on();

    you create an object:
    LightOnCommand cmd;

    Now you can:

    Store cmd in a queue
    Execute it later
    Keep a history for undo/redo
    Send it to another thread

    That's why the action is wrapped in an object.
----
🧩 Components
1.Command (Interface / Abstract Class)
    Declares an execute() method
    ConcreteCommand
2.Implements the command
    Binds receiver with action
3.Receiver
    The actual object that performs the action
4.Invoker
    Triggers the command
5.Client
    Creates and configures command objects
----
🧭 When to Use?
Use Command Pattern when:
    You want to parameterize objects with operations
    You need undo/redo functionality
    You want to queue or schedule requests
    You want to decouple sender and receiver
----
1. Without Command Pattern

Suppose you directly call:
light.on();
light.off();
fan.on();

After these functions execute:
❌ You cannot undo them.
❌ You cannot save them.
❌ You cannot execute them later.
❌ You cannot send them to another thread.

The function call is gone.

2. With Command Pattern

Instead of executing immediately:
LightOnCommand cmd(&light);
Now cmd is an object.

Objects can be:
stored
copied
queued
logged
executed later
Undo

Suppose every command implements:

class Command {
public:
    virtual void execute() = 0;
    virtual void undo() = 0;
};

Example:

class LightOnCommand : public Command {
public:
    void execute() {
        light.on();
    }

    void undo() {
        light.off();
    }
};
*/

#include <iostream>
using namespace std;

// Command Interface
class Command {
public:
    virtual void execute() = 0;
    virtual ~Command() {}
};

// Receiver
class Light {
public:
    void turnOn() {
        cout << "Light is ON\n";
    }

    void turnOff() {
        cout << "Light is OFF\n";
    }
};

// Concrete Command: Turn On
class LightOnCommand : public Command {
    Light* light;
public:
    LightOnCommand(Light* l) : light(l) {}

    void execute() override {
        light->turnOn();
    }
};

// Concrete Command: Turn Off
class LightOffCommand : public Command {
    Light* light;
public:
    LightOffCommand(Light* l) : light(l) {}

    void execute() override {
        light->turnOff();
    }
};

// Invoker
class RemoteControl {
    Command* command;
public:
    void setCommand(Command* cmd) {
        command = cmd;
    }

    void pressButton() {
        command->execute();
    }
};

// Client
int main() {
    Light light;

    LightOnCommand onCmd(&light);
    LightOffCommand offCmd(&light);

    RemoteControl remote;

    remote.setCommand(&onCmd);
    remote.pressButton();  // Light ON

    remote.setCommand(&offCmd);
    remote.pressButton();  // Light OFF

    return 0;
}