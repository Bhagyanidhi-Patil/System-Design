#include <iostream>
#include <queue>
#include <mutex>
#include <thread>

using namespace std;

class ThreadSafeQueue {
private:
    queue<int> q;
    mutex mtx;

public:

    // Producer
    void push(int value) {
        lock_guard<mutex> lock(mtx);

        q.push(value);

        cout << "Produced: " << value << endl;
    }

    // Consumer
    bool pop(int &value) {
        lock_guard<mutex> lock(mtx);

        if (q.empty()) {
            cout << "Queue is empty" << endl;
            return false;
        }

        value = q.front();
        q.pop();

        cout << "Consumed: " << value << endl;

        return true;
    }
};

int main() {

    ThreadSafeQueue q;

    thread producer([&q]() {
        for (int i = 1; i <= 10; i++) {
            q.push(i);
        }
    });

    thread consumer([&q]() {
        for (int i = 1; i <= 10; i++) {

            int value;

            if (q.pop(value)) {
                // Successfully consumed
            }
        }
    });

    producer.join();
    consumer.join();

    cout << "Done!" << endl;

    return 0;
}