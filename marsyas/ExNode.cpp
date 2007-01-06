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
   \date    Jan 4, 2007
*/
#include "ExNode.h"

using namespace std;
using namespace Marsyas;

void Marsyas::loadlib_Real(ExRecord* st)
{
    st->addReserved("Real|R.cos(mrs_real)|(mrs_natural)",new ExFun_RealCos("mrs_real","Real.cos(mrs_real)"));
    st->addReserved("Real|R.acos(mrs_real)",new ExFun_RealACos("mrs_real","Real.acos(mrs_real)"));
    st->addReserved("Real|R.cosh(mrs_real)",new ExFun_RealCosH("mrs_real","Real.cosh(mrs_real)"));

    st->addReserved("Real|R.sin(mrs_real)",new ExFun_RealSin("mrs_real","Real.sin(mrs_real)"));
    st->addReserved("Real|R.asin(mrs_real)",new ExFun_RealASin("mrs_real","Real.asin(mrs_real)"));
    st->addReserved("Real|R.sinh(mrs_real)",new ExFun_RealSinH("mrs_real","Real.sinh(mrs_real)"));

    st->addReserved("Real|R.tan(mrs_real)",new ExFun_RealTan("mrs_real","Real.tan(mrs_real)"));
    st->addReserved("Real|R.atan(mrs_real)",new ExFun_RealATan("mrs_real","Real.atan(mrs_real)"));

    st->addReserved("Real|R.log|ln(mrs_real)",new ExFun_RealLog("mrs_real","Real.log(mrs_real)"));
    st->addReserved("Real|R.log2(mrs_real)",new ExFun_RealLog2("mrs_real","Real.log2(mrs_real)"));
    st->addReserved("Real|R.log10(mrs_real)",new ExFun_RealLog10("mrs_real","Real.log10(mrs_real)"));

    st->addReserved("Real|R.rand()",new ExFun_RealRand("mrs_real","Real.rand()"));

    st->addReserved("Real|R.sqrt(mrs_real)",new ExFun_RealSqrt("mrs_real","Real.sqrt(mrs_real)"));
#define _PI_VAL_ 3.14159265358979323846264338327950288419716939937510
#define _E_VAL_  2.7182818284590452353602874713526624977572470936999595749669676277240766303535
    st->addReserved("Real|R.e",(mrs_real)_E_VAL_);
    st->addReserved("Real|R.pi",(mrs_real)_PI_VAL_);
    st->addReserved("Real|R.pi2",(mrs_real)(_PI_VAL_ / 2.0));
    st->addReserved("Real|R.pi4",(mrs_real)(_PI_VAL_ / 4.0));
    st->addReserved("Real|R.dpr",(mrs_real)(360.0 / (2.0 * _PI_VAL_)));
    st->addReserved("Real|R.rpd",(mrs_real)(( 2.0 * _PI_VAL_) / 360.0));
}
void Marsyas::loadlib_Natural(ExRecord* st)
{
    st->addReserved("Natural|N.abs(mrs_natural)",new ExFun_NaturalAbs("mrs_real","Natural.abs(mrs_real)"));
    st->addReserved("Natural|N.rand()",new ExFun_NaturalRand("mrs_natural","Natural.rand()"));
    st->addReserved("Natural|N.min(mrs_natural,mrs_natural)",new ExFun_NaturalMin("mrs_natural","Natural.min(mrs_natural,mrs_natural)"));
    st->addReserved("Natural|N.max(mrs_natural,mrs_natural)",new ExFun_NaturalMax("mrs_natural","Natural.max(mrs_natural,mrs_natural)"));
    st->addReserved("Natural|N.srand(mrs_natural)",new ExFun_NaturalSRand("mrs_natural","Natural.srand(mrs_natural)"));
    st->addReserved("Natural|N.randmax",(mrs_natural)RAND_MAX);
}
void Marsyas::loadlib_String(ExRecord* st)
{
    st->addReserved("String|S.len(mrs_string)",new ExFun_StrLen("mrs_natural","String.len(mrs_string)"));
    st->addReserved("String|S.sub(mrs_string,mrs_natural,mrs_natural)",new ExFun_StrSub("mrs_string","String.sub(mrs_string,mrs_natural,mrs_natural)"));
}
void Marsyas::loadlib_Stream(ExRecord* st)
{
//    st->addReserved("Stream.op",new ExFun_StreamOutString("_fun","Stream.op")); // for lookup
    st->addReserved("Stream.op(mrs_string)",new ExFun_StreamOutString("mrs_string","Stream.op(mrs_string)"));
    st->addReserved("Stream.op(mrs_real)",new ExFun_StreamOutReal("mrs_real","Stream.op(mrs_real)"));
    st->addReserved("Stream.op(mrs_natural)",new ExFun_StreamOutNatural("mrs_natural","Stream.op(mrs_natural)"));
    st->addReserved("Stream.op(mrs_bool)",new ExFun_StreamOutBool("mrs_bool","Stream.op(mrs_bool)"));

    st->addReserved("Stream.opn(mrs_string)",new ExFun_StreamOutNString("mrs_string","Stream.opn(mrs_string)"));
    st->addReserved("Stream.opn(mrs_real)",new ExFun_StreamOutNReal("mrs_real","Stream.opn(mrs_real)"));
    st->addReserved("Stream.opn(mrs_natural)",new ExFun_StreamOutNNatural("mrs_natural","Stream.opn(mrs_natural)"));
    st->addReserved("Stream.opn(mrs_bool)",new ExFun_StreamOutNBool("mrs_bool","Stream.opn(mrs_bool)"));
}
void Marsyas::load_symbols(ExRecord* st)
{
    loadlib_Real(st);
    loadlib_Natural(st);
    loadlib_String(st);
    loadlib_Stream(st);
}
void Marsyas::loadlib_timer(ExRecord* st, TmTimer** tmr)
{
    st->addReserved("Timer|Tmr.cur",(TmTimer**)tmr,"Timer.cur",T_VAR);
    st->addReserved("Timer|Tmr.prefix(mrs_timer)",new ExFun_TimerGetPrefix("mrs_string","Timer.prefix(mrs_timer)"));
    st->addReserved("Timer|Tmr.name(mrs_timer)",new ExFun_TimerGetName("mrs_string","Timer.name(mrs_timer)"));
    st->addReserved("Timer|Tmr.type(mrs_timer)",new ExFun_TimerGetType("mrs_string","Timer.type(mrs_timer)"));
    st->addReserved("Timer|Tmr.time(mrs_timer)",new ExFun_TimerGetTime("mrs_natural","Timer.time(mrs_timer)"));
    st->addReserved("Timer|Tmr.upd(mrs_timer,mrs_string,mrs_real)",new ExFun_TimerUpdReal("mrs_bool","Timer.upd(mrs_timer,mrs_string,mrs_real)"));
    st->addReserved("Timer|Tmr.upd(mrs_timer,mrs_string,mrs_natural)",new ExFun_TimerUpdNatural("mrs_bool","Timer.upd(mrs_timer,mrs_string,mrs_natural)"));
    st->addReserved("Timer|Tmr.upd(mrs_timer,mrs_string,mrs_string)",new ExFun_TimerUpdString("mrs_bool","Timer.upd(mrs_timer,mrs_string,mrs_string)"));
    st->addReserved("Timer|Tmr.upd(mrs_timer,mrs_string,mrs_bool)",new ExFun_TimerUpdBool("mrs_bool","Timer.upd(mrs_timer,mrs_string,mrs_bool)"));
    st->addReserved("Timer|Tmr.ival(mrs_timer,mrs_string)",new ExFun_TimerIntrvlSize("mrs_natural","Timer.ival(mrs_timer,mrs_string)"));
}

