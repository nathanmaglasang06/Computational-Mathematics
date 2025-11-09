# llllib - LLL Lattice Reduction Library

## NAME
**llllib** - Lenstra-Lenstra-Lovász (LLL) lattice basis reduction library

## SYNOPSIS
```cpp
#include "llllib.h"

std::vector<long long> lll_scaled(
    const std::vector<std::vector<long long>>& B, 
    long long X, 
    Frac delta = Frac(9999, 10000)
);

std::vector<std::vector<Frac>> lll1(
    std::vector<std::vector<Frac>> B, 
    Frac delta = Frac(9999, 10000)
);
```

## DESCRIPTION
The **llllib** library provides implementations of the LLL lattice reduction algorithm for cryptographic and computational applications. It is specifically optimized for 3×3 lattice bases and uses exact rational arithmetic via Boost's `rational<long long>`.

### Key Features
- Exact rational arithmetic (no floating-point errors)
- Specialized scaled LLL for Coppersmith-style attacks
- Standard LLL reduction for general lattices
- Fixed 3×3 dimension for optimal performance

## FUNCTIONS

### lll_scaled
```cpp
std::vector<long long> lll_scaled(
    const std::vector<std::vector<long long>>& B,
    long long X,
    Frac delta = Frac(9999, 10000)
);
```

Performs scaled LLL reduction on a 3×3 integer basis matrix. This is primarily used in Coppersmith-style attacks where different columns need different scaling factors.

**Parameters:**
- `B` - A 3×3 basis matrix (outer vector has 3 rows, each inner vector has 3 columns)
- `X` - Scaling parameter (typically a bound on the solution size)
- `delta` - LLL reduction parameter (default: 0.9999, range: 0.25 < δ < 1)

**Returns:**
- A vector `[r0, r1, r2]` representing the shortest lattice vector after unscaling

**Scaling Behavior:**
- Column 0 is scaled by X²
- Column 1 is scaled by X
- Column 2 is unscaled

**Example:**
```cpp
#include "llllib.h"
#include <iostream>
#include <vector>

int main() {
    // Coppersmith lattice for finding small roots
    std::vector<std::vector<long long>> basis = {
        {1, 0, 123},   // Row 0
        {0, 1, 456},   // Row 1
        {0, 0, 789}    // Row 2
    };
    
    long long X = 1000;  // Solution bound
    
    auto result = lll_scaled(basis, X);
    
    std::cout << "Reduced vector: [" 
              << result[0] << ", " 
              << result[1] << ", " 
              << result[2] << "]" << std::endl;
    
    return 0;
}
```

### lll1
```cpp
std::vector<std::vector<Frac>> lll1(
    std::vector<std::vector<Frac>> B,
    Frac delta = Frac(9999, 10000)
);
```

Performs standard LLL reduction on a 3×3 basis of rational vectors.

**Parameters:**
- `B` - A 3×3 basis matrix with `Frac` (rational) entries
- `delta` - LLL reduction parameter (default: 0.9999, range: 0.25 < δ < 1)

**Returns:**
- An LLL-reduced basis matrix

**Example:**
```cpp
#include "llllib.h"
#include <boost/rational.hpp>
#include <vector>

using Frac = boost::rational<long long>;

int main() {
    std::vector<std::vector<Frac>> basis = {
        {Frac(3), Frac(1), Frac(0)},
        {Frac(1), Frac(2), Frac(1)},
        {Frac(0), Frac(1), Frac(3)}
    };
    
    auto reduced = lll1(basis);
    
    // reduced now contains the LLL-reduced basis
    for (const auto& row : reduced) {
        for (const auto& val : row) {
            std::cout << val << " ";
        }
        std::cout << std::endl;
    }
    
    return 0;
}
```

## TYPES

### Frac
```cpp
using Frac = boost::rational<long long>;
```

A rational number type supporting exact arithmetic. Used throughout the library to avoid floating-point precision issues.

**Construction:**
```cpp
Frac a(3, 4);      // 3/4
Frac b(5);         // 5/1
Frac c = a + b;    // 23/4
```

## DELTA PARAMETER

The `delta` parameter controls the quality of the reduction:

- **δ = 0.25**: Minimal reduction (fast, poor quality)
- **δ = 0.75**: Standard LLL (good balance)
- **δ = 0.99**: High quality (slower, better reduction)
- **δ = 0.9999**: Near-optimal (default, cryptographic quality)

Higher values produce shorter basis vectors but require more computation.

## BUILDING

### CMakeLists.txt
```cmake
cmake_minimum_required(VERSION 3.21)
project(MyProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

find_package(Boost REQUIRED)

add_library(llllib llllib.cpp)
target_link_libraries(llllib PUBLIC Boost::boost)

add_executable(myapp main.cpp)
target_link_libraries(myapp PRIVATE llllib)
```

### Compilation
```bash
mkdir build && cd build
cmake ..
make
```

## REQUIREMENTS

- **C++17** or later (for structured bindings)
- **Boost** library (for `boost::rational`)
- **CMake 3.21+** (or adjust version in CMakeLists.txt)

## LIMITATIONS

- Fixed to 3×3 matrices only
- No support for larger dimensions
- Integer overflow possible with very large inputs (values beyond ~10^18)

## ERROR HANDLING

The library throws `std::runtime_error` in the following cases:

- **Linearly dependent basis**: If the input basis vectors are not linearly independent
  ```cpp
  try {
      auto result = lll_scaled(basis, X);
  } catch (const std::runtime_error& e) {
      std::cerr << "Error: " << e.what() << std::endl;
  }
  ```

## APPLICATIONS

### Cryptography
- Breaking RSA with partial key exposure (Coppersmith's attack)
- Finding small solutions to polynomial equations modulo N
- Subset sum problems

### Number Theory
- Finding short vectors in lattices
- Simultaneous Diophantine approximation
- Integer relation detection

## SEE ALSO

- Lenstra, A.K., Lenstra, H.W., Lovász, L. (1982). "Factoring polynomials with rational coefficients"
- Coppersmith, D. (1996). "Finding a small root of a univariate modular equation"

## AUTHOR
Nathan Maglasang (7/11/2025)

## NOTES

The default `delta = 9999/10000` provides near-optimal reduction suitable for cryptographic applications while maintaining reasonable performance.

For debugging, you can print rational numbers:
```cpp
Frac x(22, 7);
std::cout << x << std::endl;  // Outputs: 22/7
```
