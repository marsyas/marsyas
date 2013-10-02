

#include <marsyas/expr/ExParser.h>
#include <marsyas/expr/ExScanner.h>

using namespace Marsyas;

void ExParser::Get() {
  for (;;) {
    t = la;
    la = scanner->Scan();
    if (la->kind <= maxT) { ++errDist; break; }

    if (dummyToken != t) {
      dummyToken->kind = t->kind;
      dummyToken->pos = t->pos;
      dummyToken->col = t->col;
      dummyToken->line = t->line;
      dummyToken->next = NULL;
      coco_string_delete(dummyToken->val);
      dummyToken->val = coco_string_create(t->val);
      t = dummyToken;
    }
    la = t;
  }
}

void ExParser::Expect(int n) {
  if (la->kind==n) Get(); else { SynErr(n); }
}

void ExParser::ExpectWeak(int n, int follow) {
  if (la->kind == n) Get();
  else {
    SynErr(n);
    while (!StartOf(follow)) Get();
  }
}

bool ExParser::WeakSeparator(int n, int syFol, int repFol) {
  if (la->kind == n) {Get(); return true;}
  else if (StartOf(repFol)) {return false;}
  else {
    SynErr(n);
    while (!(StartOf(syFol) || StartOf(repFol) || StartOf(0))) {
      Get();
    }
    return StartOf(syFol);
  }
}

void ExParser::Alias(std::string& nm) {
  Expect(46);
  Name(nm);
}

void ExParser::Name(std::string& nm) {
  Expect(5);
  nm=t->val;
}

void ExParser::CName(std::string& nm) {
  Expect(6);
  nm=t->val;
}

void ExParser::AddOp(int& m) {
  if (la->kind == 23) {
    Get();
    m=OP_ADD;
  } else if (la->kind == 24) {
    Get();
    m=OP_SUB;
  } else SynErr(62);
}

void ExParser::MulOp(int& m) {
  if (la->kind == 25) {
    Get();
    m=OP_MUL;
  } else if (la->kind == 26) {
    Get();
    m=OP_DIV;
  } else if (la->kind == 27) {
    Get();
    m=OP_MOD;
  } else SynErr(63);
}

void ExParser::RelOp(int& m) {
  switch (la->kind) {
  case 34: {
    Get();
    m=OP_EQ;
    break;
  }
  case 35: {
    Get();
    m=OP_NE;
    break;
  }
  case 36: {
    Get();
    m=OP_GT;
    break;
  }
  case 37: {
    Get();
    m=OP_GE;
    break;
  }
  case 38: {
    Get();
    m=OP_LT;
    break;
  }
  case 39: {
    Get();
    m=OP_LE;
    break;
  }
  default: SynErr(64); break;
  }
}

void ExParser::LAsgnOp(int& type) {
  switch (la->kind) {
  case 14: {
    Get();
    type=OP_ADD;
    break;
  }
  case 15: {
    Get();
    type=OP_SUB;
    break;
  }
  case 16: {
    Get();
    type=OP_MUL;
    break;
  }
  case 17: {
    Get();
    type=OP_DIV;
    break;
  }
  case 18: {
    Get();
    type=OP_MOD;
    break;
  }
  case 22: {
    Get();
    type=OP_OR;
    break;
  }
  case 21: {
    Get();
    type=OP_AND;
    break;
  }
  default: SynErr(65); break;
  }
}

void ExParser::RAsgnOp(int& type) {
  switch (la->kind) {
  case 9: {
    Get();
    type=OP_ADD;
    break;
  }
  case 10: {
    Get();
    type=OP_SUB;
    break;
  }
  case 11: {
    Get();
    type=OP_MUL;
    break;
  }
  case 12: {
    Get();
    type=OP_DIV;
    break;
  }
  case 13: {
    Get();
    type=OP_MOD;
    break;
  }
  case 20: {
    Get();
    type=OP_OR;
    break;
  }
  case 19: {
    Get();
    type=OP_AND;
    break;
  }
  default: SynErr(66); break;
  }
}

void ExParser::Exprs(ExNode** u) {
  ExNode* v=NULL; if(fail)return;
  Task(u);
  if(fail)return;
  while (la->kind == 42) {
    Get();
    Task(&v);
    if(!fail)*u=expr_append(*u,v); else {delete *u; *u=NULL;}
  }
}

