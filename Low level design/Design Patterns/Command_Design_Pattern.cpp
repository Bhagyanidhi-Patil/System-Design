/*
The Command Design Pattern is a behavioral design pattern that turns a request into a stand-alone object 
containing all information about the request.

🧠 Core Idea
👉 In the Command Pattern, the client does not directly call the function on the receiver.
    Instead, it:
        Creates a command object (which contains the request)
        Passes that command to an invoker
        The invoker calls execute() on the command
        The command then calls the actual function on the receiver

🔁 Flow in simple terms
    Without Command Pattern:
        Client → directly calls → Receiver.function()

    With Command Pattern:
        Client → Command → Invoker → Command.execute() → Receiver.function()

📦 Real-Life Analogy
Think of a remote control
    You press a button (invoker)
    It triggers a command (like “turn on TV”)
    The TV (receiver) performs the action

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

🧭 When to Use?
Use Command Pattern when:
    You want to parameterize objects with operations
    You need undo/redo functionality
    You want to queue or schedule requests
    You want to decouple sender and receiver
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