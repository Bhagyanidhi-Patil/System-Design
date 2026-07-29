# Run-Time Polymorphism

Run-time polymorphism is a type of polymorphism where the function to be executed is determined during program execution (runtime) based on the actual object. It is also called dynamic binding or late binding.

## Achieved using

- **Virtual Functions** — A virtual function is a member function declared with the `virtual` keyword in the base class. It allows the derived class's overridden function to be called through a base class pointer or reference, enabling run-time polymorphism.
- **Function Overriding** — Function overriding is the process of redefining a base class's virtual function in the derived class with the same function signature to provide a specific implementation.

## Example: Animal Sounds

Suppose we have different animals. Every animal can make a sound.

```text
Dog   → Barks
Cat   → Meows
Cow   → Moos
```

```cpp
#include <iostream>
using namespace std;

class Animal {
public:
    virtual void sound() {
        cout << "Animal Sound\n";
    }
};

class Dog : public Animal {
public:
    void sound() override {
        cout << "Dog Barks\n";
    }
};

class Cat : public Animal {
public:
    void sound() override {
        cout << "Cat Meows\n";
    }
};

int main() {
    Animal* a;
    Dog d;
    Cat c;

    a = &d;
    a->sound();

    a = &c;
    a->sound();

    return 0;
}
```

**Output:**

```text
Dog Barks
Cat Meows
```

### What happened?

The pointer is:

```cpp
Animal* a;
```

But at runtime, it points to different objects:

- Sometimes `Dog`
- Sometimes `Cat`

The same function `sound()` behaves differently. This is run-time polymorphism.

## Why is `virtual` needed?

Without `virtual`:

```cpp
Animal* a = &d;
a->sound();
```

Output:

```text
Animal Sound
```

Because the compiler calls the base class version.

With `virtual`:

Output:

```text
Dog Barks
```

The function is chosen at runtime based on the actual object.

## How does the compiler implement it?

- A **VTable** is a hidden table created for every class that has at least one virtual function. 
- Even though `Dog` does not explicitly declare a virtual function, it inherits the virtual function from `Animal`. So, Dog has VTable created.

### Easy rule to remember

A VTable is created for every polymorphic class.

A polymorphic class is a class that:

- declares at least one virtual function, or
- inherits at least one virtual function.

### Example VTables

**For `Animal`**

```text
Animal VTable
-----------------------
sound() --> Animal::sound()
```

**For `Dog`**

```text
Dog VTable
-----------------------
sound() --> Dog::sound()
```

It also creates a hidden pointer called **VPTR**.

Every object of a class containing virtual functions gets a hidden pointer called `VPTR` (virtual pointer).

```text
Dog Object
------------------------
VPTR  ------------+
                  |
                  v
           Dog VTable
```

## Complete flow

```cpp
Animal* a = new Dog();
a->sound();
```

1. Create `Dog` object.

```text
Dog Object
-----------------------
VPTR
-----------------------
```

2. `VPTR` points to `Dog`'s VTable.

```text
Dog Object

VPTR
 |
 |
 V

Dog VTable

sound() --> Dog::sound()
```

3. When `a->sound()` is executed, the compiler does **not** directly call `Animal::sound()`.

Instead, it uses the `VPTR` to look up the correct function in the VTable.

### Visual diagram

```text
Animal* a

      │
      ▼
 +------------------+
 |   Dog Object     |
 |------------------|
 | VPTR ------------|--------------------+
 +------------------+                    |
                                         ▼
                              +--------------------+
                              |   Dog VTable       |
                              |--------------------|
                              | sound() ---------->| Dog::sound()
                              +--------------------+
```