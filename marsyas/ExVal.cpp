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
   \class ExVal
   \brief ExVal is the abstract value type for expression tree nodes. Every
          expression results in a ExVal of the basic types: string, natural,
          real, bool.
   \author Neil Burroughs  inb@cs.uvic.ca
   \version 1.0
   \date    Jan 04, 2007
*/
#include "ExNode.h"
#include "ExVal.h"

using namespace std;
using namespace Marsyas;

std::string Marsyas::dtos(mrs_real d) { char nn[256]; sprintf(nn,"%f",d); return std::string(nn); }
std::string Marsyas::ltos(mrs_natural l) { char nn[256]; sprintf(nn,"%ld",l); return std::string(nn); }
std::string Marsyas::btos(mrs_bool b) { return (b) ? "true" : "false"; }
mrs_natural Marsyas::stol(std::string n)
{
    long num=0; unsigned int i=0; bool neg=false;
    if (n[0]=='-') { neg=true; i=1; }
    for (;i<n.length();i++) {
        num = (num*10) + (n[i] - '0');
    }
    return (neg) ? -num : num;
}
std::string Marsyas::prep_string(std::string s)
{
    s=s.substr(1,s.length()-2); // remove quotes
    int c=0; bool f=false;
    for (unsigned int i=0;i<s.length();i++,c++) {
        if (f) {
            if (s[i]=='n') { c--; s[c]='\n'; }
            if (s[i]=='t') { c--; s[c]='\t'; }
            f=false;
        } else s[c]=s[i];
        if (s[i]==92) { f=true; }
    }
    s=s.substr(0,c);
    return s;
}
/******************************************************************/
void ExVal::clear()
{
    type_      ="";
    string_    ="";
    bool_      =false;
    natural_   =0;
    real_      =0.0;
    if (fun_) delete fun_;
    fun_       =NULL;
    timer_     =NULL;
    scheduler_ =NULL;
}
void ExVal::set(ExFun* x)       { clear(); type_=(x==NULL) ? "" : x->getType(); fun_=x; }
void ExVal::set(const string x) { clear(); type_="mrs_string";    string_=x; }
void ExVal::set(mrs_real x)     { clear(); type_="mrs_real";      real_=x; }
void ExVal::set(mrs_natural x)  { clear(); type_="mrs_natural";   natural_=x; }
void ExVal::set(mrs_bool x)     { clear(); type_="mrs_bool";      bool_=x; }
void ExVal::set(TmTimer** t)    { clear(); type_="mrs_timer";     timer_=t; }
void ExVal::set(VScheduler** t) { clear(); type_="mrs_scheduler"; scheduler_=t; }
void ExVal::set(const ExVal& v)
{
    kind_      =v.kind_;
    type_      =v.type_;
    string_    =v.string_;
    natural_   =v.natural_;
    real_      =v.real_;
    bool_      =v.bool_;
    fun_       =(v.fun_==NULL) ? NULL : v.fun_->copy();
    timer_     =v.timer_;
    scheduler_ =v.scheduler_;
}
ExVal::~ExVal()
{
    if (fun_!=NULL) delete fun_;
}
std::string ExVal::toString() const
{
    if (type_=="mrs_string") { return string_; }
    else if (type_=="mrs_real") { return dtos(real_); }
    else if (type_=="mrs_natural") { return ltos(natural_); }
    else if (type_=="mrs_bool") { return btos(bool_); }
    else if (type_=="mrs_fun") { return "<mrs_fun>"; }
    else if (type_=="mrs_timer") { return "<mrs_timer>"; }
    else if (type_=="mrs_scheduler") { return "<mrs_scheduler>"; }
    return "unknown value";
}
ExVal ExVal::defaultExValue(std::string type)//{{{
{
    if (type=="mrs_string") return "";
    if (type=="mrs_bool") return false;
    if (type=="mrs_natural") return (long)0;
    if (type=="mrs_real") return (double)0.0;
    if (type=="mrs_timer") { ExVal v((TmTimer**)NULL); return v; }
    if (type=="mrs_scheduler") { ExVal v((VScheduler**)NULL); return v; }
    return false;
}//}}}

