/* 
Implement-strlen:
    strlen is a C/C++ library function that returns the length of a C-style string.
    Example: char str[] = "hello";
    Here, memory actually looks like: 'h' 'e' 'l' 'l' 'o' '\0'
    It counts characters until it reaches '\0', but does NOT include the null character.

Internally, strlen does something very simple:
    Start at the first character of the string
    Move forward one character at a time
    Count each character
    Stop when '\0' (null terminator) is found
    Return the count
*/

#include <iostream>
using namespace std;

size_t my_strlen(const char* str){
    size_t len = 0;
    while(str[len]!=0){
        len++;
    }
    return len;
}

int main(){
    const char* text = "hello,world!";
    cout<<"Length of the string is: "<<my_strlen(text)<<endl;
    return 0;
}

/*
size_t - An unsigned integer type used to represent sizes and counts in memory.
It is what functions like strlen, sizeof, vector::size() return.

Key properties of size_t:
    1. It is unsigned , So it can only represent 0 or positive values. No negative numbers.
    2. It is used for sizes
    Used for: array length , memory sizes , string length , container sizes.

-------------------------------------------------------------------------------------------------

uint32_t vs size_t
Both are unsigned integer types, but they are used for very different purposes.

1. uint32_t : Fixed-width unsigned integer. Always exactly 32 bits.
              Defined in: #include <cstdint>
              Range: 0 to 2^32 - 1 (~4.29 billion)
              Size is always 4 bytes

2. size_t : Unsigned integer type used for sizes and memory indexing.
            Properties: On 32-bit system → 32 bits , On 64-bit system → 64 bits
            Always big enough to represent maximum possible memory size
*/