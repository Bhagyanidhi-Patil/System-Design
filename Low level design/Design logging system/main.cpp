#include <iostream>
#include <fstream>
#include <vector>
#include <string>
using namespace std;

enum LogLevel{
    DEBUG,
    INFO,
    WARNING,
    ERROR
};

string getLevelString(LogLevel level){
    switch(level){
        case DEBUG:
            return "DEBUG";
        case INFO:
            return "INFO";
        case WARNING:
            return "WARN";
        case ERROR:
            return "ERROR";
        default:
            return "";
    }
    return "";
}

class ILogAppender{
public:
    virtual void append(LogLevel level,const string& message)=0;
    virtual ~ILogAppender() = default;
};

class ConsoleAppender:public ILogAppender{
public:
    void append(LogLevel level,const string& message)override{
        cout << "[" << getLevelString(level) << "] "<< message << endl;
    }
};

class FileAppender:public ILogAppender{
private:
    ofstream file;
public:
    FileAppender(string filename){
        file.open(filename,ios::app); //ios::app means: Append new data at the end of the existing file.
    }
    void append(LogLevel level,const string& message)override{
        file << "[" << getLevelString(level) << "] "
            << message << endl;
    }
};

enum AppenderType
{
    CONSOLE,
    FILES
};

class AppenderFactory
{
public:

    static ILogAppender* createAppender(AppenderType type,string filename = "")
    {
        if(type == CONSOLE)
        {
            return new ConsoleAppender();
        }

        if(type == FILES)
        {
            return new FileAppender(filename);
        }

        return nullptr;
    }
};

class Logger{
private:
    vector<ILogAppender*>appenders;

    void notify(LogLevel level,const string& message){
        for(ILogAppender* appender : appenders)
        {
            appender->append(level, message);
        }
    }
public:
    void addAppender(ILogAppender* appender){
        appenders.push_back(appender);
    }
    
    void log(LogLevel level,
             const string& message)
    {
       notify(level,message);
    }

    void debug(const string& message)
    {
        log(DEBUG, message);
    }

    void info(const string& message)
    {
        log(INFO, message);
    }

    void warning(const string& message)
    {
        log(WARNING, message);
    }

    void error(const string& message)
    {
        log(ERROR, message);
    }
};

int main()
{
    Logger logger;

    ILogAppender* console =
        AppenderFactory::createAppender(CONSOLE);

    ILogAppender* file =
        AppenderFactory::createAppender(
            FILES,
            "application.log");


    logger.addAppender(console);
    logger.addAppender(file);


    logger.debug("Starting application");

    logger.info("Server started");

    logger.warning("Memory usage is high");

    logger.error("Database connection failed");


    delete console;
    delete file;

    return 0;
}


/*
ifstream = input file stream .It is used to read data from a file.
ofstream ofstream = output file stream .It is used to write data to a file.

Design pattern used - factory + observer 

*/