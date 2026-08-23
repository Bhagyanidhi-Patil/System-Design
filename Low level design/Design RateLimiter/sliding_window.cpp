#include<iostream>
#include<unordered_map>
#include<chrono>
#include<queue>
#include<mutex>
using namespace std;

class RateLimiter{
public:
    virtual bool allowRequest(const string &userId)=0;
    ~RateLimiter(){}
};

class SlidingWindowRateLimiter:public RateLimiter{
private:
    unordered_map<string,queue<chrono::steady_clock::time_point>>m;
    int maxRequests;
    chrono::seconds windowSize;
    mutex mtx;

public:
    SlidingWindowRateLimiter(int maxReq,int windowsec){
        maxRequests = maxReq;
        windowSize = chrono::seconds(windowsec);
    }

    bool allowRequest(const string &userId) override{
        lock_guard<mutex>lock(mtx);
        auto now = chrono::steady_clock::now();

    
        if (m.find(userId) == m.end())
        {
            m[userId] = queue<chrono::steady_clock::time_point>();
        }

        while (!m[userId].empty())
        {
            auto elapsed = chrono::duration_cast<chrono::seconds>(
                now - m[userId].front());

            if (elapsed >= windowSize)
                m[userId].pop();
            else
                break;
        }

        if (m[userId].size() < maxRequests)
        {
            m[userId].push(now);
            return true;
        }

        return false;

    }
};

#include <thread>

int main()
{
    // Maximum 3 requests in 10 seconds
    SlidingWindowRateLimiter limiter(3, 10);

    cout << "====== Alice ======" << endl;

    // Alice sends 4 requests immediately
    for (int i = 1; i <= 4; i++)
    {
        cout << "Alice Request " << i << " : "
             << (limiter.allowRequest("Alice") ? "Allowed" : "Rejected")
             << endl;
    }

    cout << "\n====== Bob ======" << endl;

    // Bob also sends 4 requests immediately
    for (int i = 1; i <= 4; i++)
    {
        cout << "Bob Request " << i << " : "
             << (limiter.allowRequest("Bob") ? "Allowed" : "Rejected")
             << endl;
    }

    cout << "\nWaiting for 11 seconds...\n" << endl;

    // Wait until the sliding window expires
    this_thread::sleep_for(chrono::seconds(11));

    cout << "====== Alice After Window Expired ======" << endl;

    for (int i = 5; i <= 8; i++)
    {
        cout << "Alice Request " << i << " : "
             << (limiter.allowRequest("Alice") ? "Allowed" : "Rejected")
             << endl;
    }

    cout << "\n====== Bob After Window Expired ======" << endl;

    for (int i = 5; i <= 7; i++)
    {
        cout << "Bob Request " << i << " : "
             << (limiter.allowRequest("Bob") ? "Allowed" : "Rejected")
             << endl;
    }

    return 0;
}