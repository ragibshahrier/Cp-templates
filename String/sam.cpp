#include <bits/stdc++.h>
using namespace std;

struct SuffixAutomaton {
    // ---- pick ONE alphabet ----
    const static int K = 26;  static int idx(char c) { return c - 'a'; }                 // a-z
    // const static int K = 52;  static int idx(char c) { return islower(c) ? c - 'a' : c - 'A' + 26; }  // a-z A-Z
    // const static int K = 62;  static int idx(char c) { return islower(c) ? c - 'a' : isupper(c) ? c - 'A' + 26 : c - '0' + 52; } // a-z A-Z 0-9
    // const static int K = 10;  static int idx(char c) { return c - '0'; }                 // digits only
    // const static int K = 128; static int idx(char c) { return (unsigned char)c; }        // any ASCII

    struct State {
        int len = 0;        // length of the LONGEST substring in this class
        int link = -1;      // suffix link: state of the longest suffix that lives in a different class
        int next[K];        // DAG transitions
        // --- tweakable auxiliary fields ---
        long long cnt = 0;  // #occurrences of every substring of this state (valid after build_counts)
        int first_pos = -1; // 0-based END index of the first occurrence
        bool is_clone = false;
        State() { memset(next, -1, sizeof(next)); }
    };

    vector<State> st;
    int last;           // state of the whole string inserted so far

    SuffixAutomaton(int max_len = 0) {
        st.reserve(2 * max_len + 2);
        st.emplace_back();  // root = 0 (the empty string)
        last = 0;
    }

    // Append one character. O(1) amortized.
    void extend(char ch, int pos = 0) {
        int c = idx(ch);
        int cur = st.size();
        st.emplace_back();
        st[cur].len = st[last].len + 1;
        st[cur].cnt = 1;            // a real prefix end -> 1 occurrence
        st[cur].first_pos = pos;

        int p = last;
        while (p != -1 && st[p].next[c] == -1) {
            st[p].next[c] = cur;
            p = st[p].link;
        }
        if (p == -1) {
            st[cur].link = 0;
        } else {
            int q = st[p].next[c];
            if (st[p].len + 1 == st[q].len) {
                st[cur].link = q;
            } else {
                int clone = st.size();
                State tmp = st[q];  // copy first: push_back(st[q]) is unsafe if vector reallocates
                st.push_back(tmp);  // copies next[], link, first_pos
                st[clone].len = st[p].len + 1;
                st[clone].cnt = 0;  // clones add no new end positions
                st[clone].is_clone = true;
                while (p != -1 && st[p].next[c] == q) {
                    st[p].next[c] = clone;
                    p = st[p].link;
                }
                st[q].link = st[cur].link = clone;
            }
        }
        last = cur;
    }

    void build(const string &s) { for (int i = 0; i < (int)s.size(); i++) extend(s[i], i); }

    // States sorted by len DESCENDING. Valid order for both:
    //  - link tree bottom-up (child len > parent len)
    //  - DAG reverse-topological (every transition goes to a larger len)
    vector<int> order_desc() {
        int sz = st.size(), mx = 0;
        for (auto &x : st) mx = max(mx, x.len);
        vector<int> bucket(mx + 1, 0), order(sz);
        for (auto &x : st) bucket[x.len]++;
        for (int i = 1; i <= mx; i++) bucket[i] += bucket[i - 1];
        for (int i = sz - 1; i >= 0; i--) order[--bucket[st[i].len]] = i;
        reverse(order.begin(), order.end());
        return order;
    }

    // Call ONCE after building, if you need cnt.
    void build_counts() {
        for (int u : order_desc())
            if (st[u].link != -1) st[st[u].link].cnt += st[u].cnt;
    }

    // Walk pattern from root. Returns state, or -1 if p is not a substring.
    int walk(const string &p) {
        int v = 0;
        for (char ch : p) { v = st[v].next[idx(ch)]; if (v == -1) return -1; }
        return v;
    }
};

