# Compile-Time Polymorphism (Static Binding)

Compile-time polymorphism is a type of polymorphism where the function to be executed is determined by the compiler during compilation. It is also called static binding or early binding.

## Achieved using

- **Function Overloading** — Defining multiple functions with the same name but different parameter lists (different number, type, or order of parameters) within the same class. The compiler decides which function to call based on the arguments passed.
- **Operator Overloading** — Giving existing operators (such as `+`, `-`, `==`, etc.) a new meaning for user-defined objects by defining how they should behave.

## Function Overloading Example

Same function name, different parameters.

```cpp
#include <iostream>
using namespace std;

class Calculator {
public:
    int add(int a, int b) {
        return a + b;
    }

    int add(int a, int b, int c) {
        return a + b + c;
    }

    double add(double a, double b) {
        return a + b;
    }
};

int main() {
    Calculator c;

    cout << c.add(2, 3) << endl;
    cout << c.add(2, 3, 4) << endl;
    cout << c.add(2.5, 3.5) << endl;

    return 0;
}
```

**Output:**

```text
5
9
6
```

The compiler decides which `add()` to call during compilation.

Hence, compile-time polymorphism.

## Name Mangling (Function Mangling)

Name mangling is the process by which the C++ compiler generates a unique internal name for a function by encoding information such as the function name, parameter types, namespaces, and class names.

Consider function overloading:

```cpp
void print(int x);
void print(double x);
void print(string x);
```

All three functions have the same name: `print`.

If the compiler stored all of them simply as `print`, it would not know which one to call.

So the compiler creates different internal names.

### Example (compiler-specific)

```text
print(int)      → _Z5printi
print(double)   → _Z5printd
print(string)   → _Z5printNSt7stringE
```

> Note: The exact mangled names vary between compilers (GCC, Clang, MSVC, etc.), but the idea is the same.

