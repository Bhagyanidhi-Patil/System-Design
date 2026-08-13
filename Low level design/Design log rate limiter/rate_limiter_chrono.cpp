#include <iostream>
#include <chrono>
#include <unordered_map>
#include <string>
#include <thread>
#include <mutex>
using namespace std;

enum logLevel{
    INFO,
    ERROR,
    WARN,
    DEBUG
};

class logLimiter{
private:
    unordered_map<string,chrono::steady_clock::time_point>m;
    chrono::steady_clock::duration timewindow;
    mutex mtx;

    string makeKey(logLevel level,const string& message){
        return to_string(level)+"|"+message;
    }

    string leveltostring(logLevel level){
        switch(level){
            case INFO: return "INFO";
            case DEBUG: return "DEBUG";
            case WARN : return "WARN";
            case ERROR : return "ERROR";
            default: return "UNKNOWN";
        }
    }

public:
    logLimiter(chrono::steady_clock::duration window):timewindow(window){}

    bool printMessage(logLevel level,const string& message){
        auto now = chrono::steady_clock::now();
        string key = makeKey(level,message);
        lock_guard<mutex>lock(mtx);

        if(m.find(key)==m.end()){
            m[key] = now;
            cout << "[" << leveltostring(level) << "] "
                 << message << endl;

            return true;
        }

        auto elapsed = chrono::duration_cast<chrono::seconds>(now-m[key]);
        if(elapsed>=timewindow){
            m[key] = now;
            cout << "[" << leveltostring(level) << "] "
                 << message << endl;

            return true;
        }
        return false;
    }
};

int main() {
    logLimiter logger(chrono::seconds(5));

    logger.printMessage(INFO, "Service started");

    this_thread::sleep_for(chrono::seconds(2));

    logger.printMessage(INFO, "Service started");

    this_thread::sleep_for(chrono::seconds(4));

    logger.printMessage(INFO, "Service started");

    logger.printMessage(ERROR, "Database failed");

    return 0;
}