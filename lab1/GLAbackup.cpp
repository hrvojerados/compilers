
#include<bits/stdc++.h>
using ll = long long;

using namespace std;
set<string> intersetion(set<string> S1, set<string> S2) {
    set<string> ret;
    set_intersection(S1.begin(),S1.end(),S2.begin(), S2.end(),inserter(ret, ret.begin()));
    return ret;
}
bool chkSet(set <string> R) {
    for (string str : R) {
        ll i = 0;
        while (str[i] != '_') 
            i++;
        i++;
        if (str[i] == 'R' && str[i + 1] == '0') 
            return true;
    }
    return false;
}
ll getRegex(set<string> &R) {
    // pronađi regex iz presjeka R i F koji ima najmanji prefiks
    ll ret = 1e18;
    for (string r : R) {
        ll n;
        string tmp = "";
        ll i = 0;
        while (r[i] != '_') {
            if (r[i] != '\0')
                tmp += r[i];
            i++;
        }
        i++;
        n = stoll(tmp);
        tmp = r.substr(i, i + 2);
        if (tmp == "R0" && n < ret)
            ret = n; 
    }
    return ret;
}
void execute(
    list<string> &L,
    unordered_map<string, bool> &isState,
    unordered_map<string, bool> &isToken,
    string &LAstate,
    ll &lineNum,
    ll &current,
    ll &start,
    ll &lsRecognised,
    string &text
    ) {
    //akcije koje treba implementirati:
    //-
    //NOVI_REDAK
    //UDJI_U_STANJE <stanje>
    //<leksicka jedinka>{
    //VRATI_SE n
    for (string act : L) {
        string arg = "";
        string tmp = "";
        ll i = 0;
        while (act[i] != ' ' && i < act.length()) {
            if (act[i] != '\0')
                tmp += act[i];
            i++;
        }
        i++;
        while (i < act.length()) {
            if (act[i] != '\0')
                arg += act[i];
            i++;
        }
        act = tmp;
        //cerr << act << " " << arg << "\n";
        if (act == "NOVI_REDAK")
            lineNum++;
        else if (act == "-") {

        } else if (act == "UDJI_U_STANJE") {
            if (isState[arg]) {
                LAstate = arg;
            } else {
                cerr << "State" << arg << "doesn't exist\n";
            }
        } else if (act == "VRATI_SE") {
            ll n;
            try {
                n = stoll(arg, nullptr, 10);
            } catch (exception ex) {
                cerr << "Argument of \"VRATI_SE\" must be a number\n";
            }
            current = n;
        } else {
            if (isToken[act]) {
                string tmp = "";
                for (ll i = start; i < lsRecognised; i++) {
                    if (text[i] != '\0')
                        tmp += text[i];
                }
                cout << act << " " << lineNum << " " << tmp <<"\n";
            } else {
                cerr << "Token \"" << act << "\" doesn't exist\n";
            }
        }
    }
}
void loadActions(vector<list<string>> &actions) { //nije testirano
    ifstream actionsFile("../actions.txt");
    if (actionsFile.is_open()) {
        ll i = 0;
        string line;
        while(actionsFile) {
            getline(actionsFile, line);
            list<string> L;
            string act = "";
            for (char ch : line) {
                if (ch == ',') {
                    //cerr << "action: " << act << "\n";
                    L.push_back(act);
                    act = "";
                } else if (ch != '\0')
                    act += ch;
            }
            actions.push_back(L);
        }
    }
    actionsFile.close();
}
void loadNKA(map<pair<string,string>, set<string>> &delta) {
    ifstream mapings("../delta.txt");
    string line;
    if (mapings.is_open()) {
        while(mapings) {
            getline(mapings, line);
            if (line == "") //lexical analyzer generator creates an extra line
                continue;   // so this piece of code is used to escape it
            ll i = 0;
            string Sstate = "";
            while (line[i] != ',') {
                if (line[i] != '\0')
                    Sstate += line[i];
                i++;
            }
            i++;
            string ch = "";
            while (line[i] != '=') {
                if (line[i] != '\0')
                    ch += line[i];
                i++;
            }
            if (ch == "\\n")
                ch = "\n";
            else if (ch == "\\t")
                ch = "\t";
            i++;
            string FState = line.substr(i, line.length());
            pair<string, string> p = pair(Sstate, ch);
            if (delta.count(p) == 0) {
                set<string> tmp;
                tmp.insert(FState);
                delta[p] = tmp;
                //cerr << Sstate << "," << ch << "->" << FState << "\n";
            } else {
                //cerr << Sstate << "," << ch << "->" << FState << "\n";
                delta[p].insert(FState);
            }
        }
    } else {
        cerr << "Couldn't open the file\n";
    }
    mapings.close();
    
}
set<string> epsSet(set<string> Q, string a, map<pair<string,string>, set<string>> & delta) {
    set<string> ret;
    for (string q : Q) {
        pair<string, string> p = pair(q, a);
        if (delta.count(p) == 0)
            continue;
        set<string> X = delta[p];
        stack <string> S;
        for (string x : X) {
            S.push(x);
        }
        while (!S.empty()) {
            string state = S.top();
            S.pop();
            ret.insert(state);
            pair<string, string> p1 = pair(state, "$");
            if (delta.count(p1) == 0)
                continue;
            for (string q1 : delta[p1])
                S.push(q1);
        }
    }
    return ret;
}
int main() {
    /*
        Oke znači ideja je ovakva:
        Ovdje implementiram pseduo kod za eps prijelaze, sto znaci da mi treba funkcija 
        epsSet. Sve se moze to implementirati ovdje osim eps tablice  za koju cu imati posebnu load funkciju
        Generator ce napraviti file u kojem su opisani prijelazi tog NKA i onda ce LA samo preko loada stvoriti funkciju prijelaza.
        Još nisam siguran kako bih trebao implementirati akcije. Hoce li njih implementirati generator (ako da onda ne znam kako to izvesti)
        ili ih mogu samo pretpostaviti.
    */
    map<pair<string,string>, set<string>> delta;
    ll current = 0; //zavrsetak
    ll lsRecognized = 0; //posljednji
    ll start = 0; //početak
    ll expr = -1; //izlaz
    string LAstate = "";
    unordered_map<string, bool> isState;

    ifstream LAstates("../states.txt");
    if (LAstates.is_open()) {
        string tmp;
        LAstates >> LAstate;
        //cerr << "Stanje: " << LAstate << "\n";
        isState[LAstate] = true;
        while (LAstates >> tmp) {
            isState[tmp] = true;
            //cerr << "Stanje: " << tmp << "\n";
        }
    } else {
        cerr << "Couldn't open file states.txt\n";
    }
    LAstates.close();

    unordered_map<string, bool> isToken;
    ifstream tokens("../tokens.txt");
    if (tokens.is_open()) {
        string tmp;
        while(tokens >> tmp) {
            //cerr << "token: " << tmp << "\n";
            isToken[tmp] = true;
        } 
    } else {
        cerr << "Couldn't open file states.txt\n";
    }
    tokens.close();
    
    vector<list<string>> actions;
    loadActions(actions);

    ll lineNum = 0; //redak
    loadNKA(delta); //loadaj epsNKA
    
    string text;
    getline(cin, text, '\0');
    string a;

    set<string> Q;
    Q.insert(LAstate);

    set<string> R = epsSet(Q, "$", delta);
    while (text.size() > start) {
        if (!chkSet(R) && R.size() != 0) {
            a = text[current];
            current++; 
            Q = R;
            R = epsSet(Q, a, delta);
        } else if(chkSet(R)) {
            expr = getRegex(R);
            lsRecognized = current;
            a = text[current];
            current++; //Hmmmmm greska?
            Q = R;
            R = epsSet(Q, a, delta); 
        } else if(R.size() == 0 && expr == -1) {
            current = start;
            cerr << "Ispravljam pogrešku na poziciji "<< start << "\n"; 
            start++;
            Q.clear();
            Q.insert(LAstate);
            R = epsSet(Q, "$", delta);
        } else if (R.size() == 0 && expr != -1) {
            execute( 
                actions[expr],
                isState,
                isToken,
                LAstate,
                lineNum,
                current,
                start,
                lsRecognized,
                text);
            expr = -1;
            start = lsRecognized; // +1 navodno
            current = start;
            Q.clear();
            Q.insert(LAstate);
            R = epsSet(Q, "$", delta); //PROBLEM
        }
        
    }
}