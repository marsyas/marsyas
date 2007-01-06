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
/**
   \class ExNode
   \brief ExNode is the base class for an expression tree node.
   \author Neil Burroughs  inb@cs.uvic.ca
   \version 1.0
   \date    Jan 5, 2007
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
#ifndef __EX_NODE_H__
#define __EX_NODE_H__
#include <string>
#include <iostream>
#include "ExVal.h"
#include "ExSymTbl.h"
#include "MarControl.h"
#include "MarSystem.h"
#include "common.h"
#include "TmTimer.h"
#include "ExCommon.h"

namespace Marsyas
{
void loadlib_Real(ExRecord* st);
void loadlib_String(ExRecord* st);
void loadlib_Natural(ExRecord* st);
void loadlib_Stream(ExRecord* st);
void load_symbols(ExRecord*);
void loadlib_timer(ExRecord* st, TmTimer** tmr);

class ExNode {
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
    virtual ExNode* copy();
    std::string getType() const;
    void setType(const std::string t);
    int getKind() const { return kind; }
    void setKind(const int k);

    ExVal getValue() { return value; }
    void setValue(mrs_natural x) { value.set(x); setKind(T_CONST); setType("mrs_natural"); }
    void setValue(std::string x) { value.set(x); setKind(T_CONST); setType("mrs_string"); }
    void setValue(mrs_real x) { value.set(x); setKind(T_CONST); setType("mrs_real"); }
    void setValue(mrs_bool x) { value.set(x); setKind(T_CONST); setType("mrs_bool"); }
    virtual std::string toString();
    virtual std::string oot();
    virtual ExVal eval();
    virtual ExVal calc() { return value;  }
};
/*** Unary Operators *********************************************************/
#define UNARYOP(_NM,_KIND,_TYPE,_TO,_OP) \
class ExNode_##_NM : public ExNode { \
    ExNode* child; public: \
    ExNode_##_NM(ExNode* v) : ExNode(_KIND,_TYPE) { child=v; } \
    virtual ~ExNode_##_NM() { delete child; } \
    virtual ExVal calc() { return _OP((child->eval())._TO); } \
};
UNARYOP(MathNeg_Real   ,OP_MNEG,"mrs_real"   ,toReal()   ,-);
UNARYOP(MathNeg_Natural,OP_MNEG,"mrs_natural",toNatural(),-);
UNARYOP(BoolNeg        ,OP_BNEG,"mrs_bool"   ,toBool()   ,!);
/*** Conversions *************************************************************/
UNARYOP(NaturalToReal  ,OP_CONV,"mrs_real"   ,toNatural(),(mrs_real)   );
UNARYOP(RealToNatural  ,OP_CONV,"mrs_natural",toReal()   ,(mrs_natural));
UNARYOP(RealToString   ,OP_CONV,"mrs_string" ,toReal()   ,dtos         );
UNARYOP(NaturalToString,OP_CONV,"mrs_string" ,toNatural(),ltos         );
UNARYOP(BoolToString   ,OP_CONV,"mrs_string" ,toBool()   ,btos         );
/*** Binary Operators ********************************************************/
#define BINOP(_NM,_KIND,_OP) \
class ExNode_##_NM : public ExNode { \
    ExNode* lchild; ExNode* rchild; public: \
    ExNode_##_NM(std::string t, ExNode* u, ExNode* v) : ExNode(_KIND,t) { lchild=u; rchild=v; } \
    virtual ~ExNode_##_NM() { delete lchild; delete rchild; } \
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
/*** Controls ****************************************************************/
class ExNode_Conditional : public ExNode {
    ExNode* cond; ExNode* then_; ExNode* else_; public:
    ExNode_Conditional(std::string t, ExNode* c, ExNode* ts, ExNode* es) : ExNode(T_COND,t) { cond=c; then_=ts; else_=es; }
    ~ExNode_Conditional() { delete cond; delete then_; delete else_; }
    virtual ExVal calc() {
        ExVal v = cond->eval();
        return (v.toBool()) ? then_->eval() : else_->eval();
    }
};
/*** Controls ****************************************************************/
#define GETCTRL(_T,_METHOD,_TP) \
class ExNode_GetCtrl##_T : public ExNode { public: \
    std::string nm; MarControlPtr ptr; \
    ExNode_GetCtrl##_T(std::string n, MarControlPtr p) : ExNode(OP_GETCTRL,_TP) { nm=n; ptr=p; } \
    virtual ExVal calc() { return ptr->_METHOD; } \
};
GETCTRL(Real,toReal(),"mrs_real");
GETCTRL(String,toString(),"mrs_string");
GETCTRL(Natural,toNatural(),"mrs_natural");
GETCTRL(Bool,toBool(),"mrs_bool");

