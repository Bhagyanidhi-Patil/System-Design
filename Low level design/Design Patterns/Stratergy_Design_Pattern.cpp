/*

The Strategy Design Pattern is a behavioral design pattern. 
This pattern is useful when multiple derived classes share the same behavior, 
avoiding code duplication by defining the behavior separately in a set of strategy classes. 
To address this, the Strategy Pattern introduces an interface (or abstract class) that defines 
a common behavior. 
Multiple concrete implementations of this interface are then created, each representing a 
different strategy. 

Strategy Pattern is used when you have multiple ways (algorithms) to do something, 
and you want to switch between them at runtime.

                            UML-Diagram 
                            
                +-----------------------------+
                |   PaymentStratergy          |  <<interface>>
                +-----------------------------+
                | + payment(amount:int)       |
                +-------------+---------------+
                              ^
        ---------------------------------------------
        |                    |                      |
+------------------+  +------------------+  +------------------+
|   CreditCard     |  |       UPI        |  |    DebitCard     |
+------------------+  +------------------+  +------------------+
| + payment(int)   |  | + payment(int)   |  | + payment(int)   |
+------------------+  +------------------+  +------------------+

                     (has-a / uses)
                           |
                           ▼
                +-----------------------------+
                |     PaymentContext          |
                +-----------------------------+
                | - stratergy: PaymentStratergy* |
                +-----------------------------+
                | + setStratergy(s)           |
                | + payamount(amount:int)     |
                +-----------------------------+
*/


#include <iostream>
using namespace std;

//Strategy Interface
class PaymentStratergy{
public:
    virtual void payment(int amount)=0;
};

//Concrete Strategies (Common rule)
class CreditCard:public PaymentStratergy{
 public:
    void payment(int amount) override{
        cout<<"Amount "<<amount<<" paid using Credit card."<<endl;
    }
};
class UPI:public PaymentStratergy{
 public:
    void payment(int amount) override{
        cout<<"Amount "<<amount<<" paid using UPI."<<endl;
    }
};
class DebitCard:public PaymentStratergy{
 public:
    void payment(int amount) override{
        cout<<"Amount "<<amount<<" paid using Debit card."<<endl;
    }
};

//Context Class (Main Controller)
class PaymentContext{
private:
    PaymentStratergy *stratergy;
public:
    void setStratergy(PaymentStratergy* s){
        stratergy =s;
    }
    void payamount(int amount){
        stratergy->payment(amount);
    }
};

//Client code
int main() {
    PaymentContext context;
    CreditCard cc;
    UPI upi;
    context.setStratergy(&cc);
    context.payamount(1000);
    context.setStratergy(&upi);
    context.payamount(5000);

    return 0;
}

or

//Client code
int main() {
    PaymentContext context;

    context.setStratergy(new CreditCard());
    context.payamount(1000);
    context.setStratergy(new UPI());
    context.payamount(5000);

    return 0;
}