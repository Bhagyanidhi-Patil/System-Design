# Access Specifiers (Inside a Class)

These decide who can access the members of a class.

| Access Specifier | Inside Class | Derived Class | Outside Class |
|------------------|--------------|---------------|---------------|
| `public`         | ✅           | ✅            | ✅            |
| `protected`      | ✅           | ✅            | ❌            |
| `private`        | ✅           | ❌            | ❌            |

## Inheritance Modes

When inheriting, we write:

- `class Car : public Vehicle`
- `class Car : protected Vehicle`
- `class Car : private Vehicle`

These inheritance modes change how the parent's members appear inside the child class.

> **NOTE :**  When a **derived class** inherits from a **base class**, it can access the base class's `public` and `protected` members (but not `private` members), **irrespective of the type of inheritance**. The inheritance type (`public`, `protected`, or `private`) only **affects how other classes or objects can access the inherited members through the derived class**.

### Parent Class Example

```cpp
class Vehicle {
public:
    int speed;

protected:
    int fuel;

private:
    int engineNumber;
};
```

## Public Inheritance

```cpp
class Car : public Vehicle {
};
```

### Conversion

| Parent Member | Becomes in Child |
|---------------|------------------|
| `public`      | `public`         |
| `protected`   | `protected`      |
| `private`     | Not accessible   |
```
#include <iostream>
using namespace std;

class Vehicle {
public:
    int speed = 100;

protected:
    int fuel = 50;

private:
    int engineNumber = 12345;
};

class Car : public Vehicle {
public:
    void display() {
        cout << speed << endl;   // ✅ Accessible
        cout << fuel << endl;    // ✅ Accessible

        // cout << engineNumber; // ❌ Error
    }
};

int main() {

    Car c;

    cout << c.speed << endl;     // ✅

    // cout << c.fuel;           // ❌ Error (protected)

    // cout << c.engineNumber;   // ❌ Error (private)

    return 0;
}
```

## Protected Inheritance

```cpp
class Car : protected Vehicle {
};
```
### Conversion

| Parent Member | Becomes in Child |
|---------------|------------------|
| `public`      | `protected`      |
| `protected`   | `protected`      |
| `private`     | Not accessible   |
```cpp
class Car : protected Vehicle {
public:
    void display() {
        cout << speed;   // ✅ Accessible inside Car
        cout << fuel;    // ✅ Accessible inside Car
    }
};

int main() {
    Car c;
    // cout << c.speed;  // ❌ Error: speed becomes protected in Car
    c.display();
    return 0;
}
```

Although `speed` was public in `Vehicle`, it becomes `protected` in `Car`.



## Private Inheritance

```cpp
class Car : private Vehicle {
};
```

### Conversion

| Parent Member | Becomes in Child |
|---------------|------------------|
| `public`      | `private`        |
| `protected`   | `private`        |
| `private`     | Not accessible   |

### Example

```cpp
#include <iostream>
using namespace std;

class Vehicle {
public:
    int speed = 100;

protected:
    int fuel = 50;
};

class Car : private Vehicle {
public:
    void display() {
        cout << speed << endl;  // ✅
        cout << fuel << endl;   // ✅
    }
};

int main() {
    Car c;

    // c.speed;  // ❌
    // c.fuel;   // ❌
    c.display();
}
```

In private inheritance, both inherited `public` and `protected` members become `private` in the child class.

### Inside Car

- `Car` is a `derived class`, so it can access the inherited `public` and `protected` members of `Vehicle`.

With private inheritance, they become:

| Parent Member     | Inside Car |
|-------------------|------------|
| `public speed`    | `private`  |
| `protected fuel`  | `private`  |

