/*
each snapshot has a date + time timestamp, for example:

18-11-2026 10:20:15

We need to find exact matching date-time timestamps in two sorted lists.
1. Example
List 1:
18-11-2026 10:10:15
18-11-2026 10:20:15
18-11-2026 10:30:15
19-11-2026 09:15:00
19-11-2026 10:00:00


List 2:
18-11-2026 10:15:15
18-11-2026 10:20:15
18-11-2026 10:30:15
19-11-2026 09:30:00
19-11-2026 10:00:00


Expected output:
18-11-2026 10:20:15
18-11-2026 10:30:15
19-11-2026 10:00:00

TC -O(M+N), SC - O(1)
*/


#include <iostream>
#include <vector>
#include <string>
#include <sstream>
using namespace std;

struct Timestamp {
    int day, month, year;
    int hour, minute, second;

    bool operator==(const Timestamp& t) const {
        return day == t.day &&
               month == t.month &&
               year == t.year &&
               hour == t.hour &&
               minute == t.minute &&
               second == t.second;
    }

    bool operator<(const Timestamp& t) const {
        if (year != t.year) return year < t.year;
        if (month != t.month) return month < t.month;
        if (day != t.day) return day < t.day;
        if (hour != t.hour) return hour < t.hour;
        if (minute != t.minute) return minute < t.minute;

        return second < t.second;
    }
};

// Convert timestamp string into Timestamp object
Timestamp parse(const string& s) {
    Timestamp t;
    char c;

    stringstream ss(s);

    ss >> t.day >> c
       >> t.month >> c
       >> t.year
       >> t.hour >> c
       >> t.minute >> c
       >> t.second;

    return t;
}

// Find common timestamps
vector<Timestamp> findCommon(
    vector<Timestamp>& a,
    vector<Timestamp>& b)
{
    int i = 0, j = 0;
    vector<Timestamp> ans;

    while (i < a.size() && j < b.size()) {

        if (a[i] == b[j]) {
            ans.push_back(a[i]);
            i++;
            j++;
        }
        else if (a[i] < b[j]) {
            i++;
        }
        else {
            j++;
        }
    }

    return ans;
}

int main() {

    vector<string> list1 = {
        "18-11-2026 10:10:15",
        "18-11-2026 10:20:15",
        "18-11-2026 10:30:15",
        "19-11-2026 09:15:00"
    };

    vector<string> list2 = {
        "18-11-2026 10:15:15",
        "18-11-2026 10:20:15",
        "18-11-2026 10:30:15",
        "19-11-2026 09:30:00"
    };

    vector<Timestamp> a, b;

    for (const string& s : list1)
        a.push_back(parse(s));

    for (const string& s : list2)
        b.push_back(parse(s));

    vector<Timestamp> ans = findCommon(a, b);

    cout << "Common snapshots:\n";

    for (const Timestamp& t : ans) {
        cout << t.day << "-" << t.month << "-"
             << t.year << " "
             << t.hour << ":" << t.minute << ":"
             << t.second << "\n";
    }

    return 0;
}

/*
const Timestamp& t
This means:
- t is a reference to another Timestamp object.
- const ensures that we cannot modify that object inside the function.
- & avoids creating an unnecessary copy.

const after the function parameters
This means the function promises not to modify the current object (this).

----

Consider this input
string s = "18-11-2026 10:20:15";
stringstream ss(s);


We want to extract:
Component	Value
t.day	    18
t.month	    11
t.year	    2026
t.hour	    10
t.minute	20
t.second	15

What is char c?
char c;

We use c to consume separator characters such as:
- - between day, month and year.
- : between hour, minute and second.
- The space between date and time is automatically skipped when extracting integers.

>> extracts values one by one. The >> operator extracts data from the stream and stores it in the respective variables.
*/