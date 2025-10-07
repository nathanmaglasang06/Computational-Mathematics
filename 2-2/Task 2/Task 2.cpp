//
// Created by Nathan Maglasang on 27/9/2025.
//
#include <iostream>
using namespace std;

// Simplifying Variable Naming, long Lon looks messy
using i64 = long long;

void InputValues(i64 &a, i64 &b) {
    cout << "Enter a value for 'a': ";
    while (!(cin >> a)) {
        cout << "Please input an integer value for 'a': ";
        cin.clear();
        cin.ignore(10000, '\n');
    }
    cin.ignore(10000, '\n');

    cout << "\nEnter a value for 'b': ";
    while (!(cin >> b)) {
        cout << "Please input an integer value for 'b': ";
        cin.clear();
        cin.ignore(10000, '\n');
    }
    cin.ignore(10000, '\n');
}

int EuclideanCount_v2(i64 a, i64 b, i64 &final_a) {
    int count = 0;
    while (b != 0) {
        if (( a % b ) < ( b / 2 )) {
            i64 temp = b;
            b = a % b;
            a = temp;
            count++;
        } else {
            i64 temp = b;
            b = b - ( a % b );
            a = temp;
            count++;
        }

        count++;
    }
    final_a = a;
    return count;
}

int main() {
    i64 a, b;
    InputValues(a, b);

    i64 gcd_value;
    int count = EuclideanCount_v2(a, b, gcd_value);

    cout << "The Euclidean algorithm v2 took: " << count << " iterations\n";
    cout << "The gcd(" << a << ", " << b << ") = " << gcd_value << "\n";

    return 0;
}