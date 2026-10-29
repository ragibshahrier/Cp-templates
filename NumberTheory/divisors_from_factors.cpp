#include<bits/stdc++.h>
using namespace std;

typedef long long ll;

// Input: factorisation of n as {prime, count}, e.g. 360 -> {{2,3},{3,2},{5,1}}
// From a map<int,int> (like prime_factorisation() returns):  vector<pair<ll,int>> f(mp.begin(), mp.end());

// 1. All divisors of n (UNSORTED). O(number of divisors)
vector<ll> get_divisors(const vector<pair<ll, int>>& f) {
    vector<ll> divs = {1};
    for (auto& pc : f) {
        ll p = pc.first;
        int sz = divs.size();
        ll pw = 1;
        for (int e = 1; e <= pc.second; e++) {
            pw *= p;
            for (int i = 0; i < sz; i++) divs.push_back(divs[i] * pw);
        }
    }
    return divs;   // sort(divs.begin(), divs.end()) if you need them in order
}

// 2. Visit every divisor without storing them. cb(d) is called once per divisor
void for_each_divisor(const vector<pair<ll, int>>& f, const function<void(ll)>& cb, int i = 0, ll cur = 1) {
    if (i == (int)f.size()) { cb(cur); return; }
    for (int e = 0; e <= f[i].second; e++) {
        for_each_divisor(f, cb, i + 1, cur);
        cur *= f[i].first;
    }
}

// 3. Same, but also gives the exponent of every prime in the divisor: d = prod f[i].first ^ exps[i]
void for_each_divisor_with_exponents(const vector<pair<ll, int>>& f, const function<void(ll, vector<int>&)>& cb) {
    vector<int> exps(f.size(), 0);
    function<void(int, ll)> go = [&](int i, ll cur) {
        if (i == (int)f.size()) { cb(cur, exps); return; }
        for (int e = 0; e <= f[i].second; e++) {
            exps[i] = e;
            go(i + 1, cur);
            cur *= f[i].first;
        }
        exps[i] = 0;
    };
    go(0, 1);
}

// 4. Number of divisors = prod (cnt + 1)
ll count_divisors(const vector<pair<ll, int>>& f) {
    ll res = 1;
    for (auto& pc : f) res *= pc.second + 1;
    return res;
}

// 5. Sum of divisors = prod (1 + p + p^2 + ... + p^cnt)   (can overflow for big n -> use mod)
ll sum_divisors(const vector<pair<ll, int>>& f) {
    ll res = 1;
    for (auto& pc : f) {
        ll term = 1, pw = 1;
        for (int e = 1; e <= pc.second; e++) pw *= pc.first, term += pw;
        res *= term;
    }
    return res;
}


int main(){
    vector<pair<ll, int>> f = {{2, 3}, {3, 2}, {5, 1}};   // 360

    vector<ll> divs = get_divisors(f);
    sort(divs.begin(), divs.end());
    for (ll d : divs) cout << d << " ";
    cout << "\n";

    ll sum = 0;
    for_each_divisor(f, [&](ll d) { sum += d; });
    cout << sum << " " << sum_divisors(f) << " " << count_divisors(f) << "\n";   // 1170 1170 24

    for_each_divisor_with_exponents(f, [&](ll d, vector<int>& e) {
        if (d == 12) cout << "12 = 2^" << e[0] << " * 3^" << e[1] << " * 5^" << e[2] << "\n";
    });
}
