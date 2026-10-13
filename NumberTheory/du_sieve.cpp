#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

const int TH = 3000000;              // ~ n^(2/3); sieve below this
ll phiPre[TH + 1]; int mobPre[TH + 1];  // prefix sums from the linear sieve
unordered_map<ll, ll> memoPhi;
unordered_map<ll, ll> memoMob;



// ---- linear sieve: mu, phi, smallest prime factor, primes ----
int mu[TH+1], phi[TH+1], spf[TH+1];
vector<int> primes;


void linear_sieve() {
    mu[1] = 1; phi[1] = 1;
    for (int i = 2; i <= TH; i++) {
        if (!spf[i]) { spf[i] = i; primes.push_back(i); mu[i] = -1; phi[i] = i-1; }
        for (int p : primes) {
            if (p > spf[i] || (long long)p*i > TH) break;
            spf[p*i] = p;
            if (p == spf[i]) {               // p^2 | p*i  -> mu = 0
                mu[p*i]  = 0;
                phi[p*i] = phi[i] * p;       // phi(p*i) = p*phi(i)
            } else {
                mu[p*i]  = -mu[i];
                phi[p*i] = phi[i] * (p-1);   // multiplicative
            }
        }
    }
    for(int i = 1; i<=TH; i++){
        mobPre[i]=mobPre[i-1]+mu[i];
        phiPre[i] = phiPre[i-1]+phi[i];
    }
}

ll sumPhi(ll n) {
    if (n <= TH) return phiPre[n];
    unordered_map<ll, ll>::iterator it = memoPhi.find(n);
    if (it != memoPhi.end()) return it->second;
    ll res = (n % 2 == 0) ? (n / 2) * (n + 1) : n * ((n + 1) / 2);
    for (ll l = 2, r; l <= n; l = r + 1) {
        r = n / (n / l);
        res -= (r - l + 1) * sumPhi(n / l);
    }
    memoPhi[n] = res;
    return res;
}


ll sumMob(ll n) {
    if (n <= TH) return mobPre[n];
    unordered_map<ll, ll>::iterator it = memoMob.find(n);
    if (it != memoMob.end()) return it->second;
    ll res = 1;
    for (ll l = 2, r; l <= n; l = r + 1) {
        r = n / (n / l);
        res -= (r - l + 1) * sumMob(n / l);
    }
    memoMob[n] = res;
    return res;
}