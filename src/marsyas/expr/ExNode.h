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

/***
ExNode represents an expression tree node. Additional nodes may be added later,
but may also need to be added to the parser.

There should only ever exist a single parent of any node, that is, a node may
only be referenced by one object.

To add library functions add a line like this to ExParser.h::preload() :-
library->addRecord("Real|R.cos(mrs_real)|cos(mrs_natural)",new ExRecord(T_FUN,new ExNode_RealCos("mrs_real","Real.cos(mrs_real)"),true));
then define the function as an ExNode class, you must support the calc and copy
functions. In the constructor make sure you set is_pure to true if the function
can be reduced to a const value given const parameters, or false otherwise.

class ExNode_NatDbl : public ExNode_Fun {
    ExNode* child; public:
    ExNode_NatDbl(std::string typ, std::string sig, ExNode* x) : ExNode_Fun(typ,sig,true) { child=x; }
    virtual ExVal calc() { return false; }
    ExNode* copy() { return new ExNode_NatDbl(type,signature); }
};


***/
#ifndef MARSYAS_EX_NODE_H
#define MARSYAS_EX_NODE_H

#include <marsyas/expr/ExVal.h>
#include <marsyas/expr/ExSymTbl.h>
#include <marsyas/expr/ExCommon.h>
#include <marsyas/sched/TmTimer.h>
#include <marsyas/system/MarControl.h>
#include <marsyas/system/MarSystem.h>
#include <marsyas/common_header.h>

#include <string>
#include <iostream>
#include <cstdlib>


namespace Marsyas
{
/**
	\class ExNode
	\ingroup Scheduler
	\brief ExNode is the base class for an expression tree node.

	ExNode represents an expression tree node. Additional nodes may be
	added later, but may also need to be added to the parser.

	There should only ever exist a single parent of any node, that is, a
	node may only be referenced by one object.

	To add library functions add a line like this to ExParser.h::preload() :-
	<code>library->addRecord("Real|R.cos(mrs_real)|cos(mrs_natural)",new ExRecord(T_FUN,new ExNode_RealCos("mrs_real","Real.cos(mrs_real)"),true));</code>
	then define the function as an ExNode class, you must support the calc and copy
	functions. In the constructor make sure you set is_pure to true if the function
	can be reduced to a const value given const parameters, or false otherwise.
<pre>
class ExNode_NatDbl : public ExNode_Fun {
    ExNode* child; public:
    ExNode_NatDbl(std::string typ, std::string sig, ExNode* x) : ExNode_Fun(typ,sig,true) { child=x; }
    virtual ExVal calc() { return false; }
    ExNode* copy() { return new ExNode_NatDbl(type,signature); }
};
</pre>

   \author Neil Burroughs  inb@cs.uvic.ca
   \version 1.0
   \date    Jan 04, 2007
*/

void loadlib_Real(ExRecord* st);
void loadlib_String(ExRecord* st);
void loadlib_Natural(ExRecord* st);
void loadlib_Stream(ExRecord* st);
void loadlib_List(ExRecord* st);
void load_symbols(ExRecord*);
void loadlib_timer(ExRecord* st, TmTimer** tmr);

class ExNode : public ExRefCount {
  std::string type; int kind;
public:
  std::string val_str;
  ExVal value;

  ExNode* next; // expression(s)

  ExNode();
  ExNode(int k, std::string t);
  ExNode(int k, std::string t, ExVal v);
  ExNode(ExVal v);
  ExNode(const ExNode& v);
  virtual ~ExNode();

  void init();

  virtual bool is_const();
  bool is_list() const;
  bool is_seq() const;

  std::string getType() const;
  std::string getEvalType() const;
  void setType(const std::string t);
  int getKind() const { return kind; }
  void setKind(const int k);

  virtual ExNode* copy();

  ExVal getValue() { return value; }
  mrs_natural valToNatural() {return value.toNatural();}

  void setValue(mrs_natural x) {value.set(x); setKind(T_CONST); setType("mrs_natural");}
  void setValue(std::string x) {value.set(x); setKind(T_CONST); setType("mrs_string");}
  void setValue(mrs_real x) {value.set(x); setKind(T_CONST); setType("mrs_real");}
  void setValue(mrs_bool x) {value.set(x); setKind(T_CONST); setType("mrs_bool");}

  virtual std::string toString();
  virtual std::string oot();
  virtual ExVal eval();
  virtual ExVal calc() { return value;  }

