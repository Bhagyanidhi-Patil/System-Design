/*
The Adapter Design Pattern is a structural design pattern that allows two incompatible interfaces to work together. 
It acts as a bridge between two classes or systems by converting the interface of a class into another interface that a client expects.

🏗️ Structure
There are 3 main components:
    Target – the interface your code expects
    Adaptee – the existing class with a different interface
    Adapter – converts the Adaptee interface into the Target interface

        UML-Diagram
        
        <<interface>>
        PaymentProcessor
        -----------------
        | + pay(amount) |
        -----------------
              ▲
              │
              │ (implements)
        ---------------------
        |   PaymentAdapter  |
        ---------------------
        | - oldGateway      |
        ---------------------
        | + pay(amount)     |
        ---------------------
              │
              │ (has-a / composition)
              ▼
        -------------------------
        |  OldPaymentGateway    |
        -------------------------
        | + makePayment(amount) |
        -------------------------
*/

#include<iostream>
using namespace std;

//Target Interface
class PaymentProcess{
public:
    virtual void pay(double amount)=0;
    virtual ~PaymentProcess(){}
};

//Adaptee
class OldPaymentGateway{
public:
    void makePayment(double amount){
        cout<<"Processing payment "<<amount<<" using OLD Gateway"<<endl;
    }
};

//Adapter (Bridge between new & old)
class PaymentAdapter:public PaymentProcess{
    OldPaymentGateway* oldgateway;
public:
    PaymentAdapter(OldPaymentGateway* gateway):oldgateway(gateway){}

    void pay(double amount)override{
        oldgateway->makePayment(amount);
    }
};

//Client code
int main() {
    OldPaymentGateway* legacySystem = new OldPaymentGateway();

    PaymentProcess* processor = new PaymentAdapter(legacySystem);

    processor->pay(250.75);

    delete processor;
    delete legacySystem;
    return 0;
}