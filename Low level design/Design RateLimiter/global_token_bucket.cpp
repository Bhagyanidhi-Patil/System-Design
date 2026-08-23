#include <iostream>
#include <mutex>
#include <chrono>
#include <thread>
#include <algorithm>

using namespace std;

class RateLimiter {
public:
    virtual bool allowRequest() = 0;
    virtual ~RateLimiter() = default;
};

class TokenBucket : public RateLimiter {
private:
    int capacity;
    int refillRate;       // tokens per second
    int currTokens;

    mutex mtx;
    chrono::steady_clock::time_point lastRefill;

public:
    TokenBucket(int cap, int rate)
        : capacity(cap),
          refillRate(rate),
          currTokens(cap),
          lastRefill(chrono::steady_clock::now()) {}

    bool allowRequest() override {

        lock_guard<mutex> lock(mtx);

        auto now = chrono::steady_clock::now();

        auto elapsed =
            chrono::duration_cast<chrono::seconds>(
                now - lastRefill
            ).count();

        // Refill tokens
        currTokens = min(
            capacity,
            currTokens + static_cast<int>(elapsed * refillRate)
        );

        // Move refill time forward
        if (elapsed > 0) {
            lastRefill += chrono::seconds(elapsed);
        }

        // Consume token
        if (currTokens > 0) {
            currTokens--;
            return true;
        }

        return false;
    }
};

int main() {

    TokenBucket limiter(5, 2);
    // capacity = 5
    // refill = 2 tokens/second

    for (int i = 0; i < 7; i++) {
        cout << limiter.allowRequest() << endl;
    }

    return 0;
}