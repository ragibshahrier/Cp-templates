#include<bits/stdc++.h>
using namespace std;
typedef long long ll;

/*
  Hungarian algorithm (assignment problem)
  Complexity: O(n^2 * m) with n = min side, m = max side  (n = m = 500 runs in ~0.1s)

  cost is a 0-indexed n x m matrix. Every vertex of the SMALLER side gets exactly one
  distinct partner, total cost is minimized. Negative costs are fine.

  Pitfalls (handled below, keep in mind when modelling):
    - Missing edge: do NOT put 1e18, potentials overflow. Use a BIG that is just larger than
      any possible real total (see min_cost_max_matching).
    - Max weight with missing edges: "inf for missing + negate" maximizes the NUMBER of pairs
      first, then weight. If pairs are optional use max_weight_matching instead.
    - Matrix is passed by vector, nothing huge on the stack; no fixed N.
    - Real valued costs (euclidean distances): change cost_t to double / long double.
*/

typedef long long cost_t;

// 1. Core: returns {min total cost, match}, match[i] = column of row i (-1 if row unmatched, only when n > m)
pair<cost_t, vector<int>> hungarian(const vector<vector<cost_t>>& cost) {
  int n = cost.size();
  if (n == 0) return {0, {}};
  int m = cost[0].size();
  if (n > m) {                                        // solve on the transpose
    vector<vector<cost_t>> t(m, vector<cost_t>(n));
    for (int i = 0; i < n; i++) for (int j = 0; j < m; j++) t[j][i] = cost[i][j];
    auto res = hungarian(t);
    vector<int> match(n, -1);
    for (int j = 0; j < m; j++) match[res.second[j]] = j;
    return {res.first, match};
  }
  const cost_t INF = numeric_limits<cost_t>::max();
  vector<cost_t> u(n + 1, 0), v(m + 1, 0), minv(m + 1);
  vector<int> p(m + 1, 0), way(m + 1, 0);              // p[j] = row (1-indexed) matched to column j
  vector<bool> used(m + 1);
  for (int i = 1; i <= n; i++) {
    p[0] = i;
    int j0 = 0;
    fill(minv.begin(), minv.end(), INF);
    fill(used.begin(), used.end(), false);
    do {
      used[j0] = true;
      int i0 = p[j0], j1 = 0;
      cost_t delta = INF;
      for (int j = 1; j <= m; j++) if (!used[j]) {
        cost_t cur = cost[i0 - 1][j - 1] - u[i0] - v[j];
        if (cur < minv[j]) minv[j] = cur, way[j] = j0;
        if (minv[j] < delta) delta = minv[j], j1 = j;
      }
      for (int j = 0; j <= m; j++) {
        if (used[j]) u[p[j]] += delta, v[j] -= delta;
        else minv[j] -= delta;
      }
      j0 = j1;
    } while (p[j0] != 0);
    do {                                              // flip the augmenting path
      int j1 = way[j0];
      p[j0] = p[j1];
      j0 = j1;
    } while (j0);
  }
  vector<int> match(n, -1);
  for (int j = 1; j <= m; j++) if (p[j]) match[p[j] - 1] = j - 1;
  cost_t total = 0;
  for (int i = 0; i < n; i++) total += cost[i][match[i]];
  return {total, match};
}

// 2. Max total assignment (every vertex of the smaller side is assigned)
pair<cost_t, vector<int>> max_cost_assignment(vector<vector<cost_t>> w) {
  for (auto& row : w) for (auto& x : row) x = -x;
  auto res = hungarian(w);
  return {-res.first, res.second};
}

// ---------------- Edge list based snippets ----------------
// left side 0..n-1, right side 0..m-1
struct WEdge { int u, v; cost_t w; };
struct MatchResult {
  int count;                    // number of matched pairs
  cost_t cost;                  // total weight of matched pairs
  vector<pair<int, int>> pairs; // (left, right)
};

// 3. Max weight matching of ANY size (pairs are optional, negative edges never taken)
MatchResult max_weight_matching(int n, int m, const vector<WEdge>& edges) {
  vector<vector<cost_t>> w(n, vector<cost_t>(m, 0));       // 0 = leave both unmatched
  vector<vector<bool>> real(n, vector<bool>(m, false));
  for (auto& e : edges) if (e.w > w[e.u][e.v]) w[e.u][e.v] = e.w, real[e.u][e.v] = true;
  auto res = max_cost_assignment(w);
  MatchResult r = {0, 0, {}};
  for (int i = 0; i < n; i++) {
    int j = res.second[i];
    if (j != -1 && real[i][j]) r.count++, r.cost += w[i][j], r.pairs.push_back({i, j});
  }
  return r;
}

