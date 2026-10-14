/*
What is a Bitset?
Normally, if we want to store integers like 0, 1, 2, 3, 4, we might store each integer separately.
Instead, a bitset uses one bit to represent each integer:
- 1 means the integer is present.
- 0 means the integer is absent.

*/
#include <iostream>
using namespace std;

class BitSet{
private:
    unsigned int bits = 0;
public:
    //mark bit as 1
    void set(int pos){
        bits|=(1U <<pos);
    }

    //mark bit as 0
    void clear(int pos){
        bits&=~(1U <<pos);
    }

    // Check whether the bit is 1
    bool test(int pos){
        return (bits&(1U<<pos));
    }
};

int main() {
    BitSet b;

    b.set(2);
    b.set(5);

    cout << b.test(2) << endl;  // 1
    cout << b.test(3) << endl;  // 0

    b.clear(2);
}

/*bits belongs to the object b. 
Every time you call b.set(), b.clear(), or b.test(), those functions operate on the same bits variable. 
The value is not reset to 0 on every function call; it is initialized when the object is created and then updated as needed.
*/


#include <iostream>
#include <vector>
using namespace std;

class Bitset {
private:
    vector<unsigned int> bits;

public:
    Bitset(int n) {
        bits.resize((n + 31) / 32, 0);
    }

    void set(int pos) {
        int index = pos / 32;
        int offset = pos % 32;

        bits[index] |= (1U << offset);
    }

    void clear(int pos) {
        int index = pos / 32;
        int offset = pos % 32;

        bits[index] &= ~(1U << offset);
    }

    bool test(int pos) {
        int index = pos / 32;
        int offset = pos % 32;

        return (bits[index] & (1U << offset)) != 0;
    }
};

int main() {
    Bitset b(100);

    b.set(2);
    b.set(35);

    cout << b.test(2) << endl;   // 1
    cout << b.test(35) << endl;  // 1
    cout << b.test(10) << endl;  // 0

    b.clear(2);

    cout << b.test(2) << endl;   // 0

    return 0;
}

/*
Bitset b(100);


This means we want to represent 100 possible integers, from position 0 to position 99.
Each unsigned int stores 32 bits, so we need:
|100/32| = 4

Therefore, our vector contains 4 elements, each capable of storing 32 bits.
bits.resize((n + 31) / 32, 0);

bits[0]
32 bits → positions 0–31

bits[1]
32 bits → positions 32–63

bits[2]
32 bits → positions 64–95

bits[3]
32 allocated bits → positions 96–127

each unsigned int stores 32 bits.
- index tells us which vector element contains the bit.
- offset tells us which bit inside that element we need to access.

For pos = 2:
index = 2 / 32;   // 0
offset = 2 % 32;  // 2
So we access bits[0], specifically bit 2 inside that element.

(n + 31) / 32
If n is not divisible by 32: Adding 32 still gives enough blocks, but it may allocate one extra block.
If n is divisible by 32: Adding 32 definitely allocates one unnecessary extra block.
So we add 31 to get the correct number of blocks in both cases.
*/