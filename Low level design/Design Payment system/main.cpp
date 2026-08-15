#include <iostream>
using namespace std;

class PaymentStrategy{
public:
    virtual void pay(double amount)=0;
    virtual ~PaymentStrategy(){}
};

class CreditCardPayment:public PaymentStrategy{
public:
    void pay(double amount) override{
        cout<<"Paid "<<amount<<" using credit card"<<endl;
    }
};

class UPIPayment:public PaymentStrategy{
public:
    void pay(double amount) override{
        cout<<"Paid "<<amount<<" using UPI"<<endl;
    }
};

class PayPalPayment:public PaymentStrategy{
public:
    void pay(double amount) override{
        cout<<"Paid "<<amount<<" using PayPal"<<endl;
    }
};

class NullPayment : public PaymentStrategy {
public:
    void pay(double amount) override {
        cout << "Invalid payment method" << endl;
    }
};

enum PaymentType{
    CREDIT_CARD,
    UPI,
    PAYPAL
};

class PaymentFactory{
public:
    static PaymentStrategy* createPayment(PaymentType type){
        switch(type){
            case CREDIT_CARD:
                return new CreditCardPayment();
            case UPI:
                return new UPIPayment();
            case PAYPAL:
                return new PayPalPayment();
            default:
                return new NullPayment();
        }
    }
};

class Payment{
private:
    PaymentStrategy *strategy;
public:
    Payment(){
        strategy = new NullPayment();
    }
    void setStratergy(PaymentStrategy* newstrategy){
        delete strategy;                        //since, in main we use same payment object, delete old strategy , else it will lead to memory leak
        strategy = newstrategy;                 // if you are using seperate payment object then delete is not required here
    }
    void makePayment(double amount){
        strategy->pay(amount);
    }
    ~Payment() {
        delete strategy;
    }
};

int main(){
    PaymentStrategy* creditcardpay = PaymentFactory::createPayment(CREDIT_CARD);
    Payment payment;
    payment.setStratergy(creditcardpay);
    payment.makePayment(1000);

    PaymentStrategy* upiPay = PaymentFactory::createPayment(UPI);
    payment.setStratergy(upiPay);
    payment.makePayment(500);

    return 0;
}

/*
Initially:
Payment payment;

The constructor creates:
strategy = new NullPayment();

Then:
payment.setStratergy(creditcardpay);

Your setter only does:
strategy = newstrategy;

So the original NullPayment is lost:
NullPayment  ← leaked ❌


Payment
   |
   └──→ CreditCardPayment

Then you do:
payment.setStratergy(upiPay);

Now the CreditCardPayment pointer is overwritten too:

NullPayment       ← leaked
CreditCardPayment ← leaked
Payment
   |
   └──→ UPIPayment

Finally, the destructor only deletes the current UPIPayment.

If you want to keep your current raw-pointer approach

Then setStratergy() must delete the old strategy before replacing it:

void setStrategy(PaymentStrategy* newStrategy) {
    delete strategy;
    strategy = newStrategy;
}
*/