  ExVal getSeqRange(int lidx, int ridx);
  ExVal getSeqElem(int idx);
  void setSeqElem(int idx, ExVal v);
  std::string getElemType() const;
};
/*** Unary Operators *********************************************************/
// Unary Operators {{{
#define UNARYOP(_NM,_KIND,_TYPE,_TO,_OP) \
class ExNode_##_NM : public ExNode { \
	ExNode* child; public: \
	ExNode_##_NM(ExNode* v) : ExNode(_KIND,_TYPE) { child=v; } \
	virtual ~ExNode_##_NM() { child->deref(); } \
	virtual ExVal calc() { return _OP((child->eval())._TO); } \
};
UNARYOP(MathNeg_Real   ,OP_MNEG,"mrs_real"   ,toReal()   ,-);
UNARYOP(MathNeg_Natural,OP_MNEG,"mrs_natural",toNatural(),-);
UNARYOP(BoolNeg        ,OP_BNEG,"mrs_bool"   ,toBool()   ,!);
//}}}
/*** Conversions *************************************************************/
// Conversions {{{
UNARYOP(NaturalToReal  ,OP_CONV,"mrs_real"   ,toNatural(),(mrs_real)   );
UNARYOP(RealToNatural  ,OP_CONV,"mrs_natural",toReal()   ,(mrs_natural));
UNARYOP(RealToString   ,OP_CONV,"mrs_string" ,toReal()   ,dtos         );
UNARYOP(NaturalToString,OP_CONV,"mrs_string" ,toNatural(),ltos         );
UNARYOP(BoolToString   ,OP_CONV,"mrs_string" ,toBool()   ,btos         );
//}}}
/*** Binary Operators ********************************************************/
// Binary Operators {{{
#define BINOP(_NM,_KIND,_OP) \
class ExNode_##_NM : public ExNode { \
	ExNode* lchild; ExNode* rchild; std::string d; public: \
	ExNode_##_NM(std::string t, ExNode* u, ExNode* v) : ExNode(_KIND,t) { \
		lchild=u; rchild=v; \
		if (getType()=="mrs_real") { d="d"; } else d=""; \
	} \
	virtual ~ExNode_##_NM() { lchild->deref(); rchild->deref(); } \
	virtual ExVal calc() { return (lchild->eval()) _OP (rchild->eval()); } \
};
BINOP(ADD,OP_ADD,+);
BINOP(SUB,OP_SUB,-);
BINOP(MUL,OP_MUL,*);
BINOP(DIV,OP_DIV,/);
BINOP(MOD,OP_MOD,%);
BINOP(EQ,OP_EQ,==);
BINOP(NE,OP_NE,!=);
BINOP(GT,OP_GT,> );
BINOP(GE,OP_GE,>=);
BINOP(LT,OP_LT,< );
BINOP(LE,OP_LE,<=);
BINOP(OR,OP_OR,||);
BINOP(AND,OP_AND,&&);
//}}}
/*** Conditional *************************************************************/
class ExNode_Conditional : public ExNode//{{{
{
ExNode* cond; ExNode* then_; ExNode* else_; public:
  ExNode_Conditional(std::string t, ExNode* c, ExNode* ts, ExNode* es) : ExNode(T_COND,t) {
    cond=c; then_=ts; else_=es;
  }
  ~ExNode_Conditional() { cond->deref(); then_->deref(); else_->deref(); }
  virtual ExVal calc() {
    ExVal v = cond->eval();
    return (v.toBool()) ? then_->eval() : else_->eval();
  }
};//}}}
/*****************************************************************************/
/// map : iterate over list creating new list of same size without destroying original
class ExNode_IterMap : public ExNode//{{{
{
ExNode* xs; ExRecord* var; ExNode* exprs; public:
  ExNode_IterMap(ExNode* s, ExRecord* r, ExNode* e, std::string t) : ExNode(T_VAR,t) {
    xs=s; var=r; var->inc_ref(); exprs=e;
  }
  virtual ~ExNode_IterMap() { xs->deref(); var->deref(); exprs->deref(); }
  virtual ExVal calc() {
    ExVal liszt=xs->eval();
    mrs_natural len=liszt.toNatural();
    ExNode** new_xs=NULL;
    if (len>0) {
      new_xs=new ExNode*[len];
      for (int i=0; i<len; ++i) {
        ExVal e=liszt.getSeqElem(i);
        var->setValue(e);
        ExVal v=exprs->eval();
        new_xs[i]=new ExNode(v);
      }
    }
    return ExVal(len,new_xs);
  }
};//}}}
/// iter : iterate over list and replace each element in original list
class ExNode_IterIter : public ExNode//{{{
{
ExRecord* xs; ExRecord* var; ExNode* exprs; public:
  ExNode_IterIter(ExRecord* s, ExRecord* r, ExNode* e) : ExNode(T_VAR,"mrs_unit") {
    xs=s; xs->inc_ref(); var=r; var->inc_ref(); exprs=e;
  }
  virtual ~ExNode_IterIter() { xs->deref(); var->deref(); exprs->deref(); }
  virtual ExVal calc() {
    ExVal liszt=xs->getValue();
    mrs_natural len=liszt.toNatural();
    if (len>0) {
      for (int i=0; i<len; ++i) {
        ExVal e=liszt.getSeqElem(i);
        var->setValue(e);
        ExVal v=exprs->eval();
        xs->setValue(v,"",i);
      }
    }
    return ExVal();
  }
};//}}}
/// iterate over list and do something with each value in the list, not destroying old list
class ExNode_IterFor : public ExNode//{{{
{
ExNode* xs; ExRecord* var; ExNode* exprs; public:
  ExNode_IterFor(ExNode* s, ExRecord* r, ExNode* e) : ExNode(T_VAR,"mrs_unit") {
    xs=s; var=r; var->inc_ref(); exprs=e;
  }
  virtual ~ExNode_IterFor() { xs->deref(); var->deref(); exprs->deref(); }
  virtual ExVal calc() {
    ExVal liszt=xs->eval();
    mrs_natural len=liszt.toNatural();
    if (len>0) {
      for (int i=0; i<len; ++i) {
        ExVal e=liszt.getSeqElem(i);
        var->setValue(e);
        exprs->eval();
      }
    }
    return ExVal();
  }
};//}}}
class ExNode_IterRFor : public ExNode//{{{
{
ExNode* xs; ExRecord* var; ExNode* exprs; public:
  ExNode_IterRFor(ExNode* s, ExRecord* r, ExNode* e) : ExNode(T_VAR,"mrs_unit") {
    xs=s; var=r; var->inc_ref(); exprs=e;
  }
  virtual ~ExNode_IterRFor() { xs->deref(); var->deref(); exprs->deref(); }
  virtual ExVal calc() {
    ExVal liszt=xs->eval();
    mrs_natural len=liszt.toNatural();
    if (len>0) {
      for (int i=len-1; i>=0; i--) {
        ExVal e=liszt.getSeqElem(i);
        var->setValue(e);
        exprs->eval();
      }
    }
    return ExVal();
  }
};//}}}

