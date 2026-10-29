#include<bits/stdc++.h>
using namespace std;

typedef long long ll;

// ===================================== COUNTING PROBLEMS =====================================
// Reduce the problem, then find the matching statement below. Everything is mod MOD (prime).
// Call init_factorials(N) first with N >= the largest number that goes inside nCr
// (for stars and bars that is about n + k).
//
//  1. STARS AND BARS            x1 + ... + xk = n with bounds, multisets, monotone sequences
//  2. BALLS INTO BOXES          the twelvefold way (distinct / identical, empty allowed or not)
//  3. BINOMIAL IDENTITIES       Vandermonde, hockey stick, sums of C(n, k)
//  4. LATTICE PATHS & CATALAN   grid paths, obstacles, reflection, brackets, ballot, Narayana
//  5. PERMUTATIONS              derangements, fixed points, inversions, descents, cycles, involutions
//  6. PARTITIONS / STIRLING     set partitions, Bell, integer partitions
//  7. GAPS & CIRCLES            non-adjacent choices, minimum gap, around a circle
//  8. NECKLACES (BURNSIDE)      colorings up to rotation / reflection
//  9. TREES & GRAPHS            Cayley, Prufer, forests, connected graphs
// 10. INCLUSION - EXCLUSION     divisible by any, coprime count, surjections
// 11. CONTRIBUTION TECHNIQUE    sum of max / min over subsets, subarray sums
// =============================================================================================

const ll MOD = 1e9 + 7;
vector<ll> fact, inv_fact;

ll power(ll a, ll b) {
    ll res = 1;
    a %= MOD;
    if (a < 0) a += MOD;
    while (b > 0) {
        if (b & 1) res = res * a % MOD;
        a = a * a % MOD;
        b >>= 1;
    }
    return res;
}
ll inverse(ll a) { return power(a, MOD - 2); }

void init_factorials(int n) {
    fact.assign(n + 1, 1), inv_fact.assign(n + 1, 1);
    for (int i = 1; i <= n; i++) fact[i] = fact[i - 1] * i % MOD;
    inv_fact[n] = inverse(fact[n]);
    for (int i = n; i > 0; i--) inv_fact[i - 1] = inv_fact[i] * i % MOD;
}
ll nCr(ll n, ll r) {
    if (r < 0 || r > n || n < 0) return 0;
    return fact[n] * inv_fact[r] % MOD * inv_fact[n - r] % MOD;
}
ll nPr(ll n, ll r) {
    if (r < 0 || r > n || n < 0) return 0;
    return fact[n] * inv_fact[n - r] % MOD;
}


// ===================================== 1. STARS AND BARS =====================================

// x1 + x2 + ... + xk = n,  xi >= 0
ll ways_sum_nonneg(ll n, ll k) {
    if (n < 0) return 0;
    if (k == 0) return n == 0;
    return nCr(n + k - 1, k - 1);
}

// x1 + ... + xk = n,  xi >= 1
ll ways_sum_positive(ll n, ll k) { return ways_sum_nonneg(n - k, k); }

// x1 + ... + xk = n,  xi >= low[i]
ll ways_sum_lower_bounds(ll n, const vector<ll>& low) {
    for (ll l : low) n -= l;
    return ways_sum_nonneg(n, low.size());
}

// x1 + ... + xk <= n,  xi >= 0   (add a slack variable)
ll ways_sum_at_most(ll n, ll k) { return ways_sum_nonneg(n, k + 1); }

// x1 + ... + xk = n,  0 <= xi <= up   (inclusion-exclusion on the variables that exceed). O(k)
ll ways_sum_upper_bound(ll n, ll k, ll up) {
    ll res = 0;
    for (ll j = 0; j <= k && j * (up + 1) <= n; j++) {
        ll term = nCr(k, j) * ways_sum_nonneg(n - j * (up + 1), k) % MOD;
        res = (res + (j % 2 ? MOD - term : term)) % MOD;
    }
    return res;
}

