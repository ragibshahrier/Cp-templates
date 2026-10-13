#include<bits/stdc++.h>
using namespace std;
const int N = 1e6;

#define ll long long

// ---- linear sieve: mu, phi, smallest prime factor, primes ----
int mu[N+1], phi[N+1], spf[N+1];
vector<int> primes;


void linear_sieve() {
    mu[1] = 1; phi[1] = 1;
    for (int i = 2; i <= N; i++) {
        if (!spf[i]) { spf[i] = i; primes.push_back(i); mu[i] = -1; phi[i] = i-1; }
        for (int p : primes) {
            if (p > spf[i] || (long long)p*i > N) break;
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
}


