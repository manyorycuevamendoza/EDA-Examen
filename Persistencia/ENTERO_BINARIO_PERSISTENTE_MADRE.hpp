#ifndef ENTERO_BINARIO_PERSISTENTE_MADRE_HPP
#define ENTERO_BINARIO_PERSISTENTE_MADRE_HPP

#include <cassert>
#include <vector>
using namespace std;

const int BIG_INTEGER_BITS=100000 + 20;

template<int B =311, int MOD =1000000007>
struct SubstringHash {
    int hash_1;
    int power_1;

    SubstringHash(int hash_1 =0, int power_1 =B)
        : hash_1(hash_1), power_1(power_1) {}

    SubstringHash operator + (const SubstringHash &rhs) const {
        SubstringHash resultado;
        resultado.hash_1=(1ll * hash_1 * rhs.power_1 + rhs.hash_1) % MOD;
        resultado.power_1 = (1ll * power_1 * rhs.power_1) % MOD;
        return resultado;
    }

    bool operator == (const SubstringHash &rhs) const {
        return hash_1 == rhs.hash_1;
    }

    bool operator != (const SubstringHash &rhs) const {
        return not (*this == rhs);
    }
};

struct PersistentBigIntegerNode {
    int value;
    int l, r;
    SubstringHash<> segment_hash;
    PersistentBigIntegerNode *left, *right;

    PersistentBigIntegerNode(
        int l =0, int r =BIG_INTEGER_BITS - 1, int value =0,
        SubstringHash<> segment_hash =SubstringHash(),
        PersistentBigIntegerNode *left =nullptr,
        PersistentBigIntegerNode *right =nullptr
    ) : value(value), l(l), r(r), segment_hash(segment_hash),
        left(left), right(right) {}

    void build(int valor) {
        if(l==r){
            value=valor;
            segment_hash =SubstringHash(valor + 1);
            return;
        }

        int mi=(l + r) / 2;
        left =new PersistentBigIntegerNode(l, mi);
        right=new PersistentBigIntegerNode(mi + 1, r);
        left -> build(valor);
        right -> build(valor);
        value=right -> value + left -> value;
        segment_hash = right -> segment_hash + left -> segment_hash;
    }

    bool operator < (const PersistentBigIntegerNode &rhs) const {
        const PersistentBigIntegerNode *izquierda=this;
        const PersistentBigIntegerNode *derecha = &rhs;

        while(izquierda -> l != izquierda -> r){
            if(izquierda -> right -> segment_hash
               != derecha -> right -> segment_hash){
                izquierda=izquierda -> right;
                derecha =derecha -> right;
            }
            else{
                izquierda =izquierda -> left;
                derecha=derecha -> left;
            }
        }
        return izquierda -> value < derecha -> value;
    }
};

struct PersistentBigInteger {
    vector<PersistentBigIntegerNode*> roots;

    PersistentBigInteger(){
        roots.emplace_back(new PersistentBigIntegerNode());
        roots.back() -> build(0);

        roots.emplace_back(new PersistentBigIntegerNode());
        roots.back() -> build(1);
    }

    void set_to_value(PersistentBigIntegerNode *prev,
                      PersistentBigIntegerNode *cur,
                      PersistentBigIntegerNode *val, int x, int y) {
        int l=cur -> l;
        int r =cur -> r;
        int mi=(l + r) / 2;

        if(x<=l and mi<=y){
            cur -> left=val -> left;
        }
        else if(y<l or mi<x){
            cur -> left =prev -> left;
        }
        else{
            cur -> left=new PersistentBigIntegerNode(l, mi);
            set_to_value(prev -> left, cur -> left, val -> left, x, y);
        }

        if(x <= mi + 1 and r<=y){
            cur -> right =val -> right;
        }
        else if(y<mi + 1 or r<x){
            cur -> right=prev -> right;
        }
        else{
            cur -> right =new PersistentBigIntegerNode(mi + 1, r);
            set_to_value(prev -> right, cur -> right, val -> right, x, y);
        }

        cur -> value=cur -> right -> value + cur -> left -> value;
        cur -> segment_hash =cur -> right -> segment_hash
                            + cur -> left -> segment_hash;
    }

    int query_position(int version, int x) {
        PersistentBigIntegerNode *root=roots[version];

        while(root -> l != root -> r){
            int mi = (root -> l + root -> r) / 2;
            if(x<=mi){
                root=root -> left;
            }
            else{
                root =root -> right;
            }
        }
        return root -> value;
    }

    int get_nxt_zero(PersistentBigIntegerNode *root, int x) {
        if(root -> r<x){
            return -1;
        }
        if(root -> r - root -> l + 1 == root -> value){
            return -1;
        }
        if(root -> l==root -> r){
            return root -> l;
        }

        int posicionCero=get_nxt_zero(root -> left, x);
        if(posicionCero == -1){
            return get_nxt_zero(root -> right, x);
        }
        return posicionCero;
    }

    int add(int version, int x) {
        if(query_position(version, x) != 0){
            int siguienteCero=get_nxt_zero(roots[version], x);
            assert(siguienteCero != -1);

            roots.emplace_back(new PersistentBigIntegerNode(
                0, BIG_INTEGER_BITS - 1, roots[version] -> value,
                roots[version] -> segment_hash,
                roots[version] -> left, roots[version] -> right
            ));
            set_to_value(
                roots[version], roots.back(), roots[0],
                x, siguienteCero - 1
            );

            int versionAnterior=(int)roots.size() - 1;
            roots.emplace_back(new PersistentBigIntegerNode(
                0, BIG_INTEGER_BITS - 1, roots.back() -> value,
                roots.back() -> segment_hash,
                roots.back() -> left, roots.back() -> right
            ));
            set_to_value(
                roots[versionAnterior], roots.back(), roots[1],
                siguienteCero, siguienteCero
            );
        }
        else{
            roots.emplace_back(new PersistentBigIntegerNode(
                0, BIG_INTEGER_BITS - 1, roots[version] -> value,
                roots[version] -> segment_hash,
                roots[version] -> left, roots[version] -> right
            ));
            set_to_value(roots[version], roots.back(), roots[1], x, x);
        }
        return (int)roots.size() - 1;
    }

    bool is_smaller(int v1, int v2) {
        return *roots[v1] < *roots[v2];
    }
};

#endif
