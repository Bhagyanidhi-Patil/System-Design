/*
same as previous question but input is sorted.
List 1:
[101, 102, 103, 105, 108, 110]


List 2:
[100, 102, 103, 106, 108, 112]


Expected output:
[102, 103, 108]

We maintain two pointers:
- i → points to the first list.
- j → points to the second list.
Three conditions:
1. If list1[i] == list2[j]:
   - We found a common snapshot.
   - Add it to the result.
   - Increment both pointers.
2. If list1[i] < list2[j]:
   - Increment i.
   - Why? Since lists are sorted, the current element in List 1 cannot match any future element in List 2.
3. If list1[i] > list2[j]:
   - Increment j for the same reason.

   TC - O(N+M) , SC O(1)
*/




#include <iostream>
#include <vector>
using namespace std;

vector<int> findCommonSnapshots(vector<int>& a, vector<int>& b)
{
    int i = 0, j = 0;
    vector<int> ans;

    while (i < a.size() && j < b.size())
    {
        if (a[i] == b[j])
        {
            ans.push_back(a[i]);
            i++;
            j++;
        }
        else if (a[i] < b[j])
        {
            i++;
        }
        else
        {
            j++;
        }
    }

    return ans;
}

int main()
{
    vector<int> a = {101, 102, 103, 105, 108, 110};
    vector<int> b = {100, 102, 103, 106, 108, 112};

    vector<int> ans = findCommonSnapshots(a, b);

    for (int x : ans)
        cout << x << " ";

    return 0;
}
