#include <bits/stdc++.h>
using namespace std;
//cpodigo del profe
const int L =16;
const int N=100000;
const int NODES =N * (L + 1) + 5;
const int E=2;

int n, q;
int nodes;
int root[N + 1];
int frec[NODES];
int trie[E][NODES];

int add_node(int basis =-1) {
    for(int i=0;i<E;i++){
        if(basis != -1){
            trie[i][nodes]= trie[i][basis];
        }
        else{
            trie[i][nodes] =0;
        }
    }

    if(basis != -1){
        frec[nodes] =frec[basis];
    }
    else{
        frec[nodes]=0;
    }
    return nodes++;
}

void insert(int x, int last, int pos) {
    for(int i =L - 1;i>=0;i--){
        int bit= (x >> i) & 1;

        if(last == -1 or trie[bit][last]==0){
            trie[bit][pos] =add_node();
        }
        else{
            trie[bit][pos]= add_node(trie[bit][last]);
        }

        if(last != -1 and trie[bit][last] != 0){
            last= trie[bit][last];
        }
        else{
            last =-1;
        }

        pos=trie[bit][pos];
        frec[pos]++;
    }
}

int maximize(int x, int last, int pos) {
    int respuesta=0;

    for(int i=L - 1;i>=0;i--){
        int bit = (x >> i) & 1;
        int buscado=bit ^ 1;

        int cantidadActual =frec[trie[buscado][pos]];
        int cantidadAnterior=0;
        if(last != -1){
            cantidadAnterior = frec[trie[buscado][last]];
        }

        if(cantidadActual == cantidadAnterior){
            buscado^=1;
        }
        if((bit ^ buscado) != 0){
            respuesta|= (1 << i);
        }

        pos =trie[buscado][pos];
        if(last != -1 and trie[buscado][last] != 0){
            last=trie[buscado][last];
        }
        else{
            last = -1;
        }
    }
    return respuesta;
}

int main(){
    cin.tie(0) -> sync_with_stdio(false);

    int casos;
    cin >> casos;

    while(casos--){
        cin >> n >> q;
        nodes=0;
        root[0] =add_node();

        for(int i=1;i<=n;i++){
            int numero;
            cin >> numero;
            root[i]= add_node(root[i - 1]);
            insert(numero, root[i - 1], root[i]);
        }

        while(q--){
            int clave, l,r;
            cin >> clave >> l >> r;
            cout << maximize(clave, root[l - 1], root[r]) << '\n';
        }
    }
    return 0;
}