// 4. Maximum cardinality matching, among those the min total cost (this is what the old template did)
//    perfect matching exists  <=>  count == min(n, m)
//    for "max cardinality then MAX cost" pass negated weights and negate the cost back
MatchResult min_cost_max_matching(int n, int m, const vector<WEdge>& edges) {
  cost_t maxabs = 0;
  for (auto& e : edges) maxabs = max(maxabs, e.w < 0 ? -e.w : e.w);
  cost_t BIG = 2 * (maxabs + 1) * (min(n, m) + 1);          // one more real pair always beats any cost difference
  vector<vector<cost_t>> c(n, vector<cost_t>(m, BIG));
  for (auto& e : edges) c[e.u][e.v] = min(c[e.u][e.v], e.w);
  auto res = hungarian(c);
  MatchResult r = {0, 0, {}};
  for (int i = 0; i < n; i++) {
    int j = res.second[i];
    if (j != -1 && c[i][j] != BIG) r.count++, r.cost += c[i][j], r.pairs.push_back({i, j});
  }
  return r;
}

// 5. Exactly k pairs with min total cost (full n x m cost matrix, k <= min(n, m))
//    square matrix of size n + m - k: dummy rows take the unused columns, dummy cols take the unused rows
pair<cost_t, vector<int>> min_cost_exactly_k_pairs(const vector<vector<cost_t>>& cost, int k) {
  int n = cost.size(), m = cost[0].size(), s = n + m - k;
  cost_t maxabs = 0;
  for (auto& row : cost) for (cost_t x : row) maxabs = max(maxabs, x < 0 ? -x : x);
  cost_t BIG = 2 * (maxabs + 1) * (s + 1);
  vector<vector<cost_t>> c(s, vector<cost_t>(s, 0));
  for (int i = n; i < s; i++) for (int j = m; j < s; j++) c[i][j] = BIG;   // dummy - dummy forbidden
  for (int i = 0; i < n; i++) for (int j = 0; j < m; j++) c[i][j] = cost[i][j];
  auto res = hungarian(c);
  vector<int> match(n, -1);                                  // match[i] = column or -1
  cost_t total = 0;
  for (int i = 0; i < n; i++) if (res.second[i] < m) match[i] = res.second[i], total += cost[i][match[i]];
  return {total, match};
}

// 6. Job j can take up to cap[j] workers, every worker gets one job (if total capacity allows)
//    returns {min cost, job of each worker (-1 if no capacity left)}
pair<cost_t, vector<int>> assignment_with_capacities(const vector<vector<cost_t>>& cost, const vector<int>& cap) {
  int n = cost.size(), m = cap.size();
  vector<int> owner;                                         // copied column -> original job
  for (int j = 0; j < m; j++) for (int t = 0; t < cap[j]; t++) owner.push_back(j);
  if (owner.empty()) return {0, vector<int>(n, -1)};
  vector<vector<cost_t>> c(n, vector<cost_t>(owner.size()));
  for (int i = 0; i < n; i++) for (int k = 0; k < (int)owner.size(); k++) c[i][k] = cost[i][owner[k]];
  auto res = hungarian(c);
  vector<int> job(n, -1);
  for (int i = 0; i < n; i++) if (res.second[i] != -1) job[i] = owner[res.second[i]];
  return {res.first, job};
}

// 7. Min cost cycle cover of a directed graph: every vertex has exactly one out edge and one in edge
//    (vertex-disjoint cycles covering all vertices). add (u, u, w) edges if self loops are allowed.
//    returns false if impossible, otherwise nxt[u] = successor of u
bool min_cost_cycle_cover(int n, const vector<WEdge>& edges, cost_t& total, vector<int>& nxt) {
  MatchResult r = min_cost_max_matching(n, n, edges);
  if (r.count < n) return false;
  total = r.cost;
  nxt.assign(n, -1);
  for (auto& pr : r.pairs) nxt[pr.first] = pr.second;
  return true;
}

