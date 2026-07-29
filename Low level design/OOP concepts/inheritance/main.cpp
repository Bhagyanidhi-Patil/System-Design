#include <iostream>
using namespace std;

class Vehicle {
public:
    void start() {
        cout << "Vehicle Started\n";
    }

    void stop() {
        cout << "Vehicle Stopped\n";
    }
};

class Car : public Vehicle {
public:
    void playMusic() {
        cout << "Playing Music\n";
    }
};

int main() {

    Car car;

    car.start();
    car.playMusic();
    car.stop();

    return 0;
}