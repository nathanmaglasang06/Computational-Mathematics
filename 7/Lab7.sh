#!/bin/bash

# Lab 7 Complete Setup Script
# This script sets up everything from scratch for the LLL/RSA lab

set -e  # Exit on any error

# Color codes for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Function to print colored output
print_status() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

# Function to check if a command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Detect OS
print_status "Detecting operating system..."
OS="unknown"
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    OS="linux"
    print_success "Detected Linux"
elif [[ "$OSTYPE" == "darwin"* ]]; then
    OS="macos"
    print_success "Detected macOS"
else
    print_error "Unsupported operating system: $OSTYPE"
    exit 1
fi

# Check and install Homebrew (macOS) or update package manager (Linux)
print_status "Checking package manager..."
if [[ "$OS" == "macos" ]]; then
    if ! command_exists brew; then
        print_warning "Homebrew not found. Installing Homebrew..."
        /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
        
        # Add Homebrew to PATH for Apple Silicon Macs
        if [[ $(uname -m) == "arm64" ]]; then
            echo 'eval "$(/opt/homebrew/bin/brew shellenv)"' >> ~/.zprofile
            eval "$(/opt/homebrew/bin/brew shellenv)"
        fi
        print_success "Homebrew installed"
    else
        print_success "Homebrew already installed"
    fi
elif [[ "$OS" == "linux" ]]; then
    if command_exists apt-get; then
        print_status "Using apt package manager"
        sudo apt-get update
    elif command_exists dnf; then
        print_status "Using dnf package manager"
        sudo dnf check-update || true
    elif command_exists yum; then
        print_status "Using yum package manager"
        sudo yum check-update || true
    else
        print_error "No supported package manager found (apt, dnf, yum)"
        exit 1
    fi
fi

# Install GCC/G++ compiler
print_status "Checking for C++ compiler..."
if ! command_exists g++; then
    print_warning "C++ compiler not found. Installing..."
    if [[ "$OS" == "macos" ]]; then
        # Install Xcode Command Line Tools
        xcode-select --install 2>/dev/null || true
        print_success "Xcode Command Line Tools installation initiated"
        print_warning "Please complete the Xcode installation dialog, then re-run this script"
        exit 0
    elif [[ "$OS" == "linux" ]]; then
        if command_exists apt-get; then
            sudo apt-get install -y build-essential
        elif command_exists dnf; then
            sudo dnf install -y gcc-c++ make
        elif command_exists yum; then
            sudo yum install -y gcc-c++ make
        fi
        print_success "C++ compiler installed"
    fi
else
    print_success "C++ compiler already installed ($(g++ --version | head -n1))"
fi

# Install CMake
print_status "Checking for CMake..."
if ! command_exists cmake; then
    print_warning "CMake not found. Installing..."
    if [[ "$OS" == "macos" ]]; then
        brew install cmake
    elif [[ "$OS" == "linux" ]]; then
        if command_exists apt-get; then
            sudo apt-get install -y cmake
        elif command_exists dnf; then
            sudo dnf install -y cmake
        elif command_exists yum; then
            sudo yum install -y cmake
        fi
    fi
    print_success "CMake installed"
else
    print_success "CMake already installed ($(cmake --version | head -n1))"
fi

# Install GMP library
print_status "Checking for GMP library..."
GMP_FOUND=false

if [[ "$OS" == "macos" ]]; then
    if brew list gmp &>/dev/null; then
        GMP_FOUND=true
    fi
elif [[ "$OS" == "linux" ]]; then
    if ldconfig -p | grep -q libgmp; then
        GMP_FOUND=true
    fi
fi

if [ "$GMP_FOUND" = false ]; then
    print_warning "GMP library not found. Installing..."
    if [[ "$OS" == "macos" ]]; then
        brew install gmp
    elif [[ "$OS" == "linux" ]]; then
        if command_exists apt-get; then
            sudo apt-get install -y libgmp-dev libgmpxx4ldbl
        elif command_exists dnf; then
            sudo dnf install -y gmp-devel
        elif command_exists yum; then
            sudo yum install -y gmp-devel
        fi
    fi
    print_success "GMP library installed"
else
    print_success "GMP library already installed"
fi

