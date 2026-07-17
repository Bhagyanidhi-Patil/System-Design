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

/*
strcmp() is a standard C library function used to compare 
two C-style strings (char* or const char*) lexicographically (dictionary order).

strcmp() compares two null-terminated C strings character by character.

Return Value
Return Value	Meaning
0	               Both strings are equal
< 0	               str1 is lexicographically smaller than str2
> 0	               str1 is lexicographically greater than str2

It does not return only -1, 0, or 1.
It returns the difference between the first mismatched characters.

strcmp() does not return the difference between two strings.
It returns the difference between the first pair of characters that differ.
*/