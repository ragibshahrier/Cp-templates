// C++ Program to Implement Lazy Propagation Segment Tree
#include <climits>
#include <iostream>
#include <vector>
using namespace std;

// Template class for Lazy Segment Tree
template <typename T> class LazySegmentTree {
private:
    // Segment tree and lazy array to store values and lazy updates
    vector<T> tree, lazy;
    // Input array
    vector<T> arr;
    // Size of the input array
    int n;

    // Helper function to get the left child of a node
    int left(int node) { return 2 * node + 1; }

    // Helper function to get the right child of a node
    int right(int node) { return 2 * node + 2; }

    // Helper function to calculate the middle index
    int mid(int l, int r) { return l + (r - l) / 2; }

    // Function to build the segment tree
    void build(int node, int start, int end) {
        if (start == end) {
            tree[node] = arr[start];
            return;
        }
        int m = mid(start, end);
        build(left(node), start, m);
        build(right(node), m + 1, end);
        tree[node] = min(tree[left(node)], tree[right(node)]);
    }

    // Function to propagate the lazy values
    void propagate(int node, int start, int end) {
        if (lazy[node] != 0) {
            tree[node] += lazy[node];
            if (start != end) { // Not a leaf node
                lazy[left(node)] += lazy[node];
                lazy[right(node)] += lazy[node];
            }
            lazy[node] = 0;
        }
    }

    // Function to update a range of values
    void updateRange(int node, int start, int end, int l, int r, T val) {
        propagate(node, start, end);
        if (r < start || end < l)
            return; // Out of range
        if (l <= start && end <= r) {
            lazy[node] += val;
            propagate(node, start, end);
            return;
        }
        int m = mid(start, end);
        updateRange(left(node), start, m, l, r, val);
        updateRange(right(node), m + 1, end, l, r, val);
        tree[node] = min(tree[left(node)], tree[right(node)]);
    }

    // Function to query the minimum value in a given range
    T queryRange(int node, int start, int end, int l, int r) {
        propagate(node, start, end);
        if (r < start || end < l)
            return INT_MAX;
        if (l <= start && end <= r)
            return tree[node];
        int m = mid(start, end);
        T left_min = queryRange(left(node), start, m, l, r);
        T right_min = queryRange(right(node), m + 1, end, l, r);
        return min(left_min, right_min);
    }

public:
    // Constructor to initialize the lazy segment tree with the input array
    LazySegmentTree(const vector<T>& a)
        : arr(a), n(a.size()) {
        tree.resize(4 * n);
        lazy.resize(4 * n, 0);
        build(0, 0, n - 1);
    }

    // Public function to update the range of values
    void updateRange(int l, int r, T val) {
        updateRange(0, 0, n - 1, l, r, val);
    }

    // Public function to query the minimum value in a given range
    T queryRange(int l, int r) {
        return queryRange(0, 0, n - 1, l, r);
    }
};

int main() {
    // Input array
    vector<int> arr = {1, 3, 2, 7, 9, 11};

    // Create the Lazy Segment Tree object
    LazySegmentTree<int> lst(arr);

    // Display initial information
    cout << "Lazy Segment Tree for Range Updates:" << endl;
    cout << "Original Array: ";
    for (int num : arr)
        cout << num << " ";
    cout << endl;

    // Perform some range queries and updates
    cout << "Minimum in range [1, 4]: " << lst.queryRange(1, 4) << endl;
    lst.updateRange(1, 3, 3);
    cout << "After adding 3 to range [1, 3]:" << endl;
    cout << "Minimum in range [1, 4]: " << lst.queryRange(1, 4) << endl;
    cout << "Minimum in range [0, 5]: " << lst.queryRange(0, 5) << endl;

    return 0;
}