# Verify we're in the correct directory or create project structure
print_status "Setting up project directory..."
PROJECT_DIR="Lab7_RSA_Attack"

if [[ ! -f "CMakeLists.txt" ]] || [[ ! -f "llllib.h" ]]; then
    print_warning "Project files not found in current directory."
    print_status "Creating project structure in ./$PROJECT_DIR"
    mkdir -p "$PROJECT_DIR"
    cd "$PROJECT_DIR"
else
    print_success "Project files found in current directory"
    PROJECT_DIR="."
fi

# Create all necessary source files
print_status "Creating source files..."

# Create llllib.h
cat > llllib.h << 'EOF'
#ifndef LLLLIB_H
#define LLLLIB_H

#include <vector>
#include <gmpxx.h>

using Rational = mpq_class;
using Matrix = std::vector<std::vector<Rational>>;
using Vector = std::vector<Rational>;

Rational dot(const Vector& u, const Vector& v);
Vector scalar_mult(const Rational& c, const Vector& v);
Vector vector_sub(const Vector& u, const Vector& v);
mpz_class frac_round(const Rational& frac);

struct GramSchmidtResult {
    Matrix mu;
    std::vector<Vector> Bstar;
    Vector normsq;
};

GramSchmidtResult gram_schmidt(const std::vector<Vector>& B);
std::vector<Vector> lll1(std::vector<Vector> B, const Rational& delta = Rational(9999, 10000));
std::vector<mpz_class> lll(const std::vector<std::vector<mpz_class>>& B, 
                            const mpz_class& X, 
                            const Rational& delta = Rational(9999, 10000));

#endif
EOF
print_success "Created llllib.h"

# Create llllib.cpp
cat > llllib.cpp << 'EOF'
#include "llllib.h"
#include <stdexcept>
#include <algorithm>

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
    mpz_class n = frac.get_num();
    mpz_class d = frac.get_den();
    
    if (n >= 0) {
        return (2 * n + d) / (2 * d);
    } else {
        return (2 * n - d) / (2 * d);
    }
}

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

std::vector<mpz_class> lll(const std::vector<std::vector<mpz_class>>& B, 
                            const mpz_class& X, 
                            const Rational& delta) {
    Rational X_rat(X);
    Rational X2 = X_rat * X_rat;
    
    std::vector<Vector> scaled(3, Vector(3));
    for (int i = 0; i < 3; ++i) {
        scaled[i][0] = X2 * Rational(B[i][0]);
        scaled[i][1] = X_rat * Rational(B[i][1]);
        scaled[i][2] = Rational(B[i][2]);
    }
    
    auto reduced = lll1(scaled, delta);
    
    Vector v0 = reduced[0];
    std::vector<mpz_class> result(3);
    
    Rational temp0 = v0[0] / X2;
    Rational temp1 = v0[1] / X_rat;
    
    result[0] = temp0.get_num();
    result[1] = temp1.get_num();
    result[2] = v0[2].get_num();
    
    return result;
}
EOF
print_success "Created llllib.cpp"

# Create lll.cpp (Task 1 test)
cat > lll.cpp << 'EOF'
#include <iostream>
#include <gmpxx.h>
#include "llllib.h"

int main() {
    std::cout << "=== Task 1: Testing LLL Function ===" << std::endl;
    std::cout << std::endl;
    
    std::vector<std::vector<mpz_class>> M = {
        {mpz_class(52563), mpz_class(52456), mpz_class(71853)},
        {mpz_class(43532), mpz_class(76933), mpz_class(35257)},
        {mpz_class(36923), mpz_class(37276), mpz_class(42678)}
    };
    
    std::cout << "Testing with M = [[52563, 52456, 71853]," << std::endl;
    std::cout << "                  [43532, 76933, 35257]," << std::endl;
    std::cout << "                  [36923, 37276, 42678]]" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Testing with X = 1:" << std::endl;
    auto result1 = lll(M, mpz_class(1));
    std::cout << "Result: [" << result1[0] << ", " << result1[1] << ", " << result1[2] << "]" << std::endl;
    std::cout << "Expected: [5643, 6916, -15672]" << std::endl;
    
    // Verify result
    bool correct = (result1[0] == 5643 && result1[1] == 6916 && result1[2] == -15672);
    if (correct) {
        std::cout << "\n✓ TEST PASSED!" << std::endl;
        return 0;
    } else {
        std::cout << "\n✗ TEST FAILED!" << std::endl;
        return 1;
    }
}
EOF
print_success "Created lll.cpp"

