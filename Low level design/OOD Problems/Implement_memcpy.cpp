/* Implement-memcpy
memcpy is a low-level memory-copy function from the C standard library. Signature (from <cstring> / <string.h>):

void* memcpy(void* dest, const void* src, size_t count);
"memcpy copies raw bytes from source memory to destination memory. It treats memory as a sequence of bytes and copies one byte at a time."

"It does not handle overlapping memory safely. That's what memmove is for." 
Overlapping memory means Source and destination point to some of the SAME memory area.
memcpy is NOT safe for overlapping memory.
-> memcpy(arr + 1, arr, 5);   This causes undefined behavior.

Important properties
    -> Works byte-by-byte internally
    -> Does not care about types
    -> Very fast
    -> Returns dest

Example:
1.  int a = 20;
    int b;
    memcpy(&b, &a, sizeof(int));

    int usually takes 4 bytes (32 bits). So in memory it may look like: 00000000 00000000 00000000 00010100
    
    Address    Value
    100        00010100
    101        00000000
    102        00000000
    103        00000000
    memcpy copies the RAW BYTES of a.

    If int = 4 bytes:
    Byte 1
    Byte 2
    Byte 3
    Byte 4
    All 4 bytes get copied into b.

2.  char src[] = "hello";
    char dest[10];
    memcpy(dest, src, 6);

    Each character = 1 byte.
    h -> 104
    e -> 101
    l -> 108
    l -> 108
    o -> 111
    \0 -> 0
    Why 6? Because: "hello" = 5 chars +1 for null character \0.
    
    So memory actually contains:
    [104][101][108][108][111][0]

    Initially:
    src :  [h][e][l][l][o][\0]
    dest:  [ ][ ][ ][ ][ ][ ]

    Iteration 1 : dest[0] = src[0] , copies 'h'
    dest: [h][ ][ ][ ][ ][ ]
    
    Iteration 2 : copies 'e'
    dest: [h][e][ ][ ][ ][ ]
    
    Iteration 3  : copies 'l'
    dest: [h][e][l][ ][ ][ ]
    
    Iteration 4 : copies 'l'
    dest: [h][e][l][l][ ][ ]
    
    Iteration 5 : copies 'o'
    dest: [h][e][l][l][o][ ]
    
    Iteration 6 : copies '\0'
    dest: [h][e][l][l][o][\0]

    Now dest becomes a valid C-string.
*/

#include <iostream>
using namespace std;

void* my_memcpy(void* dest, const void* src, size_t n){
    char* d = (char*)dest;
    const char* s = (const char*)src;

    for(size_t i=0;i<n;i++){
        d[i] = s[i];
    }
    return dest;
}

int main(){
    char src[] = "hello";
    char dest[10];

    my_memcpy(dest,src,6);
    cout<<dest<<endl;
}

/*
Code explaination: 

memcpy should work for ANY type: so we cannot use int*, char*. We need a generic pointer,so we use void* dest.
const void* src -> source can be any type but we promise NOT to modify source data. const protects source memory.
why size_t not int ? size_t is: standard type for memory sizes , returned by sizeof , cannot be negative.

Why convert to char*? char* d = (char*)dest;
Because void* cannot be indexed
Why char* works? char is exactly 1 byte. move exactly 1 byte forward.

char* d = (char*)dest; "Take dest pointer and treat it as a char* pointer." This is called type casting.

even if you're copying an int, memcpy still uses char* internally.
int a = 20;
int b;
my_memcpy(&b, &a, sizeof(int));
memcpy copies byte to byte so it uses char internally.char is exactly 1 byte.

void* src -> "src can point to memory of any datatype." Because void* is a generic pointer.

void* my_memcpy(...) ->Function returns a generic pointer (void*).
And inside: return dest; -> dest itself is already a void*.
char* d = (char*)dest; -> You typecast only for internal use. But you did NOT change dest.

----------------------------------------------------------------------------------------------------------------------------
Note: 
Typecasting does NOT create new memory. It only changes: how we interpret the same memory.
Typecasting does not change how the pointer is created or the memory address it stores. 
It only changes how the compiler interprets and accesses the memory at that address.
void* dest does NOT create memory. It only stores an address of some existing memory, and that memory can be of any type.

char* p
does NOT mean “it is a char type variable”. It means: “pointer to memory where each unit is 1 byte” .
----------------------------------------------------------------------------------------------------------------------------

What memcpy actually expects
void* memcpy(void* dest, const void* src, size_t n);

It expects:
address of destination
address of source
number of bytes

memcpy(dest, src, 6);
src is an array , src  → address of first element. same for dest. So, writing & is NOT needed.

int a = 10;
int b;
memcpy(&b, &a, sizeof(int));
a is a single variable , NOT an array , so a is just a value, not a pointer . We need its address.
*/