/*
** Copyright (C) 1998-2007 George Tzanetakis <gtzan@cs.uvic.ca>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#if !defined(MARSYAS_EX_PARSER_H)
#define MARSYAS_EX_PARSER_H

#include <marsyas/expr/ExNode.h>
#include <marsyas/expr/ExSymTbl.h>
#include <marsyas/expr/ExScanner.h>
#include <marsyas/sched/Scheduler.h>
#include <marsyas/system/MarSystem.h>
#include <marsyas/common_header.h>

#include <string>
#include <iostream>
#include <map>

namespace Marsyas {

class ExParser {
private:
  int _EOF;
  int _tnatural;
  int _treal;
  int _tstr;
  int _tbool;
  int _tname;
  int _cname;
  int _rasgn;
  int _lasgn;
  int _addrasgn;
  int _subrasgn;
  int _mulrasgn;
  int _divrasgn;
  int _modrasgn;
  int _addlasgn;
  int _sublasgn;
  int _mullasgn;
  int _divlasgn;
  int _modlasgn;
  int _andrasgn;
  int _orrasgn;
  int _andlasgn;
  int _orlasgn;
  int _addop;
  int _subop;
  int _mulop;
  int _divop;
  int _modop;
  int _power;
  int _lbrkt;
  int _rbrkt;
  int _rlink;
  int _llink;
  int _notop;
  int _eqop;
  int _neop;
  int _gtop;
  int _geop;
  int _ltop;
  int _leop;
  int _andop;
  int _orop;
  int _exprbrk;
  int _blkstart;
  int _blkend;
  int _ifblk;
  int _atsym;
  int _propsep;
  int _lsbrkt;
  int _rsbrkt;
  int _colon;
  int _streamlib;
  int maxT;

  Token *dummyToken;
  int errDist;
  int minErrDist;
  bool fail;

  void SynErr(int n);
  void Get();
  void Expect(int n);
  bool StartOf(int s);
  void ExpectWeak(int n, int follow);
  bool WeakSeparator(int n, int syFol, int repFol);

public:
  ExScanner *scanner;

  Token *t;			// last recognized token
  Token *la;			// lookahead token

  ExNode* tree;
  ExNode* getTree() { return tree; }

  std::map<std::string,std::string> aliases_;
  ExSymTbl symbol_table;

  MarSystem* marsystem_;
  Scheduler* scheduler_;
  TmTimer** timer_;

  bool IsLAsgn()//{{{
  {
    bool x=false;
    if (la->kind==_tname||la->kind==_cname) {
      Token* p = scanner->Peek();
      x= (p->kind==   _lasgn)
         || (p->kind==_addlasgn)
         || (p->kind==_sublasgn)
         || (p->kind==_mullasgn)
         || (p->kind==_divlasgn)
         || (p->kind==_modlasgn);
      scanner->ResetPeek();
    } else if (la->kind==_streamlib) {                          // Stream
      if (scanner->Peek()->kind==_propsep) {          // .
        if (scanner->Peek()->kind==_tname) {        // name
          if (scanner->Peek()->kind==_lasgn) {    // <<
            x=true;
          }
        }
      }
      scanner->ResetPeek();
    } else if (la->kind==_atsym) {
      x= scanner->Peek()->kind==_tname
         && scanner->Peek()->kind==_lasgn;
      scanner->ResetPeek();
    }
    return x;
  }//}}}
  bool IsCNameRAsgnAlias() {//{{{
    bool x=(la->kind==_cname)
           && (scanner->Peek()->kind==_rasgn)
           && (scanner->Peek()->kind==_atsym);
    scanner->ResetPeek();
    return x;
  }//}}}
  bool IsLink()//{{{
  {
    if (la->kind!=_cname) { return false; }
    Token* p = scanner->Peek();
    bool x = (p->kind==_rlink) || (p->kind==_llink);
    scanner->ResetPeek();
    return x;
  }//}}}
  bool in_colon()//{{{
  {
    bool x=false;
    if (la->kind==_tname) {
      if (la->val[0]=='i'&&la->val[1]=='n'&&la->val[2]=='\0') {
        Token* p=scanner->Peek();
        x=p->kind==_colon;
      }
    }
    scanner->ResetPeek();
    return x;
  }//}}}
  std::string prep_string(std::string s)//{{{
  {
    s=s.substr(1,s.length()-2); // remove quotes
    int c=0; bool f=false;
    for (unsigned int i=0; i<s.length(); ++i,++c) {
      if (f) {
        if (s[i]=='n') { c--; s[c]='\n'; }
        if (s[i]=='t') { c--; s[c]='\t'; }
        f=false;
      } else s[c]=s[i];
      if (s[i]==92) { f=true; }
    }
    s=s.substr(0,c);
    return s;
  }//}}}

  void Init()//{{{
  {
    ExRecord* library=new ExRecord();
    load_symbols(library);
    loadlib_timer(library,timer_);
    symbol_table.addTable(library);
    symbol_table.block_open();
  }//}}}

  ExNode* getFunctionCopy(std::string nm, ExNode* params)//{{{
  {
    std::string key=construct_signature(nm, params);
    ExFun* f=symbol_table.getFunctionCopy(key);
    if (f!=NULL) {
      f->setParams(params);
      if (f->is_const()) {
        ExVal xx = f->eval();
        ExNode* e = new ExNode(xx);
        delete(f);
        return e;
      }
    } else {
      MRSWARN("ExParser::getFunctionCopy   unbound function call: "+key);
    }
    return f;
  }//}}}
  int getKind(std::string nm)//{{{
  {
    ExRecord* r=symbol_table.getRecord(nm);
    return (r==NULL) ? 0 : r->getKind();
  }//}}}
  std::string getType(std::string nm)//{{{
  {
    ExRecord* r=symbol_table.getRecord(nm);
    return (r==NULL) ? "" : r->getType();
  }//}}}
  std::string getElemType(std::string nm)//{{{
  {
    ExRecord* r=symbol_table.getRecord(nm);
    return (r==NULL) ? "" : r->getElemType();
  }//}}}
  std::string getDefaultLib(std::string typ)//{{{
  {
    if (typ=="mrs_string") return "String";
    if (typ=="mrs_real") return "Real";
    if (typ=="mrs_natural") return "Natural";
    if (typ=="mrs_bool") return "Bool";
    if (typ=="mrs_timer") return "Timer";
    int len=(int)typ.length();
    if ((len>4)&&typ[len-1]=='t'&&typ[len-2]=='s'&&typ[len-3]=='i'&&typ[len-4]=='l'&&typ[len-5]==' ') return "List";
    return "";
  }//}}}

  /****************************************************************************/
  ExNode* expr_append(ExNode* u, ExNode* v)//{{{
  {
    if (u==NULL) return v;
    ExNode* x=u; while (u->next!=NULL) { u=u->next; } u->next=v;
    return x;
  } //}}}
  std::string exprs_type(ExNode* es)//{{{
  {
    if (es==NULL) return "";
    while (es->next!=NULL) { es=es->next; }
    return es->getType();
  } //}}}
  std::string construct_signature(std::string nm, ExNode* params)//{{{
  {
    std::string key=nm;
    key+="(";
    ExNode* ps=params;
    while (ps!=NULL) {
      std::string tp=ps->getType();
      key+=tp;
      if (tp=="mrs_natural") key+="|mrs_real";
      if (ps->is_list()) key+="|mrs_list";
      ps=ps->next;
      if (ps!=NULL) key+=",";
    }
    key+=")";
    return key;
  } //}}}
  bool is_alias(std::string nm) { return (aliases_.find(nm) != aliases_.end()); }
  bool is_num(std::string n)//{{{
  {
    for (unsigned int i=0; i<n.length(); ++i) {
      if (n[i]<'0'||n[i]>'9') return false;
    }
    return true;
  } //}}}
  ExNode* assignment(ExNode* u, ExRecord* r) {//{{{
    std::string rt=r->getType(); std::string ut=u->getType();
    if (rt=="mrs_real"&&ut=="mrs_natural") u=new ExNode_NaturalToReal(u);
    else if (rt=="mrs_natural"&&ut=="mrs_real") u=new ExNode_RealToNatural(u);
    if (rt==u->getType()) u=new ExNode_AsgnVar(u,r);
    else {
      MRSWARN("ExParser: Type mismatch in assignment: "+r->getType()+" << "+u->getType());
      fail=true; u->deref(); return NULL;
    }
    return u;
  }//}}}
  ExNode* do_asgn(std::string nm, ExNode* u)//{{{
  {
    if (is_alias(nm)) { return do_casgn(nm,u); }
    std::string t = getType(nm);
    // compare name type with u
    if (t=="") { // make a new variable in symbol table
      ExVal v = ExValTyped(T_VAR,u->getType());
      symbol_table.setValue(v,nm);
      t=u->getType();
    }
    ExRecord* r=symbol_table.getRecord(nm);
    return assignment(u,r);
  } //}}}
  ExNode* do_masgn(int atype, bool right_assign, std::string nm, ExNode* u)//{{{
  {
    if (is_alias(nm)) { return do_cmasgn(atype,right_assign,nm,u); }
    // get name type, it must exist because we need read it
    std::string rt=getType(nm);
    if (rt=="") {
      MRSWARN("ExParser: Unbound name '"+nm+"'");
      fail=true; u->deref(); return NULL;
    }
    ExRecord* r=symbol_table.getRecord(nm);
    ExNode* v=new ExNode_ReadVar(r,nm);
    if (!right_assign) { ExNode* a=u; u=v; v=a; }
    if (atype==OP_ADD||atype==OP_SUB) { u=do_addop(atype,u,v); }
    else if (atype==OP_MUL||atype==OP_DIV||atype==OP_MOD) { u=do_mulop(atype,u,v); }
    else { u=do_condop(atype,u,v); }
    if (u==NULL) return NULL;
    return assignment(u,r);
  }//}}}
  // assignment
  ExNode* do_alias(std::string anm, std::string cnm) { aliases_[anm]=cnm; return NULL; }
  ExNode* do_casgn(std::string nm, ExNode* u)//{{{
  {
    if (is_alias(nm)) nm=aliases_[nm];
    if (marsystem_->hasControl(nm)) {
      MarControlPtr p = marsystem_->getctrl(nm);
      // does p exist
      std::string t = p->getType();
      std::string ut= u->getType();
      // compare name type with u
      if (t=="mrs_real") {
        if (ut=="mrs_real") u=new ExNode_SetCtrlReal(nm,p,u);
        else if (ut=="mrs_natural") u=new ExNode_SetCtrlReal(nm,p,new ExNode_NaturalToReal(u));
        else {
          MRSWARN("ExParser: Cannot assign type '"+ut+"' to "+t);
          fail=true; u->deref(); return NULL;
        }
      }
      else if (t=="mrs_natural") {
        if (ut=="mrs_natural") u=new ExNode_SetCtrlNatural(nm,p,u);
        else {
          MRSWARN("ExParser: Cannot setctrl type '"+ut+"' to "+t);
          fail=true; u->deref(); return NULL;
        }
      }
      else if (t=="mrs_bool"&&ut=="mrs_bool") {
        u=new ExNode_SetCtrlBool(nm,p,u);
      }
      else if (t=="mrs_string"&&ut=="mrs_string") {
        u=new ExNode_SetCtrlString(nm,p,u);
      }
      else {
        MRSWARN("ExParser: Unknown types in setctrl");
        fail=true; u->deref(); return NULL;
      }
    } else {
      MRSWARN("ExParser: '"+nm+"' does not exist");
      fail=true; u->deref(); return NULL;
    }
    return u;
  } //}}}
  ExNode* do_cmasgn(int atype, bool right_assign, std::string nm, ExNode* u)//{{{
  {
    if (is_alias(nm)) nm=aliases_[nm];
    ExNode* v=do_getctrl(nm);
    if (v==NULL) return NULL;
    if (!right_assign) { ExNode* a=u; u=v; v=a; }
    if (atype==OP_ADD||atype==OP_SUB) { u=do_addop(atype,u,v); }
    else if (atype==OP_MUL||atype==OP_DIV||atype==OP_MOD) { u=do_mulop(atype,u,v); }
    else { u=do_condop(atype,u,v); }
    if (u==NULL) return NULL;
    return do_casgn(nm,u);
  } //}}}
  ExNode* do_getctrl(std::string nm)//{{{
  {
    if (marsystem_==NULL) {
      MRSWARN("ExParser: Control Name defined on NULL MarSystem");
      fail=true; return NULL;
    }
    if (marsystem_->hasControl(nm)) {
      MarControlPtr ptr=marsystem_->getctrl(nm);
      std::string t=ptr->getType();
      if (t=="mrs_bool") { return new ExNode_GetCtrlBool(nm,ptr); }
      else if (t=="mrs_string") { return new ExNode_GetCtrlString(nm,ptr); }
      else if (t=="mrs_natural") { return new ExNode_GetCtrlNatural(nm,ptr); }
      else if (t=="mrs_real") { return new ExNode_GetCtrlReal(nm,ptr); }
    }
    MRSWARN("ExParser: getctrl on '"+nm+"' failed.");
    fail=true;
    return NULL;
  }//}}}
  // operators
  ExNode* do_mulop(int m, ExNode* u, ExNode* v)//{{{
  {
    std::string ut=u->getType(); std::string vt=v->getType();
    std::string t=ut;
    if (ut=="mrs_real"&&vt=="mrs_natural") { v=new ExNode_NaturalToReal(v); vt="mrs_real"; }
    else if (vt=="mrs_real"&&ut=="mrs_natural") { u=new ExNode_NaturalToReal(u); ut="mrs_real"; t=ut; }

    if (ut!="mrs_real"&&ut!="mrs_natural") {
      MRSWARN("ExParser::mult  Expected mrs_real|mrs_natural types, got "+ut+" & "+vt);
      fail=true; u->deref(); v->deref(); return NULL;
    }
    ExNode* w=NULL;
    // check for const and evaluate if possible
    bool is_const = (u->is_const()&&v->is_const());
    if (m==OP_MUL) {
      if (is_const) { w=new ExNode(u->value*v->value); }
      else { u=new ExNode_MUL(t,u,v); }
    }
    else if (m==OP_DIV) {
      if (is_const) { w=new ExNode(u->value/v->value); }
      else { u=new ExNode_DIV(t,u,v); }
    }
    else if (m==OP_MOD) {
      if (is_const) { w=new ExNode(u->value%v->value); }
      else { u=new ExNode_MOD(t,u,v); }
    }
    if (w!=NULL) { u->deref(); v->deref(); u=w; }
    return u;
  } //}}}
  ExNode* do_addop(int m, ExNode* u, ExNode* v)//{{{
  {
    std::string ut=u->getType(); std::string vt=v->getType();
    // first step conversion, so that u and t are of the same type
    if (ut=="mrs_real"&&vt=="mrs_natural") { v=new ExNode_NaturalToReal(v); vt="mrs_real"; }
    else if (vt=="mrs_real"&&ut=="mrs_natural") { u=new ExNode_NaturalToReal(u); ut="mrs_real"; }
    else if (m==OP_ADD) {
      if (ut=="mrs_string") {
        if (vt=="mrs_real") {
          if (v->is_const()) { v->setValue(dtos(v->getValue().toReal())); }
          else { v=new ExNode_RealToString(v); }
        }
        else if (vt=="mrs_natural") {
          if (v->is_const()) { v->setValue(ltos(v->getValue().toNatural())); }
          else { v=new ExNode_NaturalToString(v); }
        }
        else if (vt=="mrs_bool") {
          if (v->is_const()) { v->setValue(btos(v->getValue().toBool())); }
          else { v=new ExNode_NaturalToString(v); }
        }
        vt="mrs_string";
      }
      else if (vt=="mrs_string") {
        if (ut=="mrs_real") {
          if (u->is_const()) { u->setValue(dtos(u->getValue().toReal())); }
          else { u=new ExNode_RealToString(u); }
        }
        else if (ut=="mrs_natural") {
          if (u->is_const()) { u->setValue(ltos(u->getValue().toNatural())); }
          else { u=new ExNode_NaturalToString(u); }
        }
        else if (ut=="mrs_bool") {
          if (u->is_const()) { u->setValue(btos(u->getValue().toBool())); }
          else { u=new ExNode_NaturalToString(u); }
        }
        ut="mrs_string";
      }
    }
    bool is_const = (u->is_const()&&v->is_const());
    ExNode* w=NULL;
    // type check operator
    bool list_t=false;
    if (u->is_list()&&v->is_list()) {
      if (ut==" list"&&vt!=" list") { ut=vt; }
      else if (vt==" list"&&ut!=" list") { vt=ut; }
      list_t=true;
    }
    if (ut==vt) {
      bool addable=
        (  ut=="mrs_real"
           || ut=="mrs_natural"
           || ut=="mrs_string"
           || list_t
        );
      if (m==OP_ADD&&addable) {
        if (is_const) { w=new ExNode(u->getValue()+v->getValue()); }
        else { u=new ExNode_ADD(ut,u,v); }
      }
      else if (m==OP_SUB&&(ut=="mrs_real"||ut=="mrs_natural")) {
        if (is_const) { w=new ExNode(u->getValue()-v->getValue()); }
        else { u=new ExNode_SUB(ut,u,v); }
      }
      else {
        MRSWARN("ExParser: Invalid types to addop: "+ut+" and "+vt);
        fail=true; delete u; delete v; return NULL;
      }
    }
    else {
      MRSWARN("ExParser: Invalid types to addop: "+ut+" and "+vt);
      fail=true; delete u; delete v; return NULL;
    }
    if (w!=NULL) { delete u; delete v; u=w; }
    return u;
  } //}}}
  ExNode* do_num_negate(ExNode* u)//{{{
  {
    if (u->getType()=="mrs_real") {
      if (u->is_const()) { (u->value).set(-(u->value).toReal()); }
      else { u=new ExNode_MathNeg_Real(u); }
    } else if (u->getType()=="mrs_natural") {
      if (u->is_const()) { (u->value).set(-(u->value).toNatural()); }
      else { u=new ExNode_MathNeg_Natural(u); }
    } else {
      MRSWARN("ExParser: Type mismatch in unary math negation operator");
      fail=true; delete u; return NULL;
    }
    return u;
  } //}}}
  ExNode* do_relop(int m, ExNode* u, ExNode* v)//{{{
  {
    if (u->getType()!=v->getType()) {
      MRSWARN("ExParser: Type mismatch to relational operator.");
      fail=true; delete u; delete v;
      return NULL;
    }
    bool is_const = (u->is_const()&&v->is_const());
    if (m==OP_EQ) {
      printf("@ eq\n");
      if (is_const) { ExVal a=u->value == v->value; u->deref(); v->deref(); u=new ExNode(a); }
      else { u=new ExNode_EQ("mrs_bool",u,v); }
    }
    else if (m==OP_NE) {
      if (is_const) { ExVal a=u->value != v->value; u->deref(); v->deref(); u=new ExNode(a); }
      else { u=new ExNode_NE("mrs_bool",u,v); }
    }
    else if (m==OP_GT) {
      if (is_const) { ExVal a=u->value > v->value; u->deref(); v->deref(); u=new ExNode(a); }
      else { u=new ExNode_GT("mrs_bool",u,v); }
    }
    else if (m==OP_GE) {
      if (is_const) { ExVal a=u->value >= v->value; u->deref(); v->deref(); u=new ExNode(a); }
      else { u=new ExNode_GE("mrs_bool",u,v); }
    }
    else if (m==OP_LT) {
      if (is_const) { ExVal a=u->value < v->value; u->deref(); v->deref(); u=new ExNode(a); }
      else { u=new ExNode_LT("mrs_bool",u,v); }
    }
    else if (m==OP_LE) {
      if (is_const) { ExVal a=u->value <= v->value; u->deref(); v->deref(); u=new ExNode(a); }
      else { u=new ExNode_LE("mrs_bool",u,v); }
    }
    return u;
  } //}}}
  ExNode* do_bool_negate(ExNode* v)//{{{
  {
    if (v->is_const()) { (v->value).set(!(v->value).toBool()); }
    else v=new ExNode_BoolNeg(v);
    return v;
  } //}}}
  ExNode* do_condop(int o, ExNode* u, ExNode* v)//{{{
  {
    if (u->getType()=="mrs_bool"&&v->getType()=="mrs_bool") {
      if (u->is_const()&&v->is_const()) {
        if (o==OP_AND) { u->value.set(u->value.toBool() && v->value.toBool()); }
        else { u->value.set(u->value.toBool() || v->value.toBool()); }
        delete v;
      }
      else if (o==OP_AND) u=new ExNode_AND("mrs_bool",u,v);
      else u=new ExNode_OR("mrs_bool",u,v);
    }
    else {
      MRSWARN("ExParser: Types to relational operator must bool.");
      fail=true; delete u; delete v; u=NULL;
    }
    return u;
  } //}}}
  ExNode* do_conditional(ExNode* cond, ExNode* ts, ExNode* es)//{{{
  {
    if (exprs_type(cond)!="mrs_bool") {
      MRSWARN("ExParser: Condition in conditional statement must be of type bool");
      fail=true; delete cond; delete ts; delete es;
      return NULL;
    }
    std::string tt=exprs_type(ts);
    std::string et=exprs_type(es);
    if (et!=tt||tt.length()==0||et.length()==0) {
      MRSWARN("ExParser: Type Mismatch in function");
      fail=true; delete cond; delete ts; delete es;
      return NULL;
    }
    return new ExNode_Conditional(tt,cond,ts,es);
  } //}}}
  ExNode* do_link(std::string f, std::string t)//{{{
  {
    if (marsystem_==NULL) {
      MRSWARN("ExParser: Control Name defined on NULL MarSystem");
      fail=true; return NULL;
    }
    if (marsystem_->hasControl(f)&&marsystem_->hasControl(t)) {
      MarControlPtr pf = marsystem_->getctrl(f);
      MarControlPtr pt = marsystem_->getctrl(t);
      if (pf.isInvalid()||pt.isInvalid()) {
        MRSWARN("ExParser: Cannot link controls '"+f+"' -> '"+t+"'");
        fail=true; return NULL;
      }
      if (pf->getType()!=pt->getType()) {
        MRSWARN("ExParser: linkctrl type mismatch between '"+f+"' -> '"+t+"'");
        fail=true; return NULL;
      }
      std::string tp = marsystem_->getctrl(f)->getType();
      return new ExNode_Link(pf,pt,tp);
    }
    MRSWARN("ExParser: Link controls '"+f+"' -> '"+t+"' failed.");
    fail=true; return NULL;
  }//}}}
  ExNode* do_name(bool is_fun, std::string key, ExNode* params)//{{{
  {
    (void) is_fun; // FIXME Unused parameter
    if (is_alias(key)) return do_getctrl(aliases_[key]);
    int kind=getKind(key); ExNode* u=NULL;
    if (kind==T_FUN) {
      u=getFunctionCopy(key,params);
    }
    else if (kind==T_VAR||kind==T_CONST) {
      ExRecord* nd=symbol_table.getRecord(key);
      u=new ExNode_ReadVar(nd,key);
    }
    if (u==NULL) {
      MRSWARN("ExParser::do_name("+ltos(la->col)+")  Unbound name '"+key+"'");
      fail=true; delete params; return NULL;
    }
    return u;
  } //}}}
  ExNode* do_property(ExNode* u, std::string key, ExNode* params)//{{{
  {
    if (u!=NULL) {
      key=getDefaultLib(u->getType())+"."+key;
      u->next=params; params=u; u=NULL;
    }
    int kind=getKind(key);
    if (kind==T_CONST) {
      /// for example: Real.pi or Timer.cur
      if (params!=NULL) {
        MRSWARN("ExParser::property   parameters supplied to non-function call: "+key);
        delete params; delete u; fail=true; return NULL;
      }
      ExVal v=symbol_table.getValue(key);
      u=new ExNode(v);
    } else if (kind==T_VAR) {
      /// for example: Real.pi or Timer.cur
      if (params!=NULL) {
        MRSWARN("ExParser::property   parameters supplied to non-function call: "+key);
        delete params; delete u; fail=true; return NULL;
      }
      ExRecord* r=symbol_table.getRecord(key);
      u=new ExNode_ReadVar(r,key);
    } else if (kind==T_FUN) {
      u=getFunctionCopy(key,params);
      if (u==NULL) {
//                MRSWARN("ExParser::property   unbound function call: "+key);
        params->deref(); fail=true; return NULL;
      }
    } else {
      MRSWARN("ExParser::property   unbound name: "+key);
      delete u; delete params; fail=true; u=NULL;
    }
    return u;
  }//}}}
  ExNode* do_getelem(ExNode* u, ExNode* lidx, ExNode* ridx, bool is_range)//{{{
  {
    if (!u->is_seq()) {
      MRSWARN("ExParser::getelem  not a sequence type");
      fail=true; u->deref(); lidx->deref();
      if (ridx)ridx->deref(); return NULL;
    }
//        int kind=u->getKind();
//        if (kind==T_VAR||kind==T_CONST) {
    if (is_range) u=new ExNode_Range(u,lidx,ridx);
    else u=new ExNode_GetElem(u,lidx);
//        }
    return u;
  }//}}}
  ExNode* do_setelem(std::string key, ExNode* lidx, ExNode* ridx, bool is_range, ExNode* u)//{{{
  {
    if (getKind(key)==T_VAR) {
      ExRecord* nd=symbol_table.getRecord(key);
      if (nd==NULL) {
        MRSWARN("ExParser::setelem  unbound name "+key);
        u->deref(); lidx->deref(); if(ridx)ridx->deref(); fail=true; u=NULL;
      } else if (nd->is_seq()) {
        if (nd->getElemType()==u->getType()) {
          if (is_range) {
            MRSWARN("ExParser::setelem  setting element as range not supported");
            u->deref(); u=NULL; lidx->deref(); if(ridx)ridx->deref(); fail=true;
          } else {
            u=new ExNode_SetElem(nd,lidx,u);
          }
        } else {
          MRSWARN("ExParser::setelem  type mismatch in setelem");
          u->deref(); u=NULL; lidx->deref(); if(ridx)ridx->deref(); fail=true;
        }
      } else {
        MRSWARN("ExParser::setelem  not a sequence type");
        fail=true; u->deref(); u=NULL; lidx->deref(); if(ridx)ridx->deref();
      }
    }
    return u;
  }//}}}
  ExNode* do_msetelem(std::string key, ExNode* lidx, ExNode* ridx, bool is_range, bool right_assign, int atype, ExNode* u)//{{{
  {
    // get name type, it must exist because we need read it
    std::string rt=getElemType(key);
    if (rt=="") {
      MRSWARN("ExParser: Unbound name '"+key+"'");
      fail=true; u->deref(); return NULL;
    }
    ExNode* v=NULL;
    ExRecord* nd=symbol_table.getRecord(key);
    if (nd==NULL) {
      MRSWARN("ExParser::getelem  unbound name "+key); fail=true;
    } else {
      v=do_getelem(new ExNode_ReadVar(nd,key),lidx,ridx,is_range);
    }

    if (v!=NULL) {
      if (!right_assign) { ExNode* a=u; u=v; v=a; }
      if (atype==OP_ADD||atype==OP_SUB) { u=do_addop(atype,u,v); }
      else if (atype==OP_MUL||atype==OP_DIV||atype==OP_MOD) { u=do_mulop(atype,u,v); }
      else { u=do_condop(atype,u,v); }
      if (u==NULL) return NULL;
      return do_setelem(key,lidx,ridx,is_range,u);
    }
    return NULL;
  }//}}}
  ExNode* list_append(ExNode* u, ExNode* v)//{{{
  {
    if (u==NULL) return v;
    if (u->getType()!=v->getType()) {
      MRSWARN("ExParser::list_append  type mismatch in list declaration");
      u->deref(); v->deref(); fail=true; return NULL;
    }
    ExNode* x=u; while (u->next!=NULL) { u=u->next; } u->next=v;
    return x;
  }//}}}
  ExNode* do_list(bool is_empty, ExNode* u)//{{{
  {
    if (is_empty) { return new ExNode(ExVal(0,NULL)); }
    mrs_natural len=0; ExNode* x=u; while (x!=NULL) { len++; x=x->next; }
    ExNode** elems=new ExNode*[len];
    int l=0; x=u; while (x!=NULL) { ExNode* y=x; elems[l]=x; l++; x=x->next; y->next=NULL; }
    return new ExNode(ExVal(len,(ExNode**)elems));
  }//}}}
  ExNode* do_iter(int iter_type, std::string var_nm, std::string ary_nm, ExNode* list, ExNode* exprs)//{{{
  {
    ExRecord* var=symbol_table.getRecord(var_nm);
    if (list && !list->is_seq()) {
      MRSWARN("ExParser::iterator  Expected sequence type to iterator");
      list->deref(); exprs->deref();
      fail=true;
      return NULL;
    }
    ExNode* e=NULL;
    if (iter_type==1) { // map
      if (list->getType()=="mrs_string") {
        e=new ExNode_StringMap(list,var,exprs,"mrs_string");
      } else {
        std::string rt=exprs_type(exprs)+" list";
        e=new ExNode_IterMap(list,var,exprs,rt);
      }
    }
    else if (iter_type==2) { // iter
      // note that list is null in this case, ary_nm is key to the list
      ExRecord* original=symbol_table.getRecord(ary_nm);
      if (original->getType()=="mrs_string") {
        e=new ExNode_StringIter(original,var,exprs);
      } else {
        if (original==NULL) {
          if (exprs) exprs->deref();
        }
        e=new ExNode_IterIter(original,var,exprs);
      }
    }
    else if (iter_type==3) { // for
      if (list->getType()=="mrs_string") {
        e=new ExNode_StringFor(list,var,exprs);
      } else {
        e=new ExNode_IterFor(list,var,exprs);
      }
    }
    else if (iter_type==4) { // rfor
      if (list->getType()=="mrs_string") {
        e=new ExNode_StringRFor(list,var,exprs);
      } else {
        e=new ExNode_IterRFor(list,var,exprs);
      }
    }
    return e;
  }//}}}

  /******************************************************************/


  ExParser(TmTimer** t, ExScanner *scanner);
  ~ExParser();
  void SemErr(char* msg);

  void Alias(std::string& nm);
  void Name(std::string& nm);
  void CName(std::string& nm);
  void AddOp(int& m);
  void MulOp(int& m);
  void RelOp(int& m);
  void LAsgnOp(int& type);
  void RAsgnOp(int& type);
  void Exprs(ExNode** u);
  void Task(ExNode** u);
  void LAsgn(ExNode** u);
  void Link(ExNode** u);
  void RAsgn(ExNode** u);
  void Condition(ExNode** u);
  void CondTerm(ExNode** u);
  void CondFact(ExNode** u);
  void Expr(ExNode** u);
  void Term(ExNode** u);
  void Property(ExNode** u);
  void Factor(std::string& l, ExNode** u);
  void ListElems(ExNode** u);
  void Sequence(std::string& l, ExNode** u);
  void ElemAccess(ExNode** u);
  void FactorB(std::string& l, ExNode** u);
  void Elem(ExNode*& idx);
  void Use();
  void Load();
  void UL();
  void Neil();

  void Parse();
  void Parse(Scheduler* v, MarSystem* m, ExRecord* est);

}; // end ExParser

}; // namespace

#endif // !defined(MARSYAS_EX_PARSER_H)