// x1 + ... + xk = n,  lo <= xi <= hi
ll ways_sum_in_range(ll n, ll k, ll lo, ll hi) {
    if (lo > hi) return k == 0 && n == 0;
    return ways_sum_upper_bound(n - k * lo, k, hi - lo);
}

// x1 + ... + xk = n,  0 <= xi <= up[i]  (different bounds, k <= ~20). O(2^k)
ll ways_sum_different_upper_bounds(ll n, const vector<ll>& up) {
    int k = up.size();
    ll res = 0;
    for (int mask = 0; mask < (1 << k); mask++) {
        ll rest = n;
        for (int i = 0; i < k; i++) if (mask >> i & 1) rest -= up[i] + 1;
        ll term = ways_sum_nonneg(rest, k);
        res = (res + (__builtin_popcount(mask) % 2 ? MOD - term : term)) % MOD;
    }
    return res;
}

// multisets of size k from n types ("n multichoose k")
ll multichoose(ll n, ll k) { return ways_sum_nonneg(k, n); }

// sequences of length len, values in [1, m], non-decreasing
ll count_non_decreasing_sequences(ll len, ll m) { return multichoose(m, len); }

// sequences of length len, values in [1, m], strictly increasing
ll count_strictly_increasing_sequences(ll len, ll m) { return nCr(m, len); }

// sequences of length len, values in [1, m], with total sum s
ll count_sequences_with_sum(ll len, ll m, ll s) { return ways_sum_in_range(s, len, 1, m); }


// ===================================== 2. BALLS INTO BOXES =====================================
// n balls, k boxes.  "distinct" = labeled.  Stirling / partition helpers are in section 6.

ll distinct_balls_distinct_boxes(ll n, ll k) { return power(k, n); }                 // any
ll distinct_balls_distinct_boxes_at_most_one(ll n, ll k) { return nPr(k, n); }       // injective
// distinct balls, distinct boxes, nonempty (surjective) -> distinct_balls_distinct_boxes_nonempty, section 10
ll identical_balls_distinct_boxes(ll n, ll k) { return ways_sum_nonneg(n, k); }
ll identical_balls_distinct_boxes_nonempty(ll n, ll k) { return ways_sum_positive(n, k); }
ll identical_balls_distinct_boxes_at_most_one(ll n, ll k) { return nCr(k, n); }
// distinct balls, identical boxes:   nonempty -> stirling2(n, k),   any -> sum_{i<=k} stirling2(n, i)
// identical balls, identical boxes:  any -> partitions_at_most_k_parts(n, k),  nonempty -> (n - k, k)
// at most one per box with identical boxes: 1 if n <= k else 0


// ===================================== 3. BINOMIAL IDENTITIES =====================================
// Recognize a sum on the left, replace it by the closed form on the right.

// sum_i C(m, i) * C(n, k - i) = C(m + n, k)                      (Vandermonde: choose k from two groups)
ll vandermonde(ll m, ll n, ll k) { return nCr(m + n, k); }

// sum_i C(n, i)^2 = C(2n, n)
ll sum_of_squared_binomials(ll n) { return nCr(2 * n, n); }

// sum_i C(m, i) * C(n, i) = C(m + n, m)                           (Vandermonde, C(n,i) = C(n,n-i))
ll sum_binomial_products_same_index(ll m, ll n) { return nCr(m + n, m); }

// sum_i C(m, i) * C(n, k + i) = C(m + n, m + k)                   (shifted Vandermonde)
ll sum_binomial_products_shifted(ll m, ll n, ll k) { return nCr(m + n, m + k); }

// sum_{i=r}^{n} C(i, r) = C(n + 1, r + 1)                          (hockey stick: fix the largest element)
ll hockey_stick(ll n, ll r) { return nCr(n + 1, r + 1); }

// sum_{i=0}^{b} C(a + i, i) = C(a + b + 1, b)                      (hockey stick along a diagonal)
ll hockey_stick_diagonal(ll a, ll b) { return nCr(a + b + 1, b); }

