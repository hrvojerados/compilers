#include<bits/stdc++.h>
using namespace std;
using ll = long long;
string removeOuterBrackets(string expr) {
    ll n = expr.size();
    if (n == 0) 
        return expr;
    ll bracket = 0;
    while (expr[0] == '(' && expr[n - 1] == ')') {
        bracket = 0;
        bool valid = true;
        for (int i = 1; i < n - 1; ++i) {
            if (expr[i] == '(') 
                bracket++;
            if (expr[i] == ')') 
                bracket--;
            if (bracket != 0 && i != n - 1) {
                valid = false;
                break;
            }
        }
        if (valid) {
            expr = expr.substr(1, n - 2);
            n = expr.size();
        } else {
            break; 
        }
    }
    return expr;
}
vector<string> splt(string s) {
    /*
    string temp = removeOuterBrackets(s);
    cerr << temp << "\n";
    s = temp;
    */
    ll bracket = 0;
    vector<string> ret;
    string tmp = "";
    bool esc = false;
    for (ll i = 0; i < s.length(); i++) {
        if (s[i] == '\\' && !esc)
            esc = true;
        else if (s[i] == '(' && !esc) {
            bracket++;
            if (s[i] != '\0')
                tmp += s[i];
        }
        else if(s[i] == ')' && !esc) {
            bracket--;
            if (s[i] != '\0')
                tmp += s[i];
        }
        else if (s[i] == '|' && bracket == 0 && !esc){
            /*
            if (tmp[0] == '(' && 
                tmp[s.length() - 1] == ')')
                tmp = tmp.substr(1,s.length() - 1);
            */
            ret.push_back(tmp);
            tmp = "";
        } else {
            if (esc) {
                esc = false;
                tmp += '\\';
            }
            if (s[i] != '\0')
                tmp += s[i];
        }
    }
    //cerr << "tmp: " << tmp << "\n";
    ret.push_back(tmp);
    return ret;
}
string substituteRegs(string expr, map<string,string> &regs) { //nije testirano
    string ret = "";
    bool esc = false;
    for (ll i = 0; i < expr.length(); i++) {
        if (expr[i] == '\\' && !esc)
            esc = true;
        else if (expr[i] == '{' && !esc) {
            string tmp = "";
            while (expr[i] != '}' && !esc) {
                if (expr[i] == '\\')
                    esc = true;
                else {
                    if (expr[i] != '\0') {
                        if (esc) {
                            tmp += '\\';
                            esc = false;
                        }
                        tmp += expr[i];
                    }
                }
                i++;
            }
            tmp += "}";
            ret += "(" + regs[tmp] + ")";
        } else {
            if (expr[i] != '\0') {
                if (esc) {
                    ret += '\\';
                    esc = false;
                }
                ret += expr[i];
            }
        }
    }
    //cout << ret << "\n";
    return ret;
}

