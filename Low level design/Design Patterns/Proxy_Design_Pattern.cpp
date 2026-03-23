/*
The Proxy Design Pattern is another structural design pattern where one object acts as a substitute 
(or placeholder) for another object to control access to it.
🔷 Core Idea
    A Proxy sits in front of the real object and controls how it is accessed.
    Instead of calling the real object directly, the client talks to the proxy, which can:
    Delay creation (lazy loading)
    Add security checks ,Cache results ,Log access
    
🔷 Structure
    Participants:
    Subject (Interface)
    RealSubject (Actual object)
    Proxy (Controls access)

Client never directly touches RealImage
    [Client → Proxy → RealImage]
    
                        UML-Diagram
                        
                <<interface>>
                    Subject
        ---------------------------------
        |  + request() : void            |
        ---------------------------------
                    ▲
          -------------------------
          |                       |
+---------------------+   +------------------------+
|    RealSubject      |   |         Proxy          |
+---------------------+   +------------------------+
|                     |   | - realSubject: Subject*|
+---------------------+   | - filename: string     |
| + request()         |   +------------------------+
+---------------------+   | + request()            |
                          +------------------------+
*/

#include<iostream>
using namespace std;

//Subject interface
class Image{
public:
    virtual void display()=0;
    virtual ~Image(){}
};

//Real Object
class RealImage:public Image{
    string filename;
    void loadfromdisk(){
        cout<<"Loading "<<filename<<" from disk..."<<endl;
    }
public:
    RealImage(string file):filename(file){
        loadfromdisk();
    }
    void display() override{
        cout<<"Displaying "<<filename<<endl;
    }
};

//Proxy 
class ProxyImage:public Image{
    string filename;
    RealImage* realimage;
public:
    ProxyImage(string file):filename(file),realimage(nullptr){}
    void display() override{
        if(realimage==nullptr){
            realimage = new RealImage(filename);
        }
        realimage->display();
    }
    ~ProxyImage(){
        delete realimage;
    }
};

//client code
int main() {
    Image* img = new ProxyImage("photo.jpg");

    // Image not loaded yet
    img->display();  // loads + displays
    img->display();  // only displays (no loading)

    delete img;
}