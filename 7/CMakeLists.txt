cmake_minimum_required(VERSION 3.10)
project(Lab7_LLL)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Explicitly set GMP paths for Homebrew on macOS
if(APPLE)
    # Try common Homebrew locations
    set(HOMEBREW_PREFIX "/opt/homebrew" CACHE PATH "Homebrew prefix")
    if(NOT EXISTS ${HOMEBREW_PREFIX})
        set(HOMEBREW_PREFIX "/usr/local")
    endif()

    set(GMP_INCLUDE_DIR "${HOMEBREW_PREFIX}/include" CACHE PATH "GMP include directory")
    set(GMP_LIBRARY_DIR "${HOMEBREW_PREFIX}/lib" CACHE PATH "GMP library directory")

    # Add include directory
    include_directories(${GMP_INCLUDE_DIR})

    # Find libraries
    find_library(GMP_LIBRARY NAMES gmp PATHS ${GMP_LIBRARY_DIR} NO_DEFAULT_PATH)
    find_library(GMPXX_LIBRARY NAMES gmpxx PATHS ${GMP_LIBRARY_DIR} NO_DEFAULT_PATH)
else()
    # Linux/other systems
    find_library(GMP_LIBRARY NAMES gmp)
    find_library(GMPXX_LIBRARY NAMES gmpxx)
endif()

# Check if libraries were found
if(NOT GMP_LIBRARY)
    message(FATAL_ERROR "GMP library not found. Please install it:\n"
            "  macOS: brew install gmp\n"
            "  Ubuntu: sudo apt-get install libgmp-dev\n"
            "  Then try: rm -rf * && cmake ..")
endif()

if(NOT GMPXX_LIBRARY)
    message(FATAL_ERROR "GMPXX library not found. Please install it:\n"
            "  macOS: brew install gmp\n"
            "  Ubuntu: sudo apt-get install libgmp-dev")
endif()

# Print found paths for debugging
message(STATUS "GMP include dir: ${GMP_INCLUDE_DIR}")
message(STATUS "GMP library: ${GMP_LIBRARY}")
message(STATUS "GMPXX library: ${GMPXX_LIBRARY}")

# Include project directory
include_directories(${CMAKE_SOURCE_DIR})

# Create the LLL library
add_library(llllib STATIC llllib.cpp)
target_link_libraries(llllib ${GMPXX_LIBRARY} ${GMP_LIBRARY})

# Create the main executable
add_executable(lab7 main.cpp)
target_link_libraries(lab7 llllib ${GMPXX_LIBRARY} ${GMP_LIBRARY})

# Optional: Create a test executable for Task 1
add_executable(Task1 lll.cpp)
target_link_libraries(Task1 llllib ${GMPXX_LIBRARY} ${GMP_LIBRARY})

# Enable warnings
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(llllib PRIVATE -Wall -Wextra)
    target_compile_options(lab7 PRIVATE -Wall -Wextra)
    target_compile_options(Task1 PRIVATE -Wall -Wextra)
endif()