# Create main.cpp (your provided code)
cat > main.cpp << 'MAINEOF'
#include <iostream>
#include <fstream>
#include <sstream>
#include <gmpxx.h>
#include "llllib.h"
using namespace std;

//TASK 1 is verified using lll.cpp

struct txtFile {
    mpz_class n;
    mpz_class d;
    mpz_class p0;
    mpz_class X;
};
txtFile readLabFile(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        throw runtime_error("Could not open file: " + filename);
    }

    txtFile data;
    string content;
    getline(file, content);
    file.close();
    vector<string> numbers;
    stringstream ss(content);
    string token;

    while (getline(ss, token, ',')) {
        token.erase(0, token.find_first_not_of(" \t\r\n"));
        token.erase(token.find_last_not_of(" \t\r\n") + 1);
        if (!token.empty()) {
            numbers.push_back(token);
        }
    }

    if (numbers.size() != 4) {
        throw runtime_error("Expected 4 numbers in file, found " + to_string(numbers.size()));
    }

    try {
        data.n = mpz_class(numbers[0], 10);
        data.d = mpz_class(numbers[1], 10);
        data.p0 = mpz_class(numbers[2], 10);
        data.X = mpz_class(numbers[3], 10);

        cout << "Successfully read all values:" << endl;
        cout << "  n:  " << numbers[0].length() << " digits\n";
        cout << "  d:  " << numbers[1].length() << " digits\n";
        cout << "  p0: " << numbers[2].length() << " digits\n";
        cout << "  X:  " << numbers[3].length() << " digits\n";
    } catch (const exception& e) {
        throw runtime_error("Failed to parse numbers: " + string(e.what()));
    }

    return data;
}
int main() {
    cout << "Task 1 - Run lll.cpp";
    cout << "\n\nTask 2\nReading File\n";
    txtFile data = readLabFile("lab07-2.txt");
    cout << "Data loaded successfully." << endl;
    cout << "n = " << data.n.get_str().substr(0, 50) << "\n";
    cout << "d = " << data.d.get_str().substr(0, 50) << "\n";
    cout << "p0 = " << data.p0.get_str().substr(0, 50) << "\n";
    cout << "X = " << data.X.get_str().substr(0, 50) << "\n";

    mpz_class p0_squared = data.p0 * data.p0;
    cout << "p0_squared = " << p0_squared << "\n";

    vector<vector<mpz_class>> M= {
        {mpz_class(1), 2 * data.p0, p0_squared},
        {mpz_class(0), data.n, mpz_class(0)},
        {mpz_class(0), mpz_class(0), data.n}
    };

    auto result = lll(M, data.X);

    mpz_class a_coef = result[0];
    mpz_class b_coef = result[1];
    mpz_class c_coef = result[2];
    cout << "a_coef = " << a_coef << "\n";
    cout << "b_coef = " << b_coef << "\n";
    cout << "c_coef = " << c_coef << "\n";

    mpz_class x1;
    mpz_class x2;

    if (a_coef == 0) {
        cout << "Linear equation detected\n";
        x1 = -c_coef / b_coef;
        x2 = x1;
    } else {
        cout << "Quadratic equation detected\n";
        mpz_class discriminant = b_coef * b_coef - 4 * a_coef * c_coef;

        mpz_class sqrt_disc;
        mpz_sqrt(sqrt_disc.get_mpz_t(), discriminant.get_mpz_t());

        x1 = (-b_coef + sqrt_disc) / (2 * a_coef);
        x2 = (-b_coef - sqrt_disc) / (2 * a_coef);
    }

    cout << "x1 = " << x1 << "\n";
    cout << "x2 = " << x2 << "\n";

    mpz_class p;
    mpz_class q;
    mpz_class p1 = data.p0 + x1;
    mpz_class p2 = data.p0 + x2;

    cout << "\nTesting solutions...\n";
    if (data.n % p1 == 0) {
        p = p1;
        q = data.n / p;
        cout << "Found p using x1!\n";
    } else if (data.n % p2 == 0) {
        p = p2;
        q = data.n / p;
        cout << "Found p using x2!\n";
    } else {
        cout << "Error: Neither solution worked!\n";
        return 1;
    }

    cout << "\n\nTask 3\n\nResults:\n";
    cout << "p = " << p.get_str().substr(0, 50) << "...\n";
    cout << "q = " << q.get_str().substr(0, 50) << "...\n";
    cout << "Verification: p * q == n?: " << ((p * q == data.n) ? "YES" : "NO") << "\n";

    mpz_class phi_n = (p - 1) * (q - 1);
    mpz_class e;
    mpz_invert(e.get_mpz_t(), data.d.get_mpz_t(), phi_n.get_mpz_t());
    cout << "Private key e = " << e << endl;
    cout << "\n\nTask 4: \n\n";
    string ciphertext =
    "1347109129531723028124112099668773654304403275978770987077622835039"
    "476263200052469838716773837201990422887230376844673118639664946920257"
    "412572674032804827672883663770808438611432877282813189104505969282863"
    "82490976039330443025403402327064842218613916969030093850521554201677"
    "08989584905130391807125687193335219772141406408065758504225594517716"
    "94752199551733467965922828467990130549756809651487119750933063618757"
    "58045989790717026298910548909677220085378300664869216371551545980276"
    "613276422739168414226882068876518222401219021945480848476428697945207"
    "08754188918114070811605644462955226763175079872803044435";

    mpz_class c(ciphertext);

    mpz_class m;
    mpz_powm(m.get_mpz_t(), c.get_mpz_t(), e.get_mpz_t(), data.n.get_mpz_t());

    cout << "Decrypted Message (int format) " << m << "\n\n";

    string message;
    mpz_class temp = m;

    while (temp > 0) {
        mpz_class remainder = temp % 256;
        char ch = static_cast<char>(remainder.get_ui());

        message = ch + message;

        temp /= 256;

    }

    cout << "Decoded Message:\n " << message << "\n";
    if (message == "ALICE, THIS IS BOB. PLEASE GENERATE NEW PRIMES FOR YOUR RSA") {
        cout << "\n\nMessage Correct YAY";
        return 0;
    } else {
        cout << "\n\n:(";
        return 1;
    }
}
MAINEOF
print_success "Created main.cpp"

