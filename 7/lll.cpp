#include <iostream>
#include <iomanip>
#include <gmpxx.h>
#include "llllib.h"

void printVector(const std::vector<mpz_class>& v, const std::string& name) {
    std::cout << name << " = [";
    for (size_t i = 0; i < v.size(); i++) {
        std::cout << v[i];
        if (i < v.size() - 1) std::cout << ", ";
    }
    std::cout << "]" << std::endl;
}

void printRationalVector(const Vector& v, const std::string& name) {
    std::cout << name << " = [";
    for (size_t i = 0; i < v.size(); i++) {
        std::cout << v[i];
        if (i < v.size() - 1) std::cout << ", ";
    }
    std::cout << "]" << std::endl;
}

void printMatrix(const std::vector<Vector>& M, const std::string& name) {
    std::cout << name << ":" << std::endl;
    for (size_t i = 0; i < M.size(); i++) {
        std::cout << "  [";
        for (size_t j = 0; j < M[i].size(); j++) {
            std::cout << M[i][j];
            if (j < M[i].size() - 1) std::cout << ", ";
        }
        std::cout << "]" << std::endl;
    }
}

void testBasicOperations() {
    std::cout << "=== Testing Basic Vector Operations ===" << std::endl;

    Vector u = {Rational(1), Rational(2), Rational(3)};
    Vector v = {Rational(4), Rational(5), Rational(6)};

    printRationalVector(u, "u");
    printRationalVector(v, "v");

    Rational d = dot(u, v);
    std::cout << "dot(u, v) = " << d << std::endl;
    std::cout << "Expected: 1*4 + 2*5 + 3*6 = 32" << std::endl;

    Vector scaled = scalar_mult(Rational(2), u);
    printRationalVector(scaled, "2 * u");

    Vector diff = vector_sub(v, u);
    printRationalVector(diff, "v - u");
    std::cout << std::endl;
}

void testFracRound() {
    std::cout << "=== Testing frac_round ===" << std::endl;

    std::vector<std::pair<Rational, int>> tests = {
        {Rational(5, 2), 3},      // 2.5 -> 3 (round half up)
        {Rational(-5, 2), -3},    // -2.5 -> -3
        {Rational(7, 3), 2},      // 2.333... -> 2
        {Rational(8, 3), 3},      // 2.666... -> 3
        {Rational(1, 2), 1},      // 0.5 -> 1
        {Rational(-1, 2), -1}     // -0.5 -> -1
    };

    for (const auto& [frac, expected] : tests) {
        mpz_class result = frac_round(frac);
        std::cout << "frac_round(" << frac << ") = " << result
                  << " (expected: " << expected << ")"
                  << (result == expected ? " ✓" : " ✗") << std::endl;
    }
    std::cout << std::endl;
}

void testGramSchmidt() {
    std::cout << "=== Testing Gram-Schmidt ===" << std::endl;

    std::vector<Vector> B = {
        {Rational(3), Rational(1), Rational(0)},
        {Rational(2), Rational(2), Rational(0)},
        {Rational(0), Rational(0), Rational(1)}
    };

    printMatrix(B, "Input basis B");

    auto [mu, Bstar, normsq] = gram_schmidt(B);

    std::cout << "\nGram-Schmidt orthogonalization:" << std::endl;
    printMatrix(Bstar, "B* (orthogonal basis)");

    std::cout << "\nNorm squared:" << std::endl;
    for (size_t i = 0; i < normsq.size(); i++) {
        std::cout << "  ||B*[" << i << "]||^2 = " << normsq[i] << std::endl;
    }

    std::cout << "\nMu coefficients:" << std::endl;
    for (size_t i = 0; i < mu.size(); i++) {
        for (size_t j = 0; j < mu[i].size(); j++) {
            if (j < i) {
                std::cout << "  mu[" << i << "][" << j << "] = " << mu[i][j] << std::endl;
            }
        }
    }
    std::cout << std::endl;
}

