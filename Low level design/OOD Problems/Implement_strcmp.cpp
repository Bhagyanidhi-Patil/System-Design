#include <iostream>
using namespace std;

int stringcompare(const char* s1,const char* s2){
    while(*s1==*s2){
        if(*s1=='\0')
            return 0;
        s1++;
        s2++;
    }
    return *s1-*s2;
}

int main(){
    const char* c1 = "hello";
    const char* c2 = "hello";
    const char* c3 = "world";
    cout<<stringcompare(c1,c2)<<endl;
    cout<<stringcompare(c1,c3)<<endl;
    return 0;
}