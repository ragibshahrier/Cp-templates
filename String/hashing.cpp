#include<bits/stdc++.h>
#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/tree_policy.hpp> 
using namespace std;
// #define ordered_set tree<int, null_type,less<int>, rb_tree_tag,tree_order_statistics_node_update>
using namespace __gnu_pbds;
// Global parameters so powers are precomputed only once
typedef long long ll;
const int MAXN = 2000005; // Adjust to max string length in the problem
const ll MOD1 = 1e9 + 7;
const ll MOD2 = 1e9 + 9;
ll BASE1, BASE2;
ll pw1[MAXN], pw2[MAXN];

// Call this ONCE at the very beginning of main()
void init_hash() {
    mt19937_64 rng(chrono::steady_clock::now().time_since_epoch().count());
    BASE1 = uniform_int_distribution<ll>(313, MOD1 - 1)(rng);
    BASE2 = uniform_int_distribution<ll>(313, MOD2 - 1)(rng);
    
    pw1[0] = pw2[0] = 1;
    for (int i = 1; i < MAXN; i++) {
        pw1[i] = (pw1[i - 1] * BASE1) % MOD1;
        pw2[i] = (pw2[i - 1] * BASE2) % MOD2;
    }
}

struct PolyHash {
    vector<ll> h1, h2;

    PolyHash() {}
    PolyHash(const string& s) {
        int n = s.size();
        h1.resize(n + 1, 0);
        h2.resize(n + 1, 0);
        for (int i = 0; i < n; i++) {
            h1[i + 1] = (h1[i] * BASE1 + s[i]) % MOD1;
            h2[i + 1] = (h2[i] * BASE2 + s[i]) % MOD2;
        }
    }

    // 0-indexed inclusive range [l, r]
    pair<ll, ll> get_hash(int l, int r) {
        ll res1 = (h1[r + 1] - h1[l] * pw1[r - l + 1]) % MOD1;
        if (res1 < 0) res1 += MOD1;
        
        ll res2 = (h2[r + 1] - h2[l] * pw2[r - l + 1]) % MOD2;
        if (res2 < 0) res2 += MOD2;
        
        return {res1, res2};
    }

    // Optional: Pack into a single 64-bit integer for faster unordered_map / sorting
    ll get_combined_hash(int l, int r) {
        auto p = get_hash(l, r);
        return (p.first << 32) | p.second;
    }
};


struct custom_hash {
    static uint64_t splitmix64(uint64_t x) {
        x += 0x9e3779b97f4a7c15;
        x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9;
        x = (x ^ (x >> 27)) * 0x94d049bb133111eb;
        return x ^ (x >> 31);
    }
    size_t operator()(uint64_t x) const {
        static const uint64_t FIXED_RANDOM = chrono::steady_clock::now().time_since_epoch().count();
        return splitmix64(x + FIXED_RANDOM);
    }
};

// Alias for fast typing in contest
template<typename K, typename V>
using gmap = gp_hash_table<K, V, custom_hash>;


void preprocessing(){
    init_hash();
}