ExNode::ExNode()
{
    init();
}
ExNode::ExNode(int k, std::string t)
{
    init();
    setKind(k);
    setType(t);
}
ExNode::ExNode(int k, std::string t, ExVal v)
{
    init();
    setKind(k);
    setType(t);
    value=v;
}
ExNode::ExNode(ExVal v)
{
    init();
    setKind(T_CONST);
    value=v;
}
ExNode::ExNode(const ExNode& v)
{
    setType(v.getType());
    setKind(v.getKind());
    val_str=v.val_str;
    value=v.value;
    next=NULL;
}
void ExNode::init()
{
    next=NULL;
}
bool ExNode::is_const()
{
    return (getKind()==T_CONST);
}
ExNode::~ExNode()
{
    delete next;
}
ExNode* ExNode::copy()
{
    return new ExNode(*this);
}
std::string ExNode::getType() const
{
    return (getKind()==T_CONST) ? value.getType() : type;
}
void ExNode::setType(const std::string t)
{
    type=t;
}
void ExNode::setKind(const int k)
{
    kind=k;
}
std::string ExNode::toString()
{
    std::string o = oot();
    if (next!=NULL) o=o+", "+next->toString();
    return o;
}
std::string ExNode::oot()
{
    return value.toString();
}
ExVal ExNode::eval()
{
    ExVal v = calc(); // evaluate for side-effects
    if (next!=NULL) { return next->eval(); }
    return v;
}
/*** ExFun **************************************************************/
void ExFun::setParams(ExNode* ps)
{
    bool evaluatable=true;
    num_params=0;
    if (ps!=NULL) {
        ExNode* p=ps;
        for (;p!=NULL;num_params++,p=p->next);
        params=new ExNode*[num_params];
        int i=0;
        while(i<num_params) {
            ExNode* param=ps; ps=ps->next; param->next=NULL; // sever the linked list to avoid eval() list
            std::string pt_i=param_types[i];
            std::string pst=param->getType();
            if (pt_i!=pst) {
                if (pt_i=="mrs_real") {
                    if (pst=="mrs_natural") { param=new ExNode_NaturalToReal(param); }
                    else { MRSWARN("ExNode::setParams(ExNode*)  Cannot convert types"); }
                }
                else if (pt_i=="mrs_natural") {
                    if (pst=="mrs_real") { param=new ExNode_RealToNatural(param); }
                    else { MRSWARN("ExNode::setParams(ExNode*)  Cannot convert types"); }
                }
                else { MRSWARN("ExNode::setParams(ExNode*)  Cannot convert types"); }
            }
            params[i]=param;
            if (param->getKind()!=T_CONST) { evaluatable=false; }
            i++;
        }
    }
    const_params=evaluatable;
}
void ExFun::setParamTypes(std::string t)
{
    int s=t.find('('); if (s<0) return;
    int e=t.rfind(')');
    t=t.substr(s+1,e-s-1);
    while (t!="") {
        int x=t.find(',');
        if (x>=0) {
            std::string p=t.substr(0,x);
            param_types.push_back(p);
            t=t.substr(x+1);
        }
        else {
            param_types.push_back(t);
            t="";
        }
    }
}

ExFun::~ExFun()
{
    for (int i=0;i<num_params;i++) { delete params[i]; }
    delete [] params;
}
bool ExFun::is_const()
{
    return (getKind()==T_CONST||((getKind()==T_FUN)&&is_pure&&const_params));
}
void ExFun::setSignature(const std::string s)
{
    signature=s;
    setParamTypes(s);
}

