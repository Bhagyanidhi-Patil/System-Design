//Implement min-stack

#include <iostream>
#include <stack>
using namespace std;

class MinStack{
    stack<long long>st;
    long long minValue;
public:
    MinStack(){}

    void push(int val){
        if(st.empty()){
            st.push(val);
            minValue = val;
        }
        else if(val>minValue){
            st.push(val);
        }
        else{
            // encode value
            st.push(2*val - minValue);
            minValue = val;
        }
    }

    void pop(){
        if(st.empty())return;

        long long top = st.top();
        st.pop();
        if(top<minValue){
            // restore previous minimum
            minValue = 2*minValue - top;
        }
    }

    int top(){
        if(st.empty())return -1;

        long long top = st.top();
        if(top>=minValue)
            return top;
        else
            return minValue;
    }

    int getMin(){
        if(st.empty())return -1;
        return minValue;
    }
};

int main() {
    MinStack ms;
    ms.push(5);
    ms.push(3);
    ms.push(7);
    ms.push(2);

    cout << "Min: " << ms.getMin() << endl; // 2
    ms.pop();
    cout << "Min: " << ms.getMin() << endl; // 3

    return 0;
}

/*
🎯 Why do we encode & decode?
Because we want all operations in O(1) , AND we don’t want to use extra space (like another stack)
When you pop minValue, How do you know the previous minimum to store in minValue?
Instead of using another stack, we hide the previous minimum inside the current value. That’s why we encode.

🧪 Example:
push(5), push(3), push(7), push(2), pop(), pop()

Step 1: push(5)
    Stack is empty
    push normally
    Stored stack: [5]
    minVal = 5

Step 2: push(3)
    3 < 5 → encode
    Encoded value:
    2*3 - 5 = 1
    Stored stack: [5, 1]
    minVal = 3
    👉 Conceptually stack is: [5, 3]

Step 3: push(7)
    7 ≥ 3 → normal push
    Stored stack: [5, 1, 7]
    minVal = 3
    👉 Conceptual: [5, 3, 7]

Step 4: push(2)
    2 < 3 → encode
    2*2 - 3 = 1
    Stored stack: [5, 1, 7, 1]
    minVal = 2
    👉 Conceptual: [5, 3, 7, 2]

Step 5: top()
    Top = 1
    Check:
    1 < minVal (2) → encoded
    👉 So real top = minVal = 2

Step 6: getMin()
    minVal = 2

Step 7: pop()
    Top = 1
    Check:
    1 < minVal (2) → encoded
    So decode:
    oldMin = 2*minVal - top
        = 2*2 - 1
        = 3
    Stored stack: [5, 1, 7]
    minVal = 3
    👉 Conceptual: [5, 3, 7]

Step 8: pop()
    Top = 7
    Check:
    7 ≥ minVal → normal
    Stored stack: [5, 1]
    minVal = 3
    👉 Conceptual: [5, 3]
*/