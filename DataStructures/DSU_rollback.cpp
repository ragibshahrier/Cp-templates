#include<bits/stdc++.h>
using namespace std;

struct DSU { //with rollback, no path compression -> find is O(log n)
    vector<int> par, rnk, sz;
    int c;
    vector<array<int, 3>> hist; //{child, parent, rank_increased}
    DSU(int n) : par(n + 1), rnk(n + 1, 0), sz(n + 1, 1), c(n) {
        for (int i = 1; i <= n; ++i) par[i] = i;
    }
    int find(int i) {
        while (par[i] != i) i = par[i];
        return i;
    }
    bool same(int i, int j) {
        return find(i) == find(j);
    }
    int get_size(int i) { //size of the component containing node i
        return sz[find(i)];
    }
    int count() {
        return c;    //total connected components
    }
    int merge(int i, int j) {
        if ((i = find(i)) == (j = find(j))) return -1;
        else --c;
        if (rnk[i] > rnk[j]) swap(i, j);
        par[i] = j;
        sz[j] += sz[i];
        int inc = (rnk[i] == rnk[j]);
        if (inc) rnk[j]++;
        hist.push_back({i, j, inc});
        return j;
    }
    int time() { //current version, pass it to rollback later
        return (int)hist.size();
    }
    void undo() { //undo the last successful merge
        assert(!hist.empty());
        auto [i, j, inc] = hist.back();
        hist.pop_back();
        par[i] = i;
        sz[j] -= sz[i];
        rnk[j] -= inc;
        ++c;
    }
    void rollback(int t) { //revert to the state at time t
        while ((int)hist.size() > t) undo();
    }
};