void NFA(string expr, vector<string> &delta, string pref, ll &n, string left, string right) {
    vector<string> L = splt(expr);
    if (L.size() == 1) {
        string s = L[0];
        bool esc = false;
        string state = left;
        //cerr << s << "\n";
        for (ll i = 0; i < s.length(); i++) {
            //cerr << s << "\n";
            if (s[i] == '(' && !esc) {
                string str = "";
                ll bracket = 1;
                i++;
                //cerr << i << " " << s << "\n";
                while (true) {
                    if (s[i] == '\\' && !esc)
                        esc = true;
                    else if (s[i] == '(' && !esc) {
                        bracket++;
                        if (bracket != 0)
                            str += '(';
                    }
                    else if (s[i] == ')' && !esc) {
                        bracket--;
                        if (bracket != 0)
                            str += ')';
                    }
                    else if (s[i] != '\0') {
                        if (esc) {
                            esc = false;
                            str += '\\';
                        }
                        str += s[i];
                    }
                    if (bracket == 0) { // jako ružno
                        break;
                    }
                    i++;
                }
                //cerr << s[i] << " " << i << "\n";
                //cerr << str << "\n";
                if (i == s.length() - 1) {
                    NFA(str, delta, pref, n, state, right);
                } else if (i == (s.length() - 2) && s[i + 1] == '*') {
                    NFA(str, delta, pref, n, state, right);
                    string tmp = "";
                    tmp = state + ",$=" + right;
                    delta.push_back(tmp);
                    tmp = "";
                    tmp = right + ",$=" + state;
                    delta.push_back(tmp);
                    i++;
                } else {
                    string newState = pref + "_" + to_string(n);
                    n++;
                    NFA(str, delta, pref, n, state, newState);
                    if (s[i + 1] == '*') {
                        string tmp = "";
                        tmp += (newState + ",$=" + state);
                        delta.push_back(tmp);
                        tmp = "";
                        tmp += (state + ",$=" + newState);
                        delta.push_back(tmp);
                        i++; //refactor
                    }
                    state = newState;
                }
            } else if (s[i] == '\\' && !esc) {
                esc = true;
            } else {
                //cerr << s << " " << s[i] << "\n" ;
                string arg = "";
                if (s[i] == 't' && esc) {
                    arg = "\\t";
                } else if (s[i] == 'n' && esc) {
                    arg = "\\n";
                } else if (s[i] == '_' && esc) {
                    arg = ' ';
                } else if (s[i] == '=')
                    arg = "\\=";
                else {
                    if (esc) 
                        arg += '\\';
                    arg += s[i];
                }
                esc = false;
                string tmp;
                if (i == s.length() - 1) {
                    tmp = state + "," + arg + "=" + right;
                    delta.push_back(tmp);
                    state = right;
                } else if (i == s.length() - 2 && s[i + 1] == '*') {
                    tmp = state + "," + arg + "=" + right;
                    delta.push_back(tmp);
                    tmp = state + ",$=" + right;
                    delta.push_back(tmp);
                    tmp = right + ",$=" + state;
                    delta.push_back(tmp);
                    i++;
                } else {
                    string newState = pref + "_" + to_string(n);
                    n++;
                    tmp = state + "," + arg + "=" + newState;
                    delta.push_back(tmp);
                    if (s[i + 1] == '*') {
                        tmp = newState + "," + arg + "=" + newState;
                        delta.push_back(tmp);
                        tmp = state + ",$=" + newState;
                        delta.push_back(tmp);
                        i++;
                    }
                    state = newState;
                }
            }
        }
        /*if (state != right) {
            string tmp = "";
            tmp += (state + ",$=" + right);
            delta.push_back(tmp);
        }*/
    } else{
        for (string str : L) {
            //cerr << str << "\n";
            string tmp = "";            
            string newLeft = pref + "_L" + to_string(n);
            tmp = left + ",$=" + newLeft; 
            delta.push_back(tmp);

            string newRight = pref + "_R" + to_string(n);
            tmp = newRight + ",$=" + right;
            delta.push_back(tmp);
            n++; 
            NFA(str, delta, pref, n, newLeft, newRight);
        }
    }
}
int main() {
    map<string,string> regs;    
    string line;
    getline(cin, line);
    while (line.substr(0,2) != "%X") {
        string name = "";
        ll i = 0;
        while (line[i] != ' ') {
            name += line[i];
            i++;
        }
        i++;
        string regex = "";
        while (i < line.length()) {
            if (line[i] != '\0')
                regex += line[i];
            i++;
        }
        regs[name] = substituteRegs(regex, regs);
        getline(cin, line);
    }
    ofstream states("states.txt");
    if (states.is_open()) {
        states << line.substr(3, line.length());
        states.close();
    } else {
        cerr << "Couldn't open file\n";
    }
    ofstream tokens("tokens.txt");
    getline(cin,line);
    if (tokens.is_open()) {
        tokens << line.substr(3, line.length());
        tokens.close();
    } else {
        cerr << "Couldn't open file\n";
    }
    ll i = 0;
    ll ind = 0;
    ofstream mapings("delta.txt");
    ofstream actions("actions.txt");
    while(getline(cin, line)) {
        if (line[0] == '<') {
            ll i = 1;
            string state = "";
            while (line[i] != '>') {
                state += line[i];
                i++;
            }
            i++;
            string regex = line.substr(i, line.length() - 1);
            regex = substituteRegs(regex, regs);
            vector<string> delta;
            ll n = 1;
            NFA(regex, delta, to_string(ind), n,
                 to_string(ind) + "_L0",
                 to_string(ind) + "_R0");
            mapings << (state + ",$=" + to_string(ind) + "_L0") << "\n";
            for (string str : delta) {
                if (mapings.is_open())
                    mapings << str << "\n";
                else 
                    cerr << "Couldn't open the file\n";
            }
            ind++;
        } else if (line[0] == '{') {
            getline(cin, line);
            while (line[0] != '}') {
                if (actions.is_open())
                    actions << line << ",";
                else
                    cerr << "Couldn't open the file\n";
                getline(cin, line);
            }
            actions << "\n";
        }
    } 
    mapings.close();
    actions.close();

    //test zone
    /*
    string line1; 
    cin >> line1;
    map<string, string> regs;
    regs["{sviZnakovi}"] = "\\(|\\)|\\{|\\}|\\||\\*|\\|\\$|\t|\n|\\_|!|\"|#|%|&|\'|+|,|-|.|/|0|1|2|3|4|5|6|7|8|9|:|;|<|=|>|?|@|A|B|C|D|E|F|G|H|I|J|K|L|M|N|O|P|Q|R|S|T|U|V|W|X|Y|Z|[|]|^|_|`|a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z|~";
    string s;
    s = substituteRegs(line1, regs);
    cout << s << "\n";
    vector<string> delta;
    ll n = 1;
    NFA(s, delta, "r1", n, "r1_L0", "r1_R0");
    for (string str : delta) {
        cout << str << "\n";
    }
    */
}//(a|b|c)*|a|b|c|(a|(b|ca))