// sum_{i=0}^{n} C(i, a) * C(n - i, b) = C(n + 1, a + b + 1)        (choose a+b+1 items, split at the middle one)
ll sum_binomial_split(ll n, ll a, ll b) { return nCr(n + 1, a + b + 1); }

// sum_{k=0}^{m} (-1)^k C(n, k) = (-1)^m C(n - 1, m)                (alternating prefix, n >= 1)
ll alternating_binomial_prefix(ll n, ll m) { return m % 2 ? (MOD - nCr(n - 1, m)) % MOD : nCr(n - 1, m); }

// sum_k C(n, k) = 2^n,     sum over even k = sum over odd k = 2^(n-1)  (n >= 1)
// sum_k k * C(n, k) = n * 2^(n-1)
// sum_k k^2 * C(n, k) = n (n + 1) 2^(n-2)
// sum_k C(n, k) x^k = (1 + x)^n
ll sum_k_times_binomial(ll n) { return n == 0 ? 0 : n % MOD * power(2, n - 1) % MOD; }
ll sum_k2_times_binomial(ll n) {
    if (n == 0) return 0;
    if (n == 1) return 1;
    return n % MOD * ((n + 1) % MOD) % MOD * power(2, n - 2) % MOD;
}


// ===================================== 4. LATTICE PATHS & CATALAN =====================================

// paths (0,0) -> (n,m) with steps right / up
ll grid_paths(ll n, ll m) { return n < 0 || m < 0 ? 0 : nCr(n + m, n); }

// paths (0,0) -> (n,m) avoiding blocked cells. O(b^2), b = #blocked
ll grid_paths_avoiding(ll n, ll m, vector<pair<ll, ll>> blocked) {
    sort(blocked.begin(), blocked.end());
    blocked.push_back({n, m});
    int b = blocked.size();
    vector<ll> dp(b);                                     // dp[i] = paths to blocked[i] avoiding earlier ones
    for (int i = 0; i < b; i++) {
        ll x = blocked[i].first, y = blocked[i].second;
        dp[i] = grid_paths(x, y);
        for (int j = 0; j < i; j++) {
            ll px = blocked[j].first, py = blocked[j].second;
            if (px <= x && py <= y) dp[i] = (dp[i] - dp[j] * grid_paths(x - px, y - py) % MOD + MOD) % MOD;
        }
    }
    return dp[b - 1];
}

// paths (0,0) -> (n,m) (n rights, m ups) that NEVER TOUCH the line y = x + t  (t >= 1). reflection
// "never go above y = x + c"  ->  t = c + 1
ll paths_never_touching_line(ll n, ll m, ll t) {
    if (m >= n + t) return 0;
    return (grid_paths(n, m) - nCr(n + m, n + t) + MOD) % MOD;
}

// balanced bracket sequences with n pairs / Dyck paths / binary trees with n nodes /
// triangulations of an (n+2)-gon / full binary trees with n+1 leaves
ll catalan(ll n) { return nCr(2 * n, n) * inverse(n + 1) % MOD; }

// ways to finish a bracket sequence: current balance b (>= 0), L characters remaining
ll complete_bracket_sequence(ll b, ll L) {
    if (b < 0 || b > L || (L - b) % 2) return 0;
    return (nCr(L, (L + b) / 2) - nCr(L, (L + b) / 2 + 1) + MOD) % MOD;
}

// bracket sequences with n pairs using k types of brackets
ll catalan_k_types(ll n, ll k) { return catalan(n) * power(k, n) % MOD; }

// k-ary trees with n internal nodes (k = 2 -> catalan)
ll fuss_catalan(ll n, ll k) { return nCr(k * n, n) * inverse((k - 1) * n + 1) % MOD; }

// Dyck paths with n pairs and exactly k peaks (k >= 1)
ll narayana(ll n, ll k) { return nCr(n, k) * nCr(n, k - 1) % MOD * inverse(n) % MOD; }

