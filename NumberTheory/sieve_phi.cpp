#include <bits/stdc++.h>

using namespace std;

#define ll long long

void phi_1_to_n(int n) {
    vector<int> phi(n + 1);
    for (int i = 0; i <= n; i++)
        phi[i] = i;

    for (int i = 2; i <= n; i++) {
        if (phi[i] == i) {
            for (int j = i; j <= n; j += i)
                phi[j] -= phi[j] / i;
        }
    }
}



const int N  = 1e6+7;
int spf[N+1];
vector<int> primes;


void linear_sieve() {
    for (int i = 2; i <= N; i++) {
        if (!spf[i]) { spf[i] = i; primes.push_back(i); }
        for (int p : primes) {
            if (p > spf[i] || (long long)p*i > N) break;
            spf[p*i] = p;
        }
    }
}



vector<ll> phi_segment(ll L, ll R)
{
    int len = R - L + 1;
    vector<ll> rem(len), res(len);
    for (int i = 0; i < len; i++)
        rem[i] = res[i] = L + i;
    for (int p : primes)
    {
        if ((ll)p * p > R)
            break;
        for (ll x = (L + p - 1) / p * p; x <= R; x += p)
        {
            int i = x - L;
            res[i] -= res[i] / p;
            while (rem[i] % p == 0)
                rem[i] /= p;
        }
    }
    for (int i = 0; i < len; i++)
        if (rem[i] > 1)
            res[i] -= res[i] / rem[i]; // leftover big prime
    return res;
}