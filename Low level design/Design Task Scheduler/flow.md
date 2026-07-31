## Step 1 : main() starts
Scheduler scheduler;

Object is created.

Inside Scheduler, the following members are initialized.

```cpp
priority_queue pq;
mutex mtx;
condition_variable cv;
bool running = false;
thread worker;
```

At this point

Priority Queue : Empty

running = false

Worker Thread : Not created

## Step 2
scheduler.start();

Calls

```cpp
void start()
{
    running = true;

    worker = thread(&Scheduler::run, this);
}
```

Now

Main Thread
        |
        |
        |---- creates Worker Thread

The worker immediately starts executing

run()

Worker Thread enters run()
while(running)

running is true.

So enter loop.

Creates

```cpp
unique_lock<mutex> lock(mtx);
```

Worker thread locks the mutex.

Current state

Worker owns mutex

Checks

```cpp
if(pq.empty())
```

Priority Queue is empty.

So

```cpp
cv.wait(lock);
```

What happens here?

This is very important.

wait() does two things:

- Unlocks mutex
- Puts worker thread to sleep

So now

Worker sleeping

Mutex unlocked

It consumes zero CPU while waiting.

Meanwhile...

Main thread continues.

## Step 3
scheduler.addTask(...)

Calls

```cpp
lock_guard<mutex> lock(mtx);
```

Since worker released mutex,

Main thread acquires it.

Pushes task

Task1

Run after 2 sec

Priority Queue

Task1 (2 sec)

Then

```cpp
cv.notify_one();
```

This wakes sleeping worker.

Worker becomes READY.

Main thread exits function.

Lock_guard goes out of scope.

Mutex released.

Worker wakes.

It automatically reacquires mutex before returning from

```cpp
wait()
```

Then loop starts again.

Second iteration

Queue

Task1

Not empty.

So

```cpp
auto task = pq.top();
```

Gets

Task1

Doesn't remove it yet.

Priority Queue still contains it.

Gets current time

```cpp
auto now
```

Suppose

Current = 0 sec

Task time = 2 sec

Condition

now >= nextRun

False.

So

```cpp
wait_until(lock,nextRun);
```

Again

Worker

unlock mutex

sleep until

2 sec

Notice

No busy waiting.

No while loop checking every millisecond.

It literally sleeps.

Meanwhile

Main thread adds

Task2
Run after 5 sec

Queue becomes

Task1 -> 2 sec

Task2 -> 5 sec

Then adds

Recurring
Run after 1 sec

Queue

Recurring -> 1 sec

Task1 -> 2 sec

Task2 -> 5 sec

### Question:

How did Recurring come to top?

Because Priority Queue sorts using Compare.

We'll discuss Compare later.

Worker is sleeping until...

Actually here's something important.

When Main thread inserted

Recurring (1 sec)

It calls

```cpp
notify_one()
```

Worker wakes immediately.

Why?

Because maybe a task earlier than current one has arrived.

Otherwise worker would unnecessarily sleep till 2 sec.

Worker wakes.

Again

```cpp
pq.top()
```

returns

Recurring

Current

0.5 sec

Still not ready.

So

```cpp
wait_until(1 sec)
```

Sleep again.

After 1 second

Worker wakes automatically.

Now

Current = 1 sec

Task = 1 sec

Condition

now>=nextRun

True.

Removes task

```cpp
pq.pop();
```

Queue becomes

Task1

Task2

Then

```cpp
lock.unlock();
```

### Question:

Why unlock?

Imagine execute()

takes

10 seconds

If mutex stays locked,

Nobody can add tasks.

Scheduler becomes blocked.

So we unlock before execution.

Execute

```cpp
task->execute();
```

Calls

```cpp
PrintTask::execute()
```

Prints

Executing Recurring

Then

```cpp
if(recurring)
```

True.

So

```cpp
nextRun =
now + interval;
```

Suppose interval

3 sec

Current

1 sec

New execution

4 sec

Then

```cpp
addTask(task)
```

Recurring inserted again.

Queue

Task1 ->2

Recurring ->4

Task2 ->5

Worker loops again.

Top

Task1

Wait until

2 sec

Execute.

Queue

Recurring ->4

Task2 ->5

Then waits till

4 sec

Runs recurring again.

Reinsert

Recurring ->7

Task2 ->5

Queue

Task2 ->5

Recurring ->7

Runs Task2

Runs recurring

Runs recurring...

until

Main thread sleeps

```cpp
sleep_for(12 sec)
```

finishes.

Then

```cpp
scheduler.stop();
```

Calls

```cpp
running=false;
```

Worker exits loop after waking.

Then

```cpp
join()
```

Main thread waits until worker finishes.

Program exits.