//
// Created by Nathan Maglasang on 27/9/2025.
//
#include <iostream>
using namespace std;

// Simplifying Variable Naming, long Lon looks messy
using i64 = long long;

void InputValues(i64 &a, i64 &b) {
  cout << "Enter a value for 'a': ";
  while (!(cin >> a)) {
    cout << "Please input an integer value for 'a': ";
    cin.clear();
    cin.ignore(10000, '\n');
  }
  cin.ignore(10000, '\n');

  cout << "\nEnter a value for 'b': ";
  while (!(cin >> b)) {
    cout << "Please input an integer value for 'b': ";
    cin.clear();
    cin.ignore(10000, '\n');
  }
  cin.ignore(10000, '\n');
}

int EuclideanCount(i64 a, i64 b, i64 &final_a) {
  int count = 0;
  while (b != 0) {
    i64 temp = b;
    b = a % b;
    a = temp;
    count++;
  }
  final_a = a;
  return count;
}

int main() {
  i64 a, b;
  InputValues(a, b);

  i64 gcd_value;
  int count = EuclideanCount(a, b, gcd_value);

  cout << "The Euclidean algorithm took: " << count << " iterations\n";
  cout << "The gcd(" << a << ", " << b << ") = " << gcd_value << "\n";

  return 0;
}