#include <bits/stdc++.h>

using namespace std;

enum TOKEN{
  BREAK, CASE, CONST, CONTINUE, DEFAULT, DO, ELSE, ENUM, FOR, GOTO, IF, RETURN, SIZEOF, STRUCT, SWITCH, TYPEDEF, UNION, WHILE, TYPE, MOD, ADD, SUB, TIME, DIV, bla, ket, Bla, Ket, BLA, KET, DOT, SINGLEQ, DOUBLEQ, NUM, REAL, ID, BLANK, COMMA, ASS, SEMIC, END, GT, LT, LE, GE, EQ, TOKEN_SIZE
};

struct Token{
  int token;
  string word;
};

vector<Token>reserved={
  {TYPE,"auto"},
  {BREAK,"break"},
  {CASE,"case"},
  {TYPE,"char"},
  {CONST,"const"},
  {CONTINUE,"continue"},
  {DEFAULT,"default"},
  {DO,"do"},
  {TYPE,"double"},
  {ELSE,"else"},
  {ENUM,"enum"},
  {MOD,"extern"},
  {TYPE,"float"},
  {FOR,"for"},
  {GOTO,"goto"},
  {IF,"if"},
  {TYPE,"int"},
  {TYPE,"long"},
  {MOD,"register"},
  {RETURN,"return"},
  {TYPE,"signed"},
  {SIZEOF,"sizeof"},
  {TYPE,"short"},
  {STRUCT,"strucct"},
  {SWITCH,"switch"},
  {TYPEDEF,"typedef"},
  {UNION,"union"},
  {MOD,"unsigned"},
  {TYPE,"void"},
  {MOD,"volatile"},
  {WHILE,"while"},
};

vector<Token>operators={
  {ADD,"+"},
  {SUB,"-"},
  {DIV,"/"},
  {TIME,"*"},
  {ASS,"="},
  {bla,"("},
  {ket,")"},
  {Bla,"["},
  {Ket,"]"},
  {BLA,"{"},
  {KET,"}"},
  {DOT,"."},
  {COMMA,","},
  {SEMIC,";"},
  {GT,">"},
  {LT,"<"},
  {DOUBLEQ,"\""},
  {SINGLEQ,"\'"},
  {LE,"<="},
  {GE,">="},
  {EQ,"=="},
};

vector<Token>tokens;
vector<vector<int>>automaton(1000,vector<int>(pow(2,8),0));
vector<int>tokenmap(1000,-1);

void set_auomaton(){
  //state=0 受理
  //state=1 初期状態
  //state=2 id
  tokenmap[2]=ID;
  for(char c='a';c<='z';c++)automaton[1][c]=2;
  for(char c='A';c<='Z';c++)automaton[1][c]=2;
  for(char c='a';c<='z';c++)automaton[2][c]=2;
  for(char c='A';c<='Z';c++)automaton[2][c]=2;
  for(char c='0';c<='9';c++)automaton[2][c]=2;
  automaton[2]['_']=2;
  //state=3 num
  tokenmap[3]=NUM;
  for(char c='0';c<='9';c++)automaton[1][c]=3;
  for(char c='0';c<='9';c++)automaton[3][c]=3;
  //state=4 real
  tokenmap[4]=REAL;
  automaton[3]['.']=4;
  for(char c='1';c<='9';c++)automaton[4][c]=4;
  //state=5 空白
  tokenmap[5]=BLANK;
  automaton[1][' ']=5;
  automaton[1]['\n']=5;
  automaton[5][' ']=5;
  automaton[5]['\n']=5;

  //rserved
  int now_state=6;
  for(auto it:reserved){
    int state=1;
    for(auto c:it.word){
      if(automaton[state][c]==0||automaton[state][c]==2){
        tokenmap[now_state]=ID;
        for(char c='a';c<='z';c++)automaton[now_state][c]=2;
        for(char c='A';c<='Z';c++)automaton[now_state][c]=2;
        for(char c='0';c<='9';c++)automaton[now_state][c]=2;
        automaton[now_state]['_']=2;
        automaton[state][c]=now_state++;
      }
      state=automaton[state][c];
    }
    tokenmap[state]=it.token;
  }

  //operator
  for(auto it:operators){
    if(it.word.size()==1)tokenmap[now_state]=it.token,automaton[1][it.word[0]]=now_state++;
    if(it.word.size()==2){
      int state=1;
      if(automaton[state][it.word[0]]==0)tokenmap[now_state]=it.token,automaton[state][it.word[0]]=now_state++;
      tokenmap[now_state]=it.token,automaton[state][it.word[0]]=now_state++;
    }
  }
}

