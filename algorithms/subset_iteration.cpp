#include<bits/stdc++.h>
using namespace std;

typedef long long ll;

// 1. All subsets of a vector (2^n of them, including empty). n <= ~20
void all_subsets_of_vector(vector<int>& a) {
    int n = a.size();
    for (int mask = 0; mask < (1 << n); mask++) {
        vector<int> sub;
        for (int i = 0; i < n; i++)
            if (mask >> i & 1) sub.push_back(a[i]);
        // use sub here
    }
}

// 2. All subsets of a vector with exactly k elements (Gosper's hack: next mask with same popcount)
void subsets_of_size_k(vector<int>& a, int k) {
    int n = a.size();
    if (k == 0) { /* only the empty subset */ return; }
    for (int mask = (1 << k) - 1; mask < (1 << n);) {
        vector<int> sub;
        for (int i = 0; i < n; i++)
            if (mask >> i & 1) sub.push_back(a[i]);
        // use sub here
        int c = mask & -mask, r = mask + c;
        mask = (((r ^ mask) >> 2) / c) | r;
    }
}

// 3. All submasks of mask, from largest to smallest, INCLUDING mask itself and 0
void submasks(int mask) {
    for (int s = mask; ; s = (s - 1) & mask) {
        // use s here
        if (s == 0) break;
    }
}
// non-empty submasks only:
//     for (int s = mask; s > 0; s = (s - 1) & mask) { ... }
// proper submasks only (excluding mask itself):
//     for (int s = (mask - 1) & mask; ; s = (s - 1) & mask) { ...; if (s == 0) break; }

// 4. All supermasks of mask within n bits, INCLUDING mask itself and (1<<n)-1
void supermasks(int mask, int n) {
    for (int s = mask; s < (1 << n); s = (s + 1) | mask) {
        // use s here
    }
}

// 5. Every mask together with every submask of it. total O(3^n)
void all_masks_and_their_submasks(int n) {
    for (int mask = 0; mask < (1 << n); mask++)
        for (int s = mask; ; s = (s - 1) & mask) {
            // use (mask, s) here
            if (s == 0) break;
        }
}

// Handy bit helpers
// popcount: __builtin_popcount(x) / __builtin_popcountll(x)
// lowest set bit: x & -x      index of it: __builtin_ctz(x)
// highest set bit index: 31 - __builtin_clz(x)
// the complement of mask inside n bits: ((1 << n) - 1) ^ mask


int main(){
    vector<int> a = {1, 2, 3};
    int n = a.size();
    for (int mask = 0; mask < (1 << n); mask++) {
        cout << "{ ";
        for (int i = 0; i < n; i++) if (mask >> i & 1) cout << a[i] << " ";
        cout << "} ";
    }
    cout << "\n";

    int mask = 0b1011;
    for (int s = mask; ; s = (s - 1) & mask) {
        cout << bitset<4>(s) << " ";                // 1011 1010 1001 1000 0011 0010 0001 0000
        if (s == 0) break;
    }
    cout << "\n";

    for (int s = 0b0101; s < (1 << 4); s = (s + 1) | 0b0101) cout << bitset<4>(s) << " ";  // 0101 0111 1101 1111
    cout << "\n";

    for (int m = (1 << 2) - 1; m < (1 << 4);) {     // size-2 subsets of 4 bits
        cout << bitset<4>(m) << " ";
        int c = m & -m, r = m + c;
        m = (((r ^ m) >> 2) / c) | r;
    }
    cout << "\n";
}
