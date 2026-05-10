/*
Implement-strcpy
strcpy (string copy) is a C/C++ library function that copies one C-style string into another.

It takes:
a source string
a destination buffer
and copies characters from source to destination including the null terminator '\0'.

🔥 How it works internally
strcpy:
    Starts from first character of source
    Copies each character to destination
    Continues until it hits '\0'
    Also copies the '\0' to mark end of string

So it is basically a loop-based character copy.

*/

#include <iostream>
using namespace std;

char* my_strcpy(const char* src,char* dest){
    char* original_dest = dest;
    while(*src!='\0'){
        *dest = *src;
        dest++;
        src++;
    }
    *dest = '\0';
    return original_dest;
} 

int main(){
    const char* src = "hello";
    char dest[10];
    my_strcpy(src,dest);
    cout<<dest<<endl;
    return 0;
}

/*
const char* src = "hello"; src is a pointer storing the address of the first character of the string "hello".
src -----> 'h' 'e' 'l' 'l' 'o' '\0'

char* original_dest = dest;
is used because later we move dest forward while copying.
Inside loop: dest++ changes the pointer.

So if originally:
dest ---> beginning of array

after copying "hello":
dest ---> points to '\0' at end

At the end: dest no longer points to start of copied string.
But standard strcpy must return: pointer to beginning of destination string

-------------------------------------------------------------------------------------
In memcpy implementation you do NOT need an extra pointer because:
you are not modifying dest itself.

In strcpy we did: dest++;
So original dest address got lost.
That’s why we needed: char* original_dest = dest;

But in memcpy , You use indexing: d[i] = s[i];
Here: d itself never changes , only i changes.
d - still points to beginning.
dest - also still points to beginning.
*/