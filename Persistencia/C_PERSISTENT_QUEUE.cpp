#include <iostream>
#include <vector>
using namespace::std;

template<typename data_type>
struct PersistentSegmentTree {

    struct SegmentTreeNode {
        data_type data;
        int l, r;
        SegmentTreeNode *left, *right;
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

    void build(SegmentTreeNode *root, vector<data_type> &a) {
        if (root -> l == root -> r) {
            root -> data = a[root -> l - 1];
            return;
        }
        int mi = (root -> l + root -> r) / 2;
        root -> left = new SegmentTreeNode(data_type(), root -> l, mi, nullptr, nullptr);
        root -> right = new SegmentTreeNode(data_type(), mi + 1, root -> r, nullptr, nullptr);
        build(root -> left, a);
        build(root -> right, a);
    }

    void update(int pos, data_type value, SegmentTreeNode *last, SegmentTreeNode *curr) {
        if (curr -> l == curr -> r) {
            curr -> data = value;
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

    int get_current_version() {
        return (int)version_roots.size() - 1;
    }
};

template<typename data_type>
struct PersistentQueue {
    vector<int> roots;
    vector<int> heads;
    vector<int> tails;
    PersistentSegmentTree<data_type> data;

    PersistentQueue(int max_cap) : data(max_cap) {
        roots.emplace_back(data.get_current_version());
        heads.emplace_back(0);
        tails.emplace_back(0);
    }

    void push(int version, data_type value) {
        data.update(roots[version], tails[version], value);
        roots.emplace_back(data.get_current_version());
        heads.emplace_back(heads[version]);
        tails.emplace_back(tails[version] + 1);
    }

    data_type pop(int version) {
        data_type res = data.query(roots[version], heads[version], heads[version]);
        roots.emplace_back(roots[version]);
        heads.emplace_back(heads[version] + 1);
        tails.emplace_back(tails[version]);
        return res;
    }

    void print_state(int version) {
        for (int i = heads[version]; i < tails[version]; ++i) {
            cout << data.query(version, i, i) << " ";
        }
        cout << '\n';
    }
};

int main() {
    cin.tie(0) -> sync_with_stdio(false);
    int q;
    cin >> q;
    PersistentQueue<int> Q(q + 1);
    for (int s = 1; s <= q; ++s) {
        int op;
        cin >> op;
        if (op == 1) {
            int v, x;
            cin >> v >> x;
            Q.push(v, x);
        }
        else {
            int v;
            cin >> v;
            cout << Q.pop(v) << '\n';
        }
    }
    return 0;
}