void ExParser::Task(ExNode** u) {
  if (fail) return;
  if (IsLAsgn()) {
    LAsgn(u);
  } else if (IsLink()) {
    Link(u);
  } else if (StartOf(1)) {
    RAsgn(u);
  } else SynErr(67);
}

void ExParser::LAsgn(ExNode** u) {
  int atype; std::string nm; std::string as; if (fail) return; bool seq=false; ExNode* idx=NULL;
  if (la->kind == 51) {
    Get();
    Expect(47);
    Name(nm);
    Expect(8);
    Condition(u);
    if(fail)return;
    if(nm=="opn")*u=new ExFun_StreamOutNVal(*u);
    else *u=do_property(NULL,"Stream."+nm,*u);
  } else if (la->kind == 5) {
    Name(nm);
    if (la->kind == 48) {
      Elem(idx);
      seq=true;
    }
    if (StartOf(2)) {
      LAsgnOp(atype);
      Condition(u);
      if(fail)return;
      if(!seq) *u=do_masgn(atype,false,nm,*u);
      else *u=do_msetelem(nm,idx,NULL,false,false,atype,*u);
    } else if (la->kind == 8) {
      Get();
      Condition(u);
      if(fail)return;
      if(!seq) *u=do_asgn(nm,*u);
      else *u=do_setelem(nm,idx,NULL,false,*u);
    } else SynErr(68);
  } else if (la->kind == 6) {
    CName(nm);
    if (StartOf(2)) {
      LAsgnOp(atype);
      Condition(u);
      if(fail)return; *u=do_cmasgn(atype,false,nm,*u);
    } else if (la->kind == 8) {
      Get();
      Condition(u);
      if(fail)return; *u=do_casgn(nm,*u);
    } else SynErr(69);
  } else if (la->kind == 46) {
    Alias(as);
    Expect(8);
    CName(nm);
    *u=do_alias(as,nm);
  } else SynErr(70);
}

void ExParser::Link(ExNode** u) {
  std::string nm1,nm2; if(fail)return;
  CName(nm1);
  if (la->kind == 32) {
    Get();
    CName(nm2);
    *u=do_link(nm2,nm1);
  } else if (la->kind == 31) {
    Get();
    CName(nm2);
    *u=do_link(nm1,nm2);
  } else SynErr(71);
}

void ExParser::RAsgn(ExNode** u) {
  int atype; std::string nm; std::string as; if(fail)return;
  if (IsCNameRAsgnAlias()) {
    CName(nm);
    Expect(7);
    Alias(as);
    *u=do_alias(as,nm);
  } else if (StartOf(1)) {
    Condition(u);
    if(fail)return;
    if (StartOf(3)) {
      if (la->kind == 7) {
        Get();
        if (la->kind == 51) {
          Get();
          Expect(47);
          Name(nm);
          if(fail)return;
          if(nm=="opn")*u=new ExFun_StreamOutNVal(*u);
          else *u=do_property(NULL,"Stream."+nm,*u);
        } else if (la->kind == 5) {
          Name(nm);
          if (la->kind == 48) {
            ExNode* idx=NULL;
            Elem(idx);
            *u=do_setelem(nm,idx,NULL,false,*u); return;
          }
          *u=do_asgn(nm,*u);
        } else if (la->kind == 6) {
          CName(nm);
          *u=do_casgn(nm,*u);
        } else SynErr(72);
      } else {
        RAsgnOp(atype);
        if (la->kind == 5) {
          Name(nm);
          if (la->kind == 48) {
            ExNode* idx=NULL;
            Elem(idx);
            *u=do_msetelem(nm,idx,NULL,false,true,atype,*u); return;
          }
          *u=do_masgn(atype,true,nm,*u);
        } else if (la->kind == 6) {
          CName(nm);
          *u=do_cmasgn(atype,true,nm,*u);
        } else SynErr(73);
      }
    }
  } else SynErr(74);
}

void ExParser::Condition(ExNode** u) {
  ExNode* v=NULL; bool neg=false; if (fail) return;
  if (la->kind == 33) {
    Get();
    neg=true;
  }
  CondTerm(u);
  if (fail) return; if (neg) { *u=do_bool_negate(*u); if (fail) return; }
  while (la->kind == 41) {
    Get();
    CondTerm(&v);
    if (!fail) *u=do_condop(OP_OR,*u,v); else {delete*u; *u=NULL;}
  }
}

