#include<bits/stdc++.h>
#include<iterator>
using namespace std;
using ll = long long;
bool find(string target, list<string> stringList) {
    for (string str : stringList) {
        if (str == target) 
            return true; 
        
    }
    return false;
}
string trim(string  str) {
    size_t first = str.find_first_not_of(" \t\n\r\f\v");
    if (first == string::npos) return "";
    size_t last = str.find_last_not_of(" \t\n\r\f\v");
    return str.substr(first, last - first + 1);
}
list<string> split(string input) { //provjeri ovo
    list<string> result;
    string trimmedInput = input;
    stringstream ss(trimmedInput);
    string token;
    int openParentheses = 0;
    string currentToken;
    while (getline(ss, token, ',')) {
        for (char ch : token) {
            if (ch == '(') {
                openParentheses++;
            } else if (ch == ')') {
                openParentheses--;
            }
            currentToken += ch;
            if (openParentheses == 0 && (ch == ',' || &ch == &token.back())) {
                if (currentToken.find_first_not_of(" \t") != string::npos) {
                    result.push_back(currentToken);
                }
                currentToken.clear();
                break;
            }
        }
    }
    return result;
}
class Node {
    public:
        Node* parent;
        string name;    
        list<Node*> children;

        bool isScopeParent;
        map<string, string> declarationTable;

        Node() : parent(nullptr){}
        Node(Node* parent, string name) : parent(parent), name(name) {}
        void printNode() {
            cerr << "NODE: \"" 
                << name 
                << "\" PARENT NAME: \"" 
                << parent->name 
                << "\"\n";
        }
};
class GenTree {
    public:
        Node* root;
        list<Node*> nodes;
        GenTree() : root(nullptr) {}
        GenTree(Node* root, list<Node*> nodes) : root(root), nodes(nodes) {}
        void printTree() {

        }
        void subTree(Node* node) {
        }
};
Node* root;
Node* current;
map<string, map<string, bool>> castingRelation; //nije implementirano
map<string, string> definitionTable;
bool primarni_izraz(string& tip, bool& l_izraz, int& isNIZ_ZNAKOVA);
bool postfiks_izraz(string& tip, bool& l_izraz, int& isNIZ_ZNAKOVA);
bool lista_argumenata(list<string>& tipovi);
bool unarni_izraz(string& tip, bool& l_izraz, int& isNIZ_ZNAKOVA);
bool cast_izraz(string& tip, bool& l_izraz, int& isNIZ_ZNAKOVA);
bool ime_tipa(string& tip);
bool specifikator_tipa(string& tip);
bool multiplikativni_izraz(string &tip, bool& l_izraz, int& isNIZ_ZNAKOVA);
bool aditivni_izraz(string &tip, bool& l_izraz, int& isNIZ_ZNAKOVA);
bool odnosni_izraz(string &tip, bool& l_izraz, int& isNIZ_ZNAKOVA);
bool jednakosni_izraz(string &tip, bool& l_izraz, int& isNIZ_ZNAKOVA);
bool bin_i_izraz(string &tip, bool& l_izraz, int& isNIZ_ZNAKOVA);
bool bin_xili_izraz(string &tip, bool& l_izraz, int& isNIZ_ZNAKOVA);
bool bin_ili_izraz(string &tip, bool& l_izraz, int& isNIZ_ZNAKOVA);
bool log_i_izraz(string &tip, bool& l_izraz, int& isNIZ_ZNAKOVA);
bool log_ili_izraz(string &tip, bool& l_izraz, int& isNIZ_ZNAKOVA);
bool izraz_pridruzivanja(string &tip, bool& l_izraz, int& isNIZ_ZNAKOVA);
bool izraz(string &tip, bool& l_izraz);

bool slozena_naredba(bool isloop, string context);
bool lista_naredbi(bool isloop, string context);
bool naredba(bool isloop, string context);
bool izraz_naredba(string& tip);
bool naredba_grananja(bool isloop, string context);
bool naredba_petlje(bool isloop, string context);
bool naredba_skoka(bool isloop, string context);
bool prijevodna_jedinica();
bool vanjska_deklaracija();

bool definicija_funkcije();
bool lista_parametara(list<string>& tipovi, list<string>& imena);
bool deklaracija_parametra(string& tip, string& ime);
bool lista_deklaracija();
bool deklaracija();
bool lista_init_deklaratora(string ntip);
bool init_deklarator(string ntip);
bool izravni_deklarator(string& tip, int& br_elem, string ntip);
bool inicijalizator(string& tip, list<string>& tipovi, int& br_elem);
bool lista_izraza_pridruzivanja(list<string>& tipovi, int& br_elem);

void errorOutput(Node* lf, list<Node*> children) {
    cout << lf->name << " ::= ";
    for (Node* child : children) {
        if (child->name[0] == '<') {
            cout << child->name << " ";
        } else {
            stringstream ssName(child->name);
            string tokenName;
            string lineNumber;
            string identifierName;
            ssName >> tokenName;
            ssName >> lineNumber;
            ssName >> identifierName;
            cout << tokenName << "(" << lineNumber << "," << identifierName << ") ";
        }
    }
}
bool isInt(string numValue) {
    try {
        int n = stoi(numValue);
        return true;
    } catch(...) {
        return false;
    }
}
bool isChar(string charValue) { //vrati se na ovo po potrebi
    if (charValue.length() == 3) 
        return true;
    else if (charValue.length() == 4 && 
        (charValue == "'\\t'" ||
         charValue == "'\\n'" ||
         charValue == "'\\0'" ||
         charValue == "'\\''" ||
         charValue == "'\\\"'" || 
         charValue == "\\\\")) {
        return true;
    }
    return false;
}
bool isCharArray(string charArray) {
    if (charArray.length() < 2 ||
        charArray.front() != '"' ||
        charArray.back() != '"') {
        return false;
    }
    unordered_set<char> validEscapes = {'t', 'n', '0', '\'', '\"', '\\'};
    for (int i = 1; i < charArray.length() - 1; i++) {
        if (charArray[i] == '\\') {
            if (i + 1 >= charArray.length() - 1 || validEscapes.find(charArray[i + 1]) == validEscapes.end()) {
                return false;
            }
            ++i;
        } else if (!isprint(charArray[i])) {
            return false;
        }
    }
    return true;
}
//ako nešto nije deklarirano/definirano vrati void
string isDeclared(Node* identifierNode, string name) {
    Node* pt = identifierNode;
    while (pt != nullptr) {
        if (pt->isScopeParent && pt->declarationTable.find(name) != pt->declarationTable.end()) {
            return pt->declarationTable[name];
        }
        pt = pt->parent;
    }
    return "void";
}
string isDeclaredLocally (Node* identifierNode, string name) {
    Node* pt = identifierNode;
    while (!pt->isScopeParent) {
        //cerr << "a\n";
        pt = pt->parent;
    }
    if (pt->declarationTable.find(name) != pt->declarationTable.end())
        return pt->declarationTable[name];
    return "void";
}
string isDefined(string fooName) {
    if (definitionTable.find(fooName) == definitionTable.end()) {
        return "void";
    }
    return definitionTable[fooName];
}
void declare(Node* identifierNode, string name, string type) {
    Node* pt = identifierNode;
    while(!pt->isScopeParent) {
        pt = pt->parent;
    }
    pt->declarationTable[name] = type;
}
void define(string name, string type) {
    definitionTable[name] = type;
}

