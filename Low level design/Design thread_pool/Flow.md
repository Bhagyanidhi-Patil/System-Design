## Flow:

### Step 1: main() starts
```
int main()
{
    ThreadPool pool(3);
```
`You create a thread pool with 3 worker threads.`

Memory looks like this:
```
ThreadPool

Workers : Empty

Task Queue : Empty

stop = false
```

---

### Step 2: Constructor is called
`ThreadPool(int n): stop(false)`

The pool is active.

Now the loop runs.
```
for(int i=0;i<n;i++)
```
Since n = 3, it creates:
```
Worker 1
Worker 2
Worker 3
```
using

`workers.emplace_back(...)`

Now
```
Workers

Worker 1

Worker 2

Worker 3
```
All three threads immediately start executing.

---

### Step 3: Each worker starts

Worker 1 executes

`while(true)`

Worker 2 also executes

`while(true)`

Worker 3 also executes

`while(true)`

So all workers are running simultaneously.

---

### Step 4: Workers reach cv.wait()

Each worker executes

`unique_lock<mutex> lock(mtx);`

Suppose Worker 1 gets the mutex first.

It checks
```
cv.wait(lock,[this]()
{
    return stop || !tasks.empty();
});
```
Current values

`stop = false`

`tasks.empty() = true`

Condition

`false || false = false`

Therefore Worker 1 sleeps.

When cv.wait() sleeps:

- it automatically releases the mutex
- so another worker can acquire it

Now Worker 2 locks the mutex.

Same condition.

`Queue still empty.`

`Worker 2 sleeps.`

Again mutex released.

`Worker 3 does exactly the same.`

Now
```
Worker1  Sleeping

Worker2  Sleeping

Worker3  Sleeping
```
CPU usage is almost zero because they are waiting efficiently.

---

### Step 5: submit() is called

Loop starts.
```
for(int i=1;i<=10;i++)
```
First iteration
```
pool.submit(...)
```
The lambda is
```
[i]()
{
    cout<<"Task "<<i;
}
```
For i = 1

the lambda becomes
```
[]()
{
    cout<<"Task 1";
}
```

---

### Step 6: Inside submit()
`lock_guard<mutex> lock(mtx);`

Acquire mutex.

Queue : `Empty`

Now

`tasks.emplace(task);`

Queue becomes
```
Front

Task1
```
`Unlock automatically.`

Then
```
cv.notify_one();
```
One sleeping worker wakes.

Suppose Worker 2 wakes.

---

### Step 7: Worker wakes

Worker 2 was inside

`cv.wait(...)`

Now it checks again

`stop || !tasks.empty()`

Queue is

`Task1`

So

`!tasks.empty()`

is

`true`

Condition

`false || true = true`

Therefore it continues.

---

### Step 8: Stop check
```
if(stop && tasks.empty())
```
Current values
```
stop = false

tasks.empty() = false
```
Condition

`false && false = false`

Continue.

---

### Step 9: Pick one task

`task = move(tasks.front());`

Local variable
```
task

↓

Task1 Lambda
```
Queue still contains Task1 until

`tasks.pop();`

After pop
```
Queue

Empty
```
Mutex released.

---

### Step 10: Execute task

Now

`task();`

runs
```
cout<<"Task 1 executed..."
```
Output
```
Task 1 executed by 12345
```
Worker finishes.

---

### Step 11: Worker loops again

Worker goes back

`while(true)`

Again

`cv.wait(...)`

Queue empty.

Worker sleeps again.

---

### Step 12: Second submit()

Loop in main

`i = 2 `

Submit

Queue
```
Task2
```
`notify_one()`

Maybe Worker 1 wakes.

Worker 1 executes Task2.

---

### Step 13: Third submit()

Queue
```
Task3
```
notify_one()

Worker 3 wakes.

Executes Task3.

---

### Step 14: Fourth submit()

Now all workers may already be busy.

Suppose
```
Worker1 -> Task2

Worker2 -> Task1

Worker3 -> Task3
```
You submit Task4.

Queue
```
Task4
```

**No worker is sleeping.**

So nobody wakes.

Task4 simply waits.

As soon as Worker2 finishes,

it returns to

`while(true)`

Checks queue.

Finds Task4.

Executes it.

**Same happens for Task5...Task10**

Tasks are executed whenever a worker becomes free.

**Example**
```
Queue

Task5

Task6

Task7
```
Workers continuously take
```
Take front

↓

Execute

↓

Repeat
```
After loop finishes
`return 0;`

Now

pool

goes out of scope.

`Destructor is called automatically.`

Destructor
`stop = true;`

Now
```
stop = true
```
Then
```
cv.notify_all();
```
Suppose workers are sleeping
```
Worker1 Sleeping

Worker2 Sleeping

Worker3 Sleeping
```
All wake up.

Workers wake again

Condition
```
stop || !tasks.empty()
```
Now
```
stop = true
```
Condition immediately becomes

true

Workers continue.

Stop check

Queue already empty.

So
```
if(stop && tasks.empty())
```
becomes
```
true && true

=

true
```
Each worker executes

return;

Thread exits.

join()

Destructor finally does
```
for(auto &t:workers)
    t.join();
```
Main thread waits until
```
Worker1 exits

Worker2 exits

Worker3 exits
```
After all threads finish,

program terminates safely.

---