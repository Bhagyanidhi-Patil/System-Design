#include <iostream>
#include <unordered_map>
#include <mutex>
#include <chrono>
using namespace std;

struct userInfo{
    int requestCount;
    chrono::steady_clock::time_point windowStart;

    userInfo(){
        requestCount = 0;
        windowStart = chrono::steady_clock::now();
    }
};

class RateLimiter{
public: 
    virtual bool allowRequest(const string& userId)=0;
    virtual ~RateLimiter(){}
};

class FixedWindowRateLimiter : public RateLimiter{
private:
    unordered_map<string,userInfo>m;
    int maxRequests;
    chrono::seconds windowSize;
    mutex mtx;
public:
    FixedWindowRateLimiter(int maxReq,int windowSec){
        maxRequests = maxReq;
        windowSize = chrono::seconds(windowSec);
    }

    bool allowRequest(const string& userId) override
    {
        lock_guard<mutex> lock(mtx);

        auto now = chrono::steady_clock::now();

        // First request from this user
        if (m.find(userId)==m.end())
        {
            userInfo newUser;
            newUser.requestCount = 1;
            newUser.windowStart = now;

            m[userId] = newUser;
            return true;
        }

        userInfo &user = m[userId];

        auto elapsed = chrono::duration_cast<chrono::seconds>(now - user.windowStart);

        if (elapsed >= windowSize)
        {
            user.windowStart = now;
            user.requestCount = 1;   // Current request is counted
            return true;
        }

        if (user.requestCount < maxRequests)
        {
            user.requestCount++;
            return true;
        }

        return false;
    }
};

int main()
{
    FixedWindowRateLimiter limiter(5,60);

    string user="Alice";

    for(int i=1;i<=7;i++)
    {
        if(limiter.allowRequest(user))
            cout<<"Request "<<i<<" Allowed"<<endl;
        else
            cout<<"Request "<<i<<" Rejected"<<endl;
    }

    return 0;
}

/*
For global request implmentation (insated of limiting per user):

#include <iostream>
#include <unordered_map>
#include <mutex>
#include <chrono>
#include <thread>
using namespace std;


class RateLimiter{
public:
    virtual bool allowRequest(const string& userId)=0;
    virtual ~RateLimiter(){}
};

class FixedWindow:public RateLimiter{
private:
    int maxRequest;
    int requests;
    chrono::seconds windowSize;
    chrono::steady_clock::time_point windowStart;
    mutex mtx;
public:
    FixedWindow(int maxreq,int windowsec){
        maxRequest = maxreq;
        requests = 0;
        windowSize = chrono::seconds(windowsec);
        windowStart = chrono::steady_clock::now();
    }

    bool allowRequest(const string& userId)override{
        lock_guard<mutex>lock(mtx);
        auto now = chrono::steady_clock::now();
        auto elapsed = chrono::duration_cast<chrono::seconds>(now-windowStart);
        if(elapsed>=windowSize){
            windowStart = now;
            requests=1;
            return true;
        }
        if(requests<maxRequest){
            requests++;
            return true;
        }
        return false;
    }
};

int main() {
    FixedWindow limiter(3, 5);  // 3 requests every 5 seconds

    cout << limiter.allowRequest("A") << endl;  // 1 → ALLOW
    cout << limiter.allowRequest("B") << endl;  // 1 → ALLOW
    cout << limiter.allowRequest("C") << endl;  // 1 → ALLOW
    cout << limiter.allowRequest("D") << endl;  // 0 → REJECT

    this_thread::sleep_for(chrono::seconds(5));

    cout << limiter.allowRequest("E") << endl;  // 1 → new window

    return 0;
}


*/