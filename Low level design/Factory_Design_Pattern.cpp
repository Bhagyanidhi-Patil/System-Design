/*
	• It is a "creation" design patterns. 
	• It is used to create a objects of similar type but very little interns of implementations. 
	• The goal of factory method is to hid the complexity of object creation. 
    • The client code which is using factory method pattern has no idea about how those objects are created.  

    Note: Here we can call factory method in two ways : 
    1. Factory::sendNotification("Email"); 
       in this case you have to make Factory class function as static because we are directly calling functon without creating factory object.

    2. Factory f;
       f.sendNotification("Email");  
       in this case no need to make Factory class function as static as we are creating factory object and calling function. 
*/

#include<iostream>
using namespace std;

//Base class
class Notification{
public:
    virtual void send() const =0;
    virtual ~Notification(){};
};

//Concrete classes
class EmailNotification:public Notification{
 public:
    void send() const override{
        cout<<"Sending Email notification"<<endl;
    }
};

class SMSNotification:public Notification{
public:
    void send() const override{
        cout<<"Sending SMS notification"<<endl;
    }
};

class PushNotification:public Notification{
 public:
    void send() const override{
        cout<<"Sending push notification"<<endl;
    }
};

// Factory class
class Factory{
public:
    static Notification* sendNotification(string type){
        if(type=="Email")
            return new EmailNotification();
        else if(type == "SMS")
            return new SMSNotification();
        else if(type == "Push")
            return new PushNotification();
        else
            return nullptr;
    }
};

//Client 
int main(){
    Notification * n1 = Factory::sendNotification("Email");
    n1->send();
    Notification * n2 = Factory::sendNotification("SMS");
    n2->send();
    Notification * n3 = Factory::sendNotification("Push");
    n3->send();
    
    delete n1;
    delete n2;
    delete n3;
    return 0;
}

