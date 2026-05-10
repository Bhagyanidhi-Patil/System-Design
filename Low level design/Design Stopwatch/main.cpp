#include <iostream>
#include <chrono>
#include <thread>
using namespace std;

class Stopwatch{
private:
    using Clock = chrono::steady_clock;    //Clock is another name for std::chrono::steady_clock.

    // Stores time when stopwatch was started/resumed
    Clock::time_point startTime;
    // Stores already accumulated elapsed time
    chrono::milliseconds elapsed{0};            //creating a variable named elapsed of type milliseconds, initialized to 0 milliseconds.
    // Tracks whether stopwatch is currently running
    bool running = false;

public:
    //start stopwatch from zero
    void start(){
        elapsed = chrono::milliseconds(0);    //assigning zero milliseconds to the variable elapsed.
        startTime = Clock::now();
        running = true;
    }

    //pause stopwatch
    void pause(){
        if(running == false)return;

        auto currentDuration = chrono::duration_cast<chrono::milliseconds>(Clock::now() - startTime);
        elapsed+=currentDuration;
        running = false;
    }

    //Resume stopwatch
    void resume(){
        if(running == true)return;

        startTime = Clock::now();
        running = true;
    }

    //Stop stopwatch completely
    void stop(){
        pause();
    }

    //reset stopwatch
    void reset(){
        elapsed = chrono::milliseconds(0);
        running = false;
    }

    //return elapsed time in milliseconds - It returns how much time has passed since the stopwatch started.
    long long elaspedMilliseconds(){
        // include current active session duration
        if(running == true){
            auto currentDuration = chrono::duration_cast<chrono::milliseconds>(Clock::now()-startTime);
            return (elapsed+currentDuration).count();
        }
        // If paused/stopped
        return elapsed.count();
    }
};

int main(){
    Stopwatch sw;
    cout<<"Starting stopwatch"<<endl;
    sw.start();

    //pause the current thread for 2 seconds.
    this_thread::sleep_for(chrono::milliseconds(2));

    cout<<"Elasped: "<<sw.elaspedMilliseconds()<<" ms\n";

    cout<<"Pausing..\n"<<endl;

    sw.pause();
    std::this_thread::sleep_for(std::chrono::seconds(2));

    std::cout << "Still paused: "
              << sw.elaspedMilliseconds()
              << " ms\n";

    std::cout << "Resuming...\n";

    sw.resume();

    std::this_thread::sleep_for(std::chrono::seconds(1));

    std::cout << "Final elapsed: "
              << sw.elaspedMilliseconds()
              << " ms\n";

    sw.stop();

    return 0;
}

/*
elapsed.count();
.count() in C++ is used to extract the numeric value from a std::chrono::duration.
.count() = gives the actual number stored inside a duration object.

In std::chrono, time is NOT stored as a plain number.
It is stored like this:
std::chrono::milliseconds t(2000);

Here t is NOT just 2000, it is a duration object.
To get the actual number (2000), we use:
t.count();

*/


