# SOLID Principles

## S - Single Responsibility Principle (SRP)

- A class should have only one reason to change.
- **Example**: class `InvoicePrinter` prints invoices; class `InvoiceCalculator` calculates totals.

## O - Open/Closed Principle (OCP)

- Software entities should be open for extension, closed for modification.
- **Example**: `Shape` interface with `draw()`; concrete `Circle`, `Square` extend behavior.

## L - Liskov Substitution Principle (LSP)

- Derived classes must be usable anywhere base class is expected.
- **Example**: `Bird` and `FlyingBird` split; `Penguin` does not inherit `FlyingBird`.

## I - Interface Segregation Principle (ISP)

- Many specific interfaces are better than one fat interface.
- **Example**: `IPrinter` and `IScanner` instead of `IMultiFunctionDevice` with unused methods.

## D - Dependency Inversion Principle (DIP)

- High-level modules should not depend on low-level modules; both should depend on abstractions.
- **Example**: `PaymentProcessor` depends on `IPaymentGateway`, not `PaypalGateway` directly.

---

## C++ Example

Simple SOLID demo: payment processing

```cpp
#include <iostream>
#include <memory>

// SRP + ISP: payment gateway interface handles authorization only
class IPaymentGateway {
public:
    virtual ~IPaymentGateway() = default;
    virtual bool Authorize(double amount) const = 0;
};

class StripeGateway : public IPaymentGateway {
public:
    bool Authorize(double amount) const override {
        std::cout << "Stripe: authorizing " << amount << "\n";
        return amount < 1000; // simple policy
    }
};

class PaypalGateway : public IPaymentGateway {
public:
    bool Authorize(double amount) const override {
        std::cout << "PayPal: authorizing " << amount << "\n";
        return amount < 2000;
    }
};

// SRP: PaymentProcessor handles workflow, not gateway details
class PaymentProcessor {
    std::shared_ptr<IPaymentGateway> gateway;
public:
    explicit PaymentProcessor(std::shared_ptr<IPaymentGateway> gateway) : gateway(std::move(gateway)) {}

    bool ProcessPayment(double amount) const {
        if (!gateway->Authorize(amount)) {
            std::cout << "Payment authorization failed\n";
            return false;
        }
        std::cout << "Payment successful: " << amount << "\n";
        return true;
    }
};

int main() {
    std::shared_ptr<IPaymentGateway> gateway = std::make_shared<StripeGateway>();
    PaymentProcessor processor(gateway);

    processor.ProcessPayment(1500); // fails on stripe

    processor = PaymentProcessor(std::make_shared<PaypalGateway>());
    processor.ProcessPayment(1500); // succeeds on PayPal

    return 0;
}
```

## SOLID Mapping

- **SRP**: `IPaymentGateway` authorizes; `PaymentProcessor` orchestrates processing.
- **OCP**: add new gateway classes (e.g., `SquareGateway`) without edits to `PaymentProcessor`.
- **LSP**: any `IPaymentGateway` derived type can be used by `PaymentProcessor`.
- **ISP**: `IPaymentGateway` has a focused single method.
- **DIP**: `PaymentProcessor` depends on abstraction `IPaymentGateway`.
