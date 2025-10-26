//
// Created by Nathan Maglasang on 24/10/2025.
//

#include <iostream>
using namespace std;
using lint = long long int;

//TASK 1

lint modular_power(lint base, lint exp, lint n) {
	lint result = 1;
	base = base % n;

	while (exp > 0) {
		if (exp % 2 == 1) {
			result = (__int128)result * base % n;  // Use __int128 to prevent overflow
		}
		base = (__int128)base * base % n;
		exp /= 2;
	}

	return result;
}

lint compute_Task_1(lint n, lint k) {

	lint result = 2 % n;
	for (int i = 2; i <= k; ++i) {
		result = modular_power(result, i, n);
	}

	return (result - 1 + n) % n;
	/*
	lint exponent = 1;
	for (int i = 2; i <= k; ++i) {
		exponent *= i;
	}
	lint result = 1;
	for (int i = 0; i < exponent; ++i) {
	result = (result * 2) % n;
	}
	return (result - 1 + n) % n;
	*/ // OLD VERSION - Innefiecitent
}



//TASK 2

lint gcd_Task_2(lint a, lint b) {
	while (b != 0) {
		lint temp = b;
		b = a % b;
		a = temp;
	}
	return a;
}

lint Factor_Task_2(lint n) {
	for (int k = 1; k <= 50; ++k) { //SMALL BOUND (Running on Raspberry Pi)
		lint Mk = compute_Task_1(n, k);
		lint g = gcd_Task_2(n, Mk);
		if (g > 1 && g < n) {
			return g;
		}
	}
	return n; //NO FACTOR FOUND
}

//TASK 3

bool isPrime_Task3(lint n) {
	if (n < 2) return false;
	for (lint i = 2; i * i <= n; ++i) {
		if (n % i == 0) return false;
	}
	return true;
}

lint findPrimeFactor_Task3(lint n) {
	if (isPrime_Task3(n)) return n;

	lint factor = Factor_Task_2(n);
	if (factor == n) return n;  // CAT FIND FACTOR
	return findPrimeFactor_Task3(factor);
}

//TASK 4

void factor_Task4(lint n) {
	cout << "Factoring " << n << ":\n";

	while (n > 1 && !isPrime_Task3(n)) {
		lint p = findPrimeFactor_Task3(n);

		if (p == n) {
			cout << "(Could not factor further: " << n << ")\n";
			break;
		}

		// COUNT MUTLTICITY OF PRIME
		int count = 0;
		while (n % p == 0) {
			count++;
			n /= p;
		}

		cout << p;
		if (count > 1) cout << "^" << count;
		cout << " ";
	}

	if (n > 1) {
		cout << n << " ";  // PRINT REMEANING PRIME
	}
	cout << "\n";
}




int main() {
int selection = 0;
	lint n,k;

	cout << "Select which tasks you want to complete:\n1. Task 1\n2. Task 2,3,4\nSelection: ";
	cin >> selection;
if (selection != 1 && selection != 2) {
	cout << "Ensure you select either 1 or 2: \n1. Task 1\n2. Task 2,3,4\nSelection: ";
}
	switch (selection) {
		case 1: {
			cout << "Task 1\nInput a pair of integers (n, k)\nInput n: ";
			cin >> n;
			cout << "Input k: ";
			cin >> k;
			cout << "\n--- Task 1: Compute Mk ---\n";
			lint Mk = compute_Task_1(n, k);
			cout << "Mk = " << Mk << "\n";
		}
			break;

		case 2: {
			cout << "Task 2, 3, 4\nInput a pair of integers (n, k)\nInput n: ";
			cin >> n;
			cout << "Input k: ";
			cin >> k;
			lint original_n = n;
			cout << "\n--- Task 2: Find nontrivial factor ---\n";
			lint factor = Factor_Task_2(n);
			cout << "Factor = " << factor << "\n";

			cout << "\n--- Task 3: Find prime factor ---\n";
			lint prime = findPrimeFactor_Task3(n);
			cout << "Prime factor = " << prime << "\n";

			cout << "\n--- Task 4: Full factorization ---\n";
			factor_Task4(original_n);
		}
			break;


	}



}


//TASK 4 OUTPUTS:
/*
Factoring 359446091074757:
673 127 401 131 223 359
Factoring 15419616932490265564907:
(FAILS AS THE LONG LONG INT MAX SIZE IS 9.22 × 10¹⁸ but the given value is 15,419,616,932,490,265,564,907 ≈ 1.5 × 10²²)
This can be fixed but I need to use the boost library.

 */
//Questions
/*
(1) Due to the methods of calculation, increases to the value of k! increase exponentially, where when k is 5, k! is 120, whereas when k is 10, k! is 3.6 Million
In turn this massively increases the time taken for the the calculations to complete

Additionally, in task 1, with the way I setup my computations, for each value of k, the calculations for k! are recalculated from the beggining for each iteration,
rather than using the previous value of k!, taking up large amounts of memory as the value for k gets larger (it would, once i figure out a simpler way to get around the long long int limits)

Similarly for the value of Mk, when calculating the value for Mk + 1, the value for Mk is recalculated, where it could have been stored.

(2) One way to make these algorithms much faster would be to use the Miller-Rabin method for primality testing, as using the method above checks every value between i and sqrt(n) to discern their primality
Using the Miller Rabin method would use modular arithmetic to calculate primality for much larger numbers such as 64 bits with far less iterations.




 */
