
//
// Created by Nathan Maglasang on 26/9/2025.
//

#include <numeric>
#include <iostream>
#include <unordered_set>
using namespace std;

int main() {
    std::vector<int> vec = {
        3, 7, 1, 10, 6, 2, 8, 4, 9, 5, 2, 7, 3, 10, 1, 6, 8, 4, 9, 5
    };
    vector<int> result;
    unordered_set<int> seen;
    for (int num : vec) {
        if (seen.find(num) == seen.end()) {
            result.push_back(num);
            seen.insert(num);
        }
    }
    cout << "Result: \n";
    for (int num : result) {
        cout << num << ", ";

    }

}