// ballot: a votes for A, b for B (a > b). orders where A is STRICTLY ahead all the time
ll ballot(ll a, ll b) { return (a - b) % MOD * nCr(a + b, a) % MOD * inverse(a + b) % MOD; }

// Motzkin numbers M[0..n]: paths with up/down/flat steps never below 0, chords on a circle
vector<ll> motzkin(int n) {
    vector<ll> M(n + 1);
    M[0] = 1;
    if (n >= 1) M[1] = 1;
    for (ll i = 2; i <= n; i++)
        M[i] = ((2 * i + 1) * M[i - 1] + (3 * i - 3) * M[i - 2]) % MOD * inverse(i + 2) % MOD;
    return M;
}


// ===================================== 5. PERMUTATIONS =====================================

// D[i] = permutations of i with no fixed point
vector<ll> derangements(int n) {
    vector<ll> D(n + 1, 0);
    D[0] = 1;
    for (ll i = 2; i <= n; i++) D[i] = (i - 1) * ((D[i - 1] + D[i - 2]) % MOD) % MOD;
    return D;
}

// permutations of n with exactly k fixed points
ll permutations_with_k_fixed_points(int n, int k) { return nCr(n, k) * derangements(n - k)[n - k] % MOD; }

// I[i] = involutions (p[p[x]] = x) / permutations with only 1- and 2-cycles / ways to pair up or stay single
vector<ll> involutions(int n) {
    vector<ll> I(n + 1);
    I[0] = 1;
    if (n >= 1) I[1] = 1;
    for (ll i = 2; i <= n; i++) I[i] = (I[i - 1] + (i - 1) * I[i - 2]) % MOD;
    return I;
}

// people around a round table (rotations equal): (n-1)!, also reflections equal: (n-1)!/2 (n >= 3)
ll circular_arrangements(ll n) { return n == 0 ? 1 : fact[n - 1]; }

// permutations of n where k specific elements must appear in a fixed relative order
ll permutations_with_fixed_order(ll n, ll k) { return fact[n] * inv_fact[k] % MOD; }

// arrangements of a word / multiset: n! / (c1! c2! ...)
ll multiset_permutations(const vector<ll>& counts) {
    ll n = 0, res = 1;
    for (ll c : counts) n += c, res = res * inv_fact[c] % MOD;
    return res * fact[n] % MOD;
}

// permutations of n with exactly k inversions, for all k in [0, K]. O(n K)
vector<ll> permutations_by_inversions(int n, int K) {
    vector<ll> dp(K + 1, 0);
    dp[0] = 1;
    for (int i = 1; i <= n; i++) {                     // insert element i: adds 0..i-1 inversions
        vector<ll> pre(K + 2, 0), nd(K + 1);
        for (int k = 0; k <= K; k++) pre[k + 1] = (pre[k] + dp[k]) % MOD;
        for (int k = 0; k <= K; k++) {
            int from = k - i + 1;                      // dp[from..k] contribute
            if (from < 0) from = 0;
            nd[k] = (pre[k + 1] - pre[from] + MOD) % MOD;
        }
        dp = nd;
    }
    return dp;
}

// E[n][k] = permutations of n with exactly k descents (p[i] > p[i+1])  (Eulerian numbers). O(n^2)
vector<vector<ll>> eulerian(int n) {
    vector<vector<ll>> E(n + 1, vector<ll>(n + 1, 0));
    E[0][0] = 1;
    for (ll i = 1; i <= n; i++)
        for (ll k = 0; k < i; k++)
            E[i][k] = ((k + 1) * E[i - 1][k] + (k ? (i - k) * E[i - 1][k - 1] : 0)) % MOD;
    return E;
}

// s[n][k] = permutations of n with exactly k cycles (unsigned Stirling numbers of the first kind). O(n^2)
vector<vector<ll>> stirling1(int n) {
    vector<vector<ll>> s(n + 1, vector<ll>(n + 1, 0));
    s[0][0] = 1;
    for (ll i = 1; i <= n; i++)
        for (ll k = 1; k <= i; k++) s[i][k] = (s[i - 1][k - 1] + (i - 1) * s[i - 1][k]) % MOD;
    return s;
}


