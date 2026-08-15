# Payment System — Requirements

## Problem Statement

Design a simple Payment System that allows a user to make payments using different payment methods such as Credit Card, UPI, and PayPal. The system should be easy to extend when a new payment method is added.

## Functional Requirements

### Must have

- User should be able to make a payment.
- System should support multiple payment methods:
      - Credit Card
      - UPI
      - PayPal
- User should be able to choose the payment method.
- System should process the payment using the selected method.
- Adding a new payment method should require minimal changes to existing code.

## Non-Functional Requirements

- **Extensible** — easy to add new payment methods.
- **Maintainable** — payment-specific logic should be separated.
- **Loosely coupled** — `Payment` should not depend directly on `CreditCard`/`UPI`/`PayPal` implementations.
- Keep the design simple for an LLD interview (no distributed systems, persistent storage, or concurrency required).

## Relationship

Text diagram:

```
Payment
      └─ has-a
             PaymentStrategy
                  ├─ CreditCard
                  ├─ UPI
                  └─ PayPal

PaymentFactory
      └─ creates PaymentStrategy
```

---

