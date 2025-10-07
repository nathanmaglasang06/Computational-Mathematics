//
// Created by Nathan Maglasang on 26/9/2025.
//

#include <numeric>
#include <iostream>
using namespace std;

int main() {
    std::vector<std::pair<int, int>> vec = {
        {19, 3}, {35, 11}, {8, 27}, {44, 14}, {22, 50},
        {16, 6}, {13, 20}, {29, 9}, {40, 5}, {12, 38}, {25, 17}
    };
    vector<int> Sublist;
    cout << "1. \n";
    for (const auto& p : vec) {
        int sum = p.first + p.second;
        std::cout << "(" << p.first << ", " << p.second
                  << ") -> sum = " << sum << std::endl;
    }
    cout << "\n2. \n";
    for (const auto& p : vec) {
        if (p.second > p.first) {
            Sublist.push_back(p.first);

        }


    }
    cout << "Sublist: ";
    for (int num : Sublist) {
        cout << num << " ";
    }
}