#ifndef LLLLIB_H
#define LLLLIB_H

#include <vector>
#include <gmpxx.h>

// Type aliases for clarity
using Rational = mpq_class;
using Matrix = std::vector<std::vector<Rational>>;
using Vector = std::vector<Rational>;

// ─── Basic Vector Helpers ────────────────────────────────────────────────

/**
 * Exact dot product of two same-length vectors.
 */
Rational dot(const Vector& u, const Vector& v);

/**
 * Multiply vector v by scalar c.
 */
Vector scalar_mult(const Rational& c, const Vector& v);

/**
 * Return u − v component-wise.
 */
Vector vector_sub(const Vector& u, const Vector& v);

/**
 * Round Rational to nearest integer without float conversion.
 */
mpz_class frac_round(const Rational& frac);

// ─── Gram–Schmidt + LLL ──────────────────────────────────────────────────

/**
 * Gram–Schmidt orthogonalization for a 3-vector basis B.
 * Returns tuple of (mu, B*, normsquared) with exact Rationals.
 */
struct GramSchmidtResult {
    Matrix mu;
    std::vector<Vector> Bstar;
    Vector normsq;
};

GramSchmidtResult gram_schmidt(const std::vector<Vector>& B);

/**
 * 3-dimensional LLL reduction using exact Rationals.
 *
 * @param B 3×3 basis – each inner vector is a lattice vector length 3.
 * @param delta Lovász parameter (default 0.9999 ≈ very strong reduction).
 * @return LLL-reduced basis (not necessarily shortest first).
 */
std::vector<Vector> lll1(std::vector<Vector> B, const Rational& delta = Rational(9999, 10000));

/**
 * Apply diagonal scaling diag(X², X, 1) to a 3×3 basis, run LLL,
 * then unscale the first vector so its coordinates are:
 *     [v0 / X²,  v1 / X,  v2]
 *
 * @param B 3×3 integer matrix [[a,b,c], [d,e,f], [g,h,i]].
 * @param X Scaling parameter (e.g. the small-root bound in Coppersmith).
 * @param delta Lovász parameter (default 0.9999).
 * @return The first vector of the reduced basis, unscaled as integers.
 */
std::vector<mpz_class> lll(const std::vector<std::vector<mpz_class>>& B,
                            const mpz_class& X,
                            const Rational& delta = Rational(9999, 10000));

#endif // LLLLIB_H