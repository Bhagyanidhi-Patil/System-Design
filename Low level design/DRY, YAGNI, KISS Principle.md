# DRY, YAGNI, KISS Principles

## DRY - Don't Repeat Yourself

- Avoid duplicate code; extract common logic into a single place.
- **Benefits**: easier maintenance, fewer bugs.

## YAGNI - You Aren't Gonna Need It

- Don't implement features until they are actually required.
- **Benefits**: faster delivery, less complexity.

## KISS - Keep It Simple, Stupid

- Prefer simple, straightforward solutions over clever/complex ones.
- **Benefits**: easier to read, test, and maintain.

---

## C++ Example

Simple demonstration with sum validation:

```cpp
#include <iostream>
using namespace std;

// DRY: shared validation function
bool IsValid(int x) {
    return x > 0;
}

// KISS: simple add function
int Add(int a, int b) {
    return a + b;
}

int main() {
    int x = 5;
    int y = 3;

    if (!IsValid(x) || !IsValid(y)) {
        std::cout << "Invalid input\n";
        return 1;
    }

    // YAGNI: no complex features yet
    std::cout << "Sum: " << Add(x, y) << "\n";
    return 0;
}
```
