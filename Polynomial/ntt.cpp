#include<bits/stdc++.h>
using namespace std;

typedef long long ll;

// ============================== NTT (polynomial multiplication) ==============================
// multiply(a, b, mod)  -> a * b with coefficients mod `mod`. ANY mod works:
//      * NTT-friendly prime (998244353, 167772161, 469762049, 754974721, ...) -> 1 direct NTT
//      * anything else (1e9+7, composite, ...) -> 3 NTTs + CRT (about 3x slower)
// multiply_exact(a, b) -> exact coefficients (no mod). inputs may be negative.
//      every result coefficient must fit in ll.
// Complexity O((n + m) log(n + m)). Sizes up to ~1e6 are fine.
//
// NTT-friendly prime = c * 2^k + 1 with 2^k >= result size:
//   998244353 = 119 * 2^23 + 1,  167772161 = 5 * 2^25 + 1,  469762049 = 7 * 2^26 + 1,  754974721 = 45 * 2^24 + 1
// =============================================================================================

ll power(ll a, ll b, ll mod) {
    ll res = 1;
    a %= mod;
    if (a < 0) a += mod;
    while (b > 0) {
        if (b & 1) res = res * a % mod;
        a = a * a % mod;
        b >>= 1;
    }
    return res;
}

bool is_prime(ll n) {
    if (n < 2) return false;
    for (ll i = 2; i * i <= n; i++) if (n % i == 0) return false;
    return true;
}

// smallest primitive root of a prime mod (cached)
ll primitive_root(ll mod) {
    static map<ll, ll> cache;
    if (cache.count(mod)) return cache[mod];
    vector<ll> primes;                                  // prime factors of mod - 1
    ll x = mod - 1;
    for (ll i = 2; i * i <= x; i++) {
        if (x % i == 0) primes.push_back(i);
        while (x % i == 0) x /= i;
    }
    if (x > 1) primes.push_back(x);
    for (ll g = 2; ; g++) {
        bool ok = true;
        for (ll q : primes) if (power(g, (mod - 1) / q, mod) == 1) { ok = false; break; }
        if (ok) return cache[mod] = g;
    }
}

// can we do a direct NTT of length `len` (power of 2) under this mod?
bool is_ntt_friendly(ll mod, ll len) {
    static map<ll, bool> prime_cache;
    if ((mod - 1) % len != 0) return false;
    if (!prime_cache.count(mod)) prime_cache[mod] = is_prime(mod);
    return prime_cache[mod];
}

// in-place NTT. a.size() must be a power of 2 and is_ntt_friendly(mod, a.size())
void ntt(vector<ll>& a, bool invert, ll mod) {
    int n = a.size();
    for (int i = 1, j = 0; i < n; i++) {                // bit reversal permutation
        int bit = n >> 1;
        for (; j & bit; bit >>= 1) j ^= bit;
        j ^= bit;
        if (i < j) swap(a[i], a[j]);
    }
    ll g = primitive_root(mod);
    for (int len = 2; len <= n; len <<= 1) {
        ll w = power(g, (mod - 1) / len, mod);
        if (invert) w = power(w, mod - 2, mod);
        vector<ll> ws(len / 2);                          // powers of w for this level
        ws[0] = 1;
        for (int j = 1; j < len / 2; j++) ws[j] = ws[j - 1] * w % mod;
        for (int i = 0; i < n; i += len) {
            for (int j = 0; j < len / 2; j++) {
                ll u = a[i + j], v = a[i + j + len / 2] * ws[j] % mod;
                a[i + j] = u + v < mod ? u + v : u + v - mod;
                a[i + j + len / 2] = u - v >= 0 ? u - v : u - v + mod;
            }
        }
    }
    if (invert) {
        ll inv_n = power(n, mod - 2, mod);
        for (ll& x : a) x = x * inv_n % mod;
    }
}

// a * b mod `mod`, where mod is an NTT-friendly prime large enough for the result size
vector<ll> multiply_ntt(vector<ll> a, vector<ll> b, ll mod) {
    int need = a.size() + b.size() - 1, len = 1;
    while (len < need) len <<= 1;
    for (ll& x : a) x = (x % mod + mod) % mod;
    for (ll& x : b) x = (x % mod + mod) % mod;
    a.resize(len), b.resize(len);
    ntt(a, false, mod), ntt(b, false, mod);
    for (int i = 0; i < len; i++) a[i] = a[i] * b[i] % mod;
    ntt(a, true, mod);
    a.resize(need);
    return a;
}

