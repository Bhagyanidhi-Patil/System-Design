# Design Stopwatch in C++

## 1. Requirements:

### Functional Requirements

The stopwatch should support:

#### 1. `start()`
Starts the stopwatch.

#### 2. `pause()`
Temporarily pauses timing.

#### 3. `resume()`
Continues timing from paused state.

#### 4. `stop()`
Stops the stopwatch completely.

#### 5. `reset()`
Resets elapsed time to zero.

#### 6. `elapsedTime()`
Returns elapsed time.

---

## 2. Do We Need a Thread?

No. A stopwatch does NOT need a background thread.

### Why?

Because stopwatch only measures:

```cpp
current_time - start_time
```

---

## 3. Which Clock Should We Use?

Use:

```cpp
std::chrono::steady_clock
```

NOT:

```cpp
system_clock
```

---

## 4. system clock vs stead clock

### 1. `system_clock`

Represents the real-world wall clock time.

#### Example:

Current date/time on your computer

```text
10 May 2026, 3:45 PM
```

### Properties

Can be adjusted by:

- user changing system time
- NTP/internet time sync
- daylight saving changes

So time can:

- jump forward
- jump backward

```cpp
#include <iostream>
#include <chrono>

int main() {
    auto now = std::chrono::system_clock::now();    //Gets the current wall-clock time.

    std::time_t t = std::chrono::system_clock::to_time_t(now);   //Converts chrono time into old C-style time_t.

    std::cout << std::ctime(&t);      //Converts time_t into readable string.
}
```

```text
Sun May 10 15:45:12 2026
```

---

### 2. `steady_clock`

Represents a monotonic clock.

### Meaning:

- time always moves forward steadily
- never jumps backward
- not affected by system time changes

### Properties:

- Constant rate
- Monotonic
- Best for measuring elapsed time

```cpp
#include <iostream>
#include <chrono>
#include <thread>

int main() {
    auto start = std::chrono::steady_clock::now();

    std::this_thread::sleep_for(std::chrono::seconds(2));

    auto end = std::chrono::steady_clock::now();

    auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << diff.count() << " ms\n";
}
```

---

## 5. State Diagram

Initially

```text
running = false
elapsed = 0
```

After `start()`

```text
running = true
startTime = now
```

After `pause()`

Add current session duration to total elapsed.

```cpp
elapsed += now - startTime
running = false
```

After `resume()`

Start counting again from current time.

```cpp
startTime = now
running = true
```