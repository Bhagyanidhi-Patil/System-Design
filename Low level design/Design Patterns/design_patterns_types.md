# Design Patterns Overview

A design pattern is simply a proven solution to a recurring software design problem. Experienced software engineers noticed that the same kinds of design problems kept appearing in different projects, so they documented reusable solutions. These documented solutions became design patterns.

## Key thing to remember

- ❌ A design pattern is not an algorithm.
- ❌ A design pattern is not a library or framework.
- ✅ It is a design solution to a recurring software problem.

So whenever you learn a design pattern, ask yourself:

`"What problem does this pattern solve?"`

Design patterns are generally divided into 3 categories based on what problem they solve:

- **Creational** → How do I create objects?
- **Structural** → How do I connect objects?
- **Behavioral** → How do objects communicate with each other?

## 1. Creational Design Patterns 🏗️

### Question they solve

How should I create objects?

Instead of creating objects directly using `new`, these patterns provide a smarter way to create them.

### Real-life analogy

Imagine you're buying a car.

Instead of building a car yourself, you go to a car factory.

The factory decides:

- which parts to use
- how to assemble it
- which model to give

You simply ask for a car.

The factory hides the creation complexity.

Exactly what Creational patterns do.

### Example

Without pattern:

```cpp
Car* car = new BMW();
```

You decide everything.

With Factory Pattern:

```cpp
Car* car = CarFactory::create("BMW");
```

Factory decides how to create it.

### Purpose

- Hide object creation
- Make code flexible
- Reduce dependency on concrete classes

### Creational Patterns

- Singleton
- Factory Method
- Abstract Factory
- Builder
- Prototype

## 2. Structural Design Patterns 🧩

### Question they solve

How do I connect different classes or objects together?

Sometimes classes don't naturally fit together.

Structural patterns organize them into larger structures.

### Real-life analogy

Imagine you're assembling a computer.

You have:

- CPU
- Monitor
- Keyboard
- Mouse

They are independent components.

Structural patterns help connect them into one working computer.

### Purpose

- Combine objects
- Reuse existing code
- Simplify relationships
- Build larger structures

### Structural Patterns

- Adapter
- Bridge
- Composite
- Decorator
- Facade
- Flyweight
- Proxy

## 3. Behavioral Design Patterns 🤝

### Question they solve

How should objects communicate with each other?

These patterns focus on behavior, responsibility, and communication.

### Real-life analogy

Imagine a restaurant.

Customer

↓

Waiter

↓

Chef

Customer never directly talks to chef.

Waiter passes messages both ways.

Behavioral patterns define how information flows.

### Another example

You subscribe to a YouTube channel.

Whenever a new video comes, you receive a notification automatically.

That is the Observer Pattern.

### Example

```text
Weather Station
    |
Temperature changes
    |
Notify all displays
```

Displays automatically update.

### Purpose

- Define communication
- Reduce coupling
- Assign responsibilities
- Make behavior flexible

### Behavioral Patterns

- Observer
- Strategy
- Command
- State
- Chain of Responsibility
- Mediator
- Iterator
- Memento
- Template Method
- Visitor
- Interpreter

## Easy Way to Remember

### One Simple Scenario

Imagine you're building a Food Delivery App.

#### Step 1: Create objects (Creational)

Customer places order.

Instead of:

```cpp
new Pizza()
```

Use:

```cpp
FoodFactory::createFood("Pizza")
```

Factory creates the correct food object.

#### Step 2: Connect objects (Structural)

You have:

- Payment Service
- Notification Service
- Delivery Service

A Facade can provide one simple interface:

```cpp
orderFood();
```

Internally it coordinates all services.

#### Step 3: Communicate (Behavioral)

Once the order status changes:

```text
Preparing
    ↓
Ready
    ↓
Out for Delivery
    ↓
Delivered
```

The app automatically notifies:

- Customer
- Delivery Partner
- Restaurant

This is an Observer Pattern, where one change triggers updates to all interested parties.

## Additional Examples

In software, each design pattern is a tool for a specific design problem.

### Example 1: Too many objects being created

**Problem**

Suppose your application should have only one database connection manager.

Without any pattern:

```cpp
Database* db1 = new Database();
Database* db2 = new Database();
Database* db3 = new Database();
```

Now you have three instances, which may waste resources or cause inconsistencies.

**Solution**

Use the Singleton Pattern.

It guarantees that only one instance exists.

**Problem solved:** "I need exactly one object."

### Example 2: Object creation logic is getting messy

**Problem**

Suppose you're creating different notifications.

```cpp
if(type == "Email")
    new EmailNotification();
else if(type == "SMS")
    new SMSNotification();
else if(type == "Push")
    new PushNotification();
```

As more notification types are added, this code becomes harder to maintain.

**Solution**

Use the Factory Pattern.

```cpp
Notification* notification = NotificationFactory::create(type);
```

The factory decides which object to create.

**Problem solved:** "Object creation is becoming complicated."

### Example 3: Two classes don't work together

**Problem**

Your application expects a payment interface:

```cpp
pay(amount);
```

But a third-party payment library provides:

```cpp
makePayment(value);
```

The interfaces don't match.

**Solution**

Use the Adapter Pattern.

The adapter converts one interface into the other.

**Problem solved:** "Existing classes are incompatible."

### Example 4: Many objects need updates

**Problem**

Suppose a stock price changes.

You need to notify:

- Mobile app
- Website
- Dashboard
- Email service

Without a pattern:

```text
Stock
   |
   |----call mobile
   |----call website
   |----call dashboard
   |----call email
```

Every time you add a new subscriber, you must modify the stock class.

**Solution**

Use the Observer Pattern.

Observers subscribe once.

Whenever the stock changes, everyone is notified automatically.

**Problem solved:** "One object needs to notify many others."