/// String iterators
/// map : iterate over list creating new list of same size without destroying original
class ExNode_StringMap : public ExNode//{{{
{
ExNode* xs; ExRecord* var; ExNode* exprs; public:
  ExNode_StringMap(ExNode* s, ExRecord* r, ExNode* e, std::string t) : ExNode(T_VAR,t) {
    xs=s; var=r; var->inc_ref(); exprs=e;
  }
  virtual ~ExNode_StringMap() { xs->deref(); var->deref(); exprs->deref(); }
  virtual ExVal calc() {
    std::string str=(xs->eval()).toString();
    std::string result="";
    mrs_natural len=(mrs_natural)str.length();
    if (len>0) {
      for (int i=0; i<len; ++i) {
        ExVal v=ExVal(((std::string)"")+str[i]);
        var->setValue(v);
        std::string r=(exprs->eval()).toString();
        result+=(exprs->eval()).toString();
      }
    }
    return ExVal(result);
  }
};//}}}
/// iter : iterate over list and replace each element in original list
class ExNode_StringIter : public ExNode//{{{
{
ExRecord* xs; ExRecord* var; ExNode* exprs; public:
  ExNode_StringIter(ExRecord* s, ExRecord* r, ExNode* e) : ExNode(T_VAR,"mrs_unit") {
    xs=s; xs->inc_ref(); var=r; var->inc_ref(); exprs=e;
  }
  virtual ~ExNode_StringIter() { xs->deref(); var->deref(); exprs->deref(); }
  virtual ExVal calc() {
    std::cout << "ITER:"<<std::endl;
    std::string str=(xs->getValue()).toString();
    std::string result="";
    mrs_natural len=(mrs_natural)str.length();
    if (len>0) {
      for (int i=0; i<len; ++i) {
        ExVal v=ExVal(((std::string)"")+str[i]);
        var->setValue(v);
        result+=(exprs->eval()).toString();
      }
    }
    ExVal v=ExVal(result);
    xs->setValue(v);
    return ExVal();
  }
};//}}}
/// iterate over list and do something with each value in the list, not destroying old list, returns unit
class ExNode_StringFor : public ExNode//{{{
{
ExNode* xs; ExRecord* var; ExNode* exprs; public:
  ExNode_StringFor(ExNode* s, ExRecord* r, ExNode* e) : ExNode(T_VAR,"mrs_unit") {
    xs=s; var=r; var->inc_ref(); exprs=e;
  }
  virtual ~ExNode_StringFor() { xs->deref(); var->deref(); exprs->deref(); }
  virtual ExVal calc() {
    std::string str=(xs->eval()).toString();
    std::string result="";
    mrs_natural len=(mrs_natural)str.length();
    if (len>0) {
      for (int i=0; i<len; ++i) {
        ExVal v=ExVal(((std::string)"")+str[i]);
        var->setValue(v);
        exprs->eval();
      }
    }
    return ExVal();
  }
};//}}}
class ExNode_StringRFor : public ExNode//{{{
{
ExNode* xs; ExRecord* var; ExNode* exprs; public:
  ExNode_StringRFor(ExNode* s, ExRecord* r, ExNode* e) : ExNode(T_VAR,"mrs_unit") {
    xs=s; var=r; var->inc_ref(); exprs=e;
  }
  virtual ~ExNode_StringRFor() { xs->deref(); var->deref(); exprs->deref(); }
  virtual ExVal calc() {
    std::string str=(xs->eval()).toString();
    std::string result="";
    mrs_natural len=(mrs_natural)str.length();
    if (len>0) {
      for (int i=len-1; i>=0; i--) {
        ExVal v=ExVal(((std::string)"")+str[i]);
        var->setValue(v);
        exprs->eval();
      }
    }
    return ExVal();
  }
};//}}}

