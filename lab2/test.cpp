#include<bits/stdc++.h>
using namespace std;


/*
int main() {
    vector<Item> items;
    string left;
    cin >> left;
    string tmp;
    list<string> right;
    while (cin >> tmp) {
        right.push_back(tmp);
    }
    pair<string, list<string>> p;
    p.first = left;
    p.second = right;
    for (int i = 0; i <= right.size(); i++) {
        Item item = *(new Item(p, i));
        item.printItem();
    }

}*/ 
#define END "term"

class Item {
    public:
        pair<string,list<string>> production;
        int ind;
        set<string> Next;
        list<string>::iterator at; 
        Item (pair<string, list<string>> production,int ind) : 
            production(production), ind(ind) {
                at = production.second.begin();
                for (int i = 0; i < ind; i++)
                    at++;
            }
        string nextSym() {
            if (ind == production.second.size())
                return "";
            return *at;
        };
        void printItem() {
            cerr << production.first << " -> ";
            list<string>::iterator it = production.second.begin();
            for (int i = 0; i < production.second.size(); i++) {
                if (i == ind)
                    cerr << "*";
                cerr << *it;
                it++;
            }
            if (ind == production.second.size()) {
                cerr << "*";
            }
            cerr << "\n";
        }
};
struct pair_hash {
    template <class T1, class T2>
    size_t operator()(const pair<T1, T2>& p) const {
        auto h1 = hash<T1>{}(p.first);
        auto h2 = hash<T2>{}(p.second);
        return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
    }
};
template <typename T>
set<T> setUnion(const set<T>& set1, const set<T>& set2) {
    set<T> result;
    set_union(
        set1.begin(), set1.end(),
        set2.begin(), set2.end(),
        inserter(result, result.begin()));  
    return result;
}
map<pair<string,string>, bool> 
calcStartsDirectlyWithSymbol ( // testiraj ovo !!!
    unordered_map<string,set<list<string>>> &G,
    unordered_map<string, bool> &emptySymbols,
    set<string> &Symbols) {

    map<pair<string,string>, bool> ret;

    for (string s1 : Symbols) {
        for (string s2 : Symbols) {
            ret[pair(s1, s2)] = false;
        }
    } 
    for (auto prod : G) {
        for (list<string> ls : prod.second) {
            list<string>::iterator it = ls.begin();
            while(
                emptySymbols[*it] && 
                it != ls.end() && 
                *it != "$"){
                ret[pair<string, string> (prod.first, *it)] = true;
                it++;
            }
            if (it != ls.end() && *it != "$") {
                pair<string, string> p = pair<string, string> (prod.first, *it);
                ret[p] = true;
            }
        }
    }
    return ret;
}

void connectR (
    string sym1,
    string sym2,
    unordered_map<string, set<string>> &mp,
    map<pair<string, string>, bool> &StartsWithSymbol
) {
    pair<string, string> p = pair<string, string>(sym1, sym2);
    StartsWithSymbol[p] = true;
    for (string sym : mp[sym2]) {
        p = pair<string, string>(sym1, sym);
        if (!StartsWithSymbol[p])
            connectR(sym1, sym, mp, StartsWithSymbol);
    }
}
map<pair<string,string>, bool> calclStartsWithSymbol(
    map<pair<string,string>, bool> &startsDirectlyWithSymbol,
    set<string> &V,
    set<string> &T) {
    
    map<pair<string, string>, bool> ret;
    for (string t : T) {
        pair<string, string> p = pair(t, t);
        ret[p] = true;
    }
    unordered_map <string, set<string>> mp;
    for (auto KeyVal : startsDirectlyWithSymbol) {
        if (KeyVal.second) {
            pair<string, string> p = KeyVal.first;
            mp[p.first].insert(p.second);
        }
    }
    for (string v : V) {
        connectR(v, v, mp, ret);
    }
    return ret;
}