#define SETCTRL(_N,_METHOD,_TP) \
class ExNode_SetCtrl##_N : public ExNode { \
    std::string nm; MarControlPtr ptr; ExNode* ex; public: \
    ExNode_SetCtrl##_N(std::string n, MarControlPtr p, ExNode* u) : ExNode(OP_SETCTRL,_TP) { nm=n; ptr=p; ex=u; } \
    ~ExNode_SetCtrl##_N() { delete ex; } \
    virtual ExVal calc() { ExVal v=ex->eval(); ptr->setValue(v._METHOD); return v; } \
};
SETCTRL(Real,toReal(),"mrs_real");
SETCTRL(String,toString(),"mrs_string");
SETCTRL(Natural,toNatural(),"mrs_natural");
SETCTRL(Bool,toBool(),"mrs_bool");

class ExNode_Link : public ExNode { public:
    MarControlPtr ptr_a; MarControlPtr ptr_b;
    ExNode_Link(MarControlPtr pf, MarControlPtr pt, std::string t) : ExNode(OP_LINK,t) { ptr_a=pf; ptr_b=pt; }
    virtual ExVal calc() { return ptr_a->linkTo(ptr_b); }
};
/***********/
class ExCNameAlias {
public:
    std::string tp; std::string nm; MarControlPtr ptr;
    ExCNameAlias() { };
    ExCNameAlias(std::string t, std::string n, MarControlPtr p) { tp=t; nm=n; ptr=p; };
    virtual ~ExCNameAlias(){};
    virtual ExNode* getctrl(){
        if (tp=="mrs_real"   ) { return new ExNode_GetCtrlReal(nm,ptr); }
        if (tp=="mrs_natural") { return new ExNode_GetCtrlNatural(nm,ptr); }
        if (tp=="mrs_string" ) { return new ExNode_GetCtrlString(nm,ptr); }
        if (tp=="mrs_bool"   ) { return new ExNode_GetCtrlBool(nm,ptr); }
        return NULL;
    };
    virtual ExNode* setctrl(ExNode* u){
        if (tp=="mrs_real"   ) { return new ExNode_SetCtrlReal(nm,ptr,u); }
        if (tp=="mrs_natural") { return new ExNode_SetCtrlNatural(nm,ptr,u); }
        if (tp=="mrs_string" ) { return new ExNode_SetCtrlString(nm,ptr,u); }
        if (tp=="mrs_bool"   ) { return new ExNode_SetCtrlBool(nm,ptr,u); }
        return NULL;
    };
};
/*** Variables****************************************************************/
class ExNode_AsgnVar : public ExNode {
    ExRecord* var; ExNode* ex; public:
    ExNode_AsgnVar(ExNode* f, ExRecord* r) : ExNode(OP_ASGN,f->getType()) { ex=f; var=r; var->inc_ref(); }
    virtual ~ExNode_AsgnVar() { var->deref(); delete ex; }
    virtual ExVal calc() { ExVal v=(ex->eval()); var->setValue("",v); return v; }
};
class ExNode_ReadVar : public ExNode {
    private: ExRecord* var; public:
    ExNode_ReadVar(ExRecord* es, std::string nm) : ExNode(T_NAME,es->getType()) { var=es; var->inc_ref(); val_str=nm; }
    virtual ~ExNode_ReadVar() { var->deref(); }
    virtual ExVal calc() { return var->getValue(); }
};
/*** Functions and Libraries *************************************************/
class ExFun : public ExNode {
protected:
    ExNode** params;
    int num_params;
    std::vector<std::string> param_types;
    bool is_pure; bool const_params;
    std::string signature;
public:
    ExFun(std::string t, std::string r) : ExNode(T_FUN,t) { setSignature(r); params=NULL; num_params=0; is_pure=false; }
    ExFun(std::string t, std::string r, bool pure) : ExNode(T_FUN,t) { setSignature(r); is_pure=pure; params=NULL; num_params=0; is_pure=false; }
    ~ExFun();
    void setSignature(const std::string);
    std::string getSignature() const { return signature; }
    void setParams(ExNode* ps);
    void setParamTypes(std::string t);
    virtual bool is_const();
    ExFun* copy()=0;
};
#define LibExNode0(_NM,_FUN) \
class ExFun_##_NM : public ExFun { public: \
    ExFun_##_NM(std::string t, std::string r) : ExFun(t,r,true) { } \
    virtual ExVal calc() { return _FUN(); } \
    ExFun* copy() { return new ExFun_##_NM (getType(),getSignature()); } \
};

