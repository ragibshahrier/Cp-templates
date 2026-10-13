#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

// sum_{i<=n} (f*g)(i), given f, g and their prefix sums F, G (each O(1)). Time: O(sqrt n)
template<class T, class Fv, class Gv, class FS, class GS>
T hyperbola(ll n, Fv f, Gv g, FS F, GS G) {
    ll s = sqrtl(n);
    while (s * s > n) s--;
    while ((s + 1) * (s + 1) <= n) s++;
    T r = 0;
    for (ll i = 1; i <= s; i++) r += f(i) * G(n / i) + g(i) * F(n / i);
    return r - F(s) * G(s);
}

int main() {
    ll n; cin >> n;
    // Example: sum of d(i), i<=n   (d = 1*1)
    auto one  = [](ll)    -> ll { return 1; };
    auto onePS = [](ll x) -> ll { return x; };
    cout << hyperbola<ll>(n, one, one, onePS, onePS) << "\n";
}