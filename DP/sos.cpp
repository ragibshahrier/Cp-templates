#include <bits/stdc++.h>
using namespace std;

// ======================= SOS DP TEMPLATE =======================
// All transforms work IN PLACE on a vector of size 1<<B. O(B * 2^B).
// No globals -> nothing to clear between testcases; just build a fresh vector.

// f[mask] = sum of a[sub] over all sub ⊆ mask        (zeta over subsets)
template<class T> void sub_sum(vector<T> &f, int B) {
    for (int i = 0; i < B; i++)
        for (int m = 0; m < (1 << B); m++)
            if (m >> i & 1) f[m] += f[m ^ (1 << i)];
}
// inverse of sub_sum (Mobius over subsets)
template<class T> void sub_inv(vector<T> &f, int B) {
    for (int i = 0; i < B; i++)
        for (int m = 0; m < (1 << B); m++)
            if (m >> i & 1) f[m] -= f[m ^ (1 << i)];
}
// f[mask] = sum of a[sup] over all sup ⊇ mask        (zeta over supersets)
template<class T> void sup_sum(vector<T> &f, int B) {
    for (int i = 0; i < B; i++)
        for (int m = 0; m < (1 << B); m++)
            if (!(m >> i & 1)) f[m] += f[m | (1 << i)];
}
// inverse of sup_sum
template<class T> void sup_inv(vector<T> &f, int B) {
    for (int i = 0; i < B; i++)
        for (int m = 0; m < (1 << B); m++)
            if (!(m >> i & 1)) f[m] -= f[m | (1 << i)];
}
// Tip: for max/min/OR/etc. instead of sum, just replace "+=" with f[m] = max(f[m], ...).
// Tip: with a modulus, use a modint type, or add "% MOD" (and "+ MOD" in the inverses).

// ======================= EXAMPLE PROBLEMS =======================

// 1) For every x in a: how many y in a with (x & y) == 0 ?
//    y must be a submask of ~x  ->  cnt over submasks of (~x & FULL)
vector<long long> countDisjoint(const vector<int> &a, int B) {
    vector<long long> f(1 << B, 0);
    for (int x : a) f[x]++;
    sub_sum(f, B);
    int FULL = (1 << B) - 1;
    vector<long long> res;
    for (int x : a) res.push_back(f[FULL ^ x]);
    return res;
}

// 2) For every x in a: how many y in a with (x | y) == x  (y is a submask of x)?
vector<long long> countSubmasks(const vector<int> &a, int B) {
    vector<long long> f(1 << B, 0);
    for (int x : a) f[x]++;
    sub_sum(f, B);
    vector<long long> res;
    for (int x : a) res.push_back(f[x]);
    return res;
}

// 3) Number of non-empty subsequences whose AND is exactly 0   (CF 449D, "Jzzhu and Numbers")
//    g[m] = #elements that are supersets of m -> 2^g[m]-1 subsets have AND ⊇ m
//    then superset-Mobius turns "AND ⊇ m" into "AND == m"
long long andZeroSubsets(const vector<int> &a, int B, long long MOD) {
    vector<long long> f(1 << B, 0);
    for (int x : a) f[x]++;
    sup_sum(f, B);
    vector<long long> pw(a.size() + 1, 1);
    for (size_t i = 1; i < pw.size(); i++) pw[i] = pw[i - 1] * 2 % MOD;
    for (auto &v : f) v = (pw[v] - 1 + MOD) % MOD;
    for (int i = 0; i < B; i++)                       // sup_inv with mod
        for (int m = 0; m < (1 << B); m++)
            if (!(m >> i & 1)) f[m] = (f[m] - f[m | (1 << i)] + MOD) % MOD;
    return f[0];
}

// 4) OR-convolution: c[k] = sum over (i | j) == k of a[i] * b[j]
vector<long long> orConvolution(vector<long long> a, vector<long long> b, int B) {
    sub_sum(a, B); sub_sum(b, B);
    for (int m = 0; m < (1 << B); m++) a[m] *= b[m];
    sub_inv(a, B);
    return a;
}

// 5) AND-convolution: c[k] = sum over (i & j) == k of a[i] * b[j]
vector<long long> andConvolution(vector<long long> a, vector<long long> b, int B) {
    sup_sum(a, B); sup_sum(b, B);
    for (int m = 0; m < (1 << B); m++) a[m] *= b[m];
    sup_inv(a, B);
    return a;
}

// 6) For every mask: max a[i] over i that are submasks of mask (-1 if none)
//    (same loop as sub_sum, with max instead of +)
vector<int> maxOverSubmasks(const vector<int> &a, int B) {
    vector<int> f(1 << B, -1);
    for (int x : a) f[x] = max(f[x], x);
    for (int i = 0; i < B; i++)
        for (int m = 0; m < (1 << B); m++)
            if (m >> i & 1) f[m] = max(f[m], f[m ^ (1 << i)]);
    return f;
}

int main() {
    int t = 2;                               // multiple testcases: nothing to clear
    while (t--) {
        int B = 3;
        vector<int> a = {1, 2, 3, 4, 6};

        for (auto v : countDisjoint(a, B)) cout << v << ' ';  // 3 2 1 3 1
        cout << '\n';
        for (auto v : countSubmasks(a, B)) cout << v << ' ';  // 1 1 3 1 3
        cout << '\n';
        cout << andZeroSubsets(a, B, 1000000007) << '\n';     // 20

        vector<long long> x = {1, 2, 0, 0}, y = {0, 1, 1, 0};  // B = 2
        for (auto v : orConvolution(x, y, 2)) cout << v << ' '; // 0 3 1 2
        cout << '\n';
        for (auto v : andConvolution(x, y, 2)) cout << v << ' '; // 4 2 0 0
        cout << '\n';
    }
    return 0;
}