#include <iostream>
#include <unordered_map>
#include <string>
#include <mutex>
using namespace std;

enum Loglevel{
    INFO,
    DEBUG,
    WARN,
    ERROR
};

class LoggerRateLimiter{
private:
    unordered_map<string,int>lastPrintedTime;
    int timeWindow;
    mutex mtx;

    //helper : create composite key
    string makeKey(Loglevel level,const string& message)const{
        return to_string(level)+"|"+message;
    }

    string levelToString(Loglevel level)const{
        switch(level){
            case INFO: return "INFO";
            case DEBUG: return "DEBUG";
            case WARN: return "WARN";
            case ERROR: return "ERROR";
            default: return "UNKOWN";
        }
    }
public:
    LoggerRateLimiter(int window):timeWindow(window){}

    bool shouldPrintMessage(int timestamp,Loglevel level,const string& message){
        string key = makeKey(level,message);
        
        lock_guard<mutex>lock(mtx);

        if(lastPrintedTime.find(key)==lastPrintedTime.end() || timestamp - lastPrintedTime[key]>=timeWindow){
            lastPrintedTime[key] = timestamp;

                cout << "[" << timestamp << "] "<< levelToString(level)<< ": "<< message << endl;

                return true;
        }
        return false;
    }
};

int main() {
    LoggerRateLimiter logger(10);

    cout << logger.shouldPrintMessage(0, INFO, "Service started") << endl;   // 1
    cout << logger.shouldPrintMessage(3, INFO, "Service started") << endl;   // 0

    cout << logger.shouldPrintMessage(3, ERROR, "Service started") << endl;  // 1 (different level)

    cout << logger.shouldPrintMessage(11, INFO, "Service started") << endl;  // 1

    cout << logger.shouldPrintMessage(11, WARN, "Disk low") << endl;         // 1

    return 0;
}