#define LibExFun1(_NM,_FUN,_T1) \
class ExFun_##_NM : public ExFun { public: \
    ExFun_##_NM(std::string t, std::string r) : ExFun(t,r,true) { } \
    virtual ExVal calc() { return _FUN((params[0]->eval())._T1); } \
    ExFun* copy() { return new ExFun_##_NM (getType(),getSignature()); } \
};
#define LibExFun2(_NM,_FUN,_T1,_T2) \
class ExFun_##_NM : public ExFun { public: \
    ExFun_##_NM(std::string t, std::string r) : ExFun(t,r,true) { } \
    virtual ExVal calc() { return _FUN((params[0]->eval())._T1,(params[1]->eval())._T2); } \
    ExFun* copy() { return new ExFun_##_NM (getType(),getSignature()); } \
};
/*** Natural Library *********************************************************/
class ExFun_NaturalMin : public ExFun { public:
    ExFun_NaturalMin(std::string t, std::string r) : ExFun(t,r,true) { }
    virtual ExVal calc() {
        mrs_natural n1=(params[0]->eval()).toNatural();
        mrs_natural n2=(params[1]->eval()).toNatural();
        return (n2<n1) ? n2 : n1;
    }
    ExFun* copy() { return new ExFun_NaturalMin(getType(),getSignature()); }
};
class ExFun_NaturalMax : public ExFun { public:
    ExFun_NaturalMax(std::string t, std::string r) : ExFun(t,r,true) { }
    virtual ExVal calc() {
        mrs_natural n1=(params[0]->eval()).toNatural();
        mrs_natural n2=(params[1]->eval()).toNatural();
        return (n2>n1) ? n2 : n1;
    }
    ExFun* copy() { return new ExFun_NaturalMax(getType(),getSignature()); }
};
class ExFun_NaturalRand : public ExFun { public:
    ExFun_NaturalRand(std::string t, std::string r) : ExFun(t,r,true) { }
    virtual ExVal calc() { return (mrs_natural)rand(); }
    ExFun* copy() { return new ExFun_NaturalRand(getType(),getSignature()); }
};
class ExFun_NaturalSRand : public ExFun { public:
    ExFun_NaturalSRand(std::string t, std::string r) : ExFun(t,r,true) { }
    virtual ExVal calc() { return (mrs_natural)0; }
    ExFun* copy() { return new ExFun_NaturalSRand(getType(),getSignature()); }
};
class ExFun_NaturalAbs : public ExFun { public:
    ExFun_NaturalAbs(std::string t, std::string r) : ExFun(t,r,true) { }
    virtual ExVal calc() { mrs_natural d = (params[0]->eval()).toNatural(); return (d<0) ? -d : d; }
    ExFun* copy() { return new ExFun_NaturalAbs(getType(),getSignature()); }
};
/*** Real Library ************************************************************/
LibExFun1(RealCos,cos,toReal());
LibExFun1(RealSqrt,sqrt,toReal());
LibExFun1(RealSin,sin,toReal());
LibExFun1(RealACos,acos,toReal());
LibExFun1(RealASin,asin,toReal());
LibExFun1(RealATan,atan,toReal());
LibExFun1(RealCosH,cosh,toReal());
LibExFun1(RealSinH,sinh,toReal());
LibExFun1(RealTan,tan,toReal());
LibExFun1(RealLog,log,toReal());
LibExFun1(RealLog10,log10,toReal());

class ExFun_RealAbs : public ExFun { public:
    ExFun_RealAbs(std::string t, std::string r) : ExFun(t,r,true) { }
    virtual ExVal calc() { mrs_real d = (params[0]->eval()).toReal(); return (d<0.0) ? -d : d; }
    ExFun* copy() { return new ExFun_RealAbs(getType(),getSignature()); }
};
class ExFun_RealLog2 : public ExFun { public:
    ExFun_RealLog2(std::string t, std::string r) : ExFun(t,r,true) { }
    virtual ExVal calc() { return log10((params[0]->eval()).toReal())/log10(2.0); }
    ExFun* copy() { return new ExFun_RealLog2(getType(),getSignature()); }
};
class ExFun_RealRand : public ExFun { public:
    ExFun_RealRand(std::string t, std::string r) : ExFun(t,"mrs_real",false) { }
    virtual ExVal calc() { return ((mrs_real)rand())/((mrs_real)RAND_MAX); }
    ExFun* copy() { return new ExFun_RealRand(getType(),getSignature()); }
};

