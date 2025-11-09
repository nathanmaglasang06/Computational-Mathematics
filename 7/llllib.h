#ifndef LLLLIB_H
#define LLLLIB_H

#include <vector>
#include <boost/rational.hpp>

using Frac = boost::rational<long long>;

/**
 * Performs scaled LLL lattice reduction for a 3x3 basis matrix.
 *
 * @param B 3x3 basis matrix as vector of vectors
 * @param X Scaling parameter
 * @param delta LLL reduction parameter (default: 0.9999)
 * @return Reduced lattice vector as [r0, r1, r2]
 */
std::vector<long long> lll_scaled(
    const std::vector<std::vector<long long>>& B,
    long long X,
    Frac delta = Frac(9999, 10000)
);

/**
 * Performs standard LLL reduction on a basis of fractional vectors.
 *
 * @param B Basis matrix (3x3 for this implementation)
 * @param delta LLL reduction parameter (default: 0.9999)
 * @return LLL-reduced basis
 */
std::vector<std::vector<Frac>> lll1(
    std::vector<std::vector<Frac>> B,
    Frac delta = Frac(9999, 10000)
);

#endif // LLLLIB_H