/*** Lists *******************************************************************/
class ExNode_SetElem : public ExNode//{{{
{
ExRecord* list; ExNode* var; ExNode* idx; public:
  ExNode_SetElem(ExRecord* xs, ExNode* i, ExNode* v) : ExNode(T_VAR,xs->getType()) {
    list=xs; list->inc_ref(); idx=i; var=v; var->inc_ref();
  }
  ~ExNode_SetElem() { list->deref(); var->deref(); idx->deref(); }
  virtual ExVal calc() {
    ExVal v=var->eval();
    mrs_natural i=(idx->eval()).toNatural();
    v.setSeqElem(i,v);
    return v;
  }
};//}}}
class ExNode_Range : public ExNode//{{{
{
ExNode* xs; ExNode* lidx; ExNode* ridx; public:
  ExNode_Range(ExNode* s, ExNode* l, ExNode* r) : ExNode(s->getKind(),s->getType()) { xs=s; lidx=l; ridx=r; }
  ~ExNode_Range() { xs->deref(); lidx->deref(); ridx->deref(); }
  virtual ExVal calc() {
    ExVal v=xs->eval();
    mrs_natural l=(lidx->eval()).toNatural();
    mrs_natural r=(ridx->eval()).toNatural();
    if (l<0) l=0; if (r<l) r=l;
    return v.getSeqRange(l,r);
  }
};//}}}
class ExNode_GetElem : public ExNode//{{{
{
ExNode* xs; ExNode* idx; public:
  ExNode_GetElem(ExNode* s, ExNode* i) : ExNode(s->getKind(),s->getElemType()) { xs=s; idx=i; }
  ~ExNode_GetElem() { xs->deref(); idx->deref(); }
  virtual ExVal calc() {
    ExVal v=xs->eval();
    mrs_natural i=(idx->eval()).toNatural();
    return v.getSeqElem(i);
  }
};//}}}
/*** Controls ****************************************************************/
// GetCtrl //{{{
#define GETCTRL(_T,_METHOD,_TP) \
class ExNode_GetCtrl##_T : public ExNode { public: \
	std::string nm; MarControlPtr ptr; \
	ExNode_GetCtrl##_T(std::string n, MarControlPtr p) : ExNode(OP_GETCTRL,_TP) { nm=n; ptr=p; } \
	virtual ExVal calc() { return ptr->_METHOD; } \
};
GETCTRL(Real,to<mrs_real>(),"mrs_real");
GETCTRL(String,to<mrs_string>(),"mrs_string");
GETCTRL(Natural,to<mrs_natural>(),"mrs_natural");
GETCTRL(Bool,to<mrs_bool>(),"mrs_bool");
//}}}
// SetCtrl //{{{
#define SETCTRL(_N,_METHOD,_TP) \
class ExNode_SetCtrl##_N : public ExNode { \
	std::string nm; MarControlPtr ptr; ExNode* ex; public: \
	ExNode_SetCtrl##_N(std::string n, MarControlPtr p, ExNode* u) : ExNode(OP_SETCTRL,_TP) { nm=n; ptr=p; ex=u; } \
	~ExNode_SetCtrl##_N() { ex->deref(); } \
	virtual ExVal calc() { ExVal v=ex->eval(); ptr->setValue(v._METHOD); return v; } \
};
SETCTRL(Real,toReal(),"mrs_real");
SETCTRL(String,toString(),"mrs_string");
SETCTRL(Natural,toNatural(),"mrs_natural");
SETCTRL(Bool,toBool(),"mrs_bool");
//}}}
class ExNode_Link : public ExNode//{{{
{
public:
  MarControlPtr ptr_a; MarControlPtr ptr_b;
  ExNode_Link(MarControlPtr pf, MarControlPtr pt, std::string t) : ExNode(OP_LINK,t) { ptr_a=pf; ptr_b=pt; }
  virtual ExVal calc() { return ptr_a->linkTo(ptr_b); }
};//}}}
/***********/
class ExCNameAlias//{{{
{
public:
  std::string tp; std::string nm; MarControlPtr ptr;
  ExCNameAlias() { };
  ExCNameAlias(std::string t, std::string n, MarControlPtr p) { tp=t; nm=n; ptr=p; };
  virtual ~ExCNameAlias() {};
  virtual ExNode* getctrl() {
    if (tp=="mrs_real"   ) { return new ExNode_GetCtrlReal(nm,ptr); }
    if (tp=="mrs_natural") { return new ExNode_GetCtrlNatural(nm,ptr); }
    if (tp=="mrs_string" ) { return new ExNode_GetCtrlString(nm,ptr); }
    if (tp=="mrs_bool"   ) { return new ExNode_GetCtrlBool(nm,ptr); }
    return NULL;
  };
  virtual ExNode* setctrl(ExNode* u) {
    if (tp=="mrs_real"   ) { return new ExNode_SetCtrlReal(nm,ptr,u); }
    if (tp=="mrs_natural") { return new ExNode_SetCtrlNatural(nm,ptr,u); }
    if (tp=="mrs_string" ) { return new ExNode_SetCtrlString(nm,ptr,u); }
    if (tp=="mrs_bool"   ) { return new ExNode_SetCtrlBool(nm,ptr,u); }
    return NULL;
  };
};//}}}
/*** Variables****************************************************************/
class ExNode_AsgnVar : public ExNode//{{{
{
ExRecord* var; ExNode* ex; std::string d; public:
  ExNode_AsgnVar(ExNode* f, ExRecord* r) : ExNode(OP_ASGN,f->getType()) {
    ex=f; var=r; var->inc_ref();
    if (f->getType()=="mrs_real") d="d"; else d="n";
  }
  virtual ~ExNode_AsgnVar() { var->deref(); ex->deref(); }
  virtual ExVal calc() { ExVal v=(ex->eval()); var->setValue(v); return v; }
};//}}}
class ExNode_ReadVar : public ExNode//{{{
{
ExRecord* var; std::string d; public:
  ExNode_ReadVar(ExRecord* es, std::string nm) : ExNode(T_NAME,es->getType()) {
    var=es; var->inc_ref(); val_str=nm;
    std::string t = es->getType();
    if (t=="mrs_real") d="d"; else d="n";
  }
  virtual ~ExNode_ReadVar() { var->deref(); }
  virtual ExVal calc() { return var->getValue(); }
};//}}}
/*** Functions and Libraries *************************************************/
class ExFun : public ExNode//{{{
{
protected:
  ExNode** params;
  int num_params;
  std::vector<std::string> param_types;
  bool is_pure; bool const_params;
  std::string signature;
public:
  ExFun(std::string t, std::string r) : ExNode(T_FUN,t) { setSignature(r); params=NULL; num_params=0; is_pure=false; }
  ExFun(std::string t, std::string r, bool pure) : ExNode(T_FUN,t) { setSignature(r); is_pure=pure; params=NULL; num_params=0; is_pure=false; }
  virtual ~ExFun();
  void setSignature(const std::string);
  std::string getSignature() const { return signature; }
  void setParams(ExNode* ps);
  void setParamTypes(std::string t);
  virtual bool is_const();
  ExFun* copy()=0;
};//}}}
// LibExNode {{{
#define LibExNode0(_NM,_FUN) \
class ExFun_##_NM : public ExFun { public: \
	ExFun_##_NM() : ExFun(type_,sig_,true) { } \
	virtual ExVal calc() { return _FUN(); } \
	ExFun* copy() { return new ExFun_##_NM (); } \
};

