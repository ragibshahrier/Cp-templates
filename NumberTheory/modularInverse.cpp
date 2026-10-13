#include<bits/stdc++.h>
using namespace std;

long long modInverse(long long A, long long m)
{
    long long m0 = m;
    long long y = 0, x = 1;

    if (m == 1)
        return 0;

    while (A > 1) {
        long long q = A / m;
        long long t = m;

        
        m = A % m, A = t;
        t = y;

        y = x - q * y;
        x = t;
    }

    if (x < 0)
        x += m0;

    return x;
}


long long modDivision(long long a, long long b, long long m){
    long long g = __gcd(b,m);

    if(a%g!=0){
        return -1;
    }

    b = b/g;
    a = a/g;
    m = m/g;

    long long inv = modInverse(b, m);

    return ( (a % m) * (inv % m) ) % m;
}