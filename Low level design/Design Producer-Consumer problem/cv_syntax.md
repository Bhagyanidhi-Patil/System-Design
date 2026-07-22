# Understanding `cv.wait(lock, []() { ... })`

The general syntax is:

```cpp
cv.wait(lock, []() {
    return condition;
});
```

Let's break it down piece by piece.

---

## `cv.wait(...)`

```cpp
cv.wait(...);
```

This tells the current thread:

> **"Sleep until the given condition becomes true."**

The thread remains blocked while the condition is `false` and resumes execution once the condition becomes `true`.

---

## `lock`

```cpp
cv.wait(lock, ...);
```

Here, `lock` is a `std::unique_lock<std::mutex>`.

```cpp
std::unique_lock<std::mutex> lock(mtx);
```

The `wait()` function uses this lock to:

1. Unlock the mutex before putting the thread to sleep.
2. Lock the mutex again after the thread wakes up.

This allows other threads to acquire the mutex and modify the shared data while the current thread is waiting.

---

## `[]()`

This is a **lambda function**.

### General Lambda Syntax

```cpp
[capture](parameters) -> return_type
{
    // body
};
```

In our case:

```cpp
[]() {
    return condition;
}
```

Let's understand each part.

---

### `[]` — Capture List

The **capture list** specifies which variables from the surrounding scope the lambda can access.

Examples:

```cpp
[]          // Capture nothing
```

```cpp
[&]         // Capture all local variables by reference
```

```cpp
[=]         // Capture all local variables by value
```

```cpp
[&q]        // Capture only 'q' by reference
```

---

### `()` — Parameters

These are the lambda's parameters.

Since there are no parameters:

```cpp
[]()
```

it's equivalent to a function like:

```cpp
bool check()
{
    ...
}
```

If the lambda had a parameter:

```cpp
[](int x)
{
    return x > 5;
}
```

it would be similar to:

```cpp
bool check(int x)
{
    return x > 5;
}
```

---

### `{}` — Function Body

The code inside `{}` is the body of the lambda.

Example:

```cpp
[]() {
    return ready;
}
```

Equivalent to writing:

```cpp
bool check()
{
    return ready;
}
```

The lambda simply evaluates the condition and returns either `true` or `false`.

When used with `cv.wait()`, the thread continues only when this function returns `true`.
