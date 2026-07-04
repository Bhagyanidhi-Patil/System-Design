#include<iostream>
#include<string>
#include<memory>
using namespace std;

class INotification{
public:
    virtual void sendMessage(string recevier,string message)=0;
    virtual ~INotification(){}
};

class EmailNotification:public INotification{
public:
    void sendMessage(string recevier,string message)override{
        cout<<"Email sent to "<<recevier<<", message: "<<message<<endl;
    }
};

class SMSNotification:public INotification{
public:
    void sendMessage(string recevier,string message)override{
        cout<<"SMS sent to "<<recevier<<", message: "<<message<<endl;
    }
};

class PushNotification:public INotification{
public:
    void sendMessage(string recevier,string message)override{
        cout<<"Push notification sent to "<<recevier<<", message: "<<message<<endl;
    }
};

enum class NotificationType{
    EMAIL,
    PUSH,
    SMS
};

// class NotificationFactory{
// public:
//     static INotification* createNotification(NotificationType type){
//         switch(type)
//         {
//             case NotificationType::SMS:
//                 return new SMSNotification();
//             case NotificationType::EMAIL:
//                 return new EmailNotification();
//             case NotificationType::PUSH:
//                 return new PushNotification();
//             default:
//                 return NULL;
//         }
//     }
// };

// class NotificationService{
// private:
//     INotification* statergy;
// public:
//     NotificationService(INotification* statergy=nullptr){
//         this->statergy = statergy;
//     }
//     void setStatergy(INotification* statergy){
//         this->statergy = statergy;
//     }
//     void notify(string recevier,string message){
//         if(statergy==nullptr){
//             cout << "No notification strategy selected.\n";
//             return;
//         }
//         statergy->sendMessage(recevier,message);
//     }
// };

// int main(){
//     INotification* notifier = NotificationFactory::createNotification(NotificationType::EMAIL);
//     NotificationService service(notifier);
//     service.notify("abc@gmail.com","Interview tommorow");
//     delete notifier;
//     cout<<endl;

//     notifier = NotificationFactory::createNotification(NotificationType::SMS);
//     service.setStatergy(notifier);
//     service.notify("9902334452","OTP is 8890");
//     delete notifier;
//     cout<<endl;

//     notifier = NotificationFactory::createNotification(NotificationType::PUSH);
//     service.setStatergy(notifier);
//     service.notify("XYZ","Your order is shipped!");
//     delete notifier;
//     cout<<endl;

//     return 0;
// }


class NotificationFactory {
public:
    static unique_ptr<INotification> createNotification(NotificationType type)
    {
        switch(type)
        {
            case NotificationType::EMAIL:
                return make_unique<EmailNotification>();

            case NotificationType::SMS:
                return make_unique<SMSNotification>();

            case NotificationType::PUSH:
                return make_unique<PushNotification>();

            default:
                return nullptr;
        }
    }
};

class NotificationService {
    unique_ptr<INotification> strategy;

public:
    NotificationService(unique_ptr<INotification> s): strategy(move(s)) {}

    void notify(string receiver, string message) {
        strategy->sendMessage(receiver, message);
    }

    void setStrategy(unique_ptr<INotification> s) {
        strategy = move(s);
    }
};


int main()
{
    auto emailNotifier =
        NotificationFactory::createNotification(NotificationType::EMAIL);

    NotificationService service(move(emailNotifier));

    service.notify("abc@gmail.com",
                   "Interview tomorrow");

    cout << endl;

    auto smsNotifier =
        NotificationFactory::createNotification(NotificationType::SMS);

    service.setStrategy(move(smsNotifier));

    service.notify("9876543210",
                   "OTP is 5678");

    cout << endl;

    auto pushNotifier =
        NotificationFactory::createNotification(NotificationType::PUSH);

    service.setStrategy(move(pushNotifier));

    service.notify("xyz",
                   "Your order is delivered");

    return 0;
}

/*
std::move is one of the most important concepts in modern C++.

Simple Definition
std::move transfers ownership of an object instead of copying it.
---
Example without std::move
string s1 = "Hello";
string s2 = s1;

Memory:
s1 -----> "Hello"
s2 -----> "Hello"

This is a copy.

Both objects exist independently.
---

Example with std::move
string s1 = "Hello";
string s2 = std::move(s1);

Memory:
Before move

s1 -----> "Hello"

After move
s1 -----> ""
s2 -----> "Hello"

Ownership of the internal buffer is transferred.

Now s1 is still valid but its value is unspecified (typically empty).
---

Why do we need std::move?

Suppose you have
unique_ptr<int> p1 = make_unique<int>(10);

Now try
unique_ptr<int> p2 = p1;
❌ Compile Error

Why?
Because then both pointers would own the same memory.

When they are destroyed,
delete memory
delete memory

Double delete → crash.

So unique_ptr cannot be copied.

Instead,
unique_ptr<int> p2 = std::move(p1);

Now

Before
p1 -----> 10

After
p1 -----> nullptr

p2 -----> 10

Ownership has moved.
---

In your notification system

Factory returns
unique_ptr<INotification>

For example
auto email =
NotificationFactory::createNotification(NotificationType::EMAIL);

Memory
email -----> EmailNotification

Now
NotificationService service(email);
❌ Error

Because NotificationService wants to own that object.
Only one owner is allowed.

So we write
NotificationService service(std::move(email));

Now
Before
email --------> EmailNotification

After
email --------> nullptr
service.strategy -----> EmailNotification

The ownership has moved.
*/