bool primarni_izraz(string& tip, bool& l_izraz, int& isNIZ_ZNAKOVA) {
    //provjera za produkciju
    cerr<<"primarni_izraz\n";
    isNIZ_ZNAKOVA = -1;
    string childrenConcat = "";
    for (Node* child : current->children) {
        stringstream ssName = stringstream(child->name);
        string tmp;
        ssName >> tmp;
        childrenConcat += tmp;
        childrenConcat += " ";
    }
    childrenConcat = trim(childrenConcat);
    //cerr << "primarni_izraz" << childrenConcat << "\n";
    if (childrenConcat == "L_ZAGRADA <izraz> D_ZAGRADA") { 
        vector<Node*> v;
        list<Node*>::iterator it = current->children.begin();
        while(it != current->children.end()) {
            v.push_back(*it);
            it++;
        }
        current = v[1];
        string izraz_tip;
        bool izraz_l_izraz;
        if (!izraz(izraz_tip, izraz_l_izraz)) 
            return false;

        tip = izraz_tip;
        l_izraz = izraz_l_izraz;
    } else if (childrenConcat == "IDN") { //nedefinirano ponašanje za eksplicitno castanje?
        Node* node = *(current->children.begin());
        stringstream ssName(node->name);
        string tokenName;
        string lineNumber;
        string identifierName;
        ssName >> tokenName;
        ssName >> lineNumber;
        ssName >> identifierName;
        if (isDeclared(current, identifierName) == "void") {
            cout << "<primarni_izraz> ::= IDN(" 
                << lineNumber << "," 
                << identifierName 
                << ")\n"; 
            return false;
        }
        tip = isDeclared(current, identifierName);
        l_izraz = (tip.substr(0,5) != "const");
    } else if(childrenConcat == "BROJ") {
        Node* node = *(current->children.begin());
        stringstream ssName(node->name);
        string tokenName;
        string lineNumber;
        string numValue;
        ssName >> tokenName;
        ssName >> lineNumber;
        ssName >> numValue;
        if (!isInt(numValue)) {
            //ISPIS GREŠKE
            cout << "<primarni_izraz> ::= BROJ(" 
                << lineNumber << "," 
                << numValue 
                << ")\n"; 
            return false;
        }
        tip = "int";
        l_izraz = false;
    } else if(childrenConcat == "ZNAK") {
        Node* node = *(current->children.begin());
        stringstream ssName(node->name);
        string tokenName;
        string lineNumber;
        string charValue;
        ssName >> tokenName;
        ssName >> lineNumber;
        ssName >> charValue;
        cerr << "isCHAR " << charValue << " " << isChar(charValue) << "\n";
        if (!isChar(charValue)) {
            //ISPIS GREŠKE
            cout << "<primarni_izraz> ::= ZNAK(" 
                << lineNumber << "," 
                << charValue 
                << ")\n"; 
            return false;
        }
        tip = "char";
        l_izraz = false;
    } else if(childrenConcat == "NIZ_ZNAKOVA") {
        Node* node = *(current->children.begin());
        stringstream ssName(node->name);
        string tokenName;
        string lineNumber;
        string charArray;
        ssName >> tokenName;
        ssName >> lineNumber;
        ssName >> charArray;
        cerr << "isCharArray " << charArray << " " << isCharArray(charArray) << "\n";
        if (!isCharArray(charArray)) {
            //ISPIS GREŠKE
            cout << "<primarni_izraz> ::= NIZ_ZNAKOVA(" 
                << lineNumber << "," 
                << charArray 
                << ")\n"; 
            return false;
        }
        tip = "niz(const(char))";
        l_izraz = false;
        isNIZ_ZNAKOVA = charArray.length() - 2;
        //cerr << isNIZ_ZNAKOVA << " " << charArray << "\n" 
    } else {
        cerr << "SINTAKSNO STABLO NE VALJA\n";
    }
    return true;
}
bool postfiks_izraz(string& tip, bool& l_izraz, int& isNIZ_ZNAKOVA) {
    cerr << "postfiks_izraz\n"; 
    string childrenConcat = "";
    for (Node* child : current->children) {
        stringstream ssName = stringstream(child->name);
        string tmp;
        ssName >> tmp;
        childrenConcat += tmp;
        childrenConcat += " ";
    }
    childrenConcat = trim(childrenConcat);
    if (childrenConcat == "<primarni_izraz>") {
        string primarni_izraz_tip;
        bool primarni_izraz_l_izraz;
        int bl;
        current = *(current->children.begin());
        if(!primarni_izraz(primarni_izraz_tip, primarni_izraz_l_izraz, bl)) {
            return false;
        }
        isNIZ_ZNAKOVA = bl;

        tip = primarni_izraz_tip;
        l_izraz = primarni_izraz_l_izraz;
    } else if (childrenConcat == "<postfiks_izraz> L_UGL_ZAGRADA <izraz> D_UGL_ZAGRADA") { //opet isto s castanjem
        Node* par = current; 
        vector<Node*> v;
        list<Node*>::iterator it = current->children.begin();
        while(it != current->children.end()) {
            v.push_back(*it);
            it++;
        }
        string postfiks_izraz_tip;
        bool postfiks_izraz_l_izraz;
        int bl;
        current = v[0];
        if (!postfiks_izraz(postfiks_izraz_tip, postfiks_izraz_l_izraz, bl)) {
            return false;
        }
        isNIZ_ZNAKOVA = -1;
        //cerr << postfiks_izraz_tip << "\n";
        if (postfiks_izraz_tip != "niz(int)" &&
            postfiks_izraz_tip != "niz(char)" &&
            postfiks_izraz_tip != "niz(const(int))" &&
            postfiks_izraz_tip != "niz(const(char))") {
            
            errorOutput(par, par->children);
            return false;
        }
        current = v[2];
        string izraz_tip;
        bool izraz_l_izraz;
        if(!izraz(izraz_tip, izraz_l_izraz)) {
            return false;
        }
        if (!castingRelation[izraz_tip]["int"]) {
            errorOutput(par, par->children);
            return false;
        }
        tip = postfiks_izraz_tip.substr(4, postfiks_izraz_tip.length());
        tip = tip.substr(0,tip.length() - 1);
        l_izraz = (tip.substr(0,5) != "const");
        //cerr << l_izraz << "\n";
    } else if (childrenConcat == "<postfiks_izraz> L_ZAGRADA D_ZAGRADA") { //castanje 
        Node* par = current;
        vector<Node*> v;
        list<Node*>::iterator it = current->children.begin();
        while(it != current->children.end()) {
            v.push_back(*it);
            it++;
        }
        current = v[0];
        string postfiks_izraz_tip;
        bool postfiks_izraz_l_izraz;
        int bl;
        if (!postfiks_izraz(postfiks_izraz_tip, postfiks_izraz_l_izraz, bl))
            return false;
        isNIZ_ZNAKOVA = -1;
        if (postfiks_izraz_tip.substr(0, 15) != "funkcija(void->") {
            errorOutput(par, par->children);
            return false;
        }
        tip = postfiks_izraz_tip.substr(15, postfiks_izraz_tip.length());
        tip = tip.substr(0, tip.length() - 1);
        l_izraz = 0;
    } else if (childrenConcat == "<postfiks_izraz> L_ZAGRADA <lista_argumenata> D_ZAGRADA") {
        Node* par = current;
        vector<Node*> v;
        list<Node*>::iterator it = current->children.begin();
        while(it != current->children.end()) {
            v.push_back(*it);
            it++;
        }
        current = v[0];
        string postfiks_izraz_tip;
        bool postfiks_izraz_l_izraz;
        int bl;
        if(!postfiks_izraz(postfiks_izraz_tip, postfiks_izraz_l_izraz, bl)) {
            return false;
        }
        isNIZ_ZNAKOVA = -1;
        current = v[2];
        list<string> lista_argumenata_tipovi;
        if(!lista_argumenata(lista_argumenata_tipovi)) {
            return false;
        }
        bool tmp = true;
        /*
        for (string str : lista_argumenata_tipovi) {
            cerr << str << " ";
        }
        cerr << "\n";*/
        if (postfiks_izraz_tip.substr(0,9) != "funkcija(")
            tmp = false;
        string params = "";
        int i = 9;
        list<string> paramsList;
        //cerr << postfiks_izraz_tip << "\n";
        while (postfiks_izraz_tip[i] != '-') { //pls nemoj da funkcije mogu biti dio niza
            if (postfiks_izraz_tip[i] == ',') {
                paramsList.push_back(params);
                params = "";
            } else {
                params += postfiks_izraz_tip[i];
            }
            i++;
        }
        paramsList.push_back(params);
        i += 2;


        string pov = postfiks_izraz_tip.substr(i, postfiks_izraz_tip.length());
        pov = pov.substr(0, pov.length() - 1);
       
        if (lista_argumenata_tipovi.size() != paramsList.size())
            tmp = false;
        list<string>::iterator it1;
        list<string>::iterator it2;
        for (it1 = lista_argumenata_tipovi.begin(),
            it2 = paramsList.begin();
            it1 != lista_argumenata_tipovi.end() && it2 != paramsList.end();
            it1++, it2++) {
                if (!castingRelation[*it1][*it2]) {
                    tmp = false;
                    break;
                }               
        }
        if (!tmp) {
            errorOutput(par, par->children);
            return false;
        }
        tip = pov;
        l_izraz = 0;
    } else if (
        childrenConcat == "<postfiks_izraz> OP_INC" ||
        childrenConcat == "<postfiks_izraz> OP_DEC") {
        Node* par = current;
        vector<Node*> v;
        list<Node*>::iterator it = current->children.begin();
        while(it != current->children.end()) {
            v.push_back(*it);
            it++;
        }
        current = v[0];
        string postfiks_izraz_tip;
        bool postfiks_izraz_l_izraz;
        int bl;
        if(!postfiks_izraz(postfiks_izraz_tip, postfiks_izraz_l_izraz, bl)) {
            return false;
        }
        isNIZ_ZNAKOVA = -1;
        if (!postfiks_izraz_l_izraz || 
            (!castingRelation[postfiks_izraz_tip]["int"])) {
            errorOutput(par, par->children);
            return false;
        }
        tip = "int";
        l_izraz = false;
    } else{
        cerr << "SINTAKSNO STABLO NE VALJA\n";
    }
    return true;
}

bool lista_argumenata(list<string>& tipovi) {
    cerr << "lista_argumenata\n";
    string childrenConcat = "";
    for (Node* child : current->children) {
        stringstream ssName = stringstream(child->name);
        string tmp;
        ssName >> tmp;
        childrenConcat += tmp;
        childrenConcat += " ";
    }
    childrenConcat = trim(childrenConcat);
    if (childrenConcat == "<izraz_pridruzivanja>") {
        current = *(current->children.begin());
        string izraz_pridruzivanja_tip;
        bool izraz_pridruzivanja_l_izraz;
        int bl; 
        if (!izraz_pridruzivanja(izraz_pridruzivanja_tip, izraz_pridruzivanja_l_izraz, bl)) {
            return false;
        }
        tipovi.clear();
        tipovi.push_back(izraz_pridruzivanja_tip);
    } else if (childrenConcat == "<lista_argumenata> ZAREZ <izraz_pridruzivanja>") {
        Node* par = current;
        vector<Node*> v;
        list<Node*>::iterator it = current->children.begin();
        while(it != current->children.end()) {
            v.push_back(*it);
            it++;
        }
        current = v[0];
        list<string> lista_argumenata_tipovi;
        if (!lista_argumenata(lista_argumenata_tipovi)) {
            return false;
        }
        current = v[2];
        string izraz_pridruzivanja_tip;
        bool izraz_pridruzivanja_l_izraz;
        int bl;
        if (!izraz_pridruzivanja(izraz_pridruzivanja_tip, izraz_pridruzivanja_l_izraz, bl)) {
            return false;
        }
        tipovi = lista_argumenata_tipovi;
        tipovi.push_back(izraz_pridruzivanja_tip);

        /*for (string str : tipovi)
            cerr << str << " ";
        cerr << "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAa\n";*/
    } else {
    
        cerr << "SINTAKSNO STABLO NE VALJA\n";
    }
    return true;
}
bool unarni_izraz(string& tip, bool& l_izraz, int& isNIZ_ZNAKOVA) {
    cerr << "unarni_izraz\n";
    Node* par = current; 
    string childrenConcat = "";
    for (Node* child : current->children) {
        stringstream ssName = stringstream(child->name);
        string tmp;
        ssName >> tmp;
        childrenConcat += tmp;
        childrenConcat += " ";
    }
    childrenConcat = trim(childrenConcat);
    if (childrenConcat == "<postfiks_izraz>") {
        current = *(current->children.begin());
        string postfiks_izraz_tip;
        bool postfiks_izraz_l_izraz;
        int bl;
        if (!postfiks_izraz(postfiks_izraz_tip, postfiks_izraz_l_izraz, bl)) {
            return false;
        }
        isNIZ_ZNAKOVA = bl;
        tip = postfiks_izraz_tip;
        l_izraz = postfiks_izraz_l_izraz;
    } else if(childrenConcat == "OP_INC <unarni_izraz>" || childrenConcat == "OP_DEC <unarni_izraz>") {

        vector<Node*> v;
        list<Node*>::iterator it = current->children.begin();
        while(it != current->children.end()) {
            v.push_back(*it);
            it++;
        }
        current = v[1];
        string unarni_izraz_tip;
        bool unarni_izraz_l_izraz;
        int bl;
        if (!unarni_izraz(unarni_izraz_tip, unarni_izraz_l_izraz, bl)) {
            return false;
        }
        isNIZ_ZNAKOVA = -1;
        if (!unarni_izraz_l_izraz || !castingRelation[unarni_izraz_tip]["int"]) {
            //ISPIS GREŠKE
            errorOutput(par, par->children);
            return false;
        }
        tip = "int";
        l_izraz = false;
    } else if (childrenConcat == "<unarni_operator> <cast_izraz>") {

        vector<Node*> v;
        list<Node*>::iterator it = current->children.begin();
        while(it != current->children.end()) {
            v.push_back(*it);
            it++;
        }
        current = v[1];
        string cast_izraz_tip;
        bool cast_izraz_l_izraz;
        int bl;
        if (!cast_izraz(cast_izraz_tip, cast_izraz_l_izraz, bl)) {
            return false;
        }
        isNIZ_ZNAKOVA = -1;
        if(!castingRelation[cast_izraz_tip]["int"]) {
            //ISPIS GREŠKE
            errorOutput(par, par->children);
            return false;
        }
        tip = "int";
        l_izraz = false;
    } else {

        cerr << "SINTAKSNO STABLO NE VALJA\n";
    }
    return true;
}
bool unarni_operator() {
    //hmm
    return true;
}
bool cast_izraz(string& tip, bool& l_izraz, int& isNIZ_ZNAKOVA) {
    cerr << "cast_izaz\n";
    Node* par = current;
    string childrenConcat = "";
    for (Node* child : current->children) {
        stringstream ssName = stringstream(child->name);
        string tmp;
        ssName >> tmp;
        childrenConcat += tmp;
        childrenConcat += " ";
    }
    childrenConcat = trim(childrenConcat);
    if (childrenConcat == "<unarni_izraz>") {
        current = *(current->children.begin());
        string unarni_izraz_tip;
        bool unarni_izraz_l_izraz;
        int bl;
        if (!unarni_izraz(unarni_izraz_tip, unarni_izraz_l_izraz, bl)) {
            return false;
        }
        isNIZ_ZNAKOVA = bl;
        tip = unarni_izraz_tip;
        l_izraz = unarni_izraz_l_izraz;
    } else if (childrenConcat == "L_ZAGRADA <ime_tipa> D_ZAGRADA <cast_izraz>") {

        vector<Node*> v;
        list<Node*>::iterator it = current->children.begin();
        while(it != current->children.end()) {
            v.push_back(*it);
            it++;
        }
        current = v[1];
        string ime_tipa_tip;
        if (!ime_tipa(ime_tipa_tip)) {
            return false;
        }
        current = v[3];
        string cast_izraz_tip;
        bool cast_izraz_l_izraz;
        int bl;
        if (!cast_izraz(cast_izraz_tip, cast_izraz_l_izraz, bl)) {
            return false;
        }
        isNIZ_ZNAKOVA = -1;
        //cerr << cast_izraz_tip << " " << ime_tipa_tip << "\n"; // mogu li pretvoriti const(int) u char ili const(char)- izgleda da da
        if (!castingRelation[cast_izraz_tip][ime_tipa_tip] && !((cast_izraz_tip == "int" || cast_izraz_tip == "const(int)") && (ime_tipa_tip == "char" || ime_tipa_tip == "const(char)"))) {
            //ISPIS GREŠKE 
            errorOutput(par, par->children);
            return false;
        }
        tip = ime_tipa_tip;
        l_izraz = false;
    } else {
        
        cerr << "SINTAKSNO STABLO NE VALJA\n";
    }
    return true;
}