# Create lab07-2.txt (data file)
cat > lab07-2.txt << 'EOF'
25680014825643351839899824073936814726678360594109255258716416852859283104507907881729376052818102493847187260696409822813455702260103016806108323857134841640458276973875066048992205968606696292944912790839330112475660515878758447507393523965581919529003418088230287326341283600014884299427811068002678908331248427624497599089566297042603458317535949850019038628219700800874179339457854277025066576815211720012625565459173886823944855035790553196178730338109806178008739500728020265744266004843228518826731660992237008933214084061420364630985603265903662289440384876131879613011832561419385086547091891, 65537, 9136258188724795982164812068439019290403695113434826661160636220128125654906958492952056394587249036772165119098141201993480454245626780095905119193259335286888778340874863936298145021883512421780678433503532081573147845811802238399047974851698483744506254766133894698543702381787590257162143591455646, 1766847064778384329583297500742918515827483896875618958121606201292619776
EOF
print_success "Created lab07-2.txt"

# Create CMakeLists.txt
cat > CMakeLists.txt << 'EOF'
cmake_minimum_required(VERSION 3.10)
project(Lab7_LLL)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(APPLE)
    set(HOMEBREW_PREFIX "/opt/homebrew" CACHE PATH "Homebrew prefix")
    if(NOT EXISTS ${HOMEBREW_PREFIX})
        set(HOMEBREW_PREFIX "/usr/local")
    endif()
    
    set(GMP_INCLUDE_DIR "${HOMEBREW_PREFIX}/include" CACHE PATH "GMP include directory")
    set(GMP_LIBRARY_DIR "${HOMEBREW_PREFIX}/lib" CACHE PATH "GMP library directory")
    
    include_directories(${GMP_INCLUDE_DIR})
    
    find_library(GMP_LIBRARY NAMES gmp PATHS ${GMP_LIBRARY_DIR} NO_DEFAULT_PATH)
    find_library(GMPXX_LIBRARY NAMES gmpxx PATHS ${GMP_LIBRARY_DIR} NO_DEFAULT_PATH)
