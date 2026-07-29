# Polymorphism

Polymorphism means "one interface, many forms."

The same function or method behaves differently depending on the object that calls it.

- `Poly` = Many
- `Morphism` = Forms

So, polymorphism = many forms.

## Example 1: Person

A person behaves differently in different situations.

```text
Person
   ↓
At Home      → Son/Daughter
At Office    → Employee
With Friends → Friend
```

The same person performs different roles in different contexts. This is polymorphism.

## Example 2: Payment App

Suppose an app has a `Pay` button.

When you click `Pay`:

- `Credit Card` → Pays using credit card.
- `UPI` → Pays using UPI.
- `Net Banking` → Pays using net banking.

The function is the same:

```cpp
pay();
```

But the behavior changes depending on the payment method. This is polymorphism.V