// small inputs: plain O(n*m) is faster. returns exact products as __int128
vector<__int128> multiply_naive(const vector<ll>& a, const vector<ll>& b) {
    vector<__int128> res(a.size() + b.size() - 1, 0);
    for (int i = 0; i < (int)a.size(); i++)
        for (int j = 0; j < (int)b.size(); j++) res[i + j] += (__int128)a[i] * b[j];
    return res;
}

// exact result as __int128 via 3 NTT primes + CRT (Garner). valid while |coef| < ~3.9e25
vector<__int128> multiply_three_primes(const vector<ll>& a, const vector<ll>& b) {
    const ll m1 = 998244353, m2 = 167772161, m3 = 469762049;
    vector<ll> r1 = multiply_ntt(a, b, m1), r2 = multiply_ntt(a, b, m2), r3 = multiply_ntt(a, b, m3);
    const ll inv_m1_mod_m2 = power(m1, m2 - 2, m2);
    const ll m1m2 = m1 * m2;
    const ll inv_m1m2_mod_m3 = power(m1m2 % m3, m3 - 2, m3);
    const __int128 M = (__int128)m1m2 * m3;
    vector<__int128> res(r1.size());
    for (int i = 0; i < (int)r1.size(); i++) {
        ll t = (r2[i] - r1[i] % m2 + m2) % m2 * inv_m1_mod_m2 % m2;
        ll x12 = r1[i] + m1 * t;                                      // correct mod m1*m2
        ll t2 = ((r3[i] - x12 % m3) % m3 + m3) % m3 * inv_m1m2_mod_m3 % m3;
        __int128 x = x12 + (__int128)m1m2 * t2;                         // correct mod m1*m2*m3
        if (x > M / 2) x -= M;                                          // negative results
        res[i] = x;
    }
    return res;
}

// a * b with every coefficient taken mod `mod`. works for ANY mod >= 1
vector<ll> multiply(const vector<ll>& a, const vector<ll>& b, ll mod = 998244353) {
    if (a.empty() || b.empty()) return {};
    int need = a.size() + b.size() - 1, len = 1;
    while (len < need) len <<= 1;
    if (min(a.size(), b.size()) <= 32 && mod < (1LL << 31)) {                       // naive is faster
        vector<ll> res(need, 0);
        for (int i = 0; i < (int)a.size(); i++)
            for (int j = 0; j < (int)b.size(); j++)
                res[i + j] = (res[i + j] + (a[i] % mod + mod) % mod * ((b[j] % mod + mod) % mod)) % mod;
        return res;
    }
    if (is_ntt_friendly(mod, len)) return multiply_ntt(a, b, mod);
    vector<ll> ra = a, rb = b;                                   // reduce first so the CRT bound holds
    for (ll& x : ra) x = (x % mod + mod) % mod;
    for (ll& x : rb) x = (x % mod + mod) % mod;
    vector<__int128> big = multiply_three_primes(ra, rb);        // needs min(n,m) * mod^2 < ~3.9e25
    vector<ll> res(need);
    for (int i = 0; i < need; i++) res[i] = (ll)(big[i] % mod);
    return res;
}

// exact a * b (no mod). inputs may be negative. every result coefficient must fit in ll
vector<ll> multiply_exact(const vector<ll>& a, const vector<ll>& b) {
    if (a.empty() || b.empty()) return {};
    vector<__int128> big = min(a.size(), b.size()) <= 32 ? multiply_naive(a, b) : multiply_three_primes(a, b);
    vector<ll> res(big.size());
    for (int i = 0; i < (int)big.size(); i++) res[i] = (ll)big[i];
    return res;
}

// product of many polynomials, divide & conquer: O(total_degree * log^2)
vector<ll> multiply_all(vector<vector<ll>> polys, ll mod = 998244353) {
    if (polys.empty()) return {1};
    while (polys.size() > 1) {
        vector<vector<ll>> next;
        for (int i = 0; i + 1 < (int)polys.size(); i += 2) next.push_back(multiply(polys[i], polys[i + 1], mod));
        if (polys.size() % 2) next.push_back(polys.back());
        polys = next;
    }
    return polys[0];
}


