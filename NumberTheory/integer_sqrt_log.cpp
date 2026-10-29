#include<bits/stdc++.h>
using namespace std;

typedef long long ll;

// All functions are exact (no floating point error) and overflow safe for n up to 9e18.

// 1. floor(sqrt(n)), n >= 0
ll sqrt_floor(ll n) {
    ll r = sqrtl((long double)n);
    while (r > 0 && r > n / r) r--;                 // r*r > n
    while (r + 1 <= n / (r + 1)) r++;               // (r+1)*(r+1) <= n
    return r;
}

// 2. ceil(sqrt(n)), n >= 0
ll sqrt_ceil(ll n) {
    ll r = sqrt_floor(n);
    return r * r == n ? r : r + 1;
}

bool is_perfect_square(ll n) { return n >= 0 && sqrt_floor(n) * sqrt_floor(n) == n; }

// 3. floor(n^(1/k)): largest r with r^k <= n. n >= 0, k >= 1
ll kth_root_floor(ll n, int k) {
    if (k == 1 || n <= 1) return n;
    auto pow_le = [&](ll r) {                       // is r^k <= n ?
        ll p = 1;
        for (int i = 0; i < k; i++) {
            if (p > n / r) return false;
            p *= r;
        }
        return true;
    };
    ll r = powl((long double)n, 1.0L / k);
    while (r > 0 && !pow_le(r)) r--;
    while (pow_le(r + 1)) r++;
    return r;
}

// 4. floor(log_b(n)): largest k with b^k <= n. n >= 1, b >= 2
int log_floor(ll n, ll b) {
    int k = 0;
    for (ll p = 1; p <= n / b; p *= b) k++;
    return k;
}

// 5. ceil(log_b(n)): smallest k with b^k >= n. n >= 1, b >= 2
int log_ceil(ll n, ll b) {
    int k = 0;
    ll p = 1;
    while (p < n) {
        k++;
        if (p > n / b) break;                       // next power already exceeds n
        p *= b;
    }
    return k;
}

// 6. base 2, O(1). n >= 1
int log2_floor(ll n) { return 63 - __builtin_clzll(n); }
int log2_ceil(ll n) { return n == 1 ? 0 : 64 - __builtin_clzll(n - 1); }
bool is_power_of_two(ll n) { return n > 0 && (n & (n - 1)) == 0; }


int main(){
    cout << sqrt_floor(15) << " " << sqrt_ceil(15) << " " << sqrt_floor(16) << " " << sqrt_ceil(16) << "\n";   // 3 4 4 4
    cout << sqrt_floor(LLONG_MAX) << "\n";                                                                     // 3037000499
    cout << kth_root_floor(1000, 3) << " " << kth_root_floor(999, 3) << "\n";                                 // 10 9
    cout << log_floor(1000, 10) << " " << log_ceil(1000, 10) << " " << log_floor(999, 10) << " " << log_ceil(1001, 10) << "\n"; // 3 3 2 4
    cout << log2_floor(8) << " " << log2_ceil(8) << " " << log2_floor(9) << " " << log2_ceil(9) << "\n";       // 3 3 3 4
}
