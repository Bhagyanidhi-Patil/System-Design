/*
The simplest interview-ready solution is to use a shared_mutex to protect the entire file-system tree.
Multiple threads may access the same Node, its content, or its children map simultaneously. 
Without synchronization, this can cause data races, corrupted data, or crashes.

A shared_mutex provides two types of locking:
- shared_lock<shared_mutex>: multiple readers can hold the lock together.
- unique_lock<shared_mutex> (exclusive lock): only one writer can hold the lock, and no readers can hold it simultaneously.

A shared_mutex allows multiple readers OR one writer at a time. 
Readers and writers cannot access the protected data simultaneously when the writer holds the exclusive lock.

Situation	                       Allowed?
Thread 1 reads + Thread 2 reads	    ✅ Yes
Thread 1 writes + Thread 2 writes	❌ No
Thread 1 reads + Thread 2 writes	❌ No
Thread 1 writes + Thread 2 reads	❌ No

Case 1: Multiple readers
    Both threads can read the same file concurrently because neither modifies it.

Case 2: A writer arrives while readers hold shared locks
    Suppose Thread 1 and Thread 2 are reading a file, and Thread 3 wants to write to it.
    Thread 1 and Thread 2 Hold shared locks and continue reading.
    Thread 3 Requests an exclusive lock and waits until all existing shared locks are released.
    Thread 3 obtains the exclusive lock and writes. Other readers and writers must wait while it holds the lock.

Case 3: A reader arrives while a writer holds the exclusive lock
    The opposite also applies.
    - Thread 1 holds unique_lock and writes.
    - Thread 2 tries to acquire shared_lock.
    - Thread 2 waits until Thread 1 releases the exclusive lock.
    This prevents Thread 2 from reading partially updated data.

Scenario :
Suppose Thread 1 and Thread 2 are reading, Thread 3 is waiting to write, and then Thread 4 requests a shared lock.
What happens to Thread 4?
There are two possible behaviors:
- Writer-preference implementation: Thread 4 waits because Thread 3 is already waiting to write. 
    This prevents writers from being delayed indefinitely by a continuous stream of new readers.
- Reader-preference or other implementation: Thread 4 may acquire the shared lock and begin reading alongside Threads 1 and 2. 
    3 must then wait for Thread 4 to finish as well.
The C++ standard does not guarantee which of these behaviors std::shared_mutex will use.
*/

#include <shared_mutex>
#include <mutex>

class FileSystem {
private:
    Node* root;
    mutable shared_mutex mtx;

    // Your existing split() function...

    void mkdir(string path) {
        unique_lock<shared_mutex> lock(mtx);

        // Your existing mkdir() implementation
    }

    bool addFile(string path) {
        unique_lock<shared_mutex> lock(mtx);

        // Your existing addFile() implementation
    }

    bool writeFile(string path, string content) {
        unique_lock<shared_mutex> lock(mtx);

        // Your existing writeFile() implementation
    }

    bool deleteNode(string path) {
        unique_lock<shared_mutex> lock(mtx);

        // Your existing deleteNode() implementation
    }

    string readFile(string path) {
        shared_lock<shared_mutex> lock(mtx);

        // Your existing readFile() implementation
    }

    void ls(string path) {
        shared_lock<shared_mutex> lock(mtx);

        // Your existing ls() implementation
    }
};