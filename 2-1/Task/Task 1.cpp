//
// Created by Nathan Maglasang on 26/9/2025.
//

#include <numeric>
#include <iostream>
using namespace std;

int main() {
    vector<int> List({83, 17, 45, 4, 90, 11, 38, 76, 55, 67, 3, 21, 49, 14, 88, 70, 6, 31, 95, 23, 58, 2, 44, 77, 9, 53, 12, 68});
vector<int> Sublist;
for (int num : List) {
    if (num % 3 == 1) {
        Sublist.push_back(num);
    }
}
    cout << "List\n";
    for (int num : List) {
        cout << num << " ";
    }
    cout << "\nSublist: ";
    for (int num : Sublist) {
        cout << num << " ";
    }


}