// ======================================= APPLICATIONS =======================================
// "sum -> count" results are returned as vector of {value, count}, sorted by value, count > 0.
// Values may be negative. Needs (max value - min value) of each array up to ~1e6.

// helper: frequency array of values shifted by their minimum
vector<ll> frequency_array(const vector<ll>& a, ll& min_value) {
    min_value = *min_element(a.begin(), a.end());
    ll max_value = *max_element(a.begin(), a.end());
    vector<ll> f(max_value - min_value + 1, 0);
    for (ll x : a) f[x - min_value]++;
    return f;
}

// 1. for every s: number of pairs (i, j) with a[i] + b[j] = s
vector<pair<ll, ll>> all_pair_sums(const vector<ll>& a, const vector<ll>& b) {
    ll mina, minb;
    vector<ll> fa = frequency_array(a, mina), fb = frequency_array(b, minb);
    vector<ll> c = multiply_exact(fa, fb);
    vector<pair<ll, ll>> res;
    for (int s = 0; s < (int)c.size(); s++) if (c[s] > 0) res.push_back({s + mina + minb, c[s]});
    return res;
}

// 2. for every d: number of pairs (i, j) with a[i] - b[j] = d
vector<pair<ll, ll>> all_pair_differences(const vector<ll>& a, vector<ll> b) {
    for (ll& x : b) x = -x;
    return all_pair_sums(a, b);
}

// 3. same array, two DIFFERENT indices i < j: number of pairs with a[i] + a[j] = s
vector<pair<ll, ll>> all_pair_sums_same_array(const vector<ll>& a) {
    ll mn;
    vector<ll> f = frequency_array(a, mn);
    vector<ll> c = multiply_exact(f, f);
    for (int v = 0; v < (int)f.size(); v++) c[2 * v] -= f[v];      // remove i == j
    vector<pair<ll, ll>> res;
    for (int s = 0; s < (int)c.size(); s++) if (c[s] > 0) res.push_back({s + 2 * mn, c[s] / 2});
    return res;
}

// 4. same array: for every d >= 0, number of pairs i < j with |a[i] - a[j]| = d
vector<pair<ll, ll>> all_pair_abs_differences_same_array(const vector<ll>& a) {
    ll mn;
    vector<ll> f = frequency_array(a, mn);
    vector<ll> rev(f.rbegin(), f.rend());
    vector<ll> c = multiply_exact(f, rev);                        // c[off + d] = #ordered (i, j) with a[i] - a[j] = d
    int off = f.size() - 1;
    vector<pair<ll, ll>> res;
    ll zero = (c[off] - (ll)a.size()) / 2;                        // remove i == j, then unordered
    if (zero > 0) res.push_back({0, zero});
    for (int d = 1; off + d < (int)c.size(); d++)
        if (c[off + d] > 0) res.push_back({d, c[off + d]});       // ordered with diff +d == unordered with |diff| d
    return res;
}

// 5. how many subsets (by index) have sum s, for every s in [0, sum(a)]. a[i] >= 0, sum(a) up to ~1e6
vector<ll> subset_sum_counts(const vector<ll>& a, ll mod = 998244353) {
    vector<vector<ll>> polys;
    for (ll x : a) {
        vector<ll> p(x + 1, 0);
        p[0] += 1, p[x] += 1;                                     // 1 + z^x
        polys.push_back(p);
    }
    return multiply_all(polys, mod);
}

// 6. which sums can be made by picking EXACTLY k items (repetition allowed) from a (a[i] >= 0)?
//    returns reachable[s] for s in [0, k * max(a)]. k * max(a) up to ~1e6
vector<bool> reachable_sums_with_k_items(const vector<ll>& a, ll k) {
    ll mx = *max_element(a.begin(), a.end());
    vector<ll> base(mx + 1, 0), res = {1};
    for (ll x : a) base[x] = 1;
    auto clamp01 = [](vector<ll>& p) { for (ll& x : p) x = x ? 1 : 0; };
    while (k > 0) {                                               // counts stay < 998244353 after clamping
        if (k & 1) res = multiply(res, base), clamp01(res);
        k >>= 1;
        if (k) base = multiply(base, base), clamp01(base);
    }
    vector<bool> reachable(res.size());
    for (int i = 0; i < (int)res.size(); i++) reachable[i] = res[i] != 0;
    return reachable;
}

