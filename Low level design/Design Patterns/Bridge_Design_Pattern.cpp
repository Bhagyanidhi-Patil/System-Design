/*
The Bridge Design Pattern is a structural design pattern that separates an abstraction from its implementation,
so the two can change independently.
“Split a class into two parts: abstraction and implementation — and connect them with a bridge.”

Real-world Example: Remote Control & TV 📺

Imagine you have:
Remotes
    Basic Remote
    Advanced Remote
TV Brands
    Sony TV
    Samsung TV

Without Bridge, you'd need:
    BasicSonyRemote
    BasicSamsungRemote
    AdvancedSonyRemote
    AdvancedSamsungRemote

As more remotes and TV brands are added, the number of classes explodes.

Instead, Bridge connects them using composition.

Block Diagram
                Abstraction
             +----------------+
             |    Remote      |
             +----------------+
             | Device* device |
             | togglePower()  |
             +----------------+
                    |
         -------------------------
         |                       |
         |                       |
+----------------+      +-------------------+
| BasicRemote    |      | AdvancedRemote    |
+----------------+      +-------------------+

                    has-a
                      |
                      v

            +------------------+
            |      Device      |  <-- Implementor
            +------------------+
            | powerOn()        |
            | powerOff()       |
            +------------------+
                  ^
                  |
         --------------------
         |                  |
+----------------+   +----------------+
| SonyTV         |   | SamsungTV      |
+----------------+   +----------------+
*/

#include <iostream>
using namespace std;

// Implementor
class Device {
public:
    virtual void powerOn() = 0;
    virtual void powerOff() = 0;
    virtual ~Device() {}
};

// Concrete Implementations
class SonyTV : public Device {
public:
    void powerOn() override {
        cout << "Sony TV ON\n";
    }

    void powerOff() override {
        cout << "Sony TV OFF\n";
    }
};

class SamsungTV : public Device {
public:
    void powerOn() override {
        cout << "Samsung TV ON\n";
    }

    void powerOff() override {
        cout << "Samsung TV OFF\n";
    }
};

// Abstraction
class Remote {
protected:
    Device* device;

public:
    Remote(Device* d) {
        device = d;
    }

    virtual void togglePower() = 0;
    virtual ~Remote() {}
};

// Refined Abstraction
class BasicRemote : public Remote {
public:
    BasicRemote(Device* d) : Remote(d) {}

    void togglePower() override {
        device->powerOn();
    }
};

// Another Refined Abstraction
class AdvancedRemote : public Remote {
public:
    AdvancedRemote(Device* d) : Remote(d) {}

    void togglePower() override {
        device->powerOff();
    }
};

int main() {

    Device* sony = new SonyTV();
    Device* samsung = new SamsungTV();

    Remote* r1 = new BasicRemote(sony);
    r1->togglePower();

    Remote* r2 = new AdvancedRemote(samsung);
    r2->togglePower();

    delete sony;
    delete samsung;
    delete r1;
    delete r2;
}