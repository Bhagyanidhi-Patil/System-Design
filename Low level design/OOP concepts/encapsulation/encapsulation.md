# Encapsulation

Encapsulation is the process of bundling data members and member functions into a single unit (class) and hiding the data from direct access by making it private. The data can then be accessed or modified only through public methods such as getters and setters.

## Real-Life Example: ATM Machine 🏧

When you withdraw money from an ATM:

- You enter your PIN.
- You enter the amount.
- You receive cash.

You don't know:

- How the ATM verifies your PIN.
- How it communicates with the bank.
- How it updates your account balance.

The ATM hides all the internal implementation and only exposes the operations you need.

This is encapsulation.

## Why do we use Encapsulation?

### 1. Data Hiding

Users cannot access sensitive variables directly.

```cpp
private:
    int salary;
```

Only approved methods can access it.

### 2. Better Security

You can validate data before storing it.

```cpp
void setAge(int age) {
    if (age > 0)
        this->age = age;
}
```

Invalid values are rejected.

### 3. Easier Maintenance

Suppose today:

```cpp
balance += amount;
```

Tomorrow you change it to:

```cpp
balance = balance + amount - tax;
```

No outside code changes, because everyone still calls:

```cpp
deposit(amount);
```

### 4. Prevents Invalid Data

Without encapsulation:

```cpp
balance = -100;
```

With encapsulation:

```cpp
deposit(-100); // ignored
```