#define LibExFun1(_NM,_FUN,_T1,type_,sig_) \
class ExFun_##_NM : public ExFun { public: \
	ExFun_##_NM() : ExFun(type_,sig_,true) { } \
	virtual ExVal calc() { return _FUN((params[0]->eval())._T1); } \
	ExFun* copy() { return new ExFun_##_NM (); } \
};
#define LibExFun2(_NM,_FUN,_T1,_T2) \
class ExFun_##_NM : public ExFun { public: \
	ExFun_##_NM() : ExFun(type_,sig_,true) { } \
	virtual ExVal calc() { return _FUN((params[0]->eval())._T1,(params[1]->eval())._T2); } \
	ExFun* copy() { return new ExFun_##_NM (); } \
};//}}}
/*** Natural Library *********************************************************/
class ExFun_NaturalMin : public ExFun//{{{
{
public:
  ExFun_NaturalMin() : ExFun("mrs_natural","Natural.min(mrs_natural,mrs_natural)",true) { }
  virtual ExVal calc() {
    mrs_natural n1=(params[0]->eval()).toNatural();
    mrs_natural n2=(params[1]->eval()).toNatural();
    return (n2<n1) ? n2 : n1;
  }
  ExFun* copy() { return new ExFun_NaturalMin(); }
};//}}}
class ExFun_NaturalMax : public ExFun//{{{
{
public:
  ExFun_NaturalMax() : ExFun("mrs_natural","Natural.max(mrs_natural,mrs_natural)",true) { }
  virtual ExVal calc() {
    mrs_natural n1=(params[0]->eval()).toNatural();
    mrs_natural n2=(params[1]->eval()).toNatural();
    return (n2>n1) ? n2 : n1;
  }
  ExFun* copy() { return new ExFun_NaturalMax(); }
};//}}}
class ExFun_NaturalRand : public ExFun//{{{
{
public:
  ExFun_NaturalRand() : ExFun("mrs_natural","Natural.rand()",true) { }
  virtual ExVal calc() { return (mrs_natural)rand(); }
  ExFun* copy() { return new ExFun_NaturalRand(); }
};//}}}
class ExFun_NaturalRandRange1 : public ExFun//{{{
{
public:
  ExFun_NaturalRandRange1() : ExFun("mrs_natural","Natural.rand(mrs_natural)",true) { }
  virtual ExVal calc() {
    mrs_natural n1=(params[0]->eval()).toNatural();
    mrs_natural on=((int)((double)rand()/(double)RAND_MAX*n1));
    return (mrs_natural)on;
  }
  ExFun* copy() { return new ExFun_NaturalRandRange1(); }
};//}}}
class ExFun_NaturalRandRange2 : public ExFun//{{{
{
public:
  ExFun_NaturalRandRange2() : ExFun("mrs_natural","Natural.rand(mrs_natural,mrs_natural)",true) { }
  virtual ExVal calc() {
    mrs_natural n1=(params[0]->eval()).toNatural();
    mrs_natural n2=(params[1]->eval()).toNatural();
    mrs_natural on=((int)((double)rand()/(double)RAND_MAX*(n2-n1)))+n1;
    return (mrs_natural)on;
  }
  ExFun* copy() { return new ExFun_NaturalRandRange2(); }
};//}}}
class ExFun_NaturalSRand : public ExFun//{{{
{
public:
  ExFun_NaturalSRand() : ExFun("mrs_natural","Natural.srand(mrs_natural)",true) { }
  virtual ExVal calc() { return (mrs_natural)0; }
  ExFun* copy() { return new ExFun_NaturalSRand(); }
};//}}}
class ExFun_NaturalAbs : public ExFun//{{{
{
public:
  ExFun_NaturalAbs() : ExFun("mrs_real","Natural.abs(mrs_real)",true) { }
  virtual ExVal calc() { mrs_natural d = (params[0]->eval()).toNatural(); return (d<0) ? -d : d; }
  ExFun* copy() { return new ExFun_NaturalAbs(); }
};//}}}
/*** Real Library ************************************************************/
LibExFun1(RealCos,cos,toReal(),"mrs_real","Real.cos(mrs_real)");
LibExFun1(RealSqrt,sqrt,toReal(),"mrs_real","Real.sqrt(mrs_real)");
LibExFun1(RealSin,sin,toReal(),"mrs_real","Real.sin(mrs_real)");
LibExFun1(RealACos,acos,toReal(),"mrs_real","Real.acos(mrs_real)");
LibExFun1(RealASin,asin,toReal(),"mrs_real","Real.asin(mrs_real)");
LibExFun1(RealATan,atan,toReal(),"mrs_real","Real.atan(mrs_real)");
LibExFun1(RealCosH,cosh,toReal(),"mrs_real","Real.cosh(mrs_real)");
LibExFun1(RealSinH,sinh,toReal(),"mrs_real","Real.sinh(mrs_real)");
LibExFun1(RealTan,tan,toReal(),"mrs_real","Real.tan(mrs_real)");
LibExFun1(RealLog,log,toReal(),"mrs_real","Real.log(mrs_real)");
LibExFun1(RealLog10,log10,toReal(),"mrs_real","Real.log10(mrs_real)");