void ExParser::CondTerm(ExNode** u) {
  ExNode* v=NULL; if (fail) return;
  CondFact(u);
  if (fail) return;
  while (la->kind == 40) {
    Get();
    CondFact(&v);
    if (!fail) *u=do_condop(OP_AND,*u,v); else {delete*u; *u=NULL;}
  }
}

void ExParser::CondFact(ExNode** u) {
  int m=0; ExNode* v=NULL; if (fail) return;
  Expr(u);
  if (fail) return;
  while (StartOf(4)) {
    RelOp(m);
    Expr(&v);
    if (!fail) *u=do_relop(m,*u,v); else {delete*u; *u=NULL;}
  }
}

void ExParser::Expr(ExNode** u) {
  int m=0; ExNode* v=NULL; bool neg=false; if (fail) return;
  if (la->kind == 24) {
    Get();
    neg=true;
  }
  Term(u);
  if(fail)return; if (neg) { *u=do_num_negate(*u); if (fail) return; }
  while (la->kind == 23 || la->kind == 24) {
    AddOp(m);
    Term(&v);
    if (!fail) *u=do_addop(m,*u,v); else {delete*u; *u=NULL;}
  }
}

void ExParser::Term(ExNode** u) {
  int m=0; ExNode* v=NULL; if (fail) return;
  Property(u);
  if (fail) return;
  while (la->kind == 25 || la->kind == 26 || la->kind == 27) {
    MulOp(m);
    Property(&v);
    if (!fail) *u=do_mulop(m,*u,v); else {delete*u; *u=NULL;}
  }
}

void ExParser::Property(ExNode** u) {
  std::string l=""; std::string n; ExNode* ps=NULL;
  Factor(l,u);
  if (fail) return;
  while (la->kind == 47) {
    Get();
    Name(n);
    if (la->kind == 29) {
      Get();
      if (StartOf(5)) {
        Exprs(&ps);
      }
      Expect(30);
    }
    if(!fail) {*u=do_property(*u,l+n,ps); l="";} else {delete*u; *u=NULL;}
  }
}

void ExParser::Factor(std::string& l, ExNode** u) {
  std::string nm; if (fail) return;
  switch (la->kind) {
  case 52: {
    Get();
    *u=new ExNode(true);
    break;
  }
  case 53: {
    Get();
    *u=new ExNode(false);
    break;
  }
  case 4: {
    Get();
    *u=new ExNode(t->val[1]=='t');
    break;
  }
  case 1: {
    Get();
    *u=new ExNode(atol(t->val));
    break;
  }
  case 2: {
    Get();
    *u=new ExNode((mrs_real)atof(t->val));
    break;
  }
  case 29: {
    Get();
    Condition(u);
    Expect(30);
    break;
  }
  case 3: case 5: case 6: case 43: case 45: case 48: {
    Sequence(l,u);
    break;
  }
  default: SynErr(75); break;
  }
}

void ExParser::ListElems(ExNode** u) {
  ExNode* v=NULL; std::string type;
  Condition(u);
  while (la->kind == 42) {
    Get();
    Condition(&v);
    if(!fail)*u=list_append(*u,v);
  }
}

void ExParser::Sequence(std::string& l, ExNode** u) {
  FactorB(l,u);
  if(fail)return;
  while (la->kind == 48) {
    ElemAccess(u);
    if(fail)return;
  }
}

void ExParser::ElemAccess(ExNode** u) {
  ExNode* lidx=NULL; ExNode* ridx=NULL; bool is_range=false; std::string nm;
  Expect(48);
  if (la->kind == 1 || la->kind == 5) {
    if (la->kind == 1) {
      Get();
      lidx=new ExNode((mrs_natural)atoi(t->val));
    } else {
      Name(nm);
      lidx=do_name(false,nm,NULL); if(fail)return;
    }
    if (la->kind == 50) {
      Get();
      is_range=true;
      if (la->kind == 1 || la->kind == 5) {
        if (la->kind == 1) {
          Get();
          ridx=new ExNode((mrs_natural)atoi(t->val));
        } else {
          Name(nm);
          ridx=do_name(false,nm,NULL); if(fail) {lidx->deref(); return;}
        }
      }
    }
  } else if (la->kind == 50) {
    Get();
    if (la->kind == 1) {
      Get();
      is_range=true; ridx=new ExNode((mrs_natural)atoi(t->val));
    } else if (la->kind == 5) {
      Name(nm);
      ridx=do_name(false,nm,NULL); if(fail)return;
    } else SynErr(76);
  } else SynErr(77);
  Expect(49);
  *u=do_getelem(*u,lidx,ridx,is_range);
}