bool ime_tipa(string& tip){
    cerr << "ime_tipa\n";
    Node* par = current;
    string childrenConcat = "";
    for (Node* child : current->children) {
        stringstream ssName = stringstream(child->name);
        string tmp;
        ssName >> tmp;
        childrenConcat += tmp;
        childrenConcat += " ";
    }
    childrenConcat = trim(childrenConcat); 
    if (childrenConcat == "<specifikator_tipa>") {
        current = *(current->children.begin());
        string specifikator_tipa_tip;
        if (!specifikator_tipa(specifikator_tipa_tip)) {
            return false;
        }
        tip = specifikator_tipa_tip;
    } else if (childrenConcat == "KR_CONST <specifikator_tipa>") {
        vector<Node*> v;
        list<Node*>::iterator it = current->children.begin();
        while(it != current->children.end()) {
            v.push_back(*it);
            it++;
        }
        current = v[1];
        string specifikator_tipa_tip;
        if (!specifikator_tipa(specifikator_tipa_tip)) {
            return false;
        }
        if (specifikator_tipa_tip == "void") {
            //ISPIS GREŠKE
            errorOutput(par, par->children);
            return false;
        }
        tip = "const(";
        tip += specifikator_tipa_tip;
        tip += ")"; 
    } else {

        cerr << "SINTAKSNO STABLO NE VALJA\n";
    }
    return true;
}
bool specifikator_tipa(string& tip) {
    cerr << "specifikator_tipa\n";
    string childrenConcat = "";
    for (Node* child : current->children) {
        stringstream ssName = stringstream(child->name);
        string tmp;
        ssName >> tmp;
        childrenConcat += tmp;
        childrenConcat += " ";
    }
    childrenConcat = trim(childrenConcat);
    if (childrenConcat == "KR_VOID") {
        tip = "void";
    } else if (childrenConcat == "KR_CHAR") {
        tip = "char";
    } else if (childrenConcat == "KR_INT") {
        tip = "int";
    } else {

        cerr << "SINTAKSNO STABLO NE VALJA\n";
    }
    return true;
}
bool multiplikativni_izraz(string& tip, bool& l_izraz, int& isNIZ_ZNAKOVA) {
    cerr << "multiplikativni_izraz\n";
    Node* par = current;
    string childrenConcat = "";
    for (Node* child : current->children) {
        stringstream ssName = stringstream(child->name);
        string tmp;
        ssName >> tmp;
        childrenConcat += tmp;
        childrenConcat += " ";
    }
    childrenConcat = trim(childrenConcat);
    if (childrenConcat == "<cast_izraz>") {
        current = *(current->children.begin());
        string cast_izraz_tip;
        bool cast_izraz_l_izraz;
        int bl;
        if (!cast_izraz(cast_izraz_tip, cast_izraz_l_izraz, bl)) {
            return false;
        }
        isNIZ_ZNAKOVA = bl;
        tip = cast_izraz_tip;
        l_izraz = cast_izraz_l_izraz;
    } else if (
        childrenConcat == "<multiplikativni_izraz> OP_PUTA <cast_izraz>" ||
        childrenConcat == "<multiplikativni_izraz> OP_DIJELI <cast_izraz>" ||
        childrenConcat == "<multiplikativni_izraz> OP_MOD <cast_izraz>") {

        vector<Node*> v;
        list<Node*>::iterator it = current->children.begin();
        while(it != current->children.end()) {
            v.push_back(*it);
            it++;
        }
        current = v[0];
        string multiplikativni_izraz_tip;
        bool multiplikativni_izraz_l_izraz;
        int bl;
        if (!multiplikativni_izraz(multiplikativni_izraz_tip, multiplikativni_izraz_l_izraz, bl)) {
            return false;
        }
        isNIZ_ZNAKOVA = -1;
        if (!castingRelation[multiplikativni_izraz_tip]["int"]) {
            //ISPIS GREŠKE
            errorOutput(par, par->children);
            return false;
        }
        current = v[2];
        string cast_izraz_tip;
        bool cast_izraz_l_izraz;
        int bl1;
        if (!cast_izraz(cast_izraz_tip, cast_izraz_l_izraz, bl1)) {
            return false;
        }
        isNIZ_ZNAKOVA = -1;
        if (!castingRelation[cast_izraz_tip]["int"]) {
            errorOutput(par, par->children);
            return false;
        }
        tip = "int";
        l_izraz = false;
    } else {

        cerr << "SINTAKSNO STABLO NE VALJA\n";
    }
    return true;
}
bool aditivni_izraz(string& tip, bool& l_izraz, int& isNIZ_ZNAKOVA) {
    cerr << "aditivni_izraz\n";
    Node* par = current;
    string childrenConcat = "";
    for (Node* child : current->children) {
        stringstream ssName = stringstream(child->name);
        string tmp;
        ssName >> tmp;
        childrenConcat += tmp;
        childrenConcat += " ";
    }
    childrenConcat = trim(childrenConcat);
    if (childrenConcat == "<multiplikativni_izraz>") {
        current = *(current->children.begin());
        string multiplikativni_izraz_tip;
        bool multiplikativni_izraz_l_izraz;
        int bl;
        if (!multiplikativni_izraz(multiplikativni_izraz_tip, multiplikativni_izraz_l_izraz, bl)) {
            return false;
        }
        isNIZ_ZNAKOVA = bl;
        tip = multiplikativni_izraz_tip;
        l_izraz = multiplikativni_izraz_l_izraz;
    } else if (childrenConcat == "<aditivni_izraz> PLUS <multiplikativni_izraz>" ||
        childrenConcat == "<aditivni_izraz> MINUS <multiplikativni_izraz>") {
        vector<Node*> v;
        list<Node*>::iterator it = current->children.begin();
        while(it != current->children.end()) {
            v.push_back(*it);
            it++;
        }
        current = v[0];
        string aditivni_izraz_tip;
        bool aditivni_izraz_l_izraz;
        int bl;
        if (!aditivni_izraz(aditivni_izraz_tip, aditivni_izraz_l_izraz, bl)) {
            return false;
        }
        isNIZ_ZNAKOVA = -1;
        if (!castingRelation[aditivni_izraz_tip]["int"]) {
            //ISPIS GREŠKE
            errorOutput(par, par->children);
            return false;
        }
        current = v[2];
        string multiplikativni_izraz_tip;
        bool multiplikativni_izraz_l_izraz;
        int bl1;
        if (!multiplikativni_izraz(multiplikativni_izraz_tip, multiplikativni_izraz_l_izraz, bl1)) {
            return false;
        }
        isNIZ_ZNAKOVA = -1;
        if (!castingRelation[multiplikativni_izraz_tip]["int"]) {
            //ISPIS GREŠKE
            errorOutput(par, par->children);
            return false;
        }
        tip = "int";
        l_izraz = false;
    } else {

        cerr << "SINTAKSNO STABLO NE VALJA\n";
    }
    return true;
}
bool odnosni_izraz(string& tip, bool& l_izraz, int& isNIZ_ZNAKOVA) {
    cerr << "odnosni_izraz\n";
    Node* par = current;
    string childrenConcat = "";
    for (Node* child : current->children) {
        stringstream ssName = stringstream(child->name);
        string tmp;
        ssName >> tmp;
        childrenConcat += tmp;
        childrenConcat += " ";
    }
    childrenConcat = trim(childrenConcat);
    if (childrenConcat == "<aditivni_izraz>") {
        current = *(current->children.begin());
        string aditivni_izraz_tip;
        bool aditivni_izraz_l_izraz;
        int bl;
        if (!aditivni_izraz(aditivni_izraz_tip, aditivni_izraz_l_izraz, bl)) {
            return false;
        }
        isNIZ_ZNAKOVA = bl;
        tip = aditivni_izraz_tip;
        l_izraz = aditivni_izraz_l_izraz; 
    } else if (
        childrenConcat == "<odnosni_izraz> OP_LT <aditivni_izraz>" ||
        childrenConcat == "<odnosni_izraz> OP_GT <aditivni_izraz>" ||
        childrenConcat == "<odnosni_izraz> OP_LTE <aditivni_izraz>" ||
        childrenConcat == "<odnosni_izraz> OP_GTE <aditivni_izraz>") {

        vector<Node*> v;
        list<Node*>::iterator it = current->children.begin();
        while(it != current->children.end()) {
            v.push_back(*it);
            it++;
        }

        current = v[0];
        string odnosni_izraz_tip;
        bool odnosni_izraz_l_izraz;
        int bl;
        if (!odnosni_izraz(odnosni_izraz_tip, odnosni_izraz_l_izraz, bl)) {
            return false;
        }
        isNIZ_ZNAKOVA = -1;
        //cerr << odnosni_izraz_tip << "\n";
        if (!castingRelation[odnosni_izraz_tip]["int"]) {
            //ISPIS GREŠKE
            errorOutput(par, par->children);
            return false;
        }
        current = v[2];
        string aditivni_izraz_tip;
        bool aditivni_izraz_l_izraz;
        int bl1;
        if (!aditivni_izraz(aditivni_izraz_tip, aditivni_izraz_l_izraz, bl1)) {
            return false;
        }
        isNIZ_ZNAKOVA = -1;
        if (!castingRelation[aditivni_izraz_tip]["int"]) {
            //ISPIS GREŠKE
            errorOutput(par, par->children);
            return false;
        }
        tip = "int";
        l_izraz = false;
    } else {

        cerr << "SINTAKSNO STABLO NE VALJA\n";
    }
    return true;
}

