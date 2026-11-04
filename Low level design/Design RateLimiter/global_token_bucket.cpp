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
        //We use static_cast<int>() because we want to convert the calculated refill amount into an integer.
        //static_cast<int>() truncates the decimal portion.


        // Move refill time forward
        if (elapsed > 0) {
            lastRefill += chrono::seconds(elapsed);
        }
        /*
        Example
        Suppose:
        - lastRefill = 10:00:00
        - now = 10:00:05.7
        - elapsed = 5 seconds (because duration_cast truncates fractional seconds).
        Your code executes:
        lastRefill += chrono::seconds(5);


        So:
        - New lastRefill = 10:00:05
        - now = 10:00:05.7
        The remaining 0.7 seconds are preserved for the next refill calculation.
        */


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