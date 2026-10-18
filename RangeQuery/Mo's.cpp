#include<bits/stdc++.h>
using namespace std;

inline long long hilbertOrder(int x, int y, int pow, int rotate) {
    if (pow == 0) return 0;
    int hpow = 1 << (pow - 1);
    int seg = (x < hpow) ? ((y < hpow) ? 0 : 3) : ((y < hpow) ? 1 : 2);
    seg = (seg + rotate) & 3;
    const int rotateDelta[4] = {3, 0, 0, 1};
    int nx = x & (x ^ hpow), ny = y & (y ^ hpow);
    int nrot = (rotate + rotateDelta[seg]) & 3;
    long long subSquareSize = 1LL << (2 * pow - 2);
    long long ans = seg * subSquareSize;
    long long add = hilbertOrder(nx, ny, pow - 1, nrot);
    ans += (seg == 1 || seg == 2) ? add : (subSquareSize - add - 1);
    return ans;
}

struct Query {
    int L, R, id;
    long long ord;
    bool operator<(const Query& other) const {
        return ord < other.ord;
    }
};

// Global state
long long current_answer = 0;
// int cnt[N];

void add(int index, const vector<int>& arr) {
    // add arr[index] to current state
}

void remove(int index, const vector<int>& arr) {
    // remove arr[index] from current state
}

void solve(int testcases){
    int n, q;
    cin >> n;
    vector<int> arr(n);
    for (int i = 0; i < n; i++) cin >> arr[i];

    cin >> q;

    int max_pow = 1;
    while ((1 << max_pow) <= n) max_pow++;

    vector<Query> queries(q);
    for (int i = 0; i < q; i++) {
        cin >> queries[i].L >> queries[i].R;
        queries[i].L--; queries[i].R--; // remove if input is 0-based
        queries[i].id = i;
        queries[i].ord = hilbertOrder(queries[i].L, queries[i].R, max_pow, 0);
    }

    sort(queries.begin(), queries.end());

    vector<long long> answers(q);
    int currL = 0, currR = -1;

    for (const auto& query : queries) {
        int L = query.L;
        int R = query.R;

        while (currL > L) { currL--; add(currL, arr); }
        while (currR < R) { currR++; add(currR, arr); }
        while (currL < L) { remove(currL, arr); currL++; }
        while (currR > R) { remove(currR, arr); currR--; }

        answers[query.id] = current_answer;
    }

    for (int i = 0; i < q; i++) cout << answers[i] << "\n";
}