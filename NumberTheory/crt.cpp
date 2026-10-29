#include<bits/stdc++.h>
using namespace std;

typedef long long ll;

// ===================================== CRT TOOLKIT =====================================
// Moduli do NOT need to be coprime. A system's answer is returned as {x, M}:
//     all solutions are x + k*M, with 0 <= x < M  (M = lcm of the moduli)
//     no solution -> {-1, -1}
// M must fit in ll (<= ~9e18). If it doesn't, use crt_coprime_mod() to get the answer mod P.
// All products use __int128, so values up to ~9e18 are safe.
// =======================================================================================

// returns g = gcd(a, b) and x, y with a*x + b*y = g
ll ext_gcd(ll a, ll b, ll& x, ll& y) {
    if (b == 0) { x = 1, y = 0; return a; }
    ll x1, y1;
    ll g = ext_gcd(b, a % b, x1, y1);
    x = y1, y = x1 - (a / b) * y1;
    return g;
}

// inverse of a mod m, -1 if gcd(a, m) != 1
ll mod_inverse(ll a, ll m) {
    ll x, y;
    ll g = ext_gcd(((a % m) + m) % m, m, x, y);
    if (g != 1) return -1;
    return (x % m + m) % m;
}

// 1. x = a1 (mod m1), x = a2 (mod m2)
pair<ll, ll> crt_pair(ll a1, ll m1, ll a2, ll m2) {
    a1 = (a1 % m1 + m1) % m1, a2 = (a2 % m2 + m2) % m2;
    ll p, q;
    ll g = ext_gcd(m1, m2, p, q);
    if ((a2 - a1) % g != 0) return {-1, -1};
    ll M = m1 / g * m2;
    // x = a1 + m1 * k,  k = (a2 - a1)/g * p  (mod m2/g)
    ll m2g = m2 / g;
    ll k = (ll)((__int128)((a2 - a1) / g) % m2g * (p % m2g) % m2g);
    if (k < 0) k += m2g;
    ll x = (ll)(((__int128)a1 + (__int128)m1 * k) % M);
    return {x, M};
}

// 2. x = a[i] (mod m[i]) for all i
pair<ll, ll> crt(const vector<ll>& a, const vector<ll>& m) {
    pair<ll, ll> res = {0, 1};
    for (int i = 0; i < (int)a.size(); i++) {
        res = crt_pair(res.first, res.second, a[i], m[i]);
        if (res.second == -1) return {-1, -1};
    }
    return res;
}

// 3. Garner: PAIRWISE COPRIME moduli, product may be astronomically large.
//    returns the unique x in [0, prod m) reduced mod P. O(n^2)
ll crt_coprime_mod(const vector<ll>& a, const vector<ll>& m, ll P) {
    int n = a.size();
    vector<ll> coef(n);                    // x = coef[0] + coef[1]*m0 + coef[2]*m0*m1 + ...
    for (int i = 0; i < n; i++) {
        ll cur = (a[i] % m[i] + m[i]) % m[i];
        ll prod = 1;                       // value of prefix sum mod m[i]
        ll val = 0;
        for (int j = 0; j < i; j++) {
            val = (ll)((val + (__int128)coef[j] * prod) % m[i]);
            prod = (ll)((__int128)prod * (m[j] % m[i]) % m[i]);
        }
        ll diff = ((cur - val) % m[i] + m[i]) % m[i];
        coef[i] = (ll)((__int128)diff * mod_inverse(prod, m[i]) % m[i]);
    }
    ll res = 0, prod = 1 % P;
    for (int i = 0; i < n; i++) {
        res = (ll)((res + (__int128)coef[i] % P * prod) % P);
        prod = (ll)((__int128)prod * (m[i] % P) % P);
    }
    return res;
}


// ===================================== APPLICATIONS =====================================

