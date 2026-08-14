#include<iostream>
#include<memory>
#include <string>
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

class NotificationFactory {
public:
    static INotification* createNotification(NotificationType type) {
        switch (type) {
            case NotificationType::EMAIL:
                return new EmailNotification();

            case NotificationType::SMS:
                return new SMSNotification();

            case NotificationType::PUSH:
                return new PushNotification();

            default:
                return nullptr;
        }
    }
};

class NotificationService {
public:

    void notify(NotificationType type,
                string receiver,
                string message)
    {
        INotification* notifier =
            NotificationFactory::createNotification(type);

        if(notifier == nullptr)
        {
            cout << "Invalid Notification Type\n";
            return;
        }

        notifier->sendMessage(receiver, message);

        delete notifier;
    }
};

/*
class NotificationService {
private:
    INotification* notifier;

public:
    NotificationService(INotification* notifier) {
        this->notifier = notifier;
    }

    void notify(string receiver, string message) {
        if (notifier == nullptr) {
            cout << "No notification selected\n";
            return;
        }

        notifier->sendMessage(receiver, message);
    }
};
*/
int main()
{
    NotificationService service;

    service.notify(NotificationType::EMAIL,
                   "abc@gmail.com",
                   "Interview tomorrow");

    cout<<endl;

    service.notify(NotificationType::SMS,
                   "9876543210",
                   "OTP is 5678");

    cout<<endl;

    service.notify(NotificationType::PUSH,
                   "Bhoomika",
                   "Your order is delivered");

    return 0;
}