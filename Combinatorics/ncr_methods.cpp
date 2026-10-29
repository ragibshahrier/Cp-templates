#include<bits/stdc++.h>
using namespace std;

typedef long long ll;
typedef long double ld;

// =============================== WHICH nCr METHOD TO USE? ===============================
//  n up to ~1e7, prime mod > n, many queries     -> A. precomputed factorials       O(n) + O(1)
//  n up to ~5000, ANY mod (even non-prime)       -> B. Pascal triangle              O(n^2)
//  no mod, answer fits in ll (n <= 66 always ok) -> C. exact                        O(r)
//  n huge (1e18), r small (< p), prime mod p     -> D. product formula              O(r + log p)
//  n, r huge (1e18), SMALL prime mod p (<= 1e6)  -> E. Lucas                        O(p) + O(log_p n)
//  n, r huge, mod = p^e or ANY composite mod     -> F. generalized Lucas + CRT      O(p^e) per prime power
//  whole row C(n, 0..K), n huge, prime mod       -> G.                              O(K log p)
//  parity / exponent of p in nCr / n!            -> H.
//  probability-style huge values (as double)     -> I. log of nCr
// Each section is independent: copy only the one you need.
// ========================================================================================

ll power(ll a, ll b, ll mod) {
    ll res = 1 % mod;
    a %= mod;
    if (a < 0) a += mod;
    while (b > 0) {
        if (b & 1) res = (__int128)res * a % mod;
        a = (__int128)a * a % mod;
        b >>= 1;
    }
    return res;
}


// ======================== A. PRECOMPUTED FACTORIALS (prime MOD > n) ========================
const ll MOD = 1e9 + 7;
vector<ll> fact, inv_fact, inv_num;

// call once with the largest n you will need
void init_factorials(int n) {
    fact.assign(n + 1, 1), inv_fact.assign(n + 1, 1), inv_num.assign(n + 1, 1);
    for (int i = 1; i <= n; i++) fact[i] = fact[i - 1] * i % MOD;
    inv_fact[n] = power(fact[n], MOD - 2, MOD);
    for (int i = n; i > 0; i--) inv_fact[i - 1] = inv_fact[i] * i % MOD;
    for (int i = 1; i <= n; i++) inv_num[i] = inv_fact[i] * fact[i - 1] % MOD;   // 1 / i
}
ll nCr(ll n, ll r) {
    if (r < 0 || r > n || n < 0) return 0;
    return fact[n] * inv_fact[r] % MOD * inv_fact[n - r] % MOD;
}
ll nPr(ll n, ll r) {                                     // ordered selections: n! / (n-r)!
    if (r < 0 || r > n || n < 0) return 0;
    return fact[n] * inv_fact[n - r] % MOD;
}
// n! / (c1! c2! ... ck!) where n = sum of counts
ll multinomial(const vector<ll>& counts) {
    ll n = 0, res = 1;
    for (ll c : counts) n += c, res = res * inv_fact[c] % MOD;
    return res * fact[n] % MOD;
}


// ======================== B. PASCAL TRIANGLE (any mod, small n) ========================
// C[i][j] for 0 <= j <= i <= n. works for composite mods too
vector<vector<ll>> pascal_triangle(int n, ll mod) {
    vector<vector<ll>> C(n + 1, vector<ll>(n + 1, 0));
    for (int i = 0; i <= n; i++) {
        C[i][0] = 1 % mod;
        for (int j = 1; j <= i; j++) C[i][j] = (C[i - 1][j - 1] + C[i - 1][j]) % mod;
    }
    return C;
}


// ======================== C. EXACT nCr (no mod) ========================
// result must fit in ll. intermediate uses __int128, so no premature overflow
ll nCr_exact(ll n, ll r) {
    if (r < 0 || r > n) return 0;
    r = min(r, n - r);
    __int128 res = 1;
    for (ll i = 1; i <= r; i++) res = res * (n - r + i) / i;      // always divisible: res = C(n-r+i, i)
    return (ll)res;
}


// ======================== D. HUGE n, SMALL r, prime p ========================
// n up to 1e18, r < p, r up to ~1e7. (by Lucas, C(n, r) = C(n mod p, r) when r < p)
ll nCr_large_n(ll n, ll r, ll p) {
    if (r < 0 || r > n) return 0;
    ll m = n % p;
    if (m < r) return 0;
    ll num = 1, den = 1;
    for (ll i = 0; i < r; i++) {
        num = (__int128)num * ((m - i) % p) % p;
        den = (__int128)den * ((i + 1) % p) % p;
    }
    return (__int128)num * power(den, p - 2, p) % p;
}


// ======================== E. LUCAS (small prime p) ========================
// n, r up to 1e18, p prime up to ~1e7. call lucas_init(p) once
ll lucas_p;
vector<ll> lucas_fact, lucas_inv_fact;
void lucas_init(ll p) {
    lucas_p = p;
    lucas_fact.assign(p, 1), lucas_inv_fact.assign(p, 1);
    for (ll i = 1; i < p; i++) lucas_fact[i] = lucas_fact[i - 1] * i % p;
    lucas_inv_fact[p - 1] = power(lucas_fact[p - 1], p - 2, p);
    for (ll i = p - 1; i > 0; i--) lucas_inv_fact[i - 1] = lucas_inv_fact[i] * i % p;
}
ll nCr_lucas(ll n, ll r) {
    if (r < 0 || r > n) return 0;
    ll p = lucas_p, res = 1;
    while (n > 0 || r > 0) {
        ll ni = n % p, ri = r % p;
        if (ri > ni) return 0;
        res = res * lucas_fact[ni] % p * lucas_inv_fact[ri] % p * lucas_inv_fact[ni - ri] % p;
        n /= p, r /= p;
    }
    return res;
}


