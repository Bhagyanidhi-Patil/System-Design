#include <iostream>
#include <unordered_map>
#include <chrono>
#include <mutex>
#include <thread>
using namespace std;

struct UserInfo
{
    double tokens;
    chrono::steady_clock::time_point lastRefillTime;
};

class RateLimiter{
public:
    virtual bool allowRequests(const string&userID)=0;
    ~RateLimiter(){}
};

class TokenBucket:public RateLimiter{
private:
    unordered_map<string,UserInfo>m;
    int capacity;
    double refillRate;
    mutex mtx;
public:
    TokenBucket(int cap,double rate){
        capacity = cap;
        refillRate = rate;
    }

    bool allowRequests(const string& userId) override{
        lock_guard<mutex>lock(mtx);
        auto now = chrono::steady_clock::now();

        if(m.find(userId)==m.end()){
            UserInfo user;
            user.tokens = capacity;
            user.lastRefillTime = now;
            m[userId] = user;
        }
        UserInfo &user = m[userId];
        auto elapsed = chrono::duration_cast<chrono::seconds>(now-user.lastRefillTime).count();

        user.tokens=min((double)capacity,user.tokens+elapsed*refillRate);
        user.lastRefillTime = now;

        if(user.tokens>=1){
            user.tokens--;
            return true;
        }
        return false;
    }
};

int main()
{
    TokenBucket limiter(5,1);

    cout<<"===== Alice ====="<<endl;

    for(int i=1;i<=7;i++)
    {
        cout<<"Request "<<i<<" : "
            <<(limiter.allowRequests("Alice")
            ?"Allowed":"Rejected")<<endl;
    }

    cout<<"\nWaiting 3 seconds...\n"<<endl;

    this_thread::sleep_for(chrono::seconds(3));

    for(int i=8;i<=12;i++)
    {
        cout<<"Request "<<i<<" : "
            <<(limiter.allowRequests("Alice")
            ?"Allowed":"Rejected")<<endl;
    }

    return 0;
}


/*
Rule to remember
now - lastRefillTime → gives duration object
duration_cast<seconds> → duration in seconds
.count() → extract the numeric value (e.g., 3, 10, 25)

That's why we use:

auto elapsed = chrono::duration_cast<chrono::seconds>(
    now - user.lastRefillTime).count();

instead of stopping at duration_cast. We need the number of seconds so we can do arithmetic like:

user.tokens += elapsed * refillRate;

where elapsed is a plain numeric value.

----
why min() ?

"min() ensures that the bucket never stores more tokens than its maximum capacity.
Even if the system is idle for a long time, the bucket fills up only to its configured limit and doesn't keep accumulating unlimited tokens."

If the current tokens + refilled tokens are less than the capacity, min() returns that value, and we keep it.
If it's greater than the capacity, min() returns the capacity.

Example 1:
Suppose

Capacity = 5 tokens
Current Tokens = 2
Elapsed = 3 seconds
Refill Rate = 1 token/sec

New tokens added
3 × 1 = 3

Total
2 + 3 = 5

Now we do
min(5,5)

Result
5
---
Another Example:

Suppose

Capacity = 5
Current Tokens = 4
Elapsed = 3 sec
Refill Rate = 1

New tokens
4 + (3 × 1)

↓

7

Can the bucket hold 7 tokens?
❌ No.

So
min(5,7)

returns
5

The bucket stays full.

---

Another Example:

Suppose

Capacity = 10
Current Tokens = 3
Elapsed = 2 sec
Refill Rate = 2

New tokens
3 + (2 × 2)

↓

7

Now
min(10,7)

returns
7

Since 7 is less than the capacity, we keep it.
*/