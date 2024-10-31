#include<bits/stdc++.h>
using namespace std;
#define int long long



typedef long long ll;
typedef ll ell;
ell expo1(ell a, ell b, ell mod) {
    ell res = 1; 
    while (b > 0) {
        if (b & 1)
            res = (res * a) % mod; 
        a = (a * a) % mod; 
        b = b >> 1;
    } 
    return res;
}

bool check_composite(ell n, ell a, ell d, ell s) {
    ell x = expo1(a, d, n);
    if (x == 1 || x == n - 1)
        return false;
    for (int r = 1; r < s; r++) {
        x = (x * x) % n;
        if (x == n - 1)
            return false;
    }
    return true;
};

bool MillerRabin(ell n) { // returns true if n is prime, else returns false.
    if (n < 2)
        return false;

    int r = 0;
    ll d = n - 1;
    while ((d & 1) == 0) {
        d >>= 1;
        r++;
    }
    vector<long long> kprimes = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97, 101, 103, 107, 109, 113, 127, 131, 137, 139, 149, 151, 157, 163, 167, 173, 179, 181, 191, 193, 197, 199, 211, 223, 227, 229, 233, 239, 241, 251, 257, 263, 269, 271, 277, 281, 283, 293, 307, 311, 313, 317, 331, 337, 347, 349, 353, 359, 367, 373, 379, 383, 389, 397, 401, 409, 419, 421, 431, 433, 439, 443, 449, 457, 461, 463, 467, 479, 487, 491, 499, 503, 509, 521, 523, 541};
    vector<long long> kprimes2 = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37};
    for (int a : kprimes) {
        if (n == a)
            return true;
        if (check_composite(n, a, d, r))
            return false;
    }
    return true;
}

int32_t main(){
    cout<<MillerRabin(452037589);
}