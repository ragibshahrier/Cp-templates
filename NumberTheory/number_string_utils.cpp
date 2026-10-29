#include<bits/stdc++.h>
using namespace std;

typedef long long ll;
typedef long double ld;

// ================= string <-> number =================
ll string_to_int(const string& s) { return stoll(s); }          // "-123" -> -123
ld string_to_double(const string& s) { return stold(s); }       // "3.14" -> 3.14

string int_to_string(ll x) { return to_string(x); }

// fixed number of digits after the decimal point (std::to_string(double) always prints 6)
string double_to_string(ld x, int precision = 6) {
    ostringstream out;
    out << fixed << setprecision(precision) << x;
    return out.str();
}

// ================= digits =================
// number of decimal digits, sign ignored. count_digits(0) = 1
int count_digits(ll x) {
    if (x == 0) return 1;
    int c = 0;
    while (x != 0) x /= 10, c++;
    return c;
}

int digit_sum(ll x) {
    int s = 0;
    while (x != 0) s += abs(x % 10), x /= 10;
    return s;
}

// reverse the digits of x (sign is kept, leading zeros vanish: 1200 -> 21)
// optional window [l, r]: digit positions counted from the LEFT, 0-indexed, inclusive.
// r = -1 means "up to the last digit".
//   reverse_digits(123456)       = 654321
//   reverse_digits(123456, 1, 3) = 143256
//   reverse_digits(123456, 2)    = 126543
// careful: the reversed number can overflow ll if x has 19 digits
ll reverse_digits(ll x, int l = 0, int r = -1) {
    bool neg = x < 0;
    string s = to_string(x);
    if (neg) s = s.substr(1);
    if (r == -1 || r >= (int)s.size()) r = s.size() - 1;
    reverse(s.begin() + l, s.begin() + r + 1);
    ll res = stoll(s);
    return neg ? -res : res;
}

bool is_palindrome_number(ll x) { return x >= 0 && reverse_digits(x) == x; }


int main(){
    cout << string_to_int("-123") + 1 << " " << string_to_double("2.5") * 2 << "\n";  // -122 5
    cout << int_to_string(42) + "!" << " " << double_to_string(3.14159, 2) << "\n";    // 42! 3.14
    cout << count_digits(0) << " " << count_digits(-9876) << " " << digit_sum(-9876) << "\n";  // 1 4 30
    cout << reverse_digits(123456) << " " << reverse_digits(123456, 1, 3) << " " << reverse_digits(123456, 2) << "\n";
    cout << reverse_digits(1200) << " " << reverse_digits(-123) << " " << is_palindrome_number(12321) << "\n";  // 21 -321 1
}
