//
// Created by Nathan Maglasang on 7/11/2025.
//
#include <iostream>
#include <vector>
#include <boost/rational.hpp>
#include <cmath>
#include <stdexcept>
#include <algorithm>

using std::vector;
using boost::rational;

using Frac = rational<long long>;

// ─── Basic Vector Helpers ────────────────────────────────────────────────

Frac dot(const vector<Frac>& u, const vector<Frac>& v) {
    Frac sum = 0;
    for (size_t i = 0; i < u.size(); ++i)
        sum += u[i] * v[i];
    return sum;
}

vector<Frac> scalar_mult(const Frac& c, const vector<Frac>& v) {
    vector<Frac> result(v.size());
    for (size_t i = 0; i < v.size(); ++i)
        result[i] = c * v[i];
    return result;
}

vector<Frac> vector_sub(const vector<Frac>& u, const vector<Frac>& v) {
    vector<Frac> result(u.size());
    for (size_t i = 0; i < u.size(); ++i)
        result[i] = u[i] - v[i];
    return result;
}

long long frac_round(const Frac& frac) {
    long long n = frac.numerator();
    long long d = frac.denominator();
    return (2 * n + d) / (2 * d);
}

// ─── Gram–Schmidt + LLL (Fixed to 3D) ────────────────────────────────────

struct GSResult {
    vector<vector<Frac>> mu;
    vector<vector<Frac>> Bstar;
    vector<Frac> normsq;
};

GSResult gram_schmidt(const vector<vector<Frac>>& B) {
    size_t n = B.size();
    vector<vector<Frac>> mu(n, vector<Frac>(n, 0));
    vector<vector<Frac>> Bstar(n);
    vector<Frac> normsq(n);

    for (size_t i = 0; i < n; ++i) {
        vector<Frac> v = B[i];
        for (size_t j = 0; j < i; ++j) {
            mu[i][j] = dot(B[i], Bstar[j]) / normsq[j];
            vector<Frac> sub = scalar_mult(mu[i][j], Bstar[j]);
            v = vector_sub(v, sub);
        }
        Bstar[i] = v;
        normsq[i] = dot(v, v);
        if (normsq[i] == 0)
            throw std::runtime_error("Input basis is linearly dependent.");
    }

    return {mu, Bstar, normsq};
}

vector<vector<Frac>> lll1(vector<vector<Frac>> B, Frac delta = Frac(9999, 10000)) {
    auto [mu, Bstar, normsq] = gram_schmidt(B);
    size_t k = 1;

    while (k < 3) {
        for (int j = (int)k - 1; j >= 0; --j) {
            long long q = frac_round(mu[k][j]);
            if (q != 0) {
                auto sub = scalar_mult(Frac(q), B[j]);
                B[k] = vector_sub(B[k], sub);
            }
        }
        std::tie(mu, Bstar, normsq) = gram_schmidt(B);

        if (normsq[k] >= (delta - mu[k][k - 1] * mu[k][k - 1]) * normsq[k - 1])
            ++k;
        else {
            std::swap(B[k], B[k - 1]);
            std::tie(mu, Bstar, normsq) = gram_schmidt(B);
            k = std::max<size_t>(k - 1, 1);
        }
    }

    return B;
}

// ─── Scaled LLL (Coppersmith Lattice) ───────────────────────────────────

vector<long long> lll_scaled(const vector<vector<long long>>& B, long long X, Frac delta = Frac(9999, 10000)) {
    Frac Xf(X);
    Frac X2 = Xf * Xf;

    vector<vector<Frac>> scaled = {
        {X2 * B[0][0], Xf * B[0][1], Frac(B[0][2])},
        {X2 * B[1][0], Xf * B[1][1], Frac(B[1][2])},
        {X2 * B[2][0], Xf * B[2][1], Frac(B[2][2])}
    };

    auto reduced = lll1(scaled, delta);
    Frac v0 = reduced[0][0];
    Frac v1 = reduced[0][1];
    Frac v2 = reduced[0][2];

    long long r0 = (v0 / X2).numerator() / (v0 / X2).denominator();
    long long r1 = (v1 / Xf).numerator() / (v1 / Xf).denominator();
    long long r2 = v2.numerator() / v2.denominator();

    return {r0, r1, r2};
}

// ─── Demo ────────────────────────────────────────────────────────────────

int main() {
    vector<vector<long long>> B = {
        {1, 0, 123},
        {0, 1, 456},
        {0, 0, 789}
    };
    long long X = 1000;

    auto v = lll_scaled(B, X);
    std::cout << "[" << v[0] << ", " << v[1] << ", " << v[2] << "]" << std::endl;
    return 0;
}