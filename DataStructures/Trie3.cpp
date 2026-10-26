#include<bits/stdc++.h>
using namespace std;

class Trie {
public:
    // ---- pick ONE alphabet ----
    const static int N = 26;  static int idx(char c) { return c - 'a'; }                 // a-z
    // const static int N = 52;  static int idx(char c) { return islower(c) ? c - 'a' : c - 'A' + 26; }  // a-z A-Z
    // const static int N = 62;  static int idx(char c) { return islower(c) ? c - 'a' : isupper(c) ? c - 'A' + 26 : c - '0' + 52; } // a-z A-Z 0-9
    // const static int N = 10;  static int idx(char c) { return c - '0'; }                 // digits only
    // const static int N = 128; static int idx(char c) { return (unsigned char)c; }        // any ASCII

    struct TrieNode {
        int next[N];
        int endCnt = 0;   // how many copies of the string end here (supports duplicates)
        int freq = 0;     // how many strings pass through here (0 => node is "dead" after erase)
        TrieNode() { fill(next, next + N, -1); }
    };

    vector<TrieNode> tree;
    Trie() { tree.emplace_back(); }

    // Normal Insert (Always modifies root 0 in-place)
    void insert(const string &s) {
        int p = 0;
        tree[p].freq++;
        for (char ch : s) {
            int c = idx(ch);
            if (tree[p].next[c] == -1) {
                tree[p].next[c] = tree.size();
                tree.emplace_back();
            }
            p = tree[p].next[c];
            tree[p].freq++;
        }
        tree[p].endCnt++;
    }

    // Normal Erase: removes ONE copy of s, returns false if s isn't present (lazy: nodes kept, freq hits 0)
    bool erase(const string &s) {
        if (!checkString(s)) return false;
        int p = 0;
        tree[p].freq--;
        for (char ch : s) {
            p = tree[p].next[idx(ch)];
            tree[p].freq--;
        }
        tree[p].endCnt--;
        return true;
    }

    // ---------- Persistent part: comment out if you don't need persistence ----------
    // NOTE: never mix normal insert/erase and persistent ops on the SAME Trie object.

    // Persistent Insert: clones path, returns new root
    int persistentInsert(int head, const string &s) {
        int old = head;
        int newHead = tree.size();
        tree.emplace_back();
        int now = newHead;

        for (char ch : s) {
            int c = idx(ch);
            if (old != -1) tree[now] = tree[old]; // Copy all children, freq, endCnt
            tree[now].freq++;

            int nextNode = tree.size();
            tree.emplace_back();
            tree[now].next[c] = nextNode;

            if (old != -1) old = tree[old].next[c];
            now = nextNode;
        }
        if (old != -1) tree[now] = tree[old];
        tree[now].freq++;
        tree[now].endCnt++;

        return newHead;
    }

    // Persistent Erase: removes ONE copy of s, returns new root (same head if s absent)
    int persistentErase(int head, const string &s) {
        if (!checkString(s, head)) return head;
        int old = head, newHead = tree.size();
        TrieNode tmp = tree[old]; tree.push_back(tmp);   // tmp: avoid reference invalidation on realloc
        int now = newHead;
        for (char ch : s) {
            int c = idx(ch);
            tree[now].freq--;
            old = tree[old].next[c];
            tmp = tree[old];
            int nx = tree.size();
            tree.push_back(tmp);
            tree[now].next[c] = nx;
            now = nx;
        }
        tree[now].freq--;
        tree[now].endCnt--;
        return newHead;
    }
    // ---------- end persistent part ----------

    // Works for BOTH! Pass 'head' for persistent, or omit it for normal Trie
    bool checkPrefix(const string &s, int head = 0) {
        int p = head;
        for (char ch : s) {
            int c = idx(ch);
            if (tree[p].next[c] == -1 || tree[tree[p].next[c]].freq == 0) return false;
            p = tree[p].next[c];
        }
        return true;
    }

    // Works for BOTH! Pass 'head' for persistent, or omit it for normal Trie
    bool checkString(const string &s, int head = 0) {
        int p = head;
        for (char ch : s) {
            int c = idx(ch);
            if (tree[p].next[c] == -1 || tree[tree[p].next[c]].freq == 0) return false;
            p = tree[p].next[c];
        }
        return tree[p].endCnt > 0;
    }

    // Number of stored strings having s as prefix (with duplicates). Works for BOTH.
    int countPrefix(const string &s, int head = 0) {
        int p = head;
        for (char ch : s) {
            p = tree[p].next[idx(ch)];
            if (p == -1) return 0;
        }
        return tree[p].freq;
    }

    // Number of copies of exactly s. Works for BOTH.
    int countString(const string &s, int head = 0) {
        int p = head;
        for (char ch : s) {
            p = tree[p].next[idx(ch)];
            if (p == -1) return 0;
        }
        return tree[p].endCnt;
    }
};

int main()
{
    // ===== Normal Trie =====
    Trie trie;

    cout << trie.checkString("hello") << endl;   //output : 0
    trie.insert("hello");
    trie.insert("help");
    trie.insert("help");                         // duplicate

    cout << trie.checkPrefix("hel") << endl;     //output : 1
    cout << trie.checkString("hell") << endl;    //output : 0
    cout << trie.checkString("hello") << endl;   //output : 1
    cout << trie.countPrefix("hel") << endl;     //output : 3
    cout << trie.countString("help") << endl;    //output : 2

    cout << trie.erase("help") << endl;          //output : 1
    cout << trie.countString("help") << endl;    //output : 1  (one copy left)
    cout << trie.erase("help") << endl;          //output : 1
    cout << trie.checkString("help") << endl;    //output : 0
    cout << trie.checkPrefix("help") << endl;    //output : 0
    cout << trie.erase("help") << endl;          //output : 0  (not present)
    cout << trie.countPrefix("hel") << endl;     //output : 1  (only "hello")

    // ===== Persistent Trie =====
    Trie pt;
    vector<int> heads;
    heads.push_back(0);                                          // v0: {}
    heads.push_back(pt.persistentInsert(heads.back(), "hello")); // v1: {hello}
    heads.push_back(pt.persistentInsert(heads.back(), "world")); // v2: {hello, world}
    heads.push_back(pt.persistentErase(heads.back(), "hello"));  // v3: {world}
    heads.push_back(pt.persistentInsert(heads[1], "trie"));      // v4: branch from v1 -> {hello, trie}

    cout << pt.checkString("hello", heads[0]) << endl;  //output : 0
    cout << pt.checkString("hello", heads[1]) << endl;  //output : 1
    cout << pt.checkString("world", heads[2]) << endl;  //output : 1
    cout << pt.checkString("hello", heads[2]) << endl;  //output : 1
    cout << pt.checkString("hello", heads[3]) << endl;  //output : 0  (erased in v3)
    cout << pt.checkPrefix("hel", heads[3]) << endl;    //output : 0
    cout << pt.checkString("world", heads[3]) << endl;  //output : 1
    cout << pt.checkString("world", heads[4]) << endl;  //output : 0  (v4 branched from v1)
    cout << pt.checkString("trie", heads[4]) << endl;   //output : 1
    cout << pt.countPrefix("", heads[4]) << endl;       //output : 2

    return 0;
}