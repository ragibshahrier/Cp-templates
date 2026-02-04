#include <bits/stdc++.h>
using namespace std;


// string s1 = "123";
// cout<<atoi(s1.c_str())<<endl;

long long binaryToInt(string s) {
    long long res = 0;
    for (char c : s) {
        res = (res << 1) | (c - '0');
    }
    return res;
}
string intToBinary(long long n) {
    if (n == 0) return "0";
    string res = "";
    while (n > 0) {
        res += (n & 1 ? '1' : '0');
        n >>= 1;
    }
    reverse(res.begin(), res.end());
    return res;
}

string baseTransform(string num, int b1, int b2) {
    if (num == "0") return "0";
    
    // Map chars to values: '0'-'9' -> 0-9, 'A'-'Z' -> 10-35
    auto val = [](char c) { 
        return isdigit(c) ? c - '0' : toupper(c) - 'A' + 10; 
    };
    auto chr = [](int v) { 
        return (char)(v < 10 ? v + '0' : v - 10 + 'A'); 
    };

    // Convert input string to a vector of digits for easier math
    vector<int> digits;
    for (char c : num) digits.push_back(val(c));

    string res = "";
    while (!digits.empty()) {
        long long rem = 0;
        vector<int> next_digits;
        
        // This is essentially BigInt division: digits / b2
        for (int d : digits) {
            long long cur = d + rem * b1;
            if (!next_digits.empty() || cur / b2 > 0) 
                next_digits.push_back(cur / b2);
            rem = cur % b2;
        }
        res += chr(rem);
        digits = next_digits;
    }
    
    reverse(res.begin(), res.end());
    return res.empty() ? "0" : res;
}