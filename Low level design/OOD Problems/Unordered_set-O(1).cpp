/* Design an Unordered Set with O(1) insertion, removal, and lookup, we can use a combination of a vector and an unordered_map.
    -  nums stores the elements in the set.
    -  mp stores the index of each number, where the number itself is used as an index into mp.
    - cnt tracks the number of elements.

    For example, if we insert 10, 20, 30:
    - nums = [10, 20, 30]
    - mp[10] = 0, mp[20] = 1, mp[30] = 2
*/


#include <iostream>
#include <vector>
using namespace std;

class UnorderedSet {
private:
    vector<int> nums;
    vector<int> mp;
    int cnt = 0;

public:
    UnorderedSet(int maxVal) {
        nums.resize(maxVal + 1);
        mp.resize(maxVal + 1, -1);
    }

    bool contains(int val) {
        if (val < 0 || val >= (int)mp.size())
            return false;

        int index = mp[val];

        if (index < 0 || index >= cnt)
            return false;

        return nums[index] == val;
    }

    bool insert(int val) {
        if (val < 0 || val >= (int)mp.size())
            return false;

        if (contains(val))
            return false;

        nums[cnt] = val;
        mp[val] = cnt;
        cnt++;

        return true;
    }

    bool remove(int val) {
        if (!contains(val))
            return false;

        int index = mp[val];
        int last = nums[cnt - 1];

        nums[index] = last;
        mp[last] = index;
        mp[val] = -1;
        cnt--;

        return true;
    }

    void iterate() {
        for (int i = 0; i < cnt; i++)
            cout << nums[i] << " ";
        cout << endl;
    }

    void clear() {
        cnt = 0;
    }

    int size() {
        return cnt;
    }

    bool empty() {
        return cnt == 0;
    }
};

int main() {
    UnorderedSet s(1000);

    s.insert(10);
    s.insert(20);
    s.insert(30);

    s.iterate();  // 10 20 30

    s.clear();

    s.insert(20);
    s.iterate();  // 20

    cout << s.contains(10) << endl; // 0
    cout << s.contains(20) << endl; // 1

    return 0;
}