void ExParser::FactorB(std::string& l, ExNode** u) {
  std::string nm; ExNode* ps=NULL; if (fail) return;
  switch (la->kind) {
  case 5: {
    Name(nm);
    bool f=false;
    if (la->kind == 29) {
      Get();
      f=true;
      if (StartOf(5)) {
        Exprs(&ps);
      }
      Expect(30);
    }
    if (fail) return;
    if (getKind(nm)==T_LIB) { l=nm+"."; *u=NULL; }
    else *u=do_name(f,nm,ps);

    break;
  }
  case 6: {
    CName(nm);
    *u=do_getctrl(nm);
    break;
  }
  case 48: {
    Get();
    bool mt=true;
    if (StartOf(1)) {
      ListElems(&ps);
      mt=false;
    }
    Expect(49);
    if(fail)return; *u=do_list(mt,ps);
    break;
  }
  case 3: {
    Get();
    *u=new ExNode(prep_string(t->val));
    break;
  }
  case 45: {
    Get();
    ExNode* c=NULL; ExNode* ts=NULL; ExNode* es=NULL;
    Condition(&c);
    if(fail)return;
    Expect(50);
    Exprs(&ts);
    if(fail) {if(c)c->deref(); return;}
    Expect(50);
    Exprs(&es);
    if(fail) {if(c)c->deref(); if(ts)ts->deref(); return;}
    Expect(44);
    *u=do_conditional(c,ts,es);
    break;
  }
  case 43: {
    Get();
    int m=0; std::string var_nm; bool in=false; std::string ary_nm; std::string tp;
    if (la->kind == 54) {
      Get();
      m=1;
    } else if (la->kind == 55) {
      Get();
      m=2;
    } else if (la->kind == 56) {
      Get();
      m=3;
    } else if (la->kind == 57) {
      Get();
      m=4;
    } else SynErr(78);
    Name(var_nm);
    if (la->kind == 58) {
      Get();
      in=true;
    }
    if (la->kind == 50) {
      Get();
      if(!in) {
        MRSWARN("ExParser::Unexpected : in iterator declaration");
        fail=true;
      }
      else if (m==2) ary_nm="in";
      else *u=do_name(false,"in",NULL);

    } else if (m==2) {
      Name(ary_nm);
      Expect(50);
    } else if (StartOf(1)) {
      Condition(u);
      Expect(50);
    } else SynErr(79);
    if(fail)return;
    tp=(m==2) ? tp=getElemType(ary_nm) : (*u)->getElemType();
    // need to insert var nm into symbol table so it
    // can be used in the expression
    symbol_table.block_open();
    ExVal v=ExValTyped(T_VAR,tp);
    symbol_table.setValue(v,var_nm);
    Exprs(&ps);
    Expect(44);
    if(fail) {
      if(*u) {(*u)->deref(); *u=NULL;}
    } else *u=do_iter(m,var_nm,ary_nm,*u,ps);
    symbol_table.block_close();
    break;
  }
  default: SynErr(80); break;
  }
}

void ExParser::Elem(ExNode*& idx) {
  std::string nm;
  Expect(48);
  if (la->kind == 1) {
    Get();
    idx=new ExNode((mrs_natural)atoi(t->val));
  } else if (la->kind == 5) {
    Name(nm);
    idx=do_name(false,nm,NULL);
  } else SynErr(81);
  Expect(49);
}

void ExParser::Use() {
  std::string nm; if(fail)return;
  Expect(59);
  Name(nm);
  symbol_table.import(nm);
  Expect(42);
}

void ExParser::Load() {
  std::string nm; if(fail)return;
  Expect(60);
  Name(nm);
  Expect(42);
}

void ExParser::UL() {
  if (la->kind == 59) {
    Use();
  } else if (la->kind == 60) {
    Load();
  } else SynErr(82);
}

void ExParser::Neil() {
  tree=NULL; std::string nm;
  while (la->kind == 59 || la->kind == 60) {
    UL();
  }
  Exprs(&tree);
  if(fail) { delete tree; tree=NULL; }
}



