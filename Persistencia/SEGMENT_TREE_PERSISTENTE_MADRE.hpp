#ifndef SEGMENT_TREE_PERSISTENTE_MADRE_HPP
#define SEGMENT_TREE_PERSISTENTE_MADRE_HPP

#include <vector>
using namespace std;

template<typename data_type>
struct PersistentSegmentTree {
    struct SegmentTreeNode {
        data_type data;
        int l, r;
        SegmentTreeNode *left, *right;

        SegmentTreeNode(data_type data, int l, int r,
                        SegmentTreeNode *left, SegmentTreeNode *right)
            : data(data), l(l), r(r), left(left), right(right) {}
    };

    
    vector<SegmentTreeNode*> version_roots;

    PersistentSegmentTree(int n) {
        version_roots.emplace_back(
            new SegmentTreeNode(data_type(), 0, n - 1, nullptr, nullptr)
        );
        build(version_roots[0]);
    }

    PersistentSegmentTree(int l, int r, vector<data_type> &a) {
        version_roots.emplace_back(
            new SegmentTreeNode(data_type(), l, r, nullptr, nullptr)
        );
        build(version_roots[0], a);
    }

    void build(SegmentTreeNode *root) {
        if(root -> l == root -> r){
            root -> data = data_type();
            return;
        }
        int mi = (root -> l + root -> r) / 2;
        root -> left = new SegmentTreeNode(data_type(), root -> l, mi, nullptr, nullptr);
        root -> right = new SegmentTreeNode(data_type(), mi + 1, root -> r, nullptr, nullptr);
        build(root -> left);
        build(root -> right);
        root -> data = root -> left -> data + root -> right -> data;
    }

    void build(SegmentTreeNode *root, vector<data_type> &a) {
        if(root -> l == root -> r){
            //el arreglo original esta indexado desde 0
            root -> data = a[root -> l - version_roots[0] -> l];
            return;
        }
        int mi = (root -> l + root -> r) / 2;
        root -> left = new SegmentTreeNode(data_type(), root -> l, mi, nullptr, nullptr);
        root -> right = new SegmentTreeNode(data_type(), mi + 1, root -> r, nullptr, nullptr);
        build(root -> left, a);
        build(root -> right, a);
        root -> data = root -> left -> data + root -> right -> data;
    }

    void update(int pos, data_type value, SegmentTreeNode *last,
                SegmentTreeNode *curr) {
        if(curr -> l == curr -> r){
            curr -> data = value;
            return;
        }
        int mi = (curr -> l + curr -> r) / 2;
        if(pos <= mi){
            //reutilizamos el hijo que no cambia
            curr -> right = last -> right;
            curr -> left = new SegmentTreeNode(last -> left -> data,
                                                curr -> l, mi, nullptr, nullptr);
            update(pos, value, last -> left, curr -> left);
        }
        else{
            curr -> left = last -> left;
            curr -> right = new SegmentTreeNode(last -> right -> data,
                                                 mi + 1, curr -> r, nullptr, nullptr);
            update(pos, value, last -> right, curr -> right);
        }
        curr -> data = curr -> left -> data + curr -> right -> data;
    }

    int update(int version, int pos, data_type value) {
        SegmentTreeNode *root = new SegmentTreeNode(
            data_type(), version_roots[0] -> l, version_roots[0] -> r,
            nullptr, nullptr
        );
        version_roots.emplace_back(root);
        update(pos, value, version_roots[version], root);
        return (int)version_roots.size() - 1;
    }

    data_type query(int x, int y, SegmentTreeNode *root) {
        if(y < root -> l or root -> r < x or x > y) return data_type(0);
        if(x <= root -> l and root -> r <= y) return root -> data;
        return query(x, y, root -> left) + query(x, y, root -> right);
    }

    data_type query(int version, int x, int y) {
        return query(x, y, version_roots[version]);
    }

    int get_current_version() {
        return (int)version_roots.size() - 1;
    }
};

#endif
