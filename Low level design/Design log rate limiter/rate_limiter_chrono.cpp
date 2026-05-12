#include <iostream>
#include <unordered_map>
#include <chrono>
#include <thread>
#include <string>
using namespace std;

enum Loglevel{
    INFO,
    DEBUG,
    WARN,
    ERROR
};

class LoggerRateLimiter{
private:
    unordered_map<string,chrono::steady_clock::time_point>lastPrintedTime;
    chrono::seconds timeWindow;

    //helper : create composite key
    string makeKey(Loglevel level,const string& message)const{
        return to_string(level)+"|"+message;
    }

    string levelToString(Loglevel level){
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

    bool shouldPrintMessage(Loglevel level,const string& message){
        auto now = chrono::steady_clock::now();
        string key = makeKey(level,message);

        // First occurrence
        if (lastPrintedTime.find(key) == lastPrintedTime.end()) {
            lastPrintedTime[key] = now;

            cout << "[" << levelToString(level) << "] "
                 << message << endl;

            return true;
        }

        auto elapsed = chrono::duration_cast<chrono::seconds>(now - lastPrintedTime[key]);

        // Print only if time window passed
        if (elapsed >= timeWindow) {

            lastPrintedTime[key] = now;

            cout << "[" << levelToString(level) << "] "
                 << message << endl;

            return true;
        }

        // Suppressed log
        cout << "[SUPPRESSED] "
             << message << endl;

        return false;
    }
};

int main() {
    LoggerRateLimiter logger(5);

    logger.shouldPrintMessage(INFO, "Service started");

    this_thread::sleep_for(chrono::seconds(2));

    logger.shouldPrintMessage(INFO, "Service started");

    this_thread::sleep_for(chrono::seconds(4));

    logger.shouldPrintMessage(INFO, "Service started");

    logger.shouldPrintMessage(ERROR, "Database failed");

    return 0;
}