// ======================== F. PRIME POWER / ANY MOD (generalized Lucas) ========================
// n, r up to 1e18. every prime power p^e dividing the mod must be <= ~1e7 (a table of that size is built)

ll ext_gcd(ll a, ll b, ll& x, ll& y) {
    if (b == 0) { x = 1, y = 0; return a; }
    ll x1, y1, g = ext_gcd(b, a % b, x1, y1);
    x = y1, y = x1 - (a / b) * y1;
    return g;
}
ll inverse_mod(ll a, ll m) {                     // gcd(a, m) must be 1
    ll x, y;
    ext_gcd(a, m, x, y);
    return (x % m + m) % m;
}

// exponent of p in n!  (Legendre)
ll legendre(ll n, ll p) {
    ll e = 0;
    while (n > 0) n /= p, e += n;
    return e;
}

// n! with every factor p removed, mod pe = p^e. table[i] = product of j <= i, p does not divide j
ll factorial_without_p(ll n, ll p, ll pe, const vector<ll>& table) {
    ll res = 1;
    while (n > 1) {
        res = (__int128)res * power(table[pe], n / pe, pe) % pe * table[n % pe] % pe;
        n /= p;
    }
    return res;
}

// C(n, r) mod p^e
ll nCr_mod_prime_power(ll n, ll r, ll p, int e) {
    if (r < 0 || r > n) return 0;
    ll pe = 1;
    for (int i = 0; i < e; i++) pe *= p;
    ll t = legendre(n, p) - legendre(r, p) - legendre(n - r, p);
    if (t >= e) return 0;
    vector<ll> table(pe + 1);
    table[0] = 1 % pe;
    for (ll i = 1; i <= pe; i++) table[i] = i % p ? table[i - 1] * i % pe : table[i - 1];
    ll res = factorial_without_p(n, p, pe, table);
    res = (__int128)res * inverse_mod(factorial_without_p(r, p, pe, table), pe) % pe;
    res = (__int128)res * inverse_mod(factorial_without_p(n - r, p, pe, table), pe) % pe;
    return (__int128)res * power(p, t, pe) % pe;
}

// C(n, r) mod m, for ANY m >= 1 (composite, non-square-free, ...)
ll nCr_any_mod(ll n, ll r, ll m) {
    if (r < 0 || r > n) return 0;
    ll x = 0, M = 1, rest = m;                   // answer so far: x mod M
    for (ll p = 2; rest > 1; p++) {
        if (p * p > rest) p = rest;              // rest is prime
        if (rest % p) continue;
        int e = 0;
        ll pe = 1;
        while (rest % p == 0) rest /= p, e++, pe *= p;
        ll a = nCr_mod_prime_power(n, r, p, e);
        // combine x (mod M) with a (mod pe), coprime moduli
        ll k = (__int128)(((a - x) % pe + pe) % pe) * inverse_mod(M % pe, pe) % pe;
        x += M * k, M *= pe;
    }
    return x % m;
}


// ======================== G. A WHOLE ROW: C(n, 0), C(n, 1), ..., C(n, K) ========================
// n up to 1e18, prime p, K < p
vector<ll> binomial_row(ll n, int K, ll p) {
    vector<ll> row(K + 1, 0);
    ll m = n % p;                                // C(n, k) = C(n mod p, k) for k < p
    row[0] = 1 % p;
    for (int k = 1; k <= K; k++) {
        if (k > n) break;
        ll factor = ((m - k + 1) % p + p) % p;
        row[k] = (__int128)row[k - 1] * factor % p * power(k, p - 2, p) % p;
    }
    return row;
}


// ======================== H. DIVISIBILITY FACTS ========================
// exponent of prime p in C(n, r) = number of carries when adding r and n-r in base p (Kummer)
ll exponent_of_p_in_nCr(ll n, ll r, ll p) { return legendre(n, p) - legendre(r, p) - legendre(n - r, p); }
bool nCr_is_odd(ll n, ll r) { return r >= 0 && r <= n && (n & r) == r; }   // Lucas with p = 2
bool nCr_divisible_by_p(ll n, ll r, ll p) { return exponent_of_p_in_nCr(n, r, p) > 0; }


// ======================== I. LOG / DOUBLE VERSION ========================
// for probabilities: C(n, r) * x^r * ... where C(n, r) alone overflows. use expl(log_nCr(n, r) + ...)
ld log_nCr(ll n, ll r) { return lgammal(n + 1) - lgammal(r + 1) - lgammal(n - r + 1); }


int main(){
    init_factorials(1000000);
    cout << nCr(10, 3) << " " << nPr(10, 3) << " " << multinomial({2, 1, 1}) << "\n";    // 120 720 12
    auto C = pascal_triangle(10, 12);
    cout << C[10][3] << "\n";                                                            // 120 % 12 = 0
    cout << nCr_exact(66, 33) << "\n";                                                   // 7219428434016265740
    cout << nCr_large_n(1000000000000000000LL, 3, MOD) << "\n";
    lucas_init(13);
    cout << nCr_lucas(1000, 300) << " " << nCr_any_mod(1000, 300, 13) << "\n";            // same
    cout << nCr_any_mod(1000000000000LL, 123456789, 1000000) << "\n";
    for (ll x : binomial_row(5, 5, MOD)) cout << x << " ";                               // 1 5 10 10 5 1
    cout << "\n";
    cout << nCr_is_odd(7, 3) << " " << nCr_is_odd(6, 3) << " " << exponent_of_p_in_nCr(10, 5, 2) << "\n";  // 1 0 2
}