// ===================================== 6. PARTITIONS / STIRLING =====================================

// S(n, k): split n distinct items into k non-empty unlabeled groups. single value O(k log n)
ll stirling2(ll n, ll k) {
    ll res = 0;
    for (ll j = 0; j <= k; j++) {
        ll term = nCr(k, j) * power(k - j, n) % MOD;
        res = (res + ((j % 2) ? MOD - term : term)) % MOD;
    }
    return res * inv_fact[k] % MOD;
}

// full table S[i][k], O(n^2)
vector<vector<ll>> stirling2_table(int n) {
    vector<vector<ll>> S(n + 1, vector<ll>(n + 1, 0));
    S[0][0] = 1;
    for (ll i = 1; i <= n; i++)
        for (ll k = 1; k <= i; k++) S[i][k] = (S[i - 1][k - 1] + k * S[i - 1][k]) % MOD;
    return S;
}

// B[i] = number of ways to partition a set of i items (Bell numbers). O(n^2) Bell triangle
vector<ll> bell_numbers(int n) {
    vector<ll> B(n + 1);
    vector<ll> row = {1};
    B[0] = 1;
    for (int i = 1; i <= n; i++) {
        vector<ll> nr = {row.back()};
        for (ll x : row) nr.push_back((nr.back() + x) % MOD);
        B[i] = nr[0];
        row = nr;
    }
    return B;
}

// p[i] = number of integer partitions of i (order doesn't matter). O(n sqrt n), pentagonal theorem
vector<ll> partitions(int n) {
    vector<ll> p(n + 1, 0);
    p[0] = 1;
    for (int i = 1; i <= n; i++) {
        for (int k = 1; ; k++) {
            int g1 = k * (3 * k - 1) / 2, g2 = k * (3 * k + 1) / 2;
            if (g1 > i) break;
            ll s = p[i - g1] + (g2 <= i ? p[i - g2] : 0);
            p[i] = (k % 2 ? p[i] + s : p[i] - s + 2 * MOD) % MOD;
        }
    }
    return p;
}

// partitions of n into at most k parts  ==  partitions of n into parts of size <= k. O(n k)
ll partitions_at_most_k_parts(int n, int k) {
    if (n < 0) return 0;
    vector<ll> dp(n + 1, 0);
    dp[0] = 1;
    for (int part = 1; part <= k; part++)
        for (int s = part; s <= n; s++) dp[s] = (dp[s] + dp[s - part]) % MOD;
    return dp[n];
}
// exactly k parts: partitions_at_most_k_parts(n - k, k)

// q[i] = partitions of i into DISTINCT parts (== into odd parts). O(n sqrt n)
vector<ll> partitions_distinct(int n) {
    vector<ll> q(n + 1, 0);
    q[0] = 1;
    for (int part = 1; part <= n; part++)
        for (int s = n; s >= part; s--) q[s] = (q[s] + q[s - part]) % MOD;
    return q;
}


// ===================================== 7. GAPS & CIRCLES =====================================

// choose k of n items in a LINE, no two adjacent  (also: binary strings with k ones, no "11")
ll choose_non_adjacent_line(ll n, ll k) { return k == 0 ? 1 : nCr(n - k + 1, k); }

// choose k of n items on a CIRCLE, no two adjacent (n >= 2)
ll choose_non_adjacent_circle(ll n, ll k) {
    if (k == 0) return 1;
    return (nCr(n - k, k) + nCr(n - k - 1, k - 1)) % MOD;
}

// choose k positions from 1..n so that consecutive chosen positions differ by at least d
ll choose_with_min_gap(ll n, ll k, ll d) { return k == 0 ? 1 : nCr(n - (k - 1) * (d - 1), k); }


// ===================================== 8. NECKLACES (BURNSIDE) =====================================

ll euler_phi(ll x) {
    ll res = x;
    for (ll p = 2; p * p <= x; p++) {
        if (x % p) continue;
        while (x % p == 0) x /= p;
        res -= res / p;
    }
    if (x > 1) res -= res / x;
    return res;
}

