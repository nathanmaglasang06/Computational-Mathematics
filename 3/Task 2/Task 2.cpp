//
// Created by Nathan Maglasang on 3/10/2025.
//
// compile: g++ -std=c++17 -O2 crt_solver.cpp -o crt_solver
#include "stdc++.h"
#include <vector>
#include <iostream>
using namespace std;

// Extended GCD: returns gcd(a,b) and sets x,y so that a*x + b*y = gcd(a,b)
long long extended_gcd(long long a, long long b, long long &x, long long &y) {
    if (b == 0) { x = (a >= 0) ? 1 : -1; y = 0; return llabs(a); }
    long long x1, y1;
    long long g = extended_gcd(b, a % b, x1, y1);
    x = y1;
    y = x1 - (a / b) * y1;
    return g;
}

// Normalize value to [0, mod-1]
long long mod_norm(__int128 v, long long mod) {
    if (mod <= 0) throw invalid_argument("modulus must be positive");
    __int128 m = mod;
    v %= m;
    if (v < 0) v += m;
    return (long long)v;
}

// Solve system of congruences via generalized CRT
// Input: vector of pairs (a_i, m_i). m_i must be > 0.
// Output: optional pair (x, M) where x is the solution modulo M, or nullopt if inconsistent.
// Matches sympy's behavior: if no equations given, returns (0,1).
optional<pair<long long,long long>> solve_congruence(const vector<pair<long long,long long>>& eqs) {
    if (eqs.empty()) return make_pair(0LL, 1LL);

    // Start with x = a0 (mod m0)
    long long r = mod_norm(eqs[0].first, eqs[0].second);
    long long m = eqs[0].second;
    if (m <= 0) throw invalid_argument("moduli must be positive");

    for (size_t i = 1; i < eqs.size(); ++i) {
        long long a2 = eqs[i].first;
        long long m2 = eqs[i].second;
        if (m2 <= 0) throw invalid_argument("moduli must be positive");
        long long r2 = mod_norm(a2, m2);

        // Solve: r + m * t ≡ r2 (mod m2)  -> m * t ≡ (r2 - r) (mod m2)
        long long x, y;
        long long g = extended_gcd(m, m2, x, y); // x*m + y*m2 = g

        long long diff = r2 - r;
        if (diff % g != 0) {
            // no solution
            return nullopt;
        }

        // Multiply t0 = (diff/g) * x  (but reduce modulo m2/g)
        long long m2_g = m2 / g;
        __int128 mult = (__int128)(diff / g) * (__int128)x;
        long long t0 = mod_norm(mult, m2_g); // t0 in [0, m2/g - 1]

        // new r = r + m * t0
        __int128 new_r = (__int128)r + (__int128)m * (__int128)t0;

        // new modulus is lcm = m/g * m2  (watch overflow)
        __int128 new_m = (__int128)(m / g) * (__int128)m2;
        if (new_m > numeric_limits<long long>::max()) {
            // If needed, one could return larger types or a big-int. For year-12 / typical inputs this is fine.
            throw overflow_error("combined modulus too large for 64-bit result");
        }

        r = mod_norm(new_r, (long long)new_m);
        m = (long long)new_m;
    }

    return make_pair(r, m);
}
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

// small helper to print result
void print_result(const optional<pair<long long,long long>>& sol) {
    if (!sol) {
        cout << "No solution (inconsistent system of congruences)\n";
    } else {
        cout << "x ≡ " << sol->first << " (mod " << sol->second << ")\n";
    }
}

int main() {
    unsigned long long xtimes;
    xtimes = 235579082340345908234098436U;
    cout << xtimes << "\n";
    // Example 1: simple coprime moduli
    vector<pair<long long,long long>> ex1 = {{2, 3}, {3, 5}}; // x ≡ 2 (mod 3), x ≡ 3 (mod 5) => x=8 (mod 15)
    auto r1 = solve_congruence(ex1);
    print_result(r1); // expected: x ≡ 8 (mod 15)


    vector<pair<long long,long long>> fin1 = {{2, 3}, {3, 5}}; // x ≡ 2 (mod 3), x ≡ 3 (mod 5) => x=8 (mod 15)
    auto f1 = solve_congruence(ex1);
    print_result(f1);

}
//235579082340345908234098436x ≡ 672391285923 mod 146329803460928340984356098231