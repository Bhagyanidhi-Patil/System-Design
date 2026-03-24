/*
The Facade Design Pattern is a structural design pattern provides a simplified interface to a larger and more complex subsystem. 
This makes the subsystem easier to use without exposing its complexity to the client.
It allows the client to interact with a system without needing to understand its internal workings or complexities.

🧱 Structure : 
Subsystem classes
    Complex logic (multiple classes)

Facade class
    Provides simple methods
    Internally calls subsystem classes

Client
    Uses only the facade (doesn’t see complexity)

                        UML-Diagram
                        
                        +--------+
                        | Client |
                        +--------+
                            |
                            v
                    +-------------------+
                    |  ComputerFacade   |
                    +-------------------+
                    | - cpu: CPU        |
                    | - memory: Memory  |
                    | - hardDrive: HardDrive |
                    +-------------------+
                    | + startComputer() |
                    +----+--------+-----+
                        |        |
                --------        --------
                |                      |
            +-----------+        +--------------+
            |    CPU    |        |    Memory    |
            +-----------+        +--------------+
            | + start() |        | + load()     |
            +-----------+        +--------------+

                        |
                        |
                +---------------+
                |  HardDrive    |
                +---------------+
                | + read()      |
                +---------------+

*/

#include<iostream>
using namespace std;

//Subsystem class
class CPU{
public:
    void start(){
        cout<<"CPU started"<<endl;
    }
};
class Memory{
public:
    void load(){
        cout<<"Memory started"<<endl;
    }
};
class HardDrive{
public:
    void read(){
        cout<<"Harddrive reading"<<endl;
    }
};

//Facade Class
class ComputerFacade {
private:
    CPU cpu;
    Memory memory;
    HardDrive hardDrive;

public:
    void startComputer() {
        cout << "Starting computer...\n";
        cpu.start();
        memory.load();
        hardDrive.read();
        cout << "Computer started successfully!\n";
    }
};

//Client code
int main() {
    ComputerFacade computer;
    computer.startComputer();
    return 0;
}