// 8. Min cost path cover of a DAG: cost = (sum of used edge weights) + path_cost * (number of paths)
//    every vertex lies on exactly one path. nxt[u] = next vertex on its path or -1.
//    path_cost = 1 and all w = 0 gives the classic minimum path cover (= n - max matching)
cost_t min_cost_path_cover_dag(int n, const vector<WEdge>& edges, cost_t path_cost, vector<int>& nxt) {
  // row u -> column v (< n): u continues to v, cost w - path_cost (saves one path)
  // row u -> column n + u : u ends its path, cost 0; total = path_cost * n + matrix cost
  cost_t maxabs = 0;
  for (auto& e : edges) maxabs = max(maxabs, e.w < 0 ? -e.w : e.w);
  maxabs += path_cost < 0 ? -path_cost : path_cost;
  cost_t BIG = 2 * (maxabs + 1) * (n + 1);
  vector<vector<cost_t>> c(n, vector<cost_t>(2 * n, BIG));
  for (int u = 0; u < n; u++) c[u][n + u] = 0;
  for (auto& e : edges) c[e.u][e.v] = min(c[e.u][e.v], e.w - path_cost);
  auto res = hungarian(c);
  nxt.assign(n, -1);
  for (int u = 0; u < n; u++) if (res.second[u] < n) nxt[u] = res.second[u];
  return res.first + path_cost * n;
}

// 9. Best bijection of an alphabet: choose a permutation f of 0..K-1 maximizing #{i : f(s[i]) == t[i]}
//    (decode a substitution cipher, relabel colors/groups to agree with a reference). returns {agree, f}
pair<ll, vector<int>> best_bijection(const vector<int>& s, const vector<int>& t, int K) {
  vector<vector<cost_t>> cnt(K, vector<cost_t>(K, 0));
  for (int i = 0; i < (int)s.size(); i++) cnt[s[i]][t[i]]++;
  auto res = max_cost_assignment(cnt);
  return {(ll)res.first, res.second};
}

// 10. Bottleneck assignment: minimize the MAXIMUM cost used (smaller side fully assigned)
//     binary search on the answer, check "assignment using only cost <= x" with a 0/1 hungarian
pair<cost_t, vector<int>> min_bottleneck_assignment(const vector<vector<cost_t>>& cost) {
  int n = cost.size(), m = cost[0].size();
  vector<cost_t> vals;
  for (auto& row : cost) for (cost_t x : row) vals.push_back(x);
  sort(vals.begin(), vals.end());
  vals.erase(unique(vals.begin(), vals.end()), vals.end());
  int lo = 0, hi = vals.size() - 1;
  vector<int> best;
  while (lo < hi) {
    int mid = (lo + hi) / 2;
    vector<vector<cost_t>> c(n, vector<cost_t>(m));
    for (int i = 0; i < n; i++) for (int j = 0; j < m; j++) c[i][j] = cost[i][j] > vals[mid];
    if (hungarian(c).first == 0) hi = mid;
    else lo = mid + 1;
  }
  vector<vector<cost_t>> c(n, vector<cost_t>(m));
  for (int i = 0; i < n; i++) for (int j = 0; j < m; j++) c[i][j] = cost[i][j] > vals[lo];
  return {vals[lo], hungarian(c).second};
}

int main() {
  // workers x jobs
  vector<vector<cost_t>> cost = {
    {4, 1, 3},
    {2, 0, 5},
    {3, 2, 2}
  };
  auto res = hungarian(cost);
  cout << "min assignment = " << res.first << '\n';                 // 5  (0->1, 1->0, 2->2)
  for (int i = 0; i < 3; i++) cout << i << " -> " << res.second[i] << '\n';
  cout << "max assignment = " << max_cost_assignment(cost).first << '\n';   // 11

  // pairs optional: one heavy edge beats two light ones
  vector<WEdge> edges = {{0, 0, 1}, {1, 1, 1}, {0, 1, 100}};
  cout << "max weight matching = " << max_weight_matching(2, 2, edges).cost << '\n';         // 100
  MatchResult r = min_cost_max_matching(2, 2, edges);
  cout << "max cardinality = " << r.count << ", min cost = " << r.cost << '\n';             // 2, 2

  // DAG 0->1->2, 3 alone: min path cover = 2
  vector<int> nxt;
  cout << "path cover = " << min_cost_path_cover_dag(4, {{0, 1, 0}, {1, 2, 0}}, 1, nxt) << '\n';
  return 0;
}
