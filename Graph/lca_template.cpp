#include <bits/stdc++.h>
using namespace std;

using vi = vector<int>;
using vvi = vector<vi>;

const int LG = 19; // 2^19 > 5e5

vvi adj;
vector<array<int, LG + 1>> par;
vi dep, sz, tin, tout;
int timer;

void dfs(int u, int p = 0) {
    tin[u] = ++timer;
    par[u][0] = p;
    dep[u] = dep[p] + 1;
    sz[u] = 1;
    for (int i = 1; i <= LG; i++) {
        par[u][i] = par[par[u][i - 1]][i - 1];
    }
    for (int v : adj[u]) {
        if (v != p) {
            dfs(v, u);
            sz[u] += sz[v];
        }
    }
    tout[u] = timer;
}

// 1. O(1) Ancestor Check: Is u an ancestor of v? (True if u == v)
bool is_anc(int u, int v) {
    if (u == 0) return true; // 0 is sentinel ancestor of everything
    return tin[u] <= tin[v] && tout[v] <= tout[u];
}

// 2. Standard LCA in O(log N)
int lca(int u, int v) {
    if (is_anc(u, v)) return u;
    if (is_anc(v, u)) return v;
    for (int k = LG; k >= 0; k--) {
        if (!is_anc(par[u][k], v)) u = par[u][k];
    }
    return par[u][0];
}

// 3. K-th ancestor of u (0-th is u)
int kth(int u, int k) {
    for (int i = 0; i <= LG; i++) {
        if (k & (1 << i)) u = par[u][i];
    }
    return u;
}

// 4. Distance between u and v (number of edges)
int dist(int u, int v) {
    int l = lca(u, v);
    return dep[u] + dep[v] - (dep[l] << 1);
}

// 5. K-th node on simple path from u to v (0-th is u)
int go(int u, int v, int k) {
    int l = lca(u, v);
    int d = dep[u] + dep[v] - (dep[l] << 1);
    if (dep[l] + k <= dep[u]) return kth(u, k);
    k -= dep[u] - dep[l];
    return kth(v, dep[v] - dep[l] - k);
}

// 6. Check if node x lies on the simple path between u and v
bool on_path(int x, int u, int v) {
    return dist(u, x) + dist(x, v) == dist(u, v);
    // Alternative fast check: int l = lca(u, v); return is_anc(l, x) && (is_anc(x, u) || is_anc(x, v));
}

// 7. Junction / Median of 3 nodes (u, v, w) OR Rooted LCA:
//    - The unique node that lies on paths (u,v), (v,w), and (u,w).
//    - Also minimizes dist(x, u) + dist(x, v) + dist(x, w).
//    - Also equals LCA(u, v) if the tree is re-rooted at w!
int junction(int u, int v, int w) {
    return lca(u, v) ^ lca(v, w) ^ lca(u, w);
}

// 8. Intersection of two paths: Path(a, b) and Path(c, d)
//    Returns {u, v} representing the common subpath.
//    If paths don't intersect at all, returns {0, 0}.
pair<int, int> path_intersection(int a, int b, int c, int d) {
    // Any shared endpoints of the intersection must be among the 4 pairwise LCAs
    int cand[4] = {lca(a, c), lca(a, d), lca(b, c), lca(b, d)};
    int L1 = lca(a, b), L2 = lca(c, d);
    
    vi valid;
    for (int x : cand) {
        // A candidate is on both paths iff it is at/below both path LCAs
        if (dep[x] >= dep[L1] && dep[x] >= dep[L2]) {
            valid.push_back(x);
        }
    }
    if (valid.empty()) return {0, 0}; // Disjoint paths!

    // Sort valid candidates by depth (deepest first)
    sort(valid.begin(), valid.end(), [](int x, int y) {
        return dep[x] > dep[y];
    });
    valid.erase(unique(valid.begin(), valid.end()), valid.end());

    if (valid.size() == 1) return {valid[0], valid[0]};
    // The two deepest distinct valid candidates are ALWAYS the endpoints of the common path!
    return {valid[0], valid[1]};
}

// 9. Number of common nodes between Path(a, b) and Path(c, d)
int common_nodes_count(int a, int b, int c, int d) {
    pair<int, int> pr = path_intersection(a, b, c, d); int u = pr.first, v = pr.second;
    if (u == 0) return 0;
    return dist(u, v) + 1; // For common edges count, remove the +1
}

// 10. Direction Step / Child Ancestor:
//     If u is a strict ancestor of v, returns the direct child of u that leads to v.
//     Crucial for rerooting subtree queries!
int child_dir(int u, int v) {
    return kth(v, dep[v] - dep[u] - 1);
}

// 11. Subtree size of u when tree is re-rooted at 'root'
int rooted_sz(int root, int u) {
    if (root == u) return sz[1]; // Whole tree size (assuming initial dfs rooted at 1)
    if (!is_anc(u, root)) return sz[u];
    int c = child_dir(u, root);
    return sz[1] - sz[c];
}

void solve() {
    int n, q;
    cin >> n >> q;

    // Clean & resize in O(n)
    adj = vvi(n + 1);
    par.assign(n + 1, {});
    dep.assign(n + 1, 0);
    sz.assign(n + 1, 0);
    tin.assign(n + 1, 0);
    tout.assign(n + 1, 0);
    timer = 0;

    for (int i = 1; i < n; i++) {
        int u, v;
        cin >> u >> v;
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    dfs(1, 0);

    // Ready for any query!
}