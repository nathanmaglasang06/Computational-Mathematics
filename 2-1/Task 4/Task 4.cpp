//
// Created by Nathan Maglasang on 26/9/2025.
//

#include <numeric>
#include <iostream>
#include <numeric>
#include <ratio>
using namespace std;

int main() {
    std::vector<std::pair<int, int>> One;
    int x;
    int y;
    int gcdtrue;
    int gcdfalse;
    int gcdvalue;
    for (int x = 1; x <= 10001; ++x) {
        for (int y = 1; y <= 10001; ++y) {
            One.push_back(std::make_pair(x, y));
        }
    }

    for (const auto& p : One) {
        gcdvalue = gcd(p.first, p.second);
        if (gcdvalue == 1) {
            gcdtrue += 1;
        } else {
            gcdfalse += 1;
        }
    }
    cout << "True: " <<gcdtrue << "\n";
    cout << "False: " << gcdfalse << "\n";
    float percentage = (float)gcdtrue / ((float)gcdfalse+(float)gcdtrue);

    cout << "Percentage: " << percentage << "%"<< endl;

    cout << "6/pi^2: " << 6/pow(3.141592653589793, 2) << endl;

}