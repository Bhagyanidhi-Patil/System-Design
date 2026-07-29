#include <iostream>
using namespace std;

class Employee {

private:
    double salary;

public:

    void setSalary(double s) {

        if(s >= 0)
            salary = s;
        else
            cout << "Invalid Salary\n";
    }

    double getSalary() {
        return salary;
    }

};

int main() {

    Employee emp;

    emp.setSalary(60000);

    cout << emp.getSalary();

}

/*
Why private?

Imagine salary were public.

emp.salary = -50000;
This makes no sense.

Using
setSalary()

we validate the value.

*/