// colorings of n beads with k colors, rotations considered equal
ll necklaces(ll n, ll k) {
    ll res = 0;
    for (ll d = 1; d * d <= n; d++) {
        if (n % d) continue;
        res = (res + euler_phi(d) % MOD * power(k, n / d)) % MOD;
        if (d != n / d) res = (res + euler_phi(n / d) % MOD * power(k, d)) % MOD;
    }
    return res * inverse(n) % MOD;
}

// colorings of n beads with k colors, rotations AND reflections considered equal
ll bracelets(ll n, ll k) {
    ll refl;                                                 // sum of fixed colorings over n reflections
    if (n % 2) refl = n % MOD * power(k, (n + 1) / 2) % MOD;
    else refl = (n / 2) % MOD * ((power(k, n / 2 + 1) + power(k, n / 2)) % MOD) % MOD;
    return (necklaces(n, k) * n % MOD + refl) % MOD * inverse(2 * n) % MOD;
}


// ===================================== 9. TREES & GRAPHS =====================================

ll labeled_trees(ll n) { return n <= 2 ? 1 : power(n, n - 2); }                  // Cayley
ll rooted_labeled_trees(ll n) { return power(n, n - 1); }
// forests on n labeled vertices with k trees, where vertices 1..k are in different trees
ll labeled_forests_with_k_given_roots(ll n, ll k) { return k == n ? 1 : k % MOD * power(n, n - k - 1) % MOD; }
// spanning trees of complete bipartite K(a, b)
ll spanning_trees_complete_bipartite(ll a, ll b) { return power(a, b - 1) * power(b, a - 1) % MOD; }
// labeled trees where vertex i has degree deg[i] (Prufer: (n-2)! / prod (deg_i - 1)!). sum deg = 2n - 2
ll trees_with_degrees(const vector<ll>& deg) {
    ll n = deg.size(), sum = 0;
    if (n == 1) return deg[0] == 0;
    ll res = fact[n - 2];
    for (ll d : deg) {
        if (d < 1) return 0;
        sum += d, res = res * inv_fact[d - 1] % MOD;
    }
    return sum == 2 * n - 2 ? res : 0;
}
// labeled trees on n vertices in which one given vertex has degree exactly d: C(n-2, d-1) (n-1)^(n-1-d)
ll trees_where_vertex_has_degree(ll n, ll d) { return nCr(n - 2, d - 1) * power(n - 1, n - 1 - d) % MOD; }

ll labeled_graphs(ll n) { return power(2, n * (n - 1) / 2); }
// c[i] = connected labeled graphs on i vertices. O(n^2)
vector<ll> connected_labeled_graphs(int n) {
    vector<ll> c(n + 1, 0);
    for (int i = 1; i <= n; i++) {
        c[i] = labeled_graphs(i);
        for (int k = 1; k < i; k++)                       // component of vertex 1 has size k
            c[i] = (c[i] - nCr(i - 1, k - 1) * c[k] % MOD * labeled_graphs(i - k) % MOD + MOD) % MOD;
    }
    return c;
}


// ===================================== 10. INCLUSION - EXCLUSION =====================================

// how many x in [1, n] are divisible by AT LEAST ONE of d[0..k-1]. O(2^k), exact (no mod)
ll count_divisible_by_any(ll n, const vector<ll>& d) {
    int k = d.size();
    ll res = 0;
    for (int mask = 1; mask < (1 << k); mask++) {
        ll l = 1;
        bool too_big = false;
        for (int i = 0; i < k && !too_big; i++) if (mask >> i & 1) {
            l = l / __gcd(l, d[i]);
            if (l > n / d[i]) too_big = true;
            else l *= d[i];
        }
        if (too_big) continue;
        res += (__builtin_popcount(mask) % 2 ? 1 : -1) * (n / l);
    }
    return res;
}