void ExParser::Parse() {
  t = NULL; fail=false;
  la = dummyToken = new Token();
  la->val = coco_string_create("Dummy Token");
  Get();
  Neil();

  if (!fail)
    Expect(0);
  delete dummyToken;
}
void ExParser::Parse(Scheduler* v, MarSystem* m, ExRecord* est)
{
  scheduler_=v;
  marsystem_=m;
  symbol_table.addTable(est);
  Parse();
}
ExParser::ExParser(TmTimer** t, ExScanner *scanner) {
  _EOF = 0;
  _tnatural = 1;
  _treal = 2;
  _tstr = 3;
  _tbool = 4;
  _tname = 5;
  _cname = 6;
  _rasgn = 7;
  _lasgn = 8;
  _addrasgn = 9;
  _subrasgn = 10;
  _mulrasgn = 11;
  _divrasgn = 12;
  _modrasgn = 13;
  _addlasgn = 14;
  _sublasgn = 15;
  _mullasgn = 16;
  _divlasgn = 17;
  _modlasgn = 18;
  _andrasgn = 19;
  _orrasgn = 20;
  _andlasgn = 21;
  _orlasgn = 22;
  _addop = 23;
  _subop = 24;
  _mulop = 25;
  _divop = 26;
  _modop = 27;
  _power = 28;
  _lbrkt = 29;
  _rbrkt = 30;
  _rlink = 31;
  _llink = 32;
  _notop = 33;
  _eqop = 34;
  _neop = 35;
  _gtop = 36;
  _geop = 37;
  _ltop = 38;
  _leop = 39;
  _andop = 40;
  _orop = 41;
  _exprbrk = 42;
  _blkstart = 43;
  _blkend = 44;
  _ifblk = 45;
  _atsym = 46;
  _propsep = 47;
  _lsbrkt = 48;
  _rsbrkt = 49;
  _colon = 50;
  _streamlib = 51;
  maxT = 61;

  timer_=t;
  minErrDist = 2; fail=false;
  errDist = minErrDist;
  this->scanner = scanner;
  Init();
}

bool ExParser::StartOf(int s) {
  const bool T = true;
  const bool x = false;

  static bool set[6][63] = {
    {T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x},
    {x,T,T,T, T,T,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,x,x,x, x,T,x,x, x,T,x,x, x,x,x,x, x,x,x,T, x,T,x,x, T,x,x,x, T,T,x,x, x,x,x,x, x,x,x},
    {x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,T, T,T,T,x, x,T,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x},
    {x,x,x,x, x,x,x,T, x,T,T,T, T,T,x,x, x,x,x,T, T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x},
    {x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,T, T,T,T,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x},
    {x,T,T,T, T,T,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,x,x,x, x,T,x,x, x,T,x,x, x,x,x,x, x,x,x,T, x,T,T,x, T,x,x,T, T,T,x,x, x,x,x,x, x,x,x}
  };



  return set[s][la->kind];
}