bool lex(string s){
  int now=0;
  int now_state=1;
  string S="";
  bool ng=false;
  while(now<s.size()){
    if(automaton[now_state][s[now]]==0){
      if(ng){
        cout<<s[now]<<" "<<"NG"<<endl;
        return false;
      }
      ng=true;

      //空白は除去
      if(tokenmap[now_state]!=BLANK)tokens.push_back({tokenmap[now_state],S});
      now_state=1;
      S="";
      continue;
    }
    ng=false;
    S=S+s[now];
    now_state=automaton[now_state][s[now]];
    now++;
  }
  if(tokenmap[now_state]!=-1){
      //空白は除去
      if(tokenmap[now_state]!=BLANK)tokens.push_back({tokenmap[now_state],S});
  }
  tokens.push_back({END,""});
  return true;
}

enum SYMBOL{
  SS=TOKEN_SIZE+1,S,E,T,N,SYMBOL_SIZE
};


struct Grammer{
  int lvalue;
  vector<int>product;
  int dot;
  int end;
  bool operator<(const Grammer&obj)const{
    if(lvalue<obj.lvalue){
      return true;
    }
    else if(lvalue==obj.lvalue){
      if(product.size()<obj.product.size()){
        return true;
      }
      else if(product.size()==obj.product.size()){
        for(int i=0;i<product.size();i++){
          if(product[i]<obj.product[i])return true;
        }
        if(product==obj.product){
          if(dot<obj.dot)return true;
          else if(dot==obj.dot){
            if(end<obj.end)return true;
          }
        }
      }
    }
    return false;
  }
  bool operator==(const Grammer&obj)const{
    return (lvalue==obj.lvalue)&&(product==obj.product)&&(dot==obj.dot)&&(end==obj.end);
  }
};

vector<Grammer>grammers={
  {SS,{S,END}},
  {S,{E}},
  {E,{T,ADD,E}},
  {E,{T,SUB,E}},
  {E,{T}},
  {T,{N,TIME,T}},
  {T,{N,DIV,T}},
  {T,{N}},
  {N,{NUM}},
};

enum ACTION{
  SHIFT, REDUCE, JUMP, ERROR, ACCEPT,
};

set<int>first(int sym){
  if(sym<TOKEN_SIZE){
    set<int>s;
    s.insert(sym);
    return s;
  }
  set<int>s;
  vector<int>syms={sym};
  while(true){
    int size=s.size();
    for(int i=0;i<grammers.size();i++){
      for(auto it:syms){
        if(grammers[i].lvalue==it){
          auto f=grammers[i].product[0];
          if(f<=TOKEN_SIZE)s.insert(f);
          else syms.push_back(f);
          break;
        }
      }
    }
    if(size==s.size())break;
  }
  return s;
}

set<Grammer>LR0closure(set<Grammer>s){
  while(true){
    int size=s.size();
    for(auto sym:s){
      for(auto it:grammers){
        if(it.lvalue==sym.product[sym.dot]){
          Grammer g={it.lvalue,it.product,0,0};
          s.insert(g);
        }
      }
    }
    if(size==s.size())break;
  }
  return s;
}

set<Grammer>LR1closure(set<Grammer>s){
  while(true){
    int size=s.size();
    for(auto sym:s){
      set<int>a;
      if(sym.dot+1==sym.product.size())a.insert(sym.end);
      else a=first(sym.product[sym.dot+1]);
      for(auto it:grammers){
        for(auto it2:a){
          if(it.lvalue==sym.product[sym.dot]){
            Grammer g;
            g={it.lvalue,it.product,0,it2};
            s.insert(g);
          }
        }
      }
    }
    if(size==s.size())break;
  }
  return s;
}

vector<vector<pair<ACTION,int>>>table(1000,vector<pair<ACTION,int>>(SYMBOL_SIZE,{ERROR,0}));
map<pair<int,int>,int>reduce;