// ======================= EXAMPLE PROBLEMS =======================
// Mental model:
//  * Each state = a set of substrings with the SAME set of end positions (endpos).
//    They are suffixes of each other, with lengths (len[link], len]. So the state holds
//    len[v] - len[link[v]] distinct substrings.
//  * link[v] = the state of the next shorter suffix, which has a BIGGER endpos set.
//    The links form a tree rooted at 0.
//  * cnt[v] = |endpos(v)| = how many times each substring of v occurs. Each prefix end
//    contributes 1, and summing up the link tree gives every suffix of it +1.
//  * Walking chars from root = reading a substring. Every path from root = a distinct substring.

// 1) Is p a substring of s?
bool isSub(SuffixAutomaton &sam, const string &p) { return sam.walk(p) != -1; }

// 2) How many times does p occur in s? (needs build_counts)
long long occurrences(SuffixAutomaton &sam, const string &p) {
    int v = sam.walk(p);
    return v == -1 ? 0 : sam.st[v].cnt;
}

// 3) First occurrence start index of p (or -1)
int firstOccurrence(SuffixAutomaton &sam, const string &p) {
    int v = sam.walk(p);
    return v == -1 ? -1 : sam.st[v].first_pos - (int)p.size() + 1;
}

// 4) Number of distinct non-empty substrings: each state owns len - len[link] of them
long long distinctSubstrings(SuffixAutomaton &sam) {
    long long r = 0;
    for (int v = 1; v < (int)sam.st.size(); v++) r += sam.st[v].len - sam.st[sam.st[v].link].len;
    return r;
}

// 5) Longest common substring of s (in sam) and t: slide t through, fall back on links when stuck
int longestCommonSubstring(SuffixAutomaton &sam, const string &t) {
    int v = 0, l = 0, best = 0;
    for (char ch : t) {
        int c = SuffixAutomaton::idx(ch);
        while (v && sam.st[v].next[c] == -1) { v = sam.st[v].link; l = sam.st[v].len; }
        if (sam.st[v].next[c] != -1) { v = sam.st[v].next[c]; l++; }
        best = max(best, l);
    }
    return best;
}

// 6) Longest substring occurring at least k times (needs build_counts)
int longestRepeated(SuffixAutomaton &sam, long long k) {
    int best = 0;
    for (int v = 1; v < (int)sam.st.size(); v++) if (sam.st[v].cnt >= k) best = max(best, sam.st[v].len);
    return best;
}

// 7) k-th (1-based) lexicographically smallest DISTINCT substring. DP on the DAG:
//    paths[v] = number of non-empty strings readable starting at v
string kthDistinct(SuffixAutomaton &sam, long long k) {
    int n = sam.st.size();
    vector<long long> paths(n, 0);
    for (int v : sam.order_desc())
        for (int c = 0; c < SuffixAutomaton::K; c++) {
            int u = sam.st[v].next[c];
            if (u != -1) paths[v] += 1 + paths[u];
        }
    if (k > paths[0]) return "";            // not enough substrings
    string res; int v = 0;
    while (k > 0) {
        for (int c = 0; c < SuffixAutomaton::K; c++) {
            int u = sam.st[v].next[c];
            if (u == -1) continue;
            if (k <= 1 + paths[u]) {        // answer starts with this char
                // map index back to char (works for the a-z / A-Z / 0-9 schemes above)
                char ch = 0; for (int x = 0; x < 128; x++) if (SuffixAutomaton::idx((char)x) == c) { ch = x; break; }
                res += ch; k--; v = u; break;
            }
            k -= 1 + paths[u];
        }
    }
    return res;
}

int main() {
    string s = "abcbcab";
    SuffixAutomaton sam(s.size());
    sam.build(s);
    sam.build_counts();

    cout << isSub(sam, "bca") << "\n";              // 1
    cout << isSub(sam, "cc") << "\n";               // 0
    cout << occurrences(sam, "bc") << "\n";         // 2
    cout << occurrences(sam, "b") << "\n";          // 3
    cout << firstOccurrence(sam, "cab") << "\n";    // 4
    cout << distinctSubstrings(sam) << "\n";        // 22
    cout << longestCommonSubstring(sam, "xxbcbxx") << "\n"; // 3  ("bcb")
    cout << longestRepeated(sam, 2) << "\n";        // 2  ("ab" / "bc")
    cout << kthDistinct(sam, 1) << "\n";            // a
    cout << kthDistinct(sam, 3) << "\n";            // abc
    return 0;
}