#include<iostream>
#include<string>
#include<memory>
#include <functional>
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

// NULL object, used null design pattern 
class NullNotification : public INotification {
public:
    void sendMessage(string receiver, string message) override {
        cout << "No notification selected" << endl;
    }
};

enum NotificationType{
    EMAIL,
    PUSH,
    SMS
};

/*
Here, instead of enum if you use enum class NotifcationType,
then in switch you can use case Notifcationtype::EMAIL, case SMS insated of case EMAIL.
*/

class NotificationFactory{
public:
    static INotification* createNotification(NotificationType type){
        switch(type)
        {
            case SMS:
                return new SMSNotification();
            case EMAIL:
                return new EmailNotification();
            case PUSH:
                return new PushNotification();
            default:
                return NULL;
        }
    }
};

class IObserver{
public:
    virtual void update(string receiver,string message);
    virtual ~IObserver(){}
};

class EmailObserver:public IObserver{
public:
    void update(string receiver,string message)override{
        INotification* n = NotificationFactory::createNotification(EMAIL);
        n->sendMessage(receiver,message);
        delete n;
    }
};

class SMSObserver:public IObserver{
public:
    void update(string receiver,string message)override{
        INotification* n = NotificationFactory::createNotification(SMS);
        n->sendMessage(receiver,message);
        delete n;
    }
};

class PushObserver:public IObserver{
public:
    void update(string receiver,string message)override{
        INotification* n = NotificationFactory::createNotification(PUSH);
        n->sendMessage(receiver,message);
        delete n;
    }
};

class NotificationEvent{
private:
    vector<IObserver*>observers;
public:
    void subscriber(IObserver* observer){
        observers.push_back(observer);
    }
    void notifyAll(string receiver,string message){
        for(IObserver* observer:observers){
            observer->update(receiver,message);
        }
    }
};
int main() {
    NotificationEvent event;

    EmailObserver email;
    SMSObserver sms;
    PushObserver push;

    event.subscriber(&email);
    event.subscriber(&sms);
    event.subscriber(&push);

    cout << "Order shipped!" << endl;

    event.notifyAll(
        "customer@example.com",
        "Your order has been shipped!"
    );

    return 0;
}