class ExFun_RealAbs : public ExFun//{{{
{
public:
  ExFun_RealAbs() : ExFun("mrs_real","Real.abs(mrs_real)",true) { }
  virtual ExVal calc() { mrs_real d = (params[0]->eval()).toReal(); return (d<0.0) ? -d : d; }
  ExFun* copy() { return new ExFun_RealAbs(); }
};//}}}
class ExFun_RealLog2 : public ExFun//{{{
{
public:
  ExFun_RealLog2() : ExFun("mrs_real","Real.log2(mrs_real)",true) { }
  virtual ExVal calc() { return log10((params[0]->eval()).toReal())/log10(2.0); }
  ExFun* copy() { return new ExFun_RealLog2(); }
};//}}}
class ExFun_RealRand : public ExFun//{{{
{
public:
  ExFun_RealRand() : ExFun("mrs_real","Real.rand()",false) { }
  virtual ExVal calc() { return ((mrs_real)rand())/((mrs_real)RAND_MAX); }
  ExFun* copy() { return new ExFun_RealRand(); }
};//}}}
/*** String Library **********************************************************/
class ExFun_StrLen : public ExFun//{{{
{
public:
  ExFun_StrLen() : ExFun("mrs_natural","String.len(mrs_string)",true) { }
  virtual ExVal calc() { return (mrs_natural)((params[0]->eval()).toString()).length(); }
  ExFun* copy() { return new ExFun_StrLen(); }
};//}}}
class ExFun_StrSub : public ExFun//{{{
{
public:
  ExFun_StrSub() : ExFun("mrs_string","String.sub(mrs_string,mrs_natural,mrs_natural)",true) { }
  virtual ExVal calc() {
    std::string str = params[0]->eval().toString();
    int s = params[1]->eval().toNatural();
    int e = params[2]->eval().toNatural();
    if (s<0) { s=0; }
    if (e>(int)(str.length()-s)) { e=((mrs_natural)str.length())-s; }
    return str.substr(s,e);
  }
  ExFun* copy() { return new ExFun_StrSub(); }
};//}}}
/*** Stream Library **********************************************************/
// StreamOut {{{
#define ExFun_StreamOutType(_TYPE,_CONVERSION,_METHOD,_type,_sig) \
class ExFun_StreamOut##_TYPE : public ExFun { public: \
	ExFun_StreamOut##_TYPE() : ExFun(_type,_sig,false) { } \
	virtual ExVal calc() { ExVal x = params[0]->eval(); std::cout << _CONVERSION(x._METHOD()); return x; } \
	ExFun* copy() { return new ExFun_StreamOut##_TYPE(); } \
};
ExFun_StreamOutType(String,     ,toString, "mrs_string","Stream.op(mrs_string)");
ExFun_StreamOutType(Real,   dtos,toReal,   "mrs_real","Stream.op(mrs_real)");
ExFun_StreamOutType(Natural,ltos,toNatural,"mrs_natural","Stream.op(mrs_natural)");
ExFun_StreamOutType(Bool,   btos,toBool,   "mrs_bool","Stream.op(mrs_bool)");
//}}}
// StreamOutNewline {{{
#define ExFun_StreamOutNType(_TYPE,_CONVERSION,_METHOD,_type,_sig) \
class ExFun_StreamOutN##_TYPE : public ExFun { public: \
	ExFun_StreamOutN##_TYPE() : ExFun(_type,_sig,false) { } \
	virtual ExVal calc() { ExVal x = params[0]->eval(); std::cout << _CONVERSION(x._METHOD()) << std::endl; return x; } \
	ExFun* copy() { return new ExFun_StreamOutN##_TYPE(); } \
};
ExFun_StreamOutNType(String, ,toString,"mrs_string","Stream.opn(mrs_string)");
ExFun_StreamOutNType(Real,dtos,toReal,"mrs_real","Stream.opn(mrs_real)");
ExFun_StreamOutNType(Natural,ltos,toNatural,"mrs_natural","Stream.opn(mrs_natural)");
ExFun_StreamOutNType(Bool,btos,toBool,"mrs_bool","Stream.opn(mrs_bool)");
class ExFun_StreamOutNVal : public ExFun {
ExNode* rec; public:
  ExFun_StreamOutNVal(ExNode* r) : ExFun("mrs_unit","Stream.opn(mrs_val)",false) { rec=r; }
  virtual ExVal calc() { ExVal x=rec->eval(); std::cout << x << std::endl; return x; }
  virtual ~ExFun_StreamOutNVal() { rec->deref(); }
  ExFun* copy() { return new ExFun_StreamOutNVal(rec); }
};
//}}}
/*** Timer Library ***********************************************************/
// TimerGetStuff {{{
#define TIMER_GET(_NM,_ZERO,_METHOD,_type,_sig) \
class ExFun_TimerGet##_NM : public ExFun { \
	ExFun* child; public: \
	ExFun_TimerGet##_NM() : ExFun(_type,_sig,false) {} \
	virtual ExVal calc() { TmTimer** t=params[0]->eval().toTimer(); return (t==NULL||*t==NULL) ? _ZERO : (*t)->_METHOD; } \
	ExFun* copy() { return new ExFun_TimerGet##_NM(); } \
};
TIMER_GET(Prefix,"",getPrefix(),"mrs_string","Timer.prefix(mrs_timer)");
TIMER_GET(Name,"",getName(),"mrs_string","Timer.name(mrs_timer)");
TIMER_GET(Type,"",getType(),"mrs_string","Timer.type(mrs_timer)");
TIMER_GET(Time,0,getTime(),"mrs_natural","Timer.time(mrs_timer)");
//}}}
/*
class ExFun_TimerGetTimer : public ExFun {
	TmTimer** tmr; public:
	ExFun_TimerGetTimer(TmTimer** tm) : ExFun(_type,_sig,false) { tmr=tm; }
	virtual ExVal calc() { return tmr; }
	ExFun* copy() { return new ExFun_TimerGetTimer(tmr); }
};
*/
class ExFun_TimerIntrvlSize : public ExFun//{{{
{
public:
  ExFun_TimerIntrvlSize() : ExFun("mrs_natural","Timer.ival(mrs_timer,mrs_string)",false) {}
  virtual ExVal calc() {
    TmTimer** t=params[0]->eval().toTimer();
    std::string ts=params[1]->eval().toString();
    return (t==NULL||*t==NULL) ? 0 : (*t)->intervalsize(ts);
  }
  ExFun* copy() { return new ExFun_TimerIntrvlSize(); }
};//}}}
// TimerUpd {{{
#define TIMER_UPD(_NM,_ZERO,_METHOD,_type,_sig) \
class ExFun_TimerUpd##_NM : public ExFun { public: \
	ExFun_TimerUpd##_NM() : ExFun(_type,_sig,false) {} \
	virtual ExVal calc() { \
		TmTimer** t=params[0]->eval().toTimer(); \
		ExVal s=params[1]->eval(); \
		ExVal v=params[2]->eval(); \
		if (t==NULL||*t==NULL) { (*t)->updtimer(s.toString(),v._METHOD); return true; } return false; \
	} \
	ExFun* copy() { return new ExFun_TimerUpd##_NM(); } \
};
TIMER_UPD(Real,0.0,toReal(),"mrs_bool","Timer.upd(mrs_timer,mrs_string,mrs_real)");
TIMER_UPD(Natural,0,toNatural(),"mrs_bool","Timer.upd(mrs_timer,mrs_string,mrs_natural)");
TIMER_UPD(String,"",toString(),"mrs_bool","Timer.upd(mrs_timer,mrs_string,mrs_string)");
TIMER_UPD(Bool,false,toBool(),"mrs_bool","Timer.upd(mrs_timer,mrs_string,mrs_bool)");
//}}}
/*****************************************************************************/
class ExFun_ListLen : public ExFun//{{{
{
public:
  ExFun_ListLen() : ExFun("mrs_natural","List.len(mrs_list)",true) { }
  virtual ExVal calc() { return (mrs_natural)(params[0]->eval()).toNatural(); }//((params[0]->eval()).toNatural()); }
  ExFun* copy() { return new ExFun_ListLen(); }
};//}}}
/*** Done ********************************************************************/

}//namespace Marsyas

#endif