bool jednakosni_izraz(string& tip, bool& l_izraz, int& isNIZ_ZNAKOVA) {
    cerr << "jednakosni_izraz\n";
    Node* par = current;
    string childrenConcat = "";
    for (Node* child : current->children) {
        stringstream ssName = stringstream(child->name);
        string tmp;
        ssName >> tmp;
        childrenConcat += tmp;
        childrenConcat += " ";
    }
    childrenConcat = trim(childrenConcat);
    if (childrenConcat == "<odnosni_izraz>") {
        current = *(current->children.begin());
        string odnosni_izraz_tip;
        bool odnosni_izraz_l_izraz;
        int bl;
        if (!odnosni_izraz(odnosni_izraz_tip, odnosni_izraz_l_izraz, bl)) {
            return false;
        }
        isNIZ_ZNAKOVA = bl;
        tip = odnosni_izraz_tip;
        l_izraz = odnosni_izraz_l_izraz;
    } else if (
        childrenConcat == "<jednakosni_izraz> OP_EQ <odnosni_izraz>" || 
        childrenConcat == "<jednakosni_izraz> OP_NEQ <odnosni_izraz>") {
        vector<Node*> v;
        list<Node*>::iterator it = current->children.begin();
        while(it != current->children.end()) {
            v.push_back(*it);
            it++;
        }
        current = v[0];
        string jednakosni_izraz_tip;
        bool jednakosni_izraz_l_izraz;
        int bl;
        if (!jednakosni_izraz(jednakosni_izraz_tip, jednakosni_izraz_l_izraz, bl)) {
            return false;
        }
        isNIZ_ZNAKOVA = -1;
        if (!castingRelation[jednakosni_izraz_tip]["int"]) {
            //ISPIS GREŠKE
            errorOutput(par, par->children);
            return false;
        }
        current = v[2];
        string odnosni_izraz_tip;
        bool odnosni_izraz_l_izraz;
        int bl1;
        if (!odnosni_izraz(odnosni_izraz_tip, odnosni_izraz_l_izraz, bl1)) {
            return false;
        }
        isNIZ_ZNAKOVA = -1;
        if (!castingRelation[odnosni_izraz_tip]["int"]) {
            //ISPIS GREŠKE
            errorOutput(par, par->children);
            return false;
        }
        tip = "int";
        l_izraz = false;
    } else {

        cerr << "SINTAKSNO STABLO NE VALJA\n";
    }
    return true;
}
bool bin_i_izraz(string& tip, bool& l_izraz, int& isNIZ_ZNAKOVA) {
    cerr << "bin_i_izraz\n";
    Node* par = current; 
    string childrenConcat = "";
    for (Node* child : current->children) {
        stringstream ssName = stringstream(child->name);
        string tmp;
        ssName >> tmp;
        childrenConcat += tmp;
        childrenConcat += " ";
    }
    childrenConcat = trim(childrenConcat);
    if (childrenConcat == "<jednakosni_izraz>") {
        current = *(current->children.begin());
        string jednakosni_izraz_tip;
        bool jednakosni_izraz_l_izraz;
        int bl;
        if (!jednakosni_izraz(jednakosni_izraz_tip, jednakosni_izraz_l_izraz, bl)) {
            return false;
        }
        isNIZ_ZNAKOVA = bl;
        tip = jednakosni_izraz_tip;
        l_izraz = jednakosni_izraz_l_izraz;
    } else if (childrenConcat == "<bin_i_izraz> OP_BIN_I <jednakosni_izraz>") {
        vector<Node*> v;
        list<Node*>::iterator it = current->children.begin();
        while(it != current->children.end()) {
            v.push_back(*it);
            it++;
        }
        current = v[0];
        string bin_i_izraz_tip;
        bool bin_i_izraz_l_izraz;
        int bl;
        if (!bin_i_izraz(bin_i_izraz_tip, bin_i_izraz_l_izraz, bl)) {
            return false;
        }
        isNIZ_ZNAKOVA = -1;
        if (!castingRelation[bin_i_izraz_tip]["int"]) {
            //ISPIŠI GREŠKU
            errorOutput(par, par->children);
            return false;
        }
        current = v[2];
        string jednakosni_izraz_tip;
        bool jednakosni_izraz_l_izraz;
        int bl1;
        if (!jednakosni_izraz(jednakosni_izraz_tip, jednakosni_izraz_l_izraz, bl1)) {
            return false;
        }
        isNIZ_ZNAKOVA = -1;
        if (!castingRelation[jednakosni_izraz_tip]["int"]) {
            //ISPIS GREŠKE
            errorOutput(par, par->children);
            return false;
        }
        tip = "int";
        l_izraz = false;
    } else {

        cerr << "SINTAKSNO STABLO NE VALJA\n";
    }
    return true;
}
bool bin_xili_izraz(string& tip, bool& l_izraz, int& isNIZ_ZNAKOVA) {
    cerr << "bin_xili_izraz\n";
    Node* par = current; 
    string childrenConcat = "";
    for (Node* child : current->children) {
        stringstream ssName = stringstream(child->name);
        string tmp;
        ssName >> tmp;
        childrenConcat += tmp;
        childrenConcat += " ";
    }
    childrenConcat = trim(childrenConcat);
    if (childrenConcat == "<bin_i_izraz>") {
        current = *(current->children.begin());
        string bin_i_izraz_tip;
        bool bin_i_izraz_l_izraz;
        int bl;
        if (!bin_i_izraz(bin_i_izraz_tip, bin_i_izraz_l_izraz, bl)) {
            return false;
        }
        isNIZ_ZNAKOVA = bl;
        tip = bin_i_izraz_tip;
        l_izraz = bin_i_izraz_l_izraz;
    } else if (childrenConcat == "<bin_xili_izraz> OP_BIN_XILI <bin_i_izraz>") {
        vector<Node*> v;
        list<Node*>::iterator it = current->children.begin();
        while(it != current->children.end()) {
            v.push_back(*it);
            it++;
        }
        current = v[0];
        string bin_xili_izraz_tip;
        bool bin_xili_izraz_l_izraz;
        int bl;
        if (!bin_xili_izraz(bin_xili_izraz_tip, bin_xili_izraz_l_izraz, bl)) {
            return false;
        }
        isNIZ_ZNAKOVA = -1;
        if (!castingRelation[bin_xili_izraz_tip]["int"]) {
            //ISPIS GREŠKE
            errorOutput(par, par->children);
            return false;
        }
        current = v[2];
        string bin_i_izraz_tip;
        bool bin_i_izraz_l_izraz;
        int bl1;
        if (!bin_i_izraz(bin_i_izraz_tip, bin_i_izraz_l_izraz, bl1)) {
            return false;
        }
        isNIZ_ZNAKOVA = -1;
        if (!castingRelation[bin_i_izraz_tip]["int"]) {
            //ISPIS GREŠKE
            errorOutput(par, par->children);
            return false;
        }
        tip = "int";
        l_izraz = false;
    } else {

        cerr << "SINTAKSNO STABLO NE VALJA\n";
    }
    return true;
}
bool bin_ili_izraz(string& tip, bool& l_izraz, int& isNIZ_ZNAKOVA) {
    cerr << "bin_ili_izraz\n";
    Node* par = current;
    string childrenConcat = "";
    for (Node* child : current->children) {
        stringstream ssName = stringstream(child->name);
        string tmp;
        ssName >> tmp;
        childrenConcat += tmp;
        childrenConcat += " ";
    }
    childrenConcat = trim(childrenConcat);
    if (childrenConcat == "<bin_xili_izraz>") {
        current = *(current->children.begin());
        string bin_xili_izraz_tip;
        bool bin_xili_izraz_l_izraz;
        int bl;
        if (!bin_xili_izraz(bin_xili_izraz_tip, bin_xili_izraz_l_izraz, bl)) {
            return false;
        }
        isNIZ_ZNAKOVA = bl;
        tip = bin_xili_izraz_tip;
        l_izraz = bin_xili_izraz_l_izraz;
    } else if (childrenConcat == "<bin_ili_izraz> OP_BIN_ILI <bin_xili_izraz>") {
        vector<Node*> v;
        list<Node*>::iterator it = current->children.begin();
        while(it != current->children.end()) {
            v.push_back(*it);
            it++;
        }
        current = v[0];
        string bin_ili_izraz_tip;
        bool bin_ili_izraz_l_izraz;
        int bl;
        if (!bin_ili_izraz(bin_ili_izraz_tip, bin_ili_izraz_l_izraz, bl)) {
            return false;
        }
        isNIZ_ZNAKOVA = -1;
        if (!castingRelation[bin_ili_izraz_tip]["int"]) {
            //ISPIS GREŠKE
            errorOutput(par, par->children);
            return false;
        }
        current = v[2];
        string bin_xili_izraz_tip;
        bool bin_xili_izraz_l_izraz;
        int bl1;
        if (!bin_xili_izraz(bin_xili_izraz_tip, bin_xili_izraz_l_izraz, bl1)) {
            return false;
        }
        isNIZ_ZNAKOVA = -1;
        if (!castingRelation[bin_xili_izraz_tip]["int"]) {
            //ISPIS GREŠKE
            errorOutput(par, par->children);
            return false;
        }
        tip = "int";
        l_izraz = false;
    } else {

        cerr << "SINTAKSNO STABLO NE VALJA\n";
    }
    return true;
}
bool log_i_izraz(string& tip, bool& l_izraz, int& isNIZ_ZNAKOVA) {
    cerr << "log_i_izraz\n";
    Node* par = current;
    string childrenConcat = "";
    for (Node* child : current->children) {
        stringstream ssName = stringstream(child->name);
        string tmp;
        ssName >> tmp;
        childrenConcat += tmp;
        childrenConcat += " ";
    }
    childrenConcat = trim(childrenConcat);
    if (childrenConcat == "<bin_ili_izraz>") {
        current = *(current->children.begin());
        string bin_ili_izraz_tip;
        bool bin_ili_izraz_l_izraz;
        int bl;
        if (!bin_ili_izraz(bin_ili_izraz_tip, bin_ili_izraz_l_izraz, bl)) {
            return false;
        }
        isNIZ_ZNAKOVA = bl;
        tip = bin_ili_izraz_tip;
        l_izraz = bin_ili_izraz_l_izraz;
    } else if (childrenConcat == "<log_i_izraz> OP_I <bin_ili_izraz>") {
        vector<Node*> v;
        list<Node*>::iterator it = current->children.begin();
        while(it != current->children.end()) {
            v.push_back(*it);
            it++;
        }
        current = v[0];
        string log_i_izraz_tip;
        bool log_i_izraz_l_izraz;
        int bl;
        if (!log_i_izraz(log_i_izraz_tip, log_i_izraz_l_izraz, bl)) {
            return false;
        }
        isNIZ_ZNAKOVA = -1;
        if (!castingRelation[log_i_izraz_tip]["int"]) {
            //ISPIS GREŠKE
            errorOutput(par, par->children);
            return false;
        }
        current = v[2];
        string bin_ili_izraz_tip;
        bool bin_ili_izraz_l_izraz;
        int bl1;
        if (!bin_ili_izraz(bin_ili_izraz_tip, bin_ili_izraz_l_izraz, bl1)) {
            return false;
        }
        isNIZ_ZNAKOVA = -1;
        if (!castingRelation[bin_ili_izraz_tip]["int"]) {
            //ISPIS GREŠKE
            errorOutput(par, par->children);
            return false;
        }
        tip = "int";
        l_izraz = false;
    } else {

        cerr << "SINTAKSNO STABLO NE VALJA\n";
    }
    return true;
}
bool log_ili_izraz(string& tip, bool& l_izraz, int& isNIZ_ZNAKOVA) {
    cerr << "log_ili_izraz\n";
    Node* par = current;
    string childrenConcat = "";
    for (Node* child : current->children) {
        stringstream ssName = stringstream(child->name);
        string tmp;
        ssName >> tmp;
        childrenConcat += tmp;
        childrenConcat += " ";
    }
    childrenConcat = trim(childrenConcat);
    if (childrenConcat == "<log_i_izraz>") {
        current = *(current->children.begin());
        string log_i_izraz_tip;
        bool log_i_izraz_l_izraz;
        int bl;
        if (!log_i_izraz(log_i_izraz_tip, log_i_izraz_l_izraz, bl)) {
            return false;
        }
        isNIZ_ZNAKOVA = bl;
        tip = log_i_izraz_tip;
        l_izraz = log_i_izraz_l_izraz;
    } else if (childrenConcat == "<log_ili_izraz> OP_ILI <log_i_izraz>") {
        vector<Node*> v;
        list<Node*>::iterator it = current->children.begin();
        while(it != current->children.end()) {
            v.push_back(*it);
            it++;
        }
        current = v[0];
        string log_ili_izraz_tip;
        bool log_ili_izraz_l_izraz;
        int bl;
        if (!log_ili_izraz(log_ili_izraz_tip, log_ili_izraz_l_izraz, bl)) {
            return false;
        }
        isNIZ_ZNAKOVA = -1;
        if (!castingRelation[log_ili_izraz_tip]["int"]) {
            //ISPIS GREŠKE 
            errorOutput(par, par->children);
            return false;
        }
        current = v[2];
        string log_i_izraz_tip;
        bool log_i_izraz_l_izraz;
        int bl1;
        if (!log_i_izraz(log_i_izraz_tip, log_i_izraz_l_izraz, bl1)) {
            return false;
        }
        isNIZ_ZNAKOVA = -1;
        if (!castingRelation[log_i_izraz_tip]["int"]) {
            //ISPIS GREŠKE
            errorOutput(par, par->children);
            return false;
        }
        tip = "int";
        l_izraz = false;
    } else {

        cerr << "SINTAKSNO STABLO NE VALJA\n";
    }
    return true;
}
bool izraz_pridruzivanja(string& tip, bool& l_izraz, int& isNIZ_ZNAKOVA) {
    cerr << "izraz_pridruzivanja\n";
    Node* par = current; 
    string childrenConcat = "";
    for (Node* child : current->children) {
        stringstream ssName = stringstream(child->name);
        string tmp;
        ssName >> tmp;
        childrenConcat += tmp;
        childrenConcat += " ";
    }
    childrenConcat = trim(childrenConcat);
    if (childrenConcat == "<log_ili_izraz>") {
        current = *(current->children.begin());
        string log_ili_izraz_tip;
        bool log_ili_izraz_l_izraz;
        int bl;
        if (!log_ili_izraz(log_ili_izraz_tip, log_ili_izraz_l_izraz, bl)) {
            return false;
        }
        isNIZ_ZNAKOVA = bl;
        tip = log_ili_izraz_tip;
        l_izraz = log_ili_izraz_l_izraz;
    } else if (childrenConcat == "<postfiks_izraz> OP_PRIDRUZI <izraz_pridruzivanja>") {
        vector<Node*> v;
        list<Node*>::iterator it = current->children.begin();
        while(it != current->children.end()) {
            v.push_back(*it);
            it++;
        }
        current = v[0];
        string postfiks_izraz_tip;
        bool postfiks_izraz_l_izraz;
        int bl;
        if (!postfiks_izraz(postfiks_izraz_tip, postfiks_izraz_l_izraz, bl)) {
            return false;
        }
        isNIZ_ZNAKOVA = -1;
        //cerr << postfiks_izraz_l_izraz << "\n";
        if (!postfiks_izraz_l_izraz) {
            //ISPIS GREŠKE
            errorOutput(par, par->children);
            return false;
        }
        current = v[2];
        string izraz_pridruzivanja_tip;
        bool izraz_pridruzivanja_l_izraz;
        int bl1;
        if (!izraz_pridruzivanja(izraz_pridruzivanja_tip, izraz_pridruzivanja_l_izraz, bl1)) {
            return false;
        }
        isNIZ_ZNAKOVA = -1;
        if (!castingRelation[izraz_pridruzivanja_tip][postfiks_izraz_tip]) {
            //ISPIS GREŠKE
            errorOutput(par, par->children);
            return false;
        }
        tip = postfiks_izraz_tip;
        l_izraz = false;
    } else {

        cerr << "SINTAKSNO STABLO NE VALJA\n";
    }
    return true;
}
bool izraz(string& tip, bool& l_izraz) {
    cerr << "izraz\n";
    Node* par = current; 
    string childrenConcat = "";
    for (Node* child : current->children) {
        stringstream ssName = stringstream(child->name);
        string tmp;
        ssName >> tmp;
        childrenConcat += tmp;
        childrenConcat += " ";
    }
    childrenConcat = trim(childrenConcat);
    if (childrenConcat == "<izraz_pridruzivanja>") {
        current = *(current->children.begin());
        string izraz_pridruzivanja_tip;
        bool izraz_pridruzivanja_l_izraz;
        int bl;
        if (!izraz_pridruzivanja(izraz_pridruzivanja_tip, izraz_pridruzivanja_l_izraz, bl)) {
            return false;
        }
        tip = izraz_pridruzivanja_tip;
        l_izraz = izraz_pridruzivanja_l_izraz;
    } else if (childrenConcat == "<izraz> ZAREZ <izraz_pridruzivanja>") {
        vector<Node*> v;
        list<Node*>::iterator it = current->children.begin();
        while(it != current->children.end()) {
            v.push_back(*it);
            it++;
        }
        current = v[0];
        string izraz_tip;
        bool izraz_l_izraz;
        if (!izraz(izraz_tip, izraz_l_izraz)) {
            return false;
        }
        current = v[2];
        string izraz_pridruzivanja_tip;
        bool izraz_pridruzivanja_l_izraz;
        int bl;
        if (!izraz_pridruzivanja(izraz_pridruzivanja_tip, izraz_pridruzivanja_l_izraz, bl)) {
            return false;
        }
        tip = izraz_pridruzivanja_tip;
        l_izraz = false;
    } else {

        cerr << "SINTAKSNO STABLO NE VALJA\n";
    }
    return true;
}

