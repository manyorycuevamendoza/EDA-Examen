#include <iostream>
#include <vector>
#include <algorithm>
using namespace::std;

const int MOD = 1e9;

template<typename data_type>
struct PersistentSegmentTree {

    struct SegmentTreeNode {
        data_type data;
        int l, r;
        SegmentTreeNode *left, *right;

        SegmentTreeNode(data_type data, int l, int r, SegmentTreeNode* left, SegmentTreeNode* right) : data(data), l(l), r(r), left(left), right(right) {}
    };

    vector<SegmentTreeNode*> version_roots;

    PersistentSegmentTree(int n) {
        version_roots.emplace_back(new SegmentTreeNode(data_type(), 0, n - 1, nullptr, nullptr));
        build(version_roots[0]);
    }

    PersistentSegmentTree(int l, int r, vector<data_type> &a) {
        version_roots.emplace_back(new SegmentTreeNode(data_type(), l, r, nullptr, nullptr));
        build(version_roots[0], a);
    }

    void build(SegmentTreeNode *root) {
        if (root -> l == root -> r) {
            root -> data = data_type();
            return;
        }
        int mi = (root -> l + root -> r) / 2;
        root -> left = new SegmentTreeNode(data_type(), root -> l, mi, nullptr, nullptr);
        root -> right = new SegmentTreeNode(data_type(), mi + 1, root -> r, nullptr, nullptr);
        build(root -> left);
        build(root -> right);
    }

    void update(int pos, data_type value, SegmentTreeNode *last, SegmentTreeNode *curr) {
        if (curr -> l == curr -> r) {
            curr -> data += value;
            return;
        }
        int mi = (curr -> l + curr -> r) / 2;
        if (pos <= mi) {
            curr -> right = last -> right;
            curr -> left = new SegmentTreeNode(last -> left -> data, curr -> l, mi, nullptr, nullptr);
            update(pos, value, last -> left, curr -> left);
        }
        else {
            curr -> left = last -> left;
            curr -> right = new SegmentTreeNode(last -> right -> data, mi + 1, curr -> r, nullptr, nullptr);
            update(pos, value, last -> right, curr -> right);
        }
        curr -> data = curr -> left -> data + curr -> right -> data;
    }

    int update(int version, int pos, data_type value) {
        SegmentTreeNode *root = new SegmentTreeNode(data_type(), version_roots[0] -> l, version_roots[0] -> r, nullptr, nullptr);
        version_roots.emplace_back(root);
        update(pos, value, version_roots[version], root);
        return (int)version_roots.size() - 1;
    }

    data_type query(int x, int y, SegmentTreeNode *root) {
        if (y < root -> l or root -> r < x or x > y) return data_type(0);
        if (x <= root -> l and root -> r <= y) return root -> data;
        return query(x, y, root -> left) + query(x, y, root -> right);
    }

    data_type query(int version, int x, int y) {
        return query(x, y, version_roots[version]);
    }

    int kth(int k, SegmentTreeNode* last, SegmentTreeNode* cur) {
        if (cur -> l == cur -> r) {
            return cur -> l;
        }
        int left_size = cur -> left -> data - last -> left -> data;
        if (left_size >= k) return kth(k, last -> left, cur -> left);
        return kth(k - left_size, last -> right, cur -> right);
    }

    int kth(int last_version, int curr_version, int k) {
        return kth(k , version_roots[last_version], version_roots[curr_version]);
    }

    int get_current_version() {
        return (int)version_roots.size() - 1;
    }
};

int main() {
    cin.tie(0) -> sync_with_stdio(false);
    int n;
    cin >> n;
    vector<int> a(n);
    cin >> a[0];
    int l, m;
    cin >> l >> m;
    for (int i = 1; i < n; ++i) {
        a[i] = (1ll * a[i - 1] * l + m) % MOD;
    }
    vector<int> values(a.begin(), a.end());
    sort(values.begin(), values.end());
    values.erase(unique(values.begin(), values.end()), values.end());
    for (int i = 0; i < n; ++i) a[i] = lower_bound(values.begin(), values.end(), a[i]) - values.begin();
    l = values.size();
    PersistentSegmentTree<int> data(l);
    for (int i = 0; i < n; ++i) {
        data.update(i, a[i], 1);
    }
    int b;
    cin >> b;
    long long res = 0;
    while (b--) {
        int q;
        cin >> q;
        int x1, lx, mx;
        cin >> x1 >> lx >> mx;
        int y1, ly, my;
        cin >> y1 >> ly >> my;
        int k1, lk, mk;
        cin >> k1 >> lk >> mk;
        int ig = min(x1, y1), jg = max(x1, y1);
        int cur = data.kth(ig - 1, jg, k1);
        res += values[cur];
        for (int i = 1; i < q; ++i) {
            x1 = (1ll * (x1 - 1) * lx + mx) % n + 1;
            y1 = (1ll * (y1 - 1) * ly + my) % n + 1;
            ig = min(x1, y1);
            jg = max(x1, y1);
            k1 = (1ll * (k1 - 1) * lk + mk) % (jg - ig + 1) + 1;
            int cur = data.kth(ig - 1, jg, k1);
            res += values[cur];
        }
    }
    cout << res << '\n';
    return 0;
}