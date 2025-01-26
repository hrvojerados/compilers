#include<bits/stdc++.h>
#include<iterator>
using namespace std;
using ll = long long;

#define END "term"
template <typename T>
inline set<T> setUnion(const set<T>& set1, const set<T>& set2) {
    set<T> result;
    set_union(
        set1.begin(), set1.end(),
        set2.begin(), set2.end(),
        inserter(result, result.begin()));  
    return result;
}



class Item {
    public:
        pair<string,list<string>> production;
        int ind;
        set<string> lookahead;
        Item(){};
        Item(pair<string, list<string>> production) : 
            production(production) {
                if (*production.second.begin() == "$")
                    this->ind = 1;
                else
                    this->ind = 0;
            } 
        Item (pair<string, list<string>> production, int ind) {
                this->production = production;
                if (*production.second.begin() == "$")
                    this->ind = 1;
                else
                    this->ind = ind;
            }
        bool operator==(const Item &other) const {
            return production == other.production && ind == other.ind && lookahead == other.lookahead;
        }
        bool operator!=(const Item &other) const {
            return !(*this == other);
        }
        bool operator<(const Item &other) const {
            if (production.first != other.production.first)
                return production.first < other.production.first;
            if (production.second != other.production.second)
                return production.second < other.production.second;
            if (ind != other.ind)
                return ind < other.ind;
            return this->lookahead < other.lookahead;
        }
        string nextSym() {
            if (ind == this->production.second.size() || this->production.second.size() == 0)
                return "";
            list<string>::iterator it = production.second.begin();
            for ( int i = 0; i != ind; i++)
                it++;
            return *it;
        }

        void printItem() {
            cerr << production.first << " -> ";
            int i = 0;
            for (
                list<string>::iterator it = production.second.begin();
                it != production.second.end();
                it++
            ) {
                //cerr << *at;
                if (i == ind)
                    cerr << "*";
                i++;
                cerr << *it;
            }
            if (ind == production.second.size()) {
                cerr << "*";
            }
            cerr << " {";
            for (string sym : lookahead) {
                cerr << sym << ",";
            }
            cerr << "}";
        }
};

