#include<bits/stdc++.h>
using namespace std;

int Fpre(int n){
    return n; // return prefix sum
}


void func1(int n){
    int ans = 0;
    for (long long l = 1, r; l <= n; l = r + 1) {
        r = n / (n / l);          // block [l, r] shares the same n/l
        // value is (n/l), block length (r - l + 1)
        ans += (n / l) * (r - l + 1);
    }

}

void func2(int n, int m){
    int ans = 0;
    long long L = min(n, m);
    for (long long l = 1, r; l <= L; l = r + 1) {
        r = min(n / (n / l), m / (m / l));
        ans += (Fpre(r) - Fpre(l-1)) * (n / l) * (m / l);
    }
}
