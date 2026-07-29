# Inheritance

Inheritance is an OOP concept where one class (child/derived class) acquires the properties and behaviors (data members and member functions) of another class (parent/base class).

It promotes code reusability because the child class can reuse the code written in the parent class.

## Example: Vehicle 🚗

Suppose we have a `Vehicle`.

Every vehicle has:

- Start
- Stop
- Fuel

Now a `Car` is also a `Vehicle`.

A `Bike` is also a `Vehicle`.

Instead of writing the same code again, they inherit it.

```
           Vehicle
          /       \
       Car       Bike
```

Both `Car` and `Bike` automatically get:

- `start()`
- `stop()`
- `fuel`

`Car` can additionally have:

- `openSunroof()`

`Bike` can additionally have:

- `kickStart()`