// 7. number of ways to pick one element from EACH array so that the total is s (all values >= 0)
//    returns ways[s] mod `mod` for s in [0, sum of maxes]
vector<ll> pick_one_from_each_sum_counts(const vector<vector<ll>>& arrays, ll mod = 998244353) {
    vector<vector<ll>> polys;
    for (auto& arr : arrays) {
        vector<ll> p(*max_element(arr.begin(), arr.end()) + 1, 0);
        for (ll x : arr) p[x]++;
        polys.push_back(p);
    }
    return multiply_all(polys, mod);
}

// 8. wildcard matching: all positions i where pattern matches text starting at i.
//    `wild` matches any single char, in BOTH strings. O((n + m) log)
vector<int> wildcard_match(const string& text, const string& pattern, char wild = '?') {
    int n = text.size(), m = pattern.size();
    if (m > n) return {};
    // sum over j of  p * t * (p - t)^2  = p^3 t - 2 p^2 t^2 + p t^3, wildcard -> 0
    vector<ll> t1(n), t2(n), t3(n), p1(m), p2(m), p3(m);
    for (int i = 0; i < n; i++) {
        ll v = text[i] == wild ? 0 : (unsigned char)text[i];
        t1[i] = v, t2[i] = v * v, t3[i] = v * v * v;
    }
    for (int j = 0; j < m; j++) {
        ll v = pattern[m - 1 - j] == wild ? 0 : (unsigned char)pattern[m - 1 - j];   // reversed
        p1[j] = v, p2[j] = v * v, p3[j] = v * v * v;
    }
    vector<ll> a = multiply_exact(p3, t1), b = multiply_exact(p2, t2), c = multiply_exact(p1, t3);
    vector<int> res;
    for (int i = 0; i + m <= n; i++) if (a[i + m - 1] - 2 * b[i + m - 1] + c[i + m - 1] == 0) res.push_back(i);
    return res;
}

// 9. for every shift i (0 <= i <= n - m): number of j with text[i + j] == pattern[j]
//    O(alphabet * (n + m) log). good for small alphabets (DNA, binary, lowercase)
vector<ll> match_count_at_each_shift(const string& text, const string& pattern) {
    int n = text.size(), m = pattern.size();
    if (m > n) return {};
    vector<ll> res(n - m + 1, 0);
    set<char> chars(pattern.begin(), pattern.end());
    for (char ch : chars) {
        vector<ll> t(n), p(m);
        for (int i = 0; i < n; i++) t[i] = text[i] == ch;
        for (int j = 0; j < m; j++) p[j] = pattern[m - 1 - j] == ch;
        vector<ll> c = multiply_exact(t, p);
        for (int i = 0; i + m <= n; i++) res[i] += c[i + m - 1];
    }
    return res;
}

// 10. multiply two big non-negative integers given as decimal strings
string multiply_big_numbers(const string& x, const string& y) {
    vector<ll> a(x.rbegin(), x.rend()), b(y.rbegin(), y.rend());
    for (ll& d : a) d -= '0';
    for (ll& d : b) d -= '0';
    vector<ll> c = multiply_exact(a, b);
    for (int i = 0; i < (int)c.size(); i++) {
        if (c[i] >= 10) {
            if (i + 1 == (int)c.size()) c.push_back(0);
            c[i + 1] += c[i] / 10, c[i] %= 10;
        }
    }
    while (c.size() > 1 && c.back() == 0) c.pop_back();
    string s;
    for (int i = c.size() - 1; i >= 0; i--) s += char('0' + c[i]);
    return s;
}


int main(){
    vector<ll> a = {1, 2, 3}, b = {1, 1};
    for (ll x : multiply(a, b)) cout << x << " ";                       // 1 3 5 3
    cout << "\n";
    for (ll x : multiply(a, b, 1000000007)) cout << x << " ";           // same, any mod
    cout << "\n";

    for (auto p : all_pair_sums({1, 2, 5}, {-1, 3})) cout << p.first << ":" << p.second << " ";   // 0:1 1:1 4:2 5:1 8:1
    cout << "\n";
    for (ll x : subset_sum_counts({1, 2, 3})) cout << x << " ";          // 1 1 1 2 1 1 1
    cout << "\n";
    for (int i : wildcard_match("abcabd", "ab?")) cout << i << " ";       // 0 3
    cout << "\n";
    cout << multiply_big_numbers("123456789", "987654321") << "\n";      // 121932631112635269
}
