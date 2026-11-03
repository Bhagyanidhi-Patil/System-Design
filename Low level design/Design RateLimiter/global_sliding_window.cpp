#include<iostream>
#include<chrono>
#include<queue>
#include<mutex>
using namespace std;

class RateLimiter{
public:
    virtual bool allowRequest()=0;
    ~RateLimiter()=default;
};

class SlidingWindowRateLimiter:public RateLimiter{
private:
    queue<chrono::steady_clock::time_point>m;
    int maxRequests;
    chrono::seconds windowSize;
    mutex mtx;

public:
    SlidingWindowRateLimiter(int maxReq,int windowsec){
        maxRequests = maxReq;
        windowSize = chrono::seconds(windowsec);
    }

    bool allowRequest() override{
        lock_guard<mutex>lock(mtx);
        auto now = chrono::steady_clock::now();
        while (!m.empty())
        { 
           if (now - m.front() >= windowSize)
                m.pop();
            else
                break;
        }

        if (m.size() < maxRequests)
        {
            m.push(now);
            return true;
        }

        return false;

    }
};

int main() {
    SlidingWindowRateLimiter limiter(3, 10);

    cout << "Alice: " << limiter.allowRequest() << endl;
    cout << "Bob: " << limiter.allowRequest() << endl;
    cout << "Charlie: " << limiter.allowRequest() << endl;
    cout << "David: " << limiter.allowRequest() << endl;

    return 0;
}