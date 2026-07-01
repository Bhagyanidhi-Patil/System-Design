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
    unordered_map<string,userInfo>users;
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

        auto it = users.find(userId);

        // First request from this user
        if (it == users.end())
        {
            userInfo newUser;
            newUser.requestCount = 1;
            newUser.windowStart = now;

            users[userId] = newUser;
            return true;
        }

        userInfo &user = it->second;

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

