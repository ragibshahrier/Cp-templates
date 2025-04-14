#include<bits/stdc++.h>
using namespace std;
#define DEBG
#ifdef DEBG
template<typename T> void _print(T x) { cerr << x; }
template<typename T, typename U> void _print(pair<T,U> p) { cerr << "{"; _print(p.first); cerr << ","; _print(p.second); cerr << "}"; }
template<typename T> void _print(vector<T> v) { cerr << "["; for (auto &i : v) { _print(i); cerr << ", "; } cerr << "]"; }
template<typename T> void _print(set<T> s) { cerr << "{"; for (auto &i : s) { _print(i); cerr << ", "; } cerr << "}"; }
template<typename T, typename U> void _print(map<T,U> m) { cerr << "["; for (auto &p : m) { _print(p); cerr << " "; } cerr << "]"; }
#define debug(x) cerr <<__LINE__<<": "<< #x << " = "; _print(x); cerr << endl;
#else
#define debug(x)
#endif