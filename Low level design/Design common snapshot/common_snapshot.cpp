/*
You are given two lists of snapshots. Find the snapshots that are common to both lists.
List 1:
[101, 102, 103, 104, 105, 106]


List 2:
[103, 105, 107, 108, 101]


Expected output:
[101, 103, 105]

Logic:
1. Insert all elements of List 1 into an unordered_set.
2. Traverse List 2.
3. For every element, check whether it exists in the set.
4. If it exists, add it to the result.
5. To avoid duplicate snapshots in the result, use another set.

TC - O(M+N) , SC - O(N+K) where K is the number of distinct common snapshots.
*/


#include <iostream>
#include <vector>
#include <unordered_set>
using namespace std;

vector<int> findCommonSnapshots(vector<int>& list1,
                                vector<int>& list2)
{
    unordered_set<int> s;

    // Step 1: Insert List 1 elements
    for (int x : list1)
    {
        s.insert(x);
    }

    unordered_set<int> common;
    vector<int> ans;

    // Step 2: Traverse List 2
    for (int x : list2)
    {
        if (s.find(x) != s.end())
        {
            if (common.insert(x).second)   //common.insert(x) returns a pair. The .second tells us whether the insertion was successful.
            {
                ans.push_back(x);
            }
        }
    }

    return ans;
}

int main()
{
    vector<int> list1 = {101, 102, 103, 104, 105, 106};
    vector<int> list2 = {103, 105, 107, 108, 101};

    vector<int> ans = findCommonSnapshots(list1, list2);

    cout << "Common snapshots: ";

    for (int x : ans)
    {
        cout << x << " ";
    }

    return 0;
}

/*
In C++, common.insert(x) returns a pair. The .second tells us whether the insertion was successful.
result contains two values:
- result.first → An iterator pointing to the element.
- result.second → A Boolean (true or false).

unordered_set<int> common;

common.insert(10);  // First insertion: true
common.insert(20);  // First insertion: true
common.insert(10);  // Duplicate: false
*/


//using single set

vector<int> findCommonSnapshots(vector<int>& list1,
                                vector<int>& list2)
{
    unordered_set<int> s(list1.begin(), list1.end());

    vector<int> ans;

    for (int x : list2)
    {
        if (s.erase(x))
        {
            ans.push_back(x);
        }
    }

    return ans;
}