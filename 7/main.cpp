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
    mpz_invert(e.get_mpz_t(), data.d.get_mpz_t(), phi_n.get_mpz_t()); //COMPUTES INVERSE MOD (a^-1 mod m)
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
        char ch = static_cast<char>(remainder.get_ui()); //GETUI CONVERTD MPZ CLASS TO UNSIGNED LONG INT FOR VAL GREATER THAN 256

        message = ch + message;

        temp /= 256;

    }

    cout << "Decoded Message:\n " << message << "\n";
    if (message == "ALICE, THIS IS BOB. PLEASE GENERATE NEW PRIMES FOR YOUR RSA") {
        cout << "\n\nMessage Correct YAY";
    } else {
        cout << "\n\n:(";
    }
}