//
// Created by Nathan Maglasang on 27/9/2025.
//

#include <iostream>
#include <numeric>
using namespace std;

struct Fraction {
    long long num;
    long long den;

    Fraction(long long n, long long d) {
        long long g = gcd(n, d);
        num = n / g;
        den = d / g;
        if (den < 0) {
            num = -num;
            den = -den;
        }
    }

    Fraction operator+(const Fraction& other) const {
        long long new_num = num * other.den + other.num * den;
        long long new_den = den * other.den;
        return Fraction(new_num, new_den);
    }
};

int main() {
    Fraction H(0, 1);

    for (int i = 1; i <= 1000; i++) {
        H = H + Fraction(1, i);
    }

    cout << "Numerator: " << H.num << endl;
    cout << "Denominator: " << H.den << endl;

    return 0;
}