bool slozena_naredba(bool isloop, string context) {
    cerr << "slozena_naredba\n";
    Node* par = current; 
    string childrenConcat = "";
    for (Node* child : current->children) {
        stringstream ssName = stringstream(child->name);
        string tmp;
        ssName >> tmp;
        childrenConcat += tmp;
        childrenConcat += " ";
    }
    childrenConcat = trim(childrenConcat);
    if (childrenConcat == "L_VIT_ZAGRADA <lista_naredbi> D_VIT_ZAGRADA") {
        vector<Node*> v;
        list<Node*>::iterator it = current->children.begin();
        while(it != current->children.end()) {
            v.push_back(*it);
            it++;
        }
        current = v[1];
        if (!lista_naredbi(isloop, context)) {
            return false;
        }
    } else if (childrenConcat == "L_VIT_ZAGRADA <lista_deklaracija> <lista_naredbi> D_VIT_ZAGRADA") {
        vector<Node*> v;
        list<Node*>::iterator it = current->children.begin();
        while(it != current->children.end()) {
            v.push_back(*it);
            it++;
        }
        current = v[1];
        if (!lista_deklaracija()) {
            return false;
        }
        //cerr << "1\n"; 
        current = v[2];
        if (!lista_naredbi(isloop, context)) {
            return false;
        }
    } else {

        cerr << "SINTAKSNO STABLO NE VALJA\n";
    }
    return true;
}
bool lista_naredbi(bool isloop, string context) {
    cerr << "lista_naredbi\n";
    string childrenConcat = "";
    for (Node* child : current->children) {
        stringstream ssName = stringstream(child->name);
        string tmp;
        ssName >> tmp;
        childrenConcat += tmp;
        childrenConcat += " ";
    }
    childrenConcat = trim(childrenConcat);
    if (childrenConcat == "<naredba>") {
        current = *(current->children.begin());
        if (!naredba(isloop, context)) {
            return false;
        }
    } else if (childrenConcat == "<lista_naredbi> <naredba>") {
        vector<Node*> v;
        list<Node*>::iterator it = current->children.begin();
        while(it != current->children.end()) {
            v.push_back(*it);
            it++;
        }
        current = v[0];
        if (!lista_naredbi(isloop, context)) {
            return false;
        }
        current = v[1];
        if(!naredba(isloop, context)) {
            return false;
        }

    } else {

        cerr << "SINTAKSNO STABLO NE VALJA\n";
    }
    return true;
}
bool naredba(bool isloop, string context) {
    cerr << "naredba\n";
    string childrenConcat = "";
    for (Node* child : current->children) {
        stringstream ssName = stringstream(child->name);
        string tmp;
        ssName >> tmp;
        childrenConcat += tmp;
        childrenConcat += " ";
    }
    childrenConcat = trim(childrenConcat);
    if (childrenConcat == "<slozena_naredba>") {
        current = *(current->children.begin());
        if (!slozena_naredba(isloop, context)) {
            return false;
        }
    } else if (childrenConcat == "<izraz_naredba>") {
        current = *(current->children.begin());
        string izraz_naredba_tip;
        if (!izraz_naredba(izraz_naredba_tip)) {
            return false;
        }
    } else if (childrenConcat == "<naredba_grananja>") {
        current = *(current->children.begin());
        if (!naredba_grananja(isloop, context)) {
            return false;
        }
    } else if (childrenConcat == "<naredba_petlje>") {
        current = *(current->children.begin());
        if (!naredba_petlje(isloop, context)) {
            return false;
        }

    } else if (childrenConcat == "<naredba_skoka>") {
        current = *(current->children.begin());
        if (!naredba_skoka(isloop, context)) {
            return false;
        }
    } else {

        cerr << "SINTAKSNO STABLO NE VALJA\n";
    }
    return true;
}
bool izraz_naredba(string& tip) {
    cerr << "izraz_naredba\n";
    string childrenConcat = "";
    for (Node* child : current->children) {
        stringstream ssName = stringstream(child->name);
        string tmp;
        ssName >> tmp;
        childrenConcat += tmp;
        childrenConcat += " ";
    }
    childrenConcat = trim(childrenConcat);
    if (childrenConcat == "TOCKAZAREZ") {
        tip = "int";
    } else if (childrenConcat == "<izraz> TOCKAZAREZ") {
        vector<Node*> v;
        list<Node*>::iterator it = current->children.begin();
        while(it != current->children.end()) {
            v.push_back(*it);
            it++;
        }
        current = v[0];
        string izraz_tip;
        bool izraz_l_izraz;
        if(!izraz(izraz_tip, izraz_l_izraz)) {
            return false;
        }
        tip = izraz_tip;
    } else {

        cerr << "SINTAKSNO STABLO NE VALJA\n";
    }
    return true;
}
bool naredba_grananja(bool isloop, string context) {
    cerr << "naredba_grananja\n";
    Node* par = current;
    string childrenConcat = "";
    for (Node* child : current->children) {
        stringstream ssName = stringstream(child->name);
        string tmp;
        ssName >> tmp;
        childrenConcat += tmp;
        childrenConcat += " ";
    }
    childrenConcat = trim(childrenConcat);
    if (childrenConcat == "KR_IF L_ZAGRADA <izraz> D_ZAGRADA <naredba>") {
        vector<Node*> v;
        list<Node*>::iterator it = current->children.begin();
        while(it != current->children.end()) {
            v.push_back(*it);
            it++;
        }
        current = v[2];
        string izraz_tip;
        bool izraz_l_izraz;
        if (!izraz(izraz_tip, izraz_l_izraz)){
            return false;
        }
        //cerr << izraz_tip << "\n";
        if (!castingRelation[izraz_tip]["int"]) {
            //ISPIS GREŠKA
            errorOutput(par, par->children);
            return false;
        }
        current = v[4];
        if(!naredba(isloop, context)) {
            return false;
        }
    } else if (childrenConcat == "KR_IF L_ZAGRADA <izraz> D_ZAGRADA <naredba> KR_ELSE <naredba>") {
        vector<Node*> v;
        list<Node*>::iterator it = current->children.begin();
        while(it != current->children.end()) {
            v.push_back(*it);
            it++;
        }
        current = v[2];
        string izraz_tip;
        bool izraz_l_izraz;
        if (!izraz(izraz_tip, izraz_l_izraz)){
            return false;
        }
        if (!castingRelation[izraz_tip]["int"]) {
            //ISPIS GREŠKE
            errorOutput(par, par->children);
            return false;
        }
        current = v[4];
        if (!naredba(isloop, context)) {
            return false;
        }
        current = v[6];
        if(!naredba(isloop, context)) {
            return false;
        }
    } else {

        cerr << "SINTAKSNO STABLO NE VALJA\n";
    }
    return true;
}
bool naredba_petlje(bool isloop, string context) {
    cerr << "naredba_petlje\n";
    Node* par = current;
    string childrenConcat = "";
    for (Node* child : current->children) {
        stringstream ssName = stringstream(child->name);
        string tmp;
        ssName >> tmp;
        childrenConcat += tmp;
        childrenConcat += " ";
    }
    childrenConcat = trim(childrenConcat);
    if (childrenConcat == "KR_WHILE L_ZAGRADA <izraz> D_ZAGRADA <naredba>") {
        vector<Node*> v;
        list<Node*>::iterator it = current->children.begin();
        while(it != current->children.end()) {
            v.push_back(*it);
            it++;
        }
        current = v[2];
        string izraz_tip;
        bool izraz_l_izraz;
        if (!izraz(izraz_tip, izraz_l_izraz)){
            return false;
        }
        if (!castingRelation[izraz_tip]["int"]) {
            //ISPIS GREŠKE
            errorOutput(par, par->children);
            return false;
        }
        current = v[4];
        if (!naredba(true, context)) {
            return false;
        }

    } else if (childrenConcat == "KR_FOR L_ZAGRADA <izraz_naredba> <izraz_naredba> D_ZAGRADA <naredba>") {
        vector<Node*> v;
        list<Node*>::iterator it = current->children.begin();
        while(it != current->children.end()) {
            v.push_back(*it);
            it++;
        }
        current = v[2];
        string izraz_naredba_tip1;
        if (!izraz_naredba(izraz_naredba_tip1)) {
            return false;
        }
        current = v[3];
        string izraz_naredba_tip2;
        if (!izraz_naredba(izraz_naredba_tip2)) {
            return false;
        }
        if (!castingRelation[izraz_naredba_tip2]["int"]) {
            //ISPIS GREŠKE
            errorOutput(par, par->children);
            return false;
        }
        current = v[5];
        if (!naredba(true, context)) {
            return false;
        }
    } else if (childrenConcat == "KR_FOR L_ZAGRADA <izraz_naredba> <izraz_naredba> <izraz> D_ZAGRADA <naredba>"){
        vector<Node*> v;
        list<Node*>::iterator it = current->children.begin();
        while(it != current->children.end()) {
            v.push_back(*it);
            it++;
        }
        current = v[2];
        string izraz_naredba_tip1;
        if(!izraz_naredba(izraz_naredba_tip1)) {
            return false;
        }
        current = v[3];
        string izraz_naredba_tip2;
        if(!izraz_naredba(izraz_naredba_tip2)) {
            return false;
        }
        if (!castingRelation[izraz_naredba_tip2]["int"]) {
            //ISPIS GREŠKE
            errorOutput(par, par->children);
            return false;
        }
        current = v[4];
        string izraz_tip;
        bool izraz_l_izraz;
        if (!izraz(izraz_tip, izraz_l_izraz)) {
            return false;
        }
        current = v[6];
        if (!naredba(true, context)) {
            return false;
        }
    } else {

        cerr << "SINTAKSNO STABLO NE VALJA\n";
    }
    return true;
}
bool naredba_skoka(bool isloop, string context) {
    cerr << "naredba_skoka\n";
    Node* par = current;
    string childrenConcat = "";
    for (Node* child : current->children) {
        stringstream ssName = stringstream(child->name);
        string tmp;
        ssName >> tmp;
        childrenConcat += tmp;
        childrenConcat += " ";
    }
    childrenConcat = trim(childrenConcat);
    if (childrenConcat == "KR_CONTINUE TOCKAZAREZ" || childrenConcat == "KR_BREAK TOCKAZAREZ") {
        vector<Node*> v;
        list<Node*>::iterator it = current->children.begin();
        while(it != current->children.end()) {
            v.push_back(*it);
            it++;
        }
        if (!isloop) {
            //ISPIS GREŠKE
            errorOutput(par, par->children);
            return false;
        }
    } else if (childrenConcat == "KR_RETURN TOCKAZAREZ") {
        vector<Node*> v;
        list<Node*>::iterator it = current->children.begin();
        while(it != current->children.end()) {
            v.push_back(*it);
            it++;
        }
        if (context != "void") {
            //ISPIS GREŠKE
            errorOutput(par, par->children);
            return false;
        }
    } else if (childrenConcat == "KR_RETURN <izraz> TOCKAZAREZ") {
        vector<Node*> v;
        list<Node*>::iterator it = current->children.begin();
        while(it != current->children.end()) {
            v.push_back(*it);
            it++;
        }
        current = v[1];
        string izraz_tip;
        bool izraz_l_izraz;
        if (!izraz(izraz_tip, izraz_l_izraz)) {
            return false;
        }
        string pov = context;
        if (context == "void" || !castingRelation[izraz_tip][pov]) {
            //ISPIS GREŠKE
            errorOutput(par, par->children);
            return false;
        }
    } else {
        cerr << "SINTAKSNO STABLO NE VALJA\n";
    }
    return true;
}
bool prijevodna_jedinica() {
    cerr << "prijevodna_jedinica\n";
    Node* par = current;
    string childrenConcat = "";
    for (Node* child : current->children) {
        stringstream ssName = stringstream(child->name);
        string tmp;
        ssName >> tmp;
        childrenConcat += tmp;
        childrenConcat += " ";
    }
    childrenConcat = trim(childrenConcat);
    if (childrenConcat == "<vanjska_deklaracija>") {
        current = *(current->children.begin());
        if (!vanjska_deklaracija()) {
            return false;
        }
    } else if (childrenConcat == "<prijevodna_jedinica> <vanjska_deklaracija>") {
        vector<Node*> v;
        list<Node*>::iterator it = current->children.begin();
        while(it != current->children.end()) {
            v.push_back(*it);
            it++;
        }
        current = v[0];
        if (!prijevodna_jedinica()) {
            return false;
        }
        current = v[1];
        if (!vanjska_deklaracija()) {
            return false;
        }
    } else {

        cerr << "SINTAKSNO STABLO NE VALJA\n";
    }
    return true;
}
bool vanjska_deklaracija() {
    cerr << "vanjska_deklaracija\n";
    string childrenConcat = "";
    for (Node* child : current->children) {
        stringstream ssName = stringstream(child->name);
        string tmp;
        ssName >> tmp;
        childrenConcat += tmp;
        childrenConcat += " ";
    }
    childrenConcat = trim(childrenConcat);
    if (childrenConcat == "<definicija_funkcije>") {
        current = *(current->children.begin());
        if(!definicija_funkcije()) {
            return false;
        }
    } else if (childrenConcat == "<deklaracija>") {
        current = *(current->children.begin());
        if(!deklaracija()) {
            return false;
        }
    } else {

        cerr << "SINTAKSNO STABLO NE VALJA\n";
    }
    return true;
}
bool definicija_funkcije() {
    cerr << "definicija_funkcije\n";
    Node* par = current;
    string childrenConcat = "";
    for (Node* child : current->children) {
        stringstream ssName = stringstream(child->name);
        string tmp;
        ssName >> tmp;
        childrenConcat += tmp;
        childrenConcat += " ";
    }
    childrenConcat = trim(childrenConcat);
    if (childrenConcat == "<ime_tipa> IDN L_ZAGRADA KR_VOID D_ZAGRADA <slozena_naredba>") {
        vector<Node*> v;
        list<Node*>::iterator it = current->children.begin();
        while(it != current->children.end()) {
            v.push_back(*it);
            it++;
        }
        Node* nodeIDN = v[1];
        stringstream ssName(v[1]->name);
        string tokenName;
        string lineNumber;
        string identifierName;
        ssName >> tokenName;
        ssName >> lineNumber;
        ssName >> identifierName;

        string fooName = identifierName;
        current = v[0];
        string ime_tipa_tip;
        if (!ime_tipa(ime_tipa_tip)){
            return false;
        }
        if (ime_tipa_tip != "char" && ime_tipa_tip != "int" && ime_tipa_tip != "void") {
            //ISPIS GREŠKE
            errorOutput(par, par->children);
            return false;
        }
        if (isDefined(fooName) != "void") {
            //ISPIS GREŠKE
            errorOutput(par, par->children);
            return false;
        }
        string type = "funkcija(void->";
        type += ime_tipa_tip;
        type += ")";
        if (isDeclared(root, fooName) != "void" && isDeclared(root,fooName) != type) {
            //ISPIS GREŠKE
            errorOutput(par, par->children);
            return false;
        }

        define(fooName, type);
        declare(root, fooName, type);
        current = v[5];
        if (!slozena_naredba(false, ime_tipa_tip)){
            return false;
        }
    } else if (childrenConcat == "<ime_tipa> IDN L_ZAGRADA <lista_parametara> D_ZAGRADA <slozena_naredba>") {
        vector<Node*> v;
        list<Node*>::iterator it = current->children.begin();
        while(it != current->children.end()) {
            v.push_back(*it);
            it++;
        }
        Node* nodeIDN = v[1];
        stringstream ssName(nodeIDN->name);
        string tokenName;
        string lineNumber;
        string identifierName;
        ssName >> tokenName;
        ssName >> lineNumber;
        ssName >> identifierName;

        string fooName = identifierName;
        current = v[0];
        string ime_tipa_tip;
    
        if (!ime_tipa(ime_tipa_tip)){
            return false;
        }

        if (ime_tipa_tip != "char" && ime_tipa_tip != "int" && ime_tipa_tip != "void") {
            //ISPIS GREŠKE
            errorOutput(par, par->children);
            return false;
        }
        if (isDefined(fooName) != "void") {
            //ISPIS GREŠKE
            errorOutput(par, par->children);
            return false;
        }
        current = v[3];
        list<string> lista_parametara_tipovi;
        list<string> lista_parametara_imena;
        if(!lista_parametara(lista_parametara_tipovi, lista_parametara_imena)) {
            return false;
        }
        string type = "funkcija(";
        for (string param: lista_parametara_tipovi) {
            type += param;
            type += ",";
        }
        type = type.substr(0, type.length());
        type = type.substr(0, type.length() - 1);
        type += "->";
        type += ime_tipa_tip;
        type += ")";
        if (isDeclared(root, fooName) != "void" && isDeclared(root, fooName) != type) {
            //ISPIS GREŠKE
            errorOutput(par, par->children);
            return false;
        }

        define(fooName, type);
        declare(root, fooName, type);
        //NADAM SE DA JE OSIGURANO DA SU LISTA IMENA I LISTA TIPOVA JEDNAKO DUGE
        list<string>::iterator itTipovi;
        list<string>::iterator itImena;
        for (itTipovi = lista_parametara_tipovi.begin(), itImena = lista_parametara_imena.begin();
            itImena != lista_parametara_imena.end() && itTipovi != lista_parametara_tipovi.end(); 
            itTipovi++, itImena++) {
                declare(v[5], *itImena, *itTipovi);
        }
        current = v[5];
        if(!slozena_naredba(false, ime_tipa_tip)) {
            return false;
        }

    } else {

        cerr << "SINTAKSNO STABLO NE VALJA\n";
    }
    return true;
}
bool lista_parametara(list<string>& tipovi, list<string>& imena) {
    cerr << "lista_parametara\n";
    Node* par = current; 
    string childrenConcat = "";
    for (Node* child : current->children) {
        stringstream ssName = stringstream(child->name);
        string tmp;
        ssName >> tmp;
        childrenConcat += tmp;
        childrenConcat += " ";
    }
    childrenConcat = trim(childrenConcat);
    if (childrenConcat == "<deklaracija_parametra>") {
        current = *(current->children.begin()); 
        string deklaracija_parametra_tip;
        string deklaracija_parametra_ime;
        if (!deklaracija_parametra(deklaracija_parametra_tip, deklaracija_parametra_ime)) {
            return false;
        }
        tipovi.clear();
        tipovi.push_back(deklaracija_parametra_tip);
        imena.push_back(deklaracija_parametra_ime);

    } else if (childrenConcat == "<lista_parametara> ZAREZ <deklaracija_parametra>") {
        vector<Node*> v;
        list<Node*>::iterator it = current->children.begin();
        while(it != current->children.end()) {
            v.push_back(*it);
            it++;
        }
        current = v[0];
        list<string> lista_parametara_tipovi;
        list<string> lista_parametara_imena;
        if (!lista_parametara(lista_parametara_tipovi, lista_parametara_imena)) {
            return false;
        }
        current = v[2];
        string deklaracija_parametra_tip;
        string deklaracija_parametra_ime;
        if (!deklaracija_parametra(deklaracija_parametra_tip, deklaracija_parametra_ime)) {
            return false;
        }
        /*
        cerr << deklaracija_parametra_ime << "\n";
        for (string str : lista_parametara_imena) {
            cerr << str << " ";
        }
        cerr << "\n";*/
        if(find(deklaracija_parametra_ime, lista_parametara_imena)) {
            //ISPIS GREŠKE
            errorOutput(par, par->children);
            return false;
        }
        lista_parametara_tipovi.push_back(deklaracija_parametra_tip);
        lista_parametara_imena.push_back(deklaracija_parametra_ime);
        tipovi = lista_parametara_tipovi;
        imena = lista_parametara_imena;

    } else {

        cerr << "SINTAKSNO STABLO NE VALJA\n";
    }
    return true;
}
bool deklaracija_parametra(string& tip, string& ime) {
    cerr << "deklaracija_parametra\n";
    Node* par = current; 
    string childrenConcat = "";
    for (Node* child : current->children) {
        stringstream ssName = stringstream(child->name);
        string tmp;
        ssName >> tmp;
        childrenConcat += tmp;
        childrenConcat += " ";
    }
    childrenConcat = trim(childrenConcat);
    if (childrenConcat == "<ime_tipa> IDN") {
        vector<Node*> v;
        list<Node*>::iterator it = current->children.begin();
        while(it != current->children.end()) {
            v.push_back(*it);
            it++;
        }
        Node* nodeIDN = v[1];
        stringstream ssName(nodeIDN->name);
        string tokenName;
        string lineNumber;
        string identifierName;
        ssName >> tokenName;
        ssName >> lineNumber;
        ssName >> identifierName;
        current = v[0];
        string ime_tipa_tip;
        if (!ime_tipa(ime_tipa_tip)) {
            return false;
        }
        if (ime_tipa_tip == "void") {
            //ISPIS GREŠKE
            errorOutput(par, par->children);
            return false;
        }
        tip = ime_tipa_tip;
        ime = identifierName;

    } else if (childrenConcat == "<ime_tipa> IDN L_UGL_ZAGRADA D_UGL_ZAGRADA") {
        vector<Node*> v;
        list<Node*>::iterator it = current->children.begin();
        while(it != current->children.end()) {
            v.push_back(*it);
            it++;
        }
        Node* nodeIDN = v[1];
        stringstream ssName(nodeIDN->name);
        string tokenName;
        string lineNumber;
        string identifierName;
        ssName >> tokenName;
        ssName >> lineNumber;
        ssName >> identifierName;
        current = v[0];
        string ime_tipa_tip;
        if (!ime_tipa(ime_tipa_tip)) {
            return false;
        }
        if (ime_tipa_tip == "void") {
            //ISPIS GREŠKE
            errorOutput(par, par->children);
            return false;
        }
        tip = "niz(";
        tip += ime_tipa_tip;
        tip += ")";
        ime = identifierName;
    } else {

        cerr << "SINTAKSNO STABLO NE VALJA\n";
    }
    return true;
}
bool lista_deklaracija() {
    cerr << "lista_deklaracija\n";
    Node* par = current; 
    string childrenConcat = "";
    for (Node* child : current->children) {
        stringstream ssName = stringstream(child->name);
        string tmp;
        ssName >> tmp;
        childrenConcat += tmp;
        childrenConcat += " ";
    }
    childrenConcat = trim(childrenConcat);
    if (childrenConcat == "<deklaracija>") {
        current = *(current->children.begin());
        if (!deklaracija()) {
            return false;
        }
    } else if (childrenConcat == "<lista_deklaracija> <deklaracija>") {
        vector<Node*> v;
        list<Node*>::iterator it = current->children.begin();
        while(it != current->children.end()) {
            v.push_back(*it);
            it++;
        }
        current = v[0];
        if (!lista_deklaracija()) {
            return false;
        }
        current = v[1];
        if (!deklaracija()) {
            return false;
        }

    } else {

        cerr << "SINTAKSNO STABLO NE VALJA\n";
    }
    return true;
}
bool deklaracija() {
    cerr << "deklaracija\n";
    Node* par = current; 
    string childrenConcat = "";
    for (Node* child : current->children) {
        stringstream ssName = stringstream(child->name);
        string tmp;
        ssName >> tmp;
        childrenConcat += tmp;
        childrenConcat += " ";
    }
    childrenConcat = trim(childrenConcat);
    if (childrenConcat == "<ime_tipa> <lista_init_deklaratora> TOCKAZAREZ") {
        vector<Node*> v;
        list<Node*>::iterator it = current->children.begin();
        while(it != current->children.end()) {
            v.push_back(*it);
            it++;
        }
        string ime_tipa_tip;
        current = v[0];
        if (!ime_tipa(ime_tipa_tip)) {
            return false;
        }
        current = v[1];
        if (!lista_init_deklaratora(ime_tipa_tip)) {
            return false;
        }

    } else {

        cerr << "SINTAKSNO STABLO NE VALJA\n";
    } 
    return true;
}
bool lista_init_deklaratora(string ntip) {
    cerr << "lista_init_deklaratora\n";
    Node* par = current;
    string childrenConcat = "";
    for (Node* child : current->children) {
        stringstream ssName = stringstream(child->name);
        string tmp;
        ssName >> tmp;
        childrenConcat += tmp;
        childrenConcat += " ";
    }
    childrenConcat = trim(childrenConcat);
    //cerr << childrenConcat << "\n";
    if (childrenConcat == "<init_deklarator>") {
        current = *(current->children.begin());
        if (!init_deklarator(ntip)) {
            return false;
        }
    } else if (childrenConcat == "<lista_init_deklaratora> ZAREZ <init_deklarator>") {
        vector<Node*> v;
        list<Node*>::iterator it = current->children.begin();
        while(it != current->children.end()) {
            v.push_back(*it);
            it++;
        }
        current = v[0];
        if (!lista_init_deklaratora(ntip)) {
            return false;
        }
        current = v[2];
        if (!init_deklarator(ntip)) {
            return false;
        }
    } else {

        cerr << "SINTAKSNO STABLO NE VALJA\n";
    }
    return true;
}
bool init_deklarator(string ntip) {
    cerr << "init_deklarator\n";
    Node* par = current;
    string childrenConcat = "";
    for (Node* child : current->children) {
        stringstream ssName = stringstream(child->name);
        string tmp;
        ssName >> tmp;
        childrenConcat += tmp;
        childrenConcat += " ";
    }
    childrenConcat = trim(childrenConcat);
    if (childrenConcat == "<izravni_deklarator>") {
        current = *(current->children.begin());
        string izravni_deklarator_tip;
        int izravni_deklarator_br_elem;
        if (!izravni_deklarator(izravni_deklarator_tip, izravni_deklarator_br_elem, ntip)){
            return false;
        }
        if (izravni_deklarator_tip == "const(int)" ||
            izravni_deklarator_tip == "const(char)" ||
            izravni_deklarator_tip == "niz(const(int))" ||
            izravni_deklarator_tip == "niz(const(char))") {
            //ISPIS GREŠKE
            errorOutput(par, par->children);
            return false;
        }
    } else if (childrenConcat == "<izravni_deklarator> OP_PRIDRUZI <inicijalizator>") {
        vector<Node*> v;
        list<Node*>::iterator it = current->children.begin();
        while(it != current->children.end()) {
            v.push_back(*it);
            it++;
        }
        current = v[0];
        string izravni_deklarator_tip;
        int izravni_deklarator_br_elem;
        if (!izravni_deklarator(izravni_deklarator_tip, izravni_deklarator_br_elem, ntip)) {
            return false;
        }
        current = v[2];
        string inicijalizator_tip;
        list<string> inicijalizator_tipovi;
        int inicijalizator_br_elem;

        if (!inicijalizator(inicijalizator_tip, inicijalizator_tipovi, inicijalizator_br_elem)) {
            return false;
        }
        bool noSemanticError = true;
        bool isArray = false;
        string T; //OVO PREGLEDATI!!!
        if (izravni_deklarator_tip.substr(0,5) == "const") {
            T = izravni_deklarator_tip.substr(6, izravni_deklarator_tip.length());
            T = T.substr(0, T.length() - 1);
        } else if (izravni_deklarator_tip.substr(0,10) == "niz(const(") {
            T = izravni_deklarator_tip.substr(10, izravni_deklarator_tip.length());
            T = T.substr(0, T.length() - 2);
            isArray = true;
        } else if (izravni_deklarator_tip.substr(0, 4) == "niz(") {
            T = izravni_deklarator_tip.substr(4, izravni_deklarator_tip.length());
            T = T.substr(0, T.length() - 1);
            isArray = true;
        } else {
            T = izravni_deklarator_tip;
        }
        //cerr << izravni_deklarator_tip << " " << T << "\n";
        if ((T == "int" || T == "char") && !isArray) {
            noSemanticError = castingRelation[inicijalizator_tip][T];
        } else if (isArray) {
            bool tmp = true;
            noSemanticError = false;
            for (string tip : inicijalizator_tipovi) {
                tmp = tmp && castingRelation[tip][T];
            }
            //cerr << inicijalizator_br_elem << " " << izravni_deklarator_br_elem << "\n";
            if (tmp && (inicijalizator_br_elem <= izravni_deklarator_br_elem)) {
                noSemanticError = true;
            } 
        } else {
            noSemanticError = false;
        }
        if (!noSemanticError) {
            //ISPIS GREŠKE
            errorOutput(par, par->children);
            return false;
        }
    } else {

        cerr << "SINTAKSNO STABLO NE VALJA\n";
    }
    return true;
}
bool isPositiveAndLessThan1024(string value) {
    try {
        int num = stoi(value);
        return (num > 0 && num <= 1024);
    } catch(...) {
        return false;
    }
}
bool izravni_deklarator(string& tip, int& br_elem, string ntip) {
    cerr << "izravni_deklarator\n";
    Node* par = current; 
    string childrenConcat = "";
    for (Node* child : current->children) {
        stringstream ssName = stringstream(child->name);
        string tmp;
        ssName >> tmp;
        childrenConcat += tmp;
        childrenConcat += " ";
    }
    childrenConcat = trim(childrenConcat);
    if (childrenConcat == "IDN") {
        stringstream ssName((*(current->children.begin()))->name);
        string tokenName;
        string lineNumber;
        string identifierName;
        ssName >> tokenName;
        ssName >> lineNumber;
        ssName >> identifierName;
        if (ntip == "void") {
            //ISPIS GREŠKE
            errorOutput(par, par->children);
            return false;
        }

        if (isDeclaredLocally(current, identifierName) != "void") {
            //ISPIS GREŠKE
            errorOutput(par, par->children);
            return false;
        }
        tip = ntip;
        declare(current, identifierName, tip); //VALJDA IDE TIP OVDJE

    } else if (childrenConcat == "IDN L_UGL_ZAGRADA BROJ D_UGL_ZAGRADA") {
        vector<Node*> v;
        list<Node*>::iterator it = current->children.begin();
        while(it != current->children.end()) {
            v.push_back(*it);
            it++;
        }
        stringstream ssName(v[0]->name);
        string tokenName1;
        string lineNumber1;
        string identifierName;
        ssName >> tokenName1;
        ssName >> lineNumber1;
        ssName >> identifierName;
        stringstream ssBROJ(v[2]->name);
        string tokenName2;
        string lineNumber2;
        string numValue;
        ssBROJ >> tokenName2;
        ssBROJ >> lineNumber2;
        ssBROJ >> numValue;
        if (ntip == "void") {
            //ISPIS GREŠKE
            errorOutput(par, par->children);
            return false;
        }
        if (isDeclaredLocally(current, identifierName) != "void") {
            //ISPIS GREŠKE
            errorOutput(par, par->children);
            return false;
        }
        if (!isPositiveAndLessThan1024(numValue)) {
            //ISPIS GREŠKE
            errorOutput(par, par->children);
            return false;
        }
        string type = "niz(";
        type += ntip;
        type += ")";
        declare(current, identifierName, type);
        tip = type;
        br_elem = stoi(numValue);
    } else if (childrenConcat == "IDN L_ZAGRADA KR_VOID D_ZAGRADA") {
        vector<Node*> v;
        list<Node*>::iterator it = current->children.begin();
        while(it != current->children.end()) {
            v.push_back(*it);
            it++;
        }
        stringstream ssName(v[0]->name);
        string tokenName;
        string lineNumber;
        string identifierName;
        ssName >> tokenName;
        ssName >> lineNumber;
        ssName >> identifierName;
        string type = "funkcija(void->";
        type += ntip;
        type += ")";
        if (isDeclaredLocally(current, identifierName) != "void" &&
            isDeclaredLocally(current, identifierName) != type) {
            //ISPIS GREŠKE
            errorOutput(par, par->children);
            return false;
        }
        if (isDeclaredLocally(current, identifierName) == "void") {
            declare(current, identifierName, type);
        }
        tip = type; 

    } else if (childrenConcat == "IDN L_ZAGRADA <lista_parametara> D_ZAGRADA") {
        vector<Node*> v;
        list<Node*>::iterator it = current->children.begin();
        while(it != current->children.end()) {
            v.push_back(*it);
            it++;
        }
        stringstream ssName(v[0]->name);
        string tokenName;
        string lineNumber;
        string identifierName;
        ssName >> tokenName;
        ssName >> lineNumber;
        ssName >> identifierName;
        Node* tmpNode = current;
        current = v[2];
        list<string> lista_parametara_tipovi;
        list<string> lista_parametara_imena;
        if (!lista_parametara(lista_parametara_tipovi, lista_parametara_imena)) {
            return false;
        }
        string type = "funkcija(";
        for (string str : lista_parametara_tipovi) {
            type += str;
            type += ",";
        }
        type[type.length() - 1] = '-';
        type += ">";
        type += ntip;
        type += ")";
        //cerr << identifierName << " " << isDeclared(tmpNode, identifierName) << " " << type << "\n";

        if (isDeclaredLocally(tmpNode, identifierName) != "void" &&
            isDeclaredLocally(tmpNode, identifierName) != type) {
            //ISPIS GREŠKE
            errorOutput(par, par->children);
            return false;
        }
        if (isDeclaredLocally(tmpNode, identifierName) == "void") {
            declare(tmpNode, identifierName, type);
        }
    } else {

        cerr << "SINTAKSNO STABLO NE VALJA\n";
    }
    return true;
}
bool inicijalizator(string& tip, list<string>& tipovi, int& br_elem) {
    cerr << "inicijalizator\n";
    Node* par = current; 
    string childrenConcat = "";
    for (Node* child : current->children) {
        stringstream ssName = stringstream(child->name);
        string tmp;
        ssName >> tmp;
        childrenConcat += tmp;
        childrenConcat += " ";
    }
    childrenConcat = trim(childrenConcat);
    if (childrenConcat == "<izraz_pridruzivanja>") {
        current = *(current->children.begin());
        string izraz_pridruzivanja_tip;
        bool izraz_pridruzivanja_l_izraz;
        int isNIZ_ZNAKOVA;
        if (!izraz_pridruzivanja(izraz_pridruzivanja_tip, izraz_pridruzivanja_l_izraz, isNIZ_ZNAKOVA)) {
            return false;
        }
        if (isNIZ_ZNAKOVA != -1) {
            br_elem = 1 + isNIZ_ZNAKOVA;
            tipovi.clear();
            for (int i = 0; i < br_elem; i++) 
                tipovi.push_back("char");
        } else {
            tip = izraz_pridruzivanja_tip;
        }
    } else if (childrenConcat == "L_VIT_ZAGRADA <lista_izraza_pridruzivanja> D_VIT_ZAGRADA") {
        vector<Node*> v;
        list<Node*>::iterator it = current->children.begin();
        while(it != current->children.end()) {
            v.push_back(*it);
            it++;
        }
        current = v[1];
        list<string> lista_izraza_pridruzivanja_tipovi;
        int lista_izraza_pridruzivanja_br_elem;
        if (!lista_izraza_pridruzivanja(tipovi, lista_izraza_pridruzivanja_br_elem)) {
            return false;
        }
        br_elem = lista_izraza_pridruzivanja_br_elem;
        tipovi = lista_izraza_pridruzivanja_tipovi;
    } else {

        cerr << "SINTAKSNO STABLO NE VALJA\n";
    }
    return true;
}
bool lista_izraza_pridruzivanja(list<string>& tipovi, int& br_elem) {
    cerr << "lista_izraza_pridruzivanja\n";
    Node* par = current; 
    string childrenConcat = "";
    for (Node* child : current->children) {
        stringstream ssName = stringstream(child->name);
        string tmp;
        ssName >> tmp;
        childrenConcat += tmp;
        childrenConcat += " ";
    }
    childrenConcat = trim(childrenConcat);
    if (childrenConcat == "<izraz_pridruzivanja>") {
        current = *(current->children.begin());
        string izraz_pridruzivanja_tip;
        bool izraz_pridruzivanja_l_izraz;
        int izraz_pridruzivanja_isNIZ_ZNAKOVA;
        if (!izraz_pridruzivanja(izraz_pridruzivanja_tip, izraz_pridruzivanja_l_izraz, izraz_pridruzivanja_isNIZ_ZNAKOVA)) {
            return false;
        }
        tipovi.push_back(izraz_pridruzivanja_tip);
        br_elem = 1;
        
    } else if (childrenConcat == "<lista_izraza_pridruzivanja> ZAREZ <izraz_pridruzivanja>") {
        vector<Node*> v;
        list<Node*>::iterator it = current->children.begin();
        while(it != current->children.end()) {
            v.push_back(*it);
            it++;
        }
        current = v[0];
        list<string> lista_izraza_pridruzivanja_tipovi;
        int lista_izraza_pridruzivanja_br_elem;
        if (!lista_izraza_pridruzivanja(lista_izraza_pridruzivanja_tipovi, lista_izraza_pridruzivanja_br_elem)) {
            return false;
        }
        current = v[2];
        string izraz_pridruzivanja_tip;
        bool izraz_pridruzivanja_l_izraz;
        int izraz_pridruzivanja_isNIZ_ZNAKOVA;
        if (!izraz_pridruzivanja(izraz_pridruzivanja_tip, izraz_pridruzivanja_l_izraz, izraz_pridruzivanja_isNIZ_ZNAKOVA)) {
            return false;
        }
        tipovi.clear();
        tipovi = lista_izraza_pridruzivanja_tipovi;
        tipovi.push_back(izraz_pridruzivanja_tip);
        br_elem = lista_izraza_pridruzivanja_br_elem + 1;

    } else {

        cerr << "SINTAKSNO STABLO NE VALJA\n";
    }
    return true;
}
int main() { //nije provjerena ispravnost generativnog stabla
    castingRelation["char"]["char"] = true;
    castingRelation["int"]["int"] = true;
    castingRelation["const(int)"]["const(int)"] = true;
    castingRelation["const(char)"]["const(char)"] = true;
    castingRelation["niz(char)"]["niz(char)"] = true;
    castingRelation["niz(int)"]["niz(int)"] = true;
    castingRelation["niz(const(int))"]["niz(const(int))"] = true;
    castingRelation["niz(const(char))"]["niz(const(char))"] = true;
    castingRelation["const(char)"]["char"] = true;
    castingRelation["const(int)"]["int"] = true;
    castingRelation["int"]["const(int)"] = true;
    castingRelation["char"]["const(char)"] = true;
    castingRelation["char"]["int"] = true;
    castingRelation["niz(int)"]["niz(const(int))"] = true;
    castingRelation["niz(char)"]["niz(const(char))"] = true; 
    string line;
    stack<pair<Node*, int>> S;
    GenTree* genTree = new GenTree();
    while(getline(cin, line)) {
        int n = 0;
        while (line[n] == ' ')
            n++;
        string name = line.substr(n, line.length());
        if (S.empty()) { 
            Node* node = new Node(nullptr, name); 
            genTree->root =  node;
            root = node;
            root->isScopeParent = true;
            S.push({node, n});
        }else if (n > S.top().second) {
            Node* node = new Node(S.top().first, name);
            S.top().first->children.push_back(node);
            genTree->nodes.push_back(node);
            S.push({node, n});
        } else {
            while (n <= S.top().second)
                S.pop();
            Node* node = new Node(S.top().first, name);
            S.top().first->children.push_back(node);
            if (S.top().first->name == "<slozena_naredba>") {
                S.top().first->isScopeParent = true;
            }
            genTree->nodes.push_back(node);
            S.push({node, n});

        }
    }
    current = root;
    if (!prijevodna_jedinica()) {
        return 0;
    }
    if (isDefined("main") != "funkcija(void->int)") {
        cout << "main\n";
        return 0;
    }
    stack<Node*> S1;
    S1.push(root);
    while (!S1.empty()) {
        Node* cur = S1.top();
        S1.pop();
        for (Node* child : cur->children) {
            S1.push(child);
        }
        if (cur->isScopeParent) {
            for (auto [name, type] : cur->declarationTable) {
                if (isDefined(name) != type) {
                    cout << "funkcija\n";
                    return 0;
                }
            }
        }
    }

}