map<string, bool> calcEmptySymbols (
map<string, set<list<string>>> &G,
set<string> &V,
set<string> &T) { 
    map<string, bool> ret;
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
                while (it != ls.end() && ret[*it]) {
                    it++;
                }
                if (it == ls.end() && !ret[prod.first]) {
                    ret[prod.first] = true;
                    change = true;
                }
            }
        }
        //cerr << "\n";
    } while(change);
    return ret;
}
map<pair<string,string>, bool> 
calcStartsDirectlyWithSymbol ( // testiraj ovo !!!
    map<string,set<list<string>>> &G,
    map<string, bool> &emptySymbols,
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
                it != ls.end() && 
                emptySymbols[*it] && 
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
    map<string, set<string>> &mp,
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
    map <string, set<string>> mp;
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

map<string, set<string>> calcStarts (
    map<pair<string,string>, bool> &StartsWithSymbol,
    set<string> &V,
    set<string> &T
    ) {
    map<string, set<string>> ret;
    for (auto kv : StartsWithSymbol) {
        if (
            kv.second && 
            (V.find(kv.first.first) != V.end()) && 
            (T.find(kv.first.second) != T.end())
            ) {
            ret[kv.first.first].insert(kv.first.second);
        }
    }
    return ret;
}
set<string> calcOneAfter(
    map<string, set<string>> &Starts,
    map<string, bool> &emptySym,
    Item &item
) {
    set<string> ret;
    list<string>::iterator it = item.production.second.begin();
    for (int i = 0; i <= item.ind; i++)
        it++;
    while (it != item.production.second.end() && emptySym[*it]) {
        ret = setUnion(ret, Starts[*it]);
        it++;
    }
    if (it == item.production.second.end()) {
        for (string sym : item.lookahead)
            ret.insert(sym);
    } else {
        ret = setUnion(ret, Starts[*it]);
    }
    return ret;
}
class ENFA {
    public:
        Item q0;
        set<Item> Q;
        set<string> Symbols;
        set<Item> finalStates;
        map<Item,map<string, set<Item>>> delta;
        ENFA (
            Item q0,
            set<Item> Q, 
            set<Item> finalStates,
            set<string> Symbols, 
            map<Item, map<string, set<Item>>> delta
            ) :
            q0(q0), Q(Q), Symbols(Symbols), delta(delta) {}          
};

class NFA {
    public:
        set<int> Q;
        map<int, set<Item>> getRealState;
        set<int> finalStates;
        set<string> Symbols;
        map<int, map<string, set<int>>> delta;
        int numOfStates;
        NFA();
        NFA(
            set<int> Q,
            map<int, set<Item>> getRealState,
            set<int> finalStates,
            set<string> Symbols,
            map<int, map<string, set<int>>> delta,
            int numOfStates) :
                Q(Q), 
                getRealState(getRealState),
                finalStates(finalStates), 
                Symbols(Symbols), 
                delta(delta),
                numOfStates(numOfStates) {}
};
class DFA {
    public:
        set<int> Q;
        map<int, set<Item>> getRealState;
        set<int> finalStates;
        set<string> Symbols;
        map<int, map<string, int>> delta;
        DFA();
        DFA(
            set<int> Q,
            map<int, set<Item>> getRealState,
            set<int> finalStates,
            set<string> Symbols,
            map<int, map<string, int>> delta) :
                Q(Q), 
                getRealState(getRealState),
                finalStates(finalStates), 
                Symbols(Symbols), 
                delta(delta) {}
};


void printGrammar(
    map<string, bool> V,
    map<string, bool> T, 
    map<string,set<list<string>>> G) {
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

inline ENFA constructENFA (
    string S, //originalno početno stanje
    map<string,set<list<string>>> &G,
    set<string> &V,
    set<string> &T
) {
    string S1 = S + "'";
    V.insert(S1);
    list<string> ls;
    ls.push_back(S);
    set<list<string>> right;
    right.insert(ls);
    G[S1] = right;
    Item q0 = Item({S1, {S}});
    q0.lookahead.insert(END);

    set<string> Symbols = setUnion(V, T);
    map<string, bool> emptySym = calcEmptySymbols(G, V, T);
    /*
    cerr << "empty symbols: ";
    for (auto kv : emptySym) {
        if (kv.second) {
            cerr << kv.first << " ";
        }
    }
    
    cerr << "\n";
    */
    //računanje započinje izravno
    map<pair<string,string>, bool> StartsDirectlyWithSymbol = 
        calcStartsDirectlyWithSymbol(G, emptySym, Symbols);
    //računanje započine 
    map<pair<string,string>, bool> StartsWithSymbol = 
        calclStartsWithSymbol(StartsDirectlyWithSymbol, V, T);
    map<string, set<string>> Starts =
        calcStarts(StartsWithSymbol, V, T);
    
    /*
    for (auto kv : Starts) {
        cerr << kv.first << " zapocinje s: ";
        for (string str : kv.second) {
            cerr << str << " ";
        }
        cerr << "\n";
    }*/
    
    set<Item> Q;
    Q.insert(q0);

    set<Item> finalStates;
    map<Item,map<string, set<Item>>> delta;

    map<Item, bool> visited;
    queue<Item> queue;
    queue.push(q0);
    //q0.printItem();
    set<pair<Item, pair<string, Item>>> transitionSet;
    while (!queue.empty()) {
        //cerr << "prvi while\n";
        Item itm = queue.front();
        queue.pop();
        //itm.printItem();
        //cerr << "\n";
        string nxt = itm.nextSym();
        if (nxt == "") {
            finalStates.insert(itm);
            continue;
        }

        Item nxtItem = Item(itm.production, itm.ind + 1);
        nxtItem.lookahead = itm.lookahead;
        delta[itm][nxt].insert(nxtItem);
    
        itm.printItem();
        cerr << "   ,  "<< nxt << "   ->   ";
        nxtItem.printItem();
        cerr << "\n";
        
        transitionSet.insert({itm, {nxt, nxtItem}});
        //oneAfter[nxtItem] = oneAfter[itm];
        Q.insert(nxtItem);
        queue.push(nxtItem);
        //nxtItem.printItem();
        if (T.find(nxt) != T.end())
            continue;
        for (list<string> ls : G[nxt]) {
            pair<string, list<string>> production = {nxt, ls};
            Item nxtItem = Item(production);
            for (string sym : calcOneAfter(Starts, emptySym, itm)) 
                nxtItem.lookahead.insert(sym);
            delta[itm]["$"].insert(nxtItem);
            
            itm.printItem();
            cerr << "   ,  $   ->   ";
            nxtItem.printItem();
            cerr << "\n";
            
            transitionSet.insert({itm,{"$", nxtItem}});
            if (!visited[nxtItem]) {
                visited[nxtItem] = true;
                queue.push(nxtItem);
                Q.insert(nxtItem);
            }

        }
    }
   
    return ENFA(q0, Q, finalStates, Symbols, delta);
}

 


inline set<Item> epsClosure(
    Item item,
    map<Item, map<string, set<Item>>> &deltaEpsNFA
) {
    set<Item> ret;
    
    stack<Item> S;
    S.push(item);
    Item itm;
    while(!S.empty()) {
        itm = S.top();
        S.pop();
        ret.insert(itm);
        if (deltaEpsNFA[itm].find("$") != deltaEpsNFA[itm].end()) {
            for (Item itm1 : deltaEpsNFA[itm]["$"]) {
                if (ret.find(itm1) == ret.end()) 
                    S.push(itm1);
            }
        }
    }
    
    return ret;
}
inline NFA constructNFA (ENFA &enfa) {
    set<int> Q;
    map<int, set<Item>> getRealState;
    set<int> finalStates;
    set<string> Symbols = enfa.Symbols;
    map<int, map<string, set<int>>> delta;
    map<set<Item>, bool> isAdded;
    map<Item, set<int>> belongsToClosure; 

    belongsToClosure[enfa.q0].insert(0);
    Q.insert(0);
    getRealState[0] = epsClosure(enfa.q0, enfa.delta);
    for (Item itm : getRealState[0]) {
        belongsToClosure[itm].insert(0);
    }
    int stateNum = 1;
    for(Item itm : enfa.Q) {
        if (itm != enfa.q0) {
            set<Item> epsCls = epsClosure(itm, enfa.delta);
            if (!isAdded[epsCls]) {
                isAdded[epsCls] = true;
                Q.insert(stateNum);
                getRealState[stateNum] = epsCls;
                for (Item itm1 : epsCls) {
                    belongsToClosure[itm1].insert(stateNum);
                    if (enfa.finalStates.find(itm1) != enfa.finalStates.end()) {
                        finalStates.insert(stateNum);
                        break;
                    }
                }
                stateNum++;
            }
        } 
    }
    /*
    for (auto kv1 : belongsToClosure) {
        Item itm69 = kv1.first;
        itm69.printItem();
        cerr << " belongs to: ";
        for (int s : kv1.second) 
            cerr << s << " ";
        cerr << "\n";
    }
    */
    for (auto kv1 : enfa.delta) {
        for (auto kv2 : kv1.second) {
            Item state = kv1.first;
            string arg = kv2.first;
            set<Item> out = kv2.second;
            if (arg == "$")
                continue;
            for (Item itm : out) {
                set<int> Q1 = belongsToClosure[state];
                set<int> Q2 = belongsToClosure[itm];
                for (int q1 : Q1) {
                    for (int q2 : Q2) {
                        delta[q1][arg].insert(q2);
                    }
                }
            }
        }
    }
    //napravi DFS da skuzis koja su stanja dohvatljiva
    stack<int> DFSstack;
    DFSstack.push(0);
    unordered_map<int, bool> visited;
    int s;
    set<int> reachable;
    while (!DFSstack.empty()) {
        s = DFSstack.top();
        DFSstack.pop();
        if (visited[s])
            continue;
        visited[s] = true;
        reachable.insert(s);
        set<int> set;
        string arg;
        for (auto [arg, set] : delta[s]) {
            for (int s1 : set) {
                if (!visited[s1]) 
                    DFSstack.push(s1);
            }
        }
    }
    //napravi mapiranja za preimenovanje stanja
    unordered_map<int, int> renameMap;
    int i = 0;
    for (int s : reachable) {
        renameMap[s] = i;
        i++;
    }
    //iteriraj kroz strukture u NFA i primjeni mapiranje
    set<int> Q1;
    map<int, set<Item>> getRealState1;
    set<int> finalStates1;
    map<int, map<string, set<int>>> delta1;
    int stateNum1 = 0;
    for (int q : Q) {
        Q1.insert(renameMap[q]);
        stateNum1++;
    }
    int s1;
    set<Item> set1;
    for (auto [s1, set1] : getRealState) {
        if (reachable.find(s1) != reachable.end())
            getRealState1[renameMap[s1]] = set1;
    }
    for (int q : finalStates) {
        finalStates1.insert(renameMap[q]);
    }
    map<string, set<int>> mp;
    for (auto [s1, mp] : delta) {
        for (auto [arg, set1] : mp) {
            for (int j : set1) {
                delta1[renameMap[s1]][arg].insert(renameMap[j]);
            }
        }
    }
    return NFA(Q1, getRealState1, finalStates1, Symbols, delta1, stateNum1); 
}

inline DFA constructDFA(NFA &nfa) {
    set<int> Q = nfa.Q;
    map<int, set<Item>> getRealState = nfa.getRealState;
    set<int> finalStates = nfa.finalStates;
    set<string> Symbols = nfa.Symbols;
    map<int, map<string, int>> delta;
    int stateNum = nfa.numOfStates;
    stack<pair<pair<int, string>, set<int>>> S;
    map<set<int>, int> stateExists;
    for (auto kv1 : nfa.delta) {
        for (auto kv2 : kv1.second) {
            pair<pair<int, string>, set<int>> p;
            pair<int, string> p1 = pair<int, string> (kv1.first, kv2.first);
            p.first = p1;
            p.second = kv2.second;
            S.push(p);
        }
    }
    while(!S.empty()) {
        pair<pair<int, string>, set<int>> K;
        K = S.top();
        S.pop();
        if (K.second.size() == 1) {
            delta[K.first.first][K.first.second] = *K.second.begin();
        } else if (K.second.size() > 1){
            if (stateExists.find(K.second) != stateExists.end()) {
                delta[K.first.first][K.first.second] = stateExists[K.second];
                continue;
            }
            stateExists[K.second] = stateNum;
            Q.insert(stateNum);
            delta[K.first.first][K.first.second] = stateNum;
            set<Item> realState;
            for (int stateInd : K.second) {
                if (nfa.finalStates.find(stateInd) != nfa.finalStates.end()) 
                    finalStates.insert(stateNum);
                for (Item itm : getRealState[stateInd])
                    realState.insert(itm);
            }
            getRealState[stateNum] = realState;
            for (string sym : Symbols) {
                set<int> newSubset;
                for (int state : K.second) {
                    if (nfa.delta[state][sym].size()) {
                        for (int state1 : nfa.delta[state][sym]) {
                            newSubset.insert(state1);
                        }
                    }
                }
                if (!newSubset.empty()) {
                    S.push({{stateNum, sym}, newSubset});
                }
            }
            stateNum++;
        }
    }
    return DFA(Q, getRealState, finalStates, Symbols, delta);
}
int main() {
    map<string,set<list<string>>> G;
    map<string, bool> V;
    set<string> Vs;
    map<string, bool> T;
    set<string> Ts;
    map<string, bool> Syn;
    string S;

    map<pair<string, list<string>>, int> wasAdded;
    string w;
    string line;

    getline(cin, line);
    stringstream ssV(line);
    ssV >> w;
    //cerr << w << "\n";
    bool first = true;
    while (ssV >> w) {
        //cerr << w << "\n";
        if (first) {
            S = w;
            first = false;
        }
        V[w] = true;
        Vs.insert(w);
    }
    getline(cin, line);
    stringstream ssT(line);
    ssT >> w;
    //cerr << w << "\n";
    while (ssT >> w) {
        //cerr << w << "\n";
        T[w] = true;
        Ts.insert(w);
    }
    getline(cin, line);
    stringstream ssSyn(line);
    ssSyn >> w;
    //cerr << w << "\n";
    while (ssSyn >> w) {
        //cerr << w << "\n";
        Syn[w] = true;
    }
    string left;
    int i = 0;
    while (getline(cin, line)) {
        stringstream ss(line);
        if (line[0] != ' ') {
            //cerr << "left\n";
            ss >> left;
            //cerr << left << "\n";
        } else {
            list<string> right;
            //cerr << "right\n";
            while (ss >> w) {
                //cerr << w << "\n";
                right.push_back(w);
            }
            wasAdded[{left,right}] = i;
            i++;
            G[left].insert(right);
        }  
    }
    printGrammar(V, T, G);
    
    ENFA enfa = constructENFA(S, G, Vs, Ts);
    //constructItems(oneAfter, G, Vs, Ts, S);
    /*
    cerr << "ENFA\n";  
    for (auto kv1 : enfa.delta) {
        Item itm1 = kv1.first;
        for (auto kv2 : kv1.second) {
            string arg = kv2.first;
            for (Item itm2 : kv2.second) {
                itm1.printItem();
                cerr << "   ,  "<< kv2.first << "   ->   ";
                itm2.printItem();
                cerr << "\n";
            }
        }
    }
    */
    NFA nfa = constructNFA(enfa);
    /*
    for (auto kv1 : nfa.delta) {
        int s1 = kv1.first;
        for (auto kv2 : kv1.second) {
            string arg = kv2.first;
            cerr << s1 << "  ,  " << arg << "  ->  ";
            for (int i : kv2.second)
                cerr << i << " ";
            cerr << "\n";
        }
    }

    for (auto kv : nfa.getRealState) {
        cerr << "State " << kv.first << ":\n";
        for (Item itm : kv.second) {
            itm.printItem();
            cerr << "\n";
        }
    }
    */
    DFA dfa = constructDFA(nfa);
    cerr << "DFA:\n";
    for (auto kv1 : dfa.delta) {
        int s1 = kv1.first;
        for (auto kv2 : kv1.second) {
            string arg = kv2.first;

            cerr <<s1 << "   ,  "<< arg << "   ->   " << kv2.second;
            cerr << "\n";

        }
    }
    for (auto kv : dfa.getRealState) {
        cerr << "State " << kv.first << ":\n";
        for (Item itm : kv.second) {
            itm.printItem();
            cerr << "\n";
        }
    }
    
    map<pair<int, string>, pair<string, pair<string, list<string>>>> LR1;
    ofstream parser("parser.txt");
    for (auto [s1, mp] : dfa.delta) {
        for (auto [sym, s2] : mp) {
            if (T[sym]) {
                list<string> ls;
                LR1[{s1, sym}] = {"pomakni",{to_string(s2), ls}};
            } else if (V[sym]) {
                list<string> ls;
                LR1[{s1, sym}] = {"stavi",{to_string(s2), ls}};
                //parser << s1 << " " << sym << " stavi " << s2 << "\n";
            }

        }
    }
    Item cmp = Item({S + "'", {S}}, 1);
    cmp.lookahead.insert(END);
    for (int q : dfa.Q) {
        for (Item itm : dfa.getRealState[q]) {
        
            if (itm == cmp) {
                list<string> ls;
                LR1[{q, END}] = {"prihvati", {"", ls}};
                continue;
            }
            else if (itm.ind == itm.production.second.size()) {
                for (string a : itm.lookahead) {
                    if (LR1.find({q, a}) == LR1.end()) {
                        LR1[{q, a}] = {"reduciraj", {itm.production}};
                    } else if (LR1[{q, a}].first == "reduciraj") {
                        if (wasAdded[itm.production] < wasAdded[LR1[{q, a}].second]) {
                            LR1[{q, a}] = {"reduciraj", {itm.production}};
                        }
                    }
                    /*
                    */
                }
            }
        }
    }
    for (auto [p, ac] : LR1) {
        if (ac.first == "reduciraj") {
            parser << p.first << " " << p.second << " reduciraj " << ac.second.first << " ";
            for (string sym : ac.second.second) {
                parser << sym << " ";
            }
            parser << "\n";
        } else if (ac.first == "prihvati"){
            parser << p.first << " " << END << " prihvati\n";
        } else if (ac.first == "pomakni") {
            parser << p.first << " " << p.second << " pomakni " << ac.second.first<< "\n";
        } else if (ac.first == "stavi") {
            parser << p.first << " " << p.second << " stavi " << ac.second.first<< "\n";
        }
    }
}