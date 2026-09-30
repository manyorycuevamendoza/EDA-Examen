#include <iostream>
#include <vector>
using namespace::std;

template<typename data_type>
struct PersistentStack {
    struct StackNode {
        data_type data;
        StackNode* next;
    };

    vector<StackNode*> version_roots;

    PersistentStack() {
        version_roots.push_back(nullptr);
    }

    void update_push(int version, data_type data) {
        version_roots.emplace_back(new StackNode(data, version_roots[version]));
    }

    void update_pop(int version) {
        version_roots.emplace_back(version_roots[version] -> next);
    }

    data_type top(int version) {
        return version_roots[version] == nullptr ? data_type(0) : version_roots[version] -> data;
    }

    void print(int version) {
        StackNode* top = version_roots[version];
        while (top != nullptr) {
            cout << top -> data << endl;
            top = top -> next;
        }
        cout << "END OF STACK" << endl;
    }
};

int main() {
    PersistentStack<int> S;
    int q;
    cin >> q;
    for (int i = 1; i <= q; ++i) {
        int t, m;
        cin >> t >> m;
        if (m == 0) {
            S.update_pop(t);
        }
        else {
            S.update_push(t, S.top(t) + m);
        }
    }
    long long res = 0;
    for (int i = 1; i <= q; ++i) res += S.top(i);
    cout << res << '\n';
    return 0;
}