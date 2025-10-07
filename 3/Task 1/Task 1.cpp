#include "stdc++.h"
using namespace std;

// Extended Euclidean Algorithm
// Returns tuple (x, y, gcd) such that a*x + b*y = gcd(a, b)
tuple<long long, long long, long long> extended_euclidean(long long a, long long b) {
    long long x0 = 1, y0 = 0, r0 = a;
    long long x1 = 0, y1 = 1, r1 = b;

    while (r1 != 0) {
        long long q = r0 / r1;
        long long r2 = r0 - q * r1;
        r0 = r1;
        r1 = r2;

        long long x2 = x0 - q * x1;
        x0 = x1;
        x1 = x2;

        long long y2 = y0 - q * y1;
        y0 = y1;
        y1 = y2;
    }

    return {x0, y0, r0};
}

// Modular inverse: returns optional<long long>
// If inverse exists, returns x such that (a*x) % m == 1
optional<long long> mod_inverse(long long a, long long m) {
    auto [x, y, g] = extended_euclidean(a, m);
    if (g != 1) return nullopt; // inverse does not exist
    long long inv = (x % m + m) % m; // normalize to [0, m-1]
    return inv;
}

int main() {
    long long a = 463762346;
    long long m = 3575683457313; // modulus

    auto inv = mod_inverse(a, m);
    if (inv) {
        cout << "Modular inverse of " << a << " mod " << m << " is: " << *inv << "\n";
        // verification
        __int128 check = (__int128)a * (*inv);
        int checked = (check % m);
        cout << "Check: a*inv mod m = " << checked << " (should be 1)\n";
    } else {
        cout << "Modular inverse does not exist (gcd(a,m) != 1)\n";
    }

    return 0;
}