// 4. a*x = b (mod m)  ->  all solutions {x0, step}: x = x0 + k*step, 0 <= x0 < step. none -> {-1, -1}
//    (there are exactly g = gcd(a, m) solutions in [0, m))
pair<ll, ll> solve_linear_congruence(ll a, ll b, ll m) {
    a = (a % m + m) % m, b = (b % m + m) % m;
    ll x, y;
    ll g = ext_gcd(a, m, x, y);
    if (b % g != 0) return {-1, -1};
    ll step = m / g;
    x = (x % step + step) % step;
    ll x0 = (ll)((__int128)x * (b / g) % step);
    return {x0, step};
}

// 5. system a[i]*x = b[i] (mod m[i])  ->  {x, M} like crt()
pair<ll, ll> solve_linear_congruence_system(const vector<ll>& a, const vector<ll>& b, const vector<ll>& m) {
    pair<ll, ll> res = {0, 1};
    for (int i = 0; i < (int)a.size(); i++) {
        pair<ll, ll> s = solve_linear_congruence(a[i], b[i], m[i]);
        if (s.second == -1) return {-1, -1};
        res = crt_pair(res.first, res.second, s.first, s.second);
        if (res.second == -1) return {-1, -1};
    }
    return res;
}

// 6. smallest x >= L with x = a[i] (mod m[i]) for all i. -1 if none
ll crt_smallest_at_least(const vector<ll>& a, const vector<ll>& m, ll L) {
    pair<ll, ll> r = crt(a, m);
    if (r.second == -1) return -1;
    ll x = r.first, M = r.second;
    if (x >= L) return x - (x - L) / M * M;
    return x + (L - x + M - 1) / M * M;
}

// 7. how many x in [L, R] satisfy x = a[i] (mod m[i]) for all i
ll crt_count_in_range(const vector<ll>& a, const vector<ll>& m, ll L, ll R) {
    pair<ll, ll> r = crt(a, m);
    if (r.second == -1 || L > R) return 0;
    ll x = r.first, M = r.second;
    // number of k with L <= x + kM <= R  ->  floor((R-x)/M) - floor((L-1-x)/M)
    auto floor_div = [](ll p, ll q) { return p / q - ((p % q != 0) && ((p < 0) != (q < 0))); };
    return floor_div(R - x, M) - floor_div(L - 1 - x, M);
}

// 8. smallest POSITIVE number leaving remainder r[i] when divided by d[i]. -1 if impossible
ll smallest_positive_with_remainders(const vector<ll>& r, const vector<ll>& d) {
    return crt_smallest_at_least(r, d, 1);
}

// 9. event i happens at times start[i], start[i] + period[i], start[i] + 2*period[i], ...
//    first time when ALL events happen together. -1 if never
ll first_common_time(const vector<ll>& start, const vector<ll>& period) {
    ll L = *max_element(start.begin(), start.end());
    return crt_smallest_at_least(start, period, L);
}

// 10. you know f mod each prime power p_i^e_i (e.g. nCr mod p^e via Lucas-like methods);
//     combine them into f mod (product). this is just crt() on {value, prime power}.
//     example: answer mod 12 from answer mod 4 = 3 and answer mod 3 = 2  ->  crt({3, 2}, {4, 3}).first = 11

// 11. x = a (mod m) given that x lies in a known range of size < M: unique, use crt_smallest_at_least(L)


int main(){
    pair<ll, ll> r = crt({2, 3, 2}, {3, 5, 7});
    cout << r.first << " " << r.second << "\n";                       // 23 105
    r = crt({1, 3}, {4, 6});
    cout << r.first << " " << r.second << "\n";                       // 9 12   (non-coprime)
    r = crt({1, 2}, {4, 6});
    cout << r.first << " " << r.second << "\n";                       // -1 -1  (no solution)

    cout << crt_coprime_mod({1, 2, 3}, {1000000007, 998244353, 1000000009}, 1000000007) << "\n"; // 1

    r = solve_linear_congruence(6, 4, 10);
    cout << r.first << " " << r.second << "\n";                       // 4 5   (x = 4, 9 mod 10)
    cout << crt_count_in_range({2, 3}, {3, 5}, 1, 100) << "\n";        // 7    (8, 23, 38, ...)
    cout << first_common_time({2, 4}, {4, 6}) << "\n";                 // 10   (2,6,10.. and 4,10,..)
}