ExParser::~ExParser() {
//    delete library;
}
void ExParser::SynErr(int n) {
  char* s;
  switch (n) {
  case 0: s = coco_string_create("EOF expected"); break;
  case 1: s = coco_string_create("tnatural expected"); break;
  case 2: s = coco_string_create("treal expected"); break;
  case 3: s = coco_string_create("tstr expected"); break;
  case 4: s = coco_string_create("tbool expected"); break;
  case 5: s = coco_string_create("tname expected"); break;
  case 6: s = coco_string_create("cname expected"); break;
  case 7: s = coco_string_create("rasgn expected"); break;
  case 8: s = coco_string_create("lasgn expected"); break;
  case 9: s = coco_string_create("addrasgn expected"); break;
  case 10: s = coco_string_create("subrasgn expected"); break;
  case 11: s = coco_string_create("mulrasgn expected"); break;
  case 12: s = coco_string_create("divrasgn expected"); break;
  case 13: s = coco_string_create("modrasgn expected"); break;
  case 14: s = coco_string_create("addlasgn expected"); break;
  case 15: s = coco_string_create("sublasgn expected"); break;
  case 16: s = coco_string_create("mullasgn expected"); break;
  case 17: s = coco_string_create("divlasgn expected"); break;
  case 18: s = coco_string_create("modlasgn expected"); break;
  case 19: s = coco_string_create("andrasgn expected"); break;
  case 20: s = coco_string_create("orrasgn expected"); break;
  case 21: s = coco_string_create("andlasgn expected"); break;
  case 22: s = coco_string_create("orlasgn expected"); break;
  case 23: s = coco_string_create("addop expected"); break;
  case 24: s = coco_string_create("subop expected"); break;
  case 25: s = coco_string_create("mulop expected"); break;
  case 26: s = coco_string_create("divop expected"); break;
  case 27: s = coco_string_create("modop expected"); break;
  case 28: s = coco_string_create("power expected"); break;
  case 29: s = coco_string_create("lbrkt expected"); break;
  case 30: s = coco_string_create("rbrkt expected"); break;
  case 31: s = coco_string_create("rlink expected"); break;
  case 32: s = coco_string_create("llink expected"); break;
  case 33: s = coco_string_create("notop expected"); break;
  case 34: s = coco_string_create("eqop expected"); break;
  case 35: s = coco_string_create("neop expected"); break;
  case 36: s = coco_string_create("gtop expected"); break;
  case 37: s = coco_string_create("geop expected"); break;
  case 38: s = coco_string_create("ltop expected"); break;
  case 39: s = coco_string_create("leop expected"); break;
  case 40: s = coco_string_create("andop expected"); break;
  case 41: s = coco_string_create("orop expected"); break;
  case 42: s = coco_string_create("exprbrk expected"); break;
  case 43: s = coco_string_create("blkstart expected"); break;
  case 44: s = coco_string_create("blkend expected"); break;
  case 45: s = coco_string_create("ifblk expected"); break;
  case 46: s = coco_string_create("atsym expected"); break;
  case 47: s = coco_string_create("propsep expected"); break;
  case 48: s = coco_string_create("lsbrkt expected"); break;
  case 49: s = coco_string_create("rsbrkt expected"); break;
  case 50: s = coco_string_create("colon expected"); break;
  case 51: s = coco_string_create("streamlib expected"); break;
  case 52: s = coco_string_create("\"true\" expected"); break;
  case 53: s = coco_string_create("\"false\" expected"); break;
  case 54: s = coco_string_create("\"map\" expected"); break;
  case 55: s = coco_string_create("\"iter\" expected"); break;
  case 56: s = coco_string_create("\"for\" expected"); break;
  case 57: s = coco_string_create("\"rfor\" expected"); break;
  case 58: s = coco_string_create("\"in\" expected"); break;
  case 59: s = coco_string_create("\"use\" expected"); break;
  case 60: s = coco_string_create("\"load\" expected"); break;
  case 61: s = coco_string_create("??? expected"); break;
  case 62: s = coco_string_create("invalid AddOp"); break;
  case 63: s = coco_string_create("invalid MulOp"); break;
  case 64: s = coco_string_create("invalid RelOp"); break;
  case 65: s = coco_string_create("invalid LAsgnOp"); break;
  case 66: s = coco_string_create("invalid RAsgnOp"); break;
  case 67: s = coco_string_create("invalid Task"); break;
  case 68: s = coco_string_create("invalid LAsgn"); break;
  case 69: s = coco_string_create("invalid LAsgn"); break;
  case 70: s = coco_string_create("invalid LAsgn"); break;
  case 71: s = coco_string_create("invalid Link"); break;
  case 72: s = coco_string_create("invalid RAsgn"); break;
  case 73: s = coco_string_create("invalid RAsgn"); break;
  case 74: s = coco_string_create("invalid RAsgn"); break;
  case 75: s = coco_string_create("invalid Factor"); break;
  case 76: s = coco_string_create("invalid ElemAccess"); break;
  case 77: s = coco_string_create("invalid ElemAccess"); break;
  case 78: s = coco_string_create("invalid FactorB"); break;
  case 79: s = coco_string_create("invalid FactorB"); break;
  case 80: s = coco_string_create("invalid FactorB"); break;
  case 81: s = coco_string_create("invalid Elem"); break;
  case 82: s = coco_string_create("invalid UL"); break;

  default:
  {
    char format[20];
    coco_sprintf(format, 20, "error %d", n);
    s = coco_string_create(format);
  }
  break;
  }
  MRSWARN("ExParser: Syntax error - line " +ltos(la->line)+ " col " +ltos(la->col)+ ": " +s);
//	printf("ExParser: Syntax error - line %d col %d: %s\n", la->line, la->col, s);
  coco_string_delete(s);
  fail=true;
}
void ExParser::SemErr(char* msg) {
  (void) msg;
  MRSWARN("ExParser: Semantic error - line " +ltos(la->line)+ " col " +ltos(la->col)+ ": " +msg);
//	printf("ExParser: Semantic error - line %d col %d: %s\n", t->line, t->col, msg);
  fail=true;
}