void testLLL1Direct() {
    std::cout << "=== Testing lll1 (direct, no scaling) ===" << std::endl;

    // Simple test case
    std::vector<Vector> B = {
        {Rational(1), Rational(1), Rational(1)},
        {Rational(-1), Rational(0), Rational(2)},
        {Rational(3), Rational(5), Rational(6)}
    };

    printMatrix(B, "Input basis B");

    auto reduced = lll1(B);

    printMatrix(reduced, "LLL-reduced basis");

    // Check norms
    std::cout << "\nVector norms:" << std::endl;
    for (size_t i = 0; i < reduced.size(); i++) {
        Rational norm = dot(reduced[i], reduced[i]);
        std::cout << "  ||v[" << i << "]||^2 = " << norm << std::endl;
    }
    std::cout << std::endl;
}

void testScaledLLL() {
    std::cout << "=== Testing Scaled LLL (Task 1 example) ===" << std::endl;

    std::vector<std::vector<mpz_class>> M = {
        {mpz_class(52563), mpz_class(52456), mpz_class(71853)},
        {mpz_class(43532), mpz_class(76933), mpz_class(35257)},
        {mpz_class(36923), mpz_class(37276), mpz_class(42678)}
    };

    std::cout << "Input matrix M:" << std::endl;
    for (const auto& row : M) {
        std::cout << "  [" << row[0] << ", " << row[1] << ", " << row[2] << "]" << std::endl;
    }

    // Test without scaling first (X=1)
    std::cout << "\n--- Testing with X = 1 (no effective scaling) ---" << std::endl;
    auto result1 = lll(M, mpz_class(1));
    printVector(result1, "Result");
    std::cout << "Expected: [5643, 6916, -15672]" << std::endl;

    // Now let's manually trace through what the scaling does
    std::cout << "\n--- Manual trace of scaling process ---" << std::endl;
    mpz_class X = 1;
    mpz_class X2 = X * X;

    std::cout << "Scaling factors: X^2 = " << X2 << ", X = " << X << ", 1 = 1" << std::endl;

    std::vector<Vector> scaled(3, Vector(3));
    for (int i = 0; i < 3; ++i) {
        scaled[i][0] = Rational(X2 * M[i][0]);
        scaled[i][1] = Rational(X * M[i][1]);
        scaled[i][2] = Rational(M[i][2]);
    }

    printMatrix(scaled, "Scaled matrix");

    std::cout << "\nRunning LLL on scaled matrix..." << std::endl;
    auto reduced = lll1(scaled);
    printMatrix(reduced, "LLL reduced (scaled)");

    std::cout << "\nUnscaling first vector..." << std::endl;
    Vector v0 = reduced[0];
    Rational temp0 = v0[0] / Rational(X2);
    Rational temp1 = v0[1] / Rational(X);
    Rational temp2 = v0[2];

    std::cout << "v0[0] / X^2 = " << v0[0] << " / " << X2 << " = " << temp0 << std::endl;
    std::cout << "v0[1] / X = " << v0[1] << " / " << X << " = " << temp1 << std::endl;
    std::cout << "v0[2] = " << temp2 << std::endl;

    std::cout << "\nConverting to integers..." << std::endl;
    mpz_class r0 = temp0.get_num();
    mpz_class r1 = temp1.get_num();
    mpz_class r2 = temp2.get_num();

    std::cout << "Result: [" << r0 << ", " << r1 << ", " << r2 << "]" << std::endl;
    std::cout << "Expected: [5643, 6916, -15672]" << std::endl;

    // Check if it's a valid combination
    std::cout << "\n--- Verification ---" << std::endl;
    std::cout << "Checking if result is an integer combination of input vectors..." << std::endl;

    std::cout << std::endl;
}

int main() {
    std::cout << std::fixed << std::setprecision(6);

    testBasicOperations();
    testFracRound();
    testGramSchmidt();
    testLLL1Direct();
    testScaledLLL();

    return 0;
}