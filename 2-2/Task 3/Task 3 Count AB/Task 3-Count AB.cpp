//
// Created by Nathan Maglasang on 27/9/2025.
//

#include <numeric>
#include <iostream>
#include <numeric>
#include <fstream>

using namespace std;

using i64 = long long;

int main() {
    i64 counta = 0;
    i64 countb = 0;
    i64 countab = 0;
    i64 n;
    vector<pair<int, int>> Pairs;

    cout << "Enter a n: ";
    cin >> n;

    for (i64 a = 1; a < n; ++a) {
        for (i64 b = 1; b < n; ++b) {
            Pairs.push_back(std::make_pair(a, b));
            if (a <= 10000) counta++;
            if (b <= 10000) countb++;
            if (a <= 10000 && b <= 10000) countab++;
        }
    }

    cout << "Count of a >= 10^3: " << counta << "\n";
    cout << "Count of b >= 10^3: " << countb << "\n";
    cout << "Total count of (a, b) >= 10^3: " << countab << "\n";
}




