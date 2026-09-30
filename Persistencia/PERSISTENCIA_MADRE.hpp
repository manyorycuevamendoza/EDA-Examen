#ifndef PERSISTENCIA_MADRE_HPP
#define PERSISTENCIA_MADRE_HPP

#include <iostream>
#include <vector>
#include "SEGMENT_TREE_PERSISTENTE_MADRE.hpp"
#include "ENTERO_BINARIO_PERSISTENTE_MADRE.hpp"
using namespace std;


template<typename data_type>
struct PersistentStack {
    struct StackNode {
        data_type data;
        StackNode* next;

        StackNode(data_type data, StackNode *next) : data(data), next(next) {}
    };

    vector<StackNode*> version_roots;

    PersistentStack(){
        version_roots.push_back(nullptr);
    }

    int update_push(int version, data_type data) {
        version_roots.emplace_back(new StackNode(data, version_roots[version]));
        return (int)version_roots.size() - 1;
    }

    int update_pop(int version) {
        StackNode *root = version_roots[version];
        version_roots.emplace_back(root == nullptr ? nullptr : root -> next);
        return (int)version_roots.size() - 1;
    }

    data_type top(int version) {
        return version_roots[version] == nullptr
            ? data_type(0) : version_roots[version] -> data;
    }

    bool empty(int version) {
        return version_roots[version] == nullptr;
    }

    void print(int version) {
        StackNode* top = version_roots[version];
        while(top != nullptr){
            cout << top -> data << '\n';
            top = top -> next;
        }
        cout << "END OF STACK\n";
    }
};

// persistent queue
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

    int push(int version, data_type value) {
        int root = data.update(roots[version], tails[version], value);
        roots.emplace_back(root);
        heads.emplace_back(heads[version]);
        tails.emplace_back(tails[version] + 1);
        return (int)roots.size() - 1;
    }

    data_type pop(int version) {
        data_type res = data.query(roots[version], heads[version], heads[version]);
        roots.emplace_back(roots[version]);
        heads.emplace_back(heads[version] + 1);
        tails.emplace_back(tails[version]);
        return res;
    }

    data_type front(int version) {
        return data.query(roots[version], heads[version], heads[version]);
    }

    bool empty(int version) {
        return heads[version] == tails[version];
    }

    int get_current_version() {
        return (int)roots.size() - 1;
    }

    void print_state(int version) {
        for(int i = heads[version]; i < tails[version]; ++i){
            cout << data.query(roots[version], i, i) << " ";
        }
        cout << '\n';
    }
};

//persistent array: en el repositorio se implementa con Segment Tree
template<typename data_type>
using PersistentArray = PersistentSegmentTree<data_type>;


//persistent trie
//Implementacion base: Trie_binario_persistente_xor.cpp
//Funciones reutilizables: add_node, insert y maximize.

//Segment Tree persistente de frecuencias: E_ELEMENT_SEGMENT y G_k_QUERY
template<typename data_type>
struct PersistentFrequencySegmentTree {
    struct SegmentTreeNode {
        data_type data;
        int l, r;
        SegmentTreeNode *left, *right;

        SegmentTreeNode(data_type data, int l, int r,
                        SegmentTreeNode *left, SegmentTreeNode *right)
            : data(data), l(l), r(r), left(left), right(right) {}
    };

    vector<SegmentTreeNode*> version_roots;

    PersistentFrequencySegmentTree(int n) {
        version_roots.emplace_back(
            new SegmentTreeNode(data_type(), 0, n - 1, nullptr, nullptr)
        );
        build(version_roots[0]);
    }

    PersistentFrequencySegmentTree(int l, int r, vector<data_type> &a) {
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
    }

    void build(SegmentTreeNode *root, vector<data_type> &a) {
        if(root -> l == root -> r){
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
            curr -> data += value;
            return;
        }
        int mi = (curr -> l + curr -> r) / 2;
        if(pos <= mi){
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
            version_roots[version] -> data,
            version_roots[0] -> l, version_roots[0] -> r,
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

    int kth(int k, SegmentTreeNode *last, SegmentTreeNode *cur) {
        if(cur -> l == cur -> r) return cur -> l;
        int left_size = cur -> left -> data - last -> left -> data;
        if(left_size >= k) return kth(k, last -> left, cur -> left);
        return kth(k - left_size, last -> right, cur -> right);
    }

    int kth(int last_version, int curr_version, int k) {
        return kth(k, version_roots[last_version], version_roots[curr_version]);
    }

    int get_current_version() {
        return (int)version_roots.size() - 1;
    }
};

#endif
