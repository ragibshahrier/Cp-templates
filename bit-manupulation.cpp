#include<bits/stdc++.h>
using namespace std;
//bit_manipulation
#define checkbit(x,n) (((x)>>(n))&1LL)
#define setbit(x,n) ((x)|=(1LL<<(n)))
#define resetbit(x,n) ((x)&=~(1LL<<(n)))
#define flipbit(x,n) ((x)^=(1LL<<(n)))
#define pow2(i) (1LL<<(i))
#define allones(n) ((1LL<<(n))-1)                 // n low bits set (n<63)
#define lowbit(x) ((x)&(-(x)))                    // lowest set bit value
#define clearlow(x) ((x)&((x)-1))                 // remove lowest set bit
#define ispow2(x) ((x)>0 && (((x)&((x)-1))==0))
#define issubset(a,b) (((a)&(b))==(a))            // a ⊆ b

inline int popcnt(long long x){ return __builtin_popcountll(x); }
inline int parity(long long x){ return __builtin_parityll(x); }
inline int ctz(long long x){ return x ? __builtin_ctzll(x) : 64; }        // trailing zeros
inline int clz(long long x){ return x ? __builtin_clzll(x) : 64; }        // leading zeros (64-bit)
inline int bitlen(long long x){ return x ? 64 - __builtin_clzll(x) : 0; } // # of bits needed
inline int msb(long long x){ return x ? 63 - __builtin_clzll(x) : -1; }   // index of highest set bit = floor(log2)
inline int lsb(long long x){ return x ? __builtin_ctzll(x) : -1; }        // index of lowest set bit
inline int ceil_log2(long long x){ return x <= 1 ? 0 : 64 - __builtin_clzll(x - 1); }
inline long long floor_pow2(long long x){ return x ? 1LL << msb(x) : 0; }
inline long long ceil_pow2(long long x){ return 1LL << ceil_log2(x); }

// next larger number with same popcount (Gosper's hack), x>0
inline long long next_same_pop(long long x){
    long long c = x & -x, r = x + c;
    return (((r ^ x) >> 2) / c) | r;
}

// xor of 0..n
inline long long xor_upto(long long n){
    long long m = n % 4;
    return m == 0 ? n : m == 1 ? 1 : m == 2 ? n + 1 : 0;
}

// count of numbers in [0,n] with bit k set
inline long long cnt_bit_upto(long long n, int k){
    long long p = 1LL << k, full = (n + 1) / (2 * p);
    return full * p + max(0LL, (n + 1) % (2 * p) - p);
}

// iteration helpers
#define forsub(s,m) for(long long s=(m); s>0; s=((s)-1)&(m))     // all non-empty submasks of m (add s=0 manually)
#define forbits(i,x) for(long long _t=(x),i; _t && ((i=__builtin_ctzll(_t)),1); _t&=_t-1) // each set bit index
#define forcomb(s,n,k) for(long long s=(1LL<<(k))-1; s<(1LL<<(n)); s=next_same_pop(s))  // all k-subsets of n bits (k>=1)

// gray code
inline long long gray(long long x){ return x ^ (x >> 1); }
inline long long inv_gray(long long g){ for(long long s = 1; s < 64; s <<= 1) g ^= g >> s; return g; }