//nemam pojma zasto ovo treba izvest ovako
unordered_map<string, set<string>> calcStarts (
unordered_map<pair<string,string>, bool> &StartsWithSymbol) {
    unordered_map<string, set<string>> ret;
    for (auto kv : StartsWithSymbol) {
        if (kv.second) {
            ret[kv.first.first].insert(kv.first.second);
        }
    }
    return ret;
}
set<string> calcOneAfter(
    unordered_map<string, set<string>> &Starts,
    unordered_map<string, bool> &emptySym,
    Item &item
) {
    set<string> ret;
    list<string>::iterator it = item.at;
    it++;
    while (emptySym[*it] && it != item.production.second.end()) {
        ret = setUnion(ret, Starts[*it]);
        it++;
    }
    if (it == item.production.second.end()) {
        ret.insert(END);
    } else {
        ret = setUnion(ret, Starts[*it]);
    }
    return ret;
}
/*unordered_map<string, set<string>> calcOneAfter(
    unordered_map<string, set<string>> &Starts,
    unordered_map<string, bool> &emptySym,
    unordered_map<string, set<list<string>>> &G
) {
    unordered_map<string, set<string>> ret;
    for (auto prod : G) {
        for (list<string> ls : prod.second) {
            list<string>::iterator prev = ls.begin();
            list<string>::iterator it = ls.begin();
            it++;
            set<string> tmp;
            while (it != ls.end()) {
                tmp = Starts[*it];
                while (emptySym[*it] && it != ls.end()) {
                    tmp = setUnion(tmp, Starts[*it]);
                    it++;
                }
                if (it == ls.end())
                    tmp.insert(END);
                else
                    tmp = setUnion(tmp, Starts[*it]);
                for (string e : tmp)
                    ret[*prev].insert(e);
                it = prev;
                it++;
            
                it++;
                prev++;
            }
            ret[*prev].insert(END);
        }
    }
    return ret;
}
*/
unordered_map<string, bool> calcEmptySymbols (
unordered_map<string, set<list<string>>> &G,
set<string> &V,
set<string> &T) { //testiraj!!

    unordered_map<string, bool> ret;
    for (string v : V) {
        ret[v] = false;
    }
    for (string t : T) {
        ret[t] = false;
    }
    for (auto it : G) {
        for (list<string> ls : it.second) {
            if (*ls.begin() == "$") {
                //cerr << it.first << "\n";
                ret[it.first] = true;
                break;
            }
        }
    }
    bool change = false;
    do {
        change = false;
        for (auto prod : G) {
            for (list<string> ls : prod.second) {
                list<string>::iterator it = ls.begin();
                while (ret[*it] && it != ls.end()) {
                    it++;
                }
                if (it == ls.end()) {
                    ret[prod.first] = true;
                    change = true;
                }
            }
        }
    } while(change);
    return ret;
}

void printGrammar(
    unordered_map<string, bool> V,
    unordered_map<string, bool> T, 
    unordered_map<string,set<list<string>>> G) {
    cerr << "Terminal symbols: ";
    for (auto sym : V) {
        cerr << sym.first << " ";
    }
    cerr << "\n";
    cerr << "non-Terminal symbols: ";
    for (auto sym : T) {
        cerr << sym.first << " ";
    }
    cerr << "\n";
    cerr << "productions:\n";
    for (auto sym : V) {
        if (G[sym.first].empty()) {
            continue;
        }
        cerr << sym.first << " -> ";
        bool fst = true;
        for (list<string> right : G[sym.first]) {
            if (!fst) {
                cerr << "|";
            }
            for (string w : right) {
                cerr << w;
            }
            fst = false;
        }
        cerr << "\n";
    }
}

int main() {
    unordered_map<string,set<list<string>>> G;
    unordered_map<string, bool> V;
    unordered_map<string, bool> T;
    set<string> Vs;
    set<string> Ts;
    set<string> Symbols;
    int nv;
    cin >> nv;
    while (nv--) {
        string v;
        cin >> v;
        V[v] = true;
        Vs.insert(v);
        Symbols.insert(v);
    }
    int nt;
    cin >> nt;
    while (nt--) {
        string t;
        cin >> t;
        T[t] = true;
        Ts.insert(t);
        Symbols.insert(t);
    }
    int n;
    cin >> n;
    while (n--) {
        string left;
        cin >> left;
        string tmp;
        list<string> right;
        int r;
        cin >> r;
        while (r--) {
            cin >> tmp;
            right.push_back(tmp);
        }
        G[left].insert(right);
    }
    printGrammar(V, T, G);
    unordered_map<string, bool> emp = calcEmptySymbols(G, Vs, Ts);
    cout << "Prazni simboli:\n";
    for (auto kv : emp) 
        if (kv.second)
            cout << kv.first << "\n";
    cout << "Počinje izravno s parovi:\n";
    map<pair<string, string>, bool> StartsDirectlyWithSymbol =
        calcStartsDirectlyWithSymbol(G, emp, Symbols);
    
    for (auto kv : StartsDirectlyWithSymbol) {
        if (kv.second) {
            cout << kv.first.first << " " << kv.first.second << "\n";
        }
    }
    cout << "Počine s parovi:\n";
    map<pair<string,string>, bool> StartsWithSymbol =
    calclStartsWithSymbol(StartsDirectlyWithSymbol, Vs, Ts);
    for (auto kv : StartsWithSymbol) {
        if (kv.second) {
            cout << kv.first.first << " " << kv.first.second << "\n";
        }
    }
}