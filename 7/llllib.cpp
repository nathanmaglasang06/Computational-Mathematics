#include "llllib.h"
#include <stdexcept>
#include <algorithm>

// ─── Basic Vector Helpers ────────────────────────────────────────────────

Rational dot(const Vector& u, const Vector& v) {
    if (u.size() != v.size()) {
        throw std::invalid_argument("Vectors must have same length");
    }
    Rational sum = 0;
    for (size_t i = 0; i < u.size(); ++i) {
        sum += u[i] * v[i];
    }
    return sum;
}

Vector scalar_mult(const Rational& c, const Vector& v) {
    Vector result(v.size());
    for (size_t i = 0; i < v.size(); ++i) {
        result[i] = c * v[i];
    }
    return result;
}

Vector vector_sub(const Vector& u, const Vector& v) {
    if (u.size() != v.size()) {
        throw std::invalid_argument("Vectors must have same length");
    }
    Vector result(u.size());
    for (size_t i = 0; i < u.size(); ++i) {
        result[i] = u[i] - v[i];
    }
    return result;
}

mpz_class frac_round(const Rational& frac) {
    // Python's Fraction rounding: round half away from zero
    // For positive: (2*n + d) // (2*d)
    // For negative: we need to handle it carefully

    mpz_class n = frac.get_num();
    mpz_class d = frac.get_den();

    // Check the sign
    if (n >= 0) {
        // Positive: round half up
        return (2 * n + d) / (2 * d);
    } else {
        // Negative: round half down (away from zero)
        // This is equivalent to: -round(abs(frac))
        return (2 * n - d) / (2 * d);
    }
}

// ─── Gram–Schmidt + LLL ──────────────────────────────────────────────────

GramSchmidtResult gram_schmidt(const std::vector<Vector>& B) {
    size_t n = B.size();

    Matrix mu(n, Vector(n, Rational(0)));
    std::vector<Vector> Bstar(n);
    Vector normsq(n);

    for (size_t i = 0; i < n; ++i) {
        Vector v = B[i];

        for (size_t j = 0; j < i; ++j) {
            mu[i][j] = dot(B[i], Bstar[j]) / normsq[j];
            v = vector_sub(v, scalar_mult(mu[i][j], Bstar[j]));
        }

        Bstar[i] = v;
        normsq[i] = dot(v, v);

        if (normsq[i] == 0) {
            throw std::runtime_error("Input basis is linearly dependent.");
        }
    }

    return {mu, Bstar, normsq};
}

std::vector<Vector> lll1(std::vector<Vector> B, const Rational& delta) {
    // Convert all elements to Rational
    for (auto& row : B) {
        for (auto& elem : row) {
            elem.canonicalize();
        }
    }

    GramSchmidtResult gs_result = gram_schmidt(B);
    Matrix mu = gs_result.mu;
    std::vector<Vector> Bstar = gs_result.Bstar;
    Vector normsq = gs_result.normsq;

    size_t k = 1;

    while (k < 3) {
        // Size reduction
        for (int j = k - 1; j >= 0; --j) {
            mpz_class q = frac_round(mu[k][j]);
            if (q != 0) {
                Vector Bj_scaled = scalar_mult(Rational(q), B[j]);
                B[k] = vector_sub(B[k], Bj_scaled);
            }
        }

        gs_result = gram_schmidt(B);
        mu = gs_result.mu;
        Bstar = gs_result.Bstar;
        normsq = gs_result.normsq;

        // Lovász condition
        if (normsq[k] >= (delta - mu[k][k-1] * mu[k][k-1]) * normsq[k-1]) {
            k++;
        } else {
            std::swap(B[k], B[k-1]);
            gs_result = gram_schmidt(B);
            mu = gs_result.mu;
            Bstar = gs_result.Bstar;
            normsq = gs_result.normsq;
            k = std::max((int)k - 1, 1);
        }
    }

    return B;
}

// ─── Scaled LLL (Coppersmith Lattice) ───────────────────────────────────

std::vector<mpz_class> lll(const std::vector<std::vector<mpz_class>>& B,
                            const mpz_class& X,
                            const Rational& delta) {
    Rational X_rat(X);
    Rational X2 = X_rat * X_rat;

    // Scale the basis
    std::vector<Vector> scaled(3, Vector(3));
    for (int i = 0; i < 3; ++i) {
        scaled[i][0] = X2 * Rational(B[i][0]);
        scaled[i][1] = X_rat * Rational(B[i][1]);
        scaled[i][2] = Rational(B[i][2]);
    }

    // Run LLL
    auto reduced = lll1(scaled, delta);

    // Unscale the first vector
    Vector v0 = reduced[0];
    std::vector<mpz_class> result(3);

    Rational temp0 = v0[0] / X2;
    Rational temp1 = v0[1] / X_rat;

    result[0] = temp0.get_num();
    result[1] = temp1.get_num();
    result[2] = v0[2].get_num();

    return result;
}