else()
    find_library(GMP_LIBRARY NAMES gmp)
    find_library(GMPXX_LIBRARY NAMES gmpxx)
endif()

if(NOT GMP_LIBRARY)
    message(FATAL_ERROR "GMP library not found")
endif()

if(NOT GMPXX_LIBRARY)
    message(FATAL_ERROR "GMPXX library not found")
endif()

include_directories(${CMAKE_SOURCE_DIR})

add_library(llllib STATIC llllib.cpp)
target_link_libraries(llllib ${GMPXX_LIBRARY} ${GMP_LIBRARY})

add_executable(lab7 main.cpp)
target_link_libraries(lab7 llllib ${GMPXX_LIBRARY} ${GMP_LIBRARY})

add_executable(Task1 lll.cpp)
target_link_libraries(Task1 llllib ${GMPXX_LIBRARY} ${GMP_LIBRARY})

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(llllib PRIVATE -Wall -Wextra)
    target_compile_options(lab7 PRIVATE -Wall -Wextra)
    target_compile_options(Task1 PRIVATE -Wall -Wextra)
endif()
EOF
print_success "Created CMakeLists.txt"

# Create README
cat > README.md << 'EOF'
# Lab 7: RSA Attack using Coppersmith's Method

This project implements an attack on RSA encryption using the LLL lattice reduction algorithm.

## Files
- `llllib.h` / `llllib.cpp` - LLL algorithm implementation
- `lll.cpp` - Task 1 verification
- `main.cpp` - Tasks 2-4 (factoring RSA, finding private key, decrypting message)
- `lab07-2.txt` - Input data (n, d, p0, X)
- `CMakeLists.txt` - Build configuration

## Building
```bash
mkdir -p build
cd build
cmake ..
make
```

## Running
```bash
# Task 1 verification
./Task1

# Full lab (Tasks 2-4)
./lab7
```

## Expected Output
Task 1 should output: [5643, 6916, -15672]
Task 4 should decrypt to: "ALICE, THIS IS BOB. PLEASE GENERATE NEW PRIMES FOR YOUR RSA"
EOF
print_success "Created README.md"

# Build the project
print_status "Building the project..."
mkdir -p build
cd build

print_status "Running CMake..."
cmake .. || {
    print_error "CMake configuration failed"
    exit 1
}

print_status "Compiling..."
make || {
    print_error "Compilation failed"
    exit 1
}

print_success "Build completed successfully!"

# Run Task 1 verification
echo ""
print_status "Running Task 1 verification (lll.cpp)..."
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
./Task1
TASK1_EXIT=$?
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"

if [ $TASK1_EXIT -eq 0 ]; then
    print_success "Task 1 verification PASSED!"
    
    echo ""
    print_status "Running main lab program (main.cpp)..."
    echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    ./lab7
    MAIN_EXIT=$?
    echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    
    if [ $MAIN_EXIT -eq 0 ]; then
        print_success "All tasks completed successfully!"
        print_success "Message decryption verified!"
    else
        print_error "Main program failed"
        exit 1
    fi
else
    print_error "Task 1 verification FAILED!"
    print_error "Expected result: [5643, 6916, -15672]"
    print_warning "Main program will not run until Task 1 passes"
    exit 1
fi

# Print summary
echo ""
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
print_success "Setup and execution complete!"
echo ""
echo "Project location: $(pwd)/.."
echo ""
echo "To run again:"
echo "  cd $(pwd)"
echo "  ./Task1 or ./build/Task1   # Run Task 1 verification"
echo "  ./lab7 or ./build/lab7     # Run full lab (Tasks 2-4)"
echo ""
print_warning "Note: These executables are NOT portable to other machines!"
print_warning "They depend on shared libraries (GMP) installed on this system."
echo ""
echo "To run on another machine, use one of these options:"
echo "  1. Run this setup script on the other machine"
echo "  2. Create a portable version with static linking (see below)"
echo ""
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"