// how many x in [1, n] are coprime to m. exact (no mod)
ll count_coprime_up_to(ll n, ll m) {
    vector<ll> primes;
    for (ll p = 2; p * p <= m; p++) {
        if (m % p) continue;
        primes.push_back(p);
        while (m % p == 0) m /= p;
    }
    if (m > 1) primes.push_back(m);
    return n - count_divisible_by_any(n, primes);
}

// surjections from n distinct items onto k distinct boxes (every box non-empty) = k! S(n, k)
ll distinct_balls_distinct_boxes_nonempty(ll n, ll k) {
    ll res = 0;
    for (ll j = 0; j <= k; j++) {
        ll term = nCr(k, j) * power(k - j, n) % MOD;
        res = (res + ((j % 2) ? MOD - term : term)) % MOD;
    }
    return res;
}

// strings of length n over an alphabet of size k that use ALL k letters  ==  surjections
// strings of length n over alphabet k that avoid a specific set of j letters  ==  (k - j)^n


// ===================================== 11. CONTRIBUTION TECHNIQUE =====================================
// "sum over all subsets / subarrays of f" -> count for each element how many times it is the answer

// sum over all k-subsets of max(subset). (a is copied and sorted)
ll sum_of_max_over_k_subsets(vector<ll> a, ll k) {
    sort(a.begin(), a.end());
    ll res = 0;
    for (ll i = 0; i < (ll)a.size(); i++) res = (res + (a[i] % MOD + MOD) % MOD * nCr(i, k - 1)) % MOD;
    return res;
}
// sum over all k-subsets of min(subset)
ll sum_of_min_over_k_subsets(vector<ll> a, ll k) {
    sort(a.begin(), a.end());
    ll n = a.size(), res = 0;
    for (ll i = 0; i < n; i++) res = (res + (a[i] % MOD + MOD) % MOD * nCr(n - 1 - i, k - 1)) % MOD;
    return res;
}
// sum over ALL non-empty subsets of (max - min)
ll sum_of_max_minus_min_over_subsets(vector<ll> a) {
    sort(a.begin(), a.end());
    ll n = a.size(), res = 0;
    for (ll i = 0; i < n; i++) {
        ll times = (power(2, i) - power(2, n - 1 - i) + MOD) % MOD;   // max in 2^i subsets, min in 2^(n-1-i)
        res = (res + (a[i] % MOD + MOD) % MOD * times) % MOD;
    }
    return res;
}
// sum over all subarrays of their sums: a[i] appears in (i + 1) * (n - i) subarrays
ll sum_of_all_subarray_sums(const vector<ll>& a) {
    ll n = a.size(), res = 0;
    for (ll i = 0; i < n; i++) res = (res + (a[i] % MOD + MOD) % MOD * ((i + 1) * (n - i) % MOD)) % MOD;
    return res;
}
// sum over all subsets of their sums: each element is in 2^(n-1) subsets
ll sum_of_all_subset_sums(const vector<ll>& a) {
    ll s = 0;
    for (ll x : a) s = (s + (x % MOD + MOD)) % MOD;
    return a.empty() ? 0 : s * power(2, a.size() - 1) % MOD;
}


int main(){
    init_factorials(2000000);
    cout << ways_sum_nonneg(5, 3) << " " << ways_sum_positive(5, 3) << " " << ways_sum_upper_bound(5, 3, 2) << "\n"; // 21 6 3
    cout << catalan(5) << " " << grid_paths_avoiding(2, 2, {{1, 1}}) << " " << complete_bracket_sequence(1, 3) << "\n"; // 42 2 2
    cout << derangements(5)[5] << " " << stirling2(5, 2) << " " << bell_numbers(5)[5] << " " << partitions(10)[10] << "\n"; // 44 15 52 42
    cout << necklaces(4, 2) << " " << bracelets(6, 2) << " " << labeled_trees(5) << "\n";                           // 6 13 125
    cout << count_divisible_by_any(100, {2, 3}) << " " << count_coprime_up_to(100, 12) << "\n";                     // 67 33
}
