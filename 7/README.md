# Lab 7 Part 2: LLL Algorithm for RSA Attack

This project implements the LLL (Lenstra-Lenstra-Lovász) lattice reduction algorithm in C++ for use in Coppersmith's attack on RSA encryption.

## Overview

This implementation recreates the Python LLL library in C++ using the GMP (GNU Multiple Precision) library for exact arbitrary-precision arithmetic. The LLL algorithm is used to find small integer combinations of lattice vectors, which enables breaking RSA when partial information about prime factors is known.

## Requirements

- C++17 or later
- CMake 3.10 or later
- GMP library (libgmp-dev)
- GMPXX library (C++ bindings for GMP)

### Installing Dependencies

**Ubuntu/Debian:**
```bash
sudo apt-get install libgmp-dev libgmpxx4ldbl
```

**macOS (using Homebrew):**
```bash
brew install gmp
```

**Fedora/RHEL:**
```bash
sudo dnf install gmp-devel
```

## Project Structure

```
.
├── CMakeLists.txt       # CMake build configuration
├── README.md            # This file
├── llllib.h             # Header file with LLL function declarations
├── llllib.cpp           # Implementation of LLL algorithm
├── main.cpp             # Main program for Lab 7 tasks
├── lll.cpp              # Test program for Task 1
├── lab07-2.txt          # Input data file (n, d, p0, X)
└── build/               # Build directory (created by CMake)
```

## Building the Project

1. Create and enter the build directory:
```bash
mkdir -p build
cd build
```

2. Run CMake to generate build files:
```bash
cmake ..
```

3. Compile the project:
```bash
make
```

This will create two executables:
- `lab7` - Main program that solves all lab tasks
- `Task1` - Test program for verifying LLL implementation

## Running the Programs

### Task 1: Testing LLL Implementation

From the build directory:
```bash
./Task1
```

This will test the LLL function with the example matrix and different X values.

### Tasks 2-4: Complete Lab Solution

From the build directory:
```bash
./lab7
```

This will:
1. Read the data from `lab07-2.txt`
2. Use LLL to find the prime factors p and q of n
3. Compute Alice's private key e
4. Decrypt and decode the intercepted message

## How It Works

### LLL Algorithm

The LLL algorithm performs lattice basis reduction to find short vectors in a lattice. Key components:

1. **Gram-Schmidt Orthogonalization**: Computes orthogonal basis and projection coefficients
2. **Size Reduction**: Ensures basis vectors are nearly orthogonal
3. **Lovász Condition**: Swaps vectors when they violate a length condition

### Coppersmith's Attack

The attack works when we know an approximation p₀ to one of the primes p:

1. Set up lattice with basis vectors [1, 2p₀, p₀²], [0, n, 0], [0, 0, n]
2. Apply LLL with scaling parameter X to find coefficients a, b, c
3. Solve quadratic equation ax² + bx + c = 0 to find x
4. Recover p = p₀ + x and q = n/p
5. Compute φ(n) = (p-1)(q-1)
6. Find private key e ≡ d⁻¹ (mod φ(n))
7. Decrypt message m ≡ c^e (mod n)

## API Reference

### Main Functions

```cpp
// Apply scaled LLL reduction
std::vector<mpz_class> lll(
    const std::vector<std::vector<mpz_class>>& B,
    const mpz_class& X,
    const Rational& delta = Rational(9999, 10000)
);

// Standard LLL reduction
std::vector<Vector> lll1(
    std::vector<Vector> B,
    const Rational& delta = Rational(9999, 10000)
);

// Gram-Schmidt orthogonalization
GramSchmidtResult gram_schmidt(const std::vector<Vector>& B);
```

### Helper Functions

```cpp
Rational dot(const Vector& u, const Vector& v);
Vector scalar_mult(const Rational& c, const Vector& v);
Vector vector_sub(const Vector& u, const Vector& v);
mpz_class frac_round(const Rational& frac);
```

## Notes

- All arithmetic is performed using exact rational numbers (GMP's `mpq_class`)
- The implementation is fixed to 3-dimensional lattices as required for the lab
- The default delta parameter (0.9999) provides very strong reduction
- Input data should be in the format specified in lab07-2.txt

## Troubleshooting

**Linker errors about GMP:**
- Ensure libgmp-dev and libgmpxx4ldbl are installed
- Check that CMake found the GMP libraries (look for messages during cmake configuration)

**File not found errors:**
- Ensure lab07-2.txt is in the same directory as the executable
- Or run the executable from the project root directory

**Compilation errors:**
- Verify you have a C++17 compatible compiler
- Check that all source files are in the correct locations

## License

This is educational software created for ZSPS2115 Computational Mathematics and Applied Cryptography.