/*** String Library **********************************************************/
class ExFun_StrLen : public ExFun { public:
    ExFun_StrLen(std::string t, std::string r) : ExFun(t,r,true) { }
    virtual ExVal calc() { return (mrs_natural)((params[0]->eval()).toString()).length(); }
    ExFun* copy() { return new ExFun_StrLen(getType(),getSignature()); }
};
class ExFun_StrSub : public ExFun { public:
    ExFun_StrSub(std::string t, std::string n) : ExFun(t,n,true) { }
    virtual ExVal calc() {
        std::string str = params[0]->eval().toString();
        mrs_natural s = params[1]->eval().toNatural();
        mrs_natural e = params[2]->eval().toNatural();
        if (s<0) { s=0; }
        if (e>((mrs_natural)str.length()-s)) { e=(mrs_natural)str.length()-s; }
        return str.substr(s,e);
    }
    ExFun* copy() { return new ExFun_StrSub(getType(),getSignature()); }
};
/*** Stream Library **********************************************************/
#define ExFun_StreamOutType(_TYPE,_CONVERSION,_METHOD) \
class ExFun_StreamOut##_TYPE : public ExFun { public: \
    ExFun_StreamOut##_TYPE(std::string t, std::string n) : ExFun(t,n,false) { } \
    virtual ExVal calc() { ExVal x = params[0]->eval(); std::cout << _CONVERSION(x._METHOD()); return x; } \
    ExFun* copy() { return new ExFun_StreamOut##_TYPE(getType(),getSignature()); } \
};
ExFun_StreamOutType(String, ,toString);
ExFun_StreamOutType(Real,dtos,toReal);
ExFun_StreamOutType(Natural,ltos,toNatural);
ExFun_StreamOutType(Bool,btos,toBool);

#define ExFun_StreamOutNType(_TYPE,_CONVERSION,_METHOD) \
class ExFun_StreamOutN##_TYPE : public ExFun { public: \
    ExFun_StreamOutN##_TYPE(std::string t, std::string n) : ExFun(t,n,false) { } \
    virtual ExVal calc() { ExVal x = params[0]->eval(); std::cout << _CONVERSION(x._METHOD()) << std::endl; return x; } \
    ExFun* copy() { return new ExFun_StreamOutN##_TYPE(getType(),getSignature()); } \
};
ExFun_StreamOutNType(String, ,toString);
ExFun_StreamOutNType(Real,dtos,toReal);
ExFun_StreamOutNType(Natural,ltos,toNatural);
ExFun_StreamOutNType(Bool,btos,toBool);
/*** Timer Library ***********************************************************/
#define TIMER_GET(_NM,_ZERO,_METHOD) \
class ExFun_TimerGet##_NM : public ExFun { \
    ExFun* child; public: \
    ExFun_TimerGet##_NM(std::string t, std::string n) : ExFun(t,n,false) {} \
    virtual ExVal calc() { TmTimer** t=params[0]->eval().toTimer(); return (t==NULL||*t==NULL) ? _ZERO : (*t)->_METHOD; } \
    ExFun* copy() { return new ExFun_TimerGet##_NM(getType(),getSignature()); } \
};
TIMER_GET(Prefix,"",getPrefix());
TIMER_GET(Name,"",getName());
TIMER_GET(Type,"",getType());
TIMER_GET(Time,0,getTime());

class ExFun_TimerGetTimer : public ExFun {
    TmTimer** tmr; public:
    ExFun_TimerGetTimer(std::string t, std::string n, TmTimer** tm) : ExFun(t,n,false) { tmr=tm; }
    virtual ExVal calc() { return tmr; }
    ExFun* copy() { return new ExFun_TimerGetTimer(getType(),getSignature(),tmr); }
};
class ExFun_TimerIntrvlSize : public ExFun { public:
    ExFun_TimerIntrvlSize(std::string t, std::string n) : ExFun(t,n,false) {}
    virtual ExVal calc() {
        TmTimer** t=params[0]->eval().toTimer();
        std::string ts=params[1]->eval().toString();
        return (t==NULL||*t==NULL) ? 0 : (*t)->intervalsize(ts);
    }
    ExFun* copy() { return new ExFun_TimerIntrvlSize(getType(),getSignature()); }
};
#define TIMER_UPD(_NM,_ZERO,_METHOD) \
class ExFun_TimerUpd##_NM : public ExFun { public: \
    ExFun_TimerUpd##_NM(std::string t, std::string n) : ExFun(t,n,false) {} \
    virtual ExVal calc() { \
        TmTimer** t=params[0]->eval().toTimer(); \
        ExVal s=params[1]->eval(); \
        ExVal v=params[2]->eval(); \
        if (t==NULL||*t==NULL) { (*t)->updtimer(s.toString(),v._METHOD); return true; } return false; \
    } \
    ExFun* copy() { return new ExFun_TimerUpd##_NM(getType(),getSignature()); } \
};
TIMER_UPD(Real,0.0,toReal());
TIMER_UPD(Natural,0,toNatural());
TIMER_UPD(String,"",toString());
TIMER_UPD(Bool,false,toBool());
/*** Done ********************************************************************/

}//namespace Marsyas

#endif