bool set_table(){
  vector<set<Grammer>>T;
  //T.push_back(LR0closure({grammers[0]}));
  T.push_back(LR1closure({grammers[0]}));
  set<tuple<int,int,int>>E;
  int A,B,C;
  while(true){
    int Tsize=T.size();
    int Esize=E.size();
    for(int i=0;i<T.size();i++){
      auto t=T[i];
      for(int j=0;j<SYMBOL_SIZE;j++){
        set<Grammer>s;
        for(auto it:t){
          if(it.dot<it.product.size()&&j==it.product[it.dot]){
            if(j==END){
              A=i,C=j;
              continue;
            }
            it.dot++;
            s.insert(it);
          }
        }
        if(s.size()==0)continue;
        //set<Grammer>ss=LR0closure(s);
        set<Grammer>ss=LR1closure(s);
        int Tite;
        bool ok=true;
        for(int k=0;k<T.size();k++){
          if(T[k]==ss){
            ok=false;
            Tite=k;
            break;
          }
        }
        if(ok)Tite=T.size(),T.push_back(ss);
        E.insert({i,Tite,j});
      }
    }
    if(Tsize==T.size()&&Esize==E.size())break;
  }
  for(auto it:E){
    int a=get<0>(it);
    int b=get<1>(it);
    int c=get<2>(it);
    table[a][c]={SHIFT,b};
  }
  table[A][C]={ACCEPT,B};

  set<pair<int,Grammer>>R;
  for(int i=0;i<T.size();i++){
    for(auto it:T[i]){
      if(it.dot==it.product.size()){
        R.insert({i,it});
      }
    }
  }
  for(auto it:R){
    if(table[it.first][it.second.end]!=make_pair(ERROR,0)){
      return false;
    }
    table[it.first][it.second.end]={REDUCE,it.second.product.size()};
    int memo;
    for(int i=0;i<grammers.size();i++)if(grammers[i].product==it.second.product)memo=i;
    reduce[{it.first,it.second.end}]=memo;
  }
  return true;
}

stack<int>history;
stack<pair<int,string>>res;

bool syntax(){
  int now=0;
  history.push(0);
  int state=0;
  int token=tokens[now].token;
  int act=table[state][token].first;
  int n=table[state][token].second;
  while(true){
    if(act==SHIFT){
      state=n;
      history.push(state);
      now++;
      token=tokens[now].token;
      act=table[state][token].first;
      n=table[state][token].second;
    }
    else if(act==REDUCE){
      token=grammers[reduce[{state,tokens[now].token}]].lvalue;
      res.push({reduce[{state,tokens[now].token}],tokens[now-1].word});
      for(int i=0;i<n;i++)history.pop();
      state=history.top();
      act=SHIFT;
      n=table[state][token].second;

      state=n;
      history.push(n);
      token=tokens[now].token;
      act=table[state][token].first;
      n=table[state][token].second;

    }
    else if(act==ACCEPT){
      res.push({reduce[{state,tokens[now].token}],tokens[now-1].word});
      return true;
    }
    else if(act==ERROR)return false;
  }
}

string execute(){
  string ans;
  while(res.size()){
    int n=res.top().first;
    string s=res.top().second;
    string S="";
    if(n==0){
      S=S+"pop rax\n";
      S=S+"ret\n";
    }
    if(n==1){
    }
    if(n==2){
      S=S+"pop rdi\n";
      S=S+"pop rax\n";
      S=S+"add rax,rdi\n";
      S=S+"push rax\n";
    }
    if(n==3){
      S=S+"pop rdi\n";
      S=S+"pop rax\n";
      S=S+"sub rax,rdi\n";
      S=S+"push rax\n";
    }
    if(n==4){
    }
    if(n==5){
      S=S+"pop rdi\n";
      S=S+"pop rax\n";
      S=S+"imul rax,rdi\n";
      S=S+"push rax\n";
    }
    if(n==6){
      S=S+"pop rdi\n";
      S=S+"pop rax\n";
      S=S+"cqo\n";
      S=S+"idiv rdi\n";
      S=S+"push rax\n";
    }
    if(n==7){
    }
    if(n==8){
      S=S+"push "+s+"\n";
    }
    res.pop();
    ans=S+ans;
  }
  ans=".intel_syntax noprefix\n.globl main\n\nmain:\n"+ans;
  return ans;
}

int main(int argc,char**argv){
  //入力
  //ファイルから入力する時
  /*
  string file=argv[1];
  ifstream ifs("./"+file);
  string S((istreambuf_iterator<char>(ifs)),istreambuf_iterator<char>());
  */

  //コードに入力を書く時
  string S="1+3*4/2-1";

  //字句解析生成
  set_auomaton();
  //構文解析表生成
  if(!set_table()){
    cout<<"LR error"<<endl;
    return 0;
  }
  //字句解析
  if(!lex(S)){
    cout<<"lex error"<<endl;
    return 0;
  }
  //構文解析
  if(!syntax()){
    cout<<"syntax error"<<endl;
    return 0;
  }

  //コンパイル&出力
  //標準出力する時
  cout<<execute()<<endl;

  //ファイルに出力する時
  /*
  ofstream outputfile("out.s");
  outputfile<<execute();
  outputfile.close();
  */
  return 0;
} 
