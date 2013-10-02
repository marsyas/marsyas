/*
** Copyright (C) 1998-2010 George Tzanetakis <gtzan@cs.uvic.ca>
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

#include <marsyas/expr/ExVal.h>
#include <marsyas/expr/ExNode.h>

using std::ostringstream;
using namespace Marsyas;

/******************************************************************/
void
ExVal::clear_list()
{
  if (list_) {
    for (int i=0; i<natural_; ++i) { list_[i]->deref(); }
    delete [] list_;
    list_=NULL;
  }
}

void
ExVal::clear()
{
  clear_list();
  type_      ="";
  string_    ="";
  bool_      =false;
  natural_   =0;
  real_      =0.0;
  if (fun_) fun_->deref();
  fun_       =NULL;
  timer_     =NULL;
  scheduler_ =NULL;
  list_      =NULL;
}

void ExVal::set(ExFun* x) { clear(); type_=(x==NULL) ? "" : x->getType(); fun_=x; }
void ExVal::set(const mrs_string x) { clear(); type_="mrs_string"; string_=x; }
void ExVal::set(double x) { clear(); type_="mrs_real"; real_=x; }
void ExVal::set(float x) { clear(); type_="mrs_real"; real_=x; }
void ExVal::set(mrs_natural x) { clear(); type_="mrs_natural"; natural_=x; }
void ExVal::set(mrs_bool x) { clear(); type_="mrs_bool"; bool_=x; }
void ExVal::set(TmTimer** t) { clear(); type_="mrs_timer"; timer_=t; }
void ExVal::set(Scheduler** t) { clear(); type_="mrs_scheduler"; scheduler_=t; }

void
ExVal::set(mrs_natural len, ExNode** xs, std::string t)
{
  clear();
  if (xs!=NULL) { list_=xs; if((*xs)!=NULL&&len>0) { t=(*xs)->getType(); } }
  else { len=0; list_=new ExNode*[len]; }
  type_=t+" list"; natural_=len;
}

void
ExVal::set(mrs_natural len, std::string t) // what is this ??
{
  (void) len; // FIXME Unused parameter
  clear(); type_=t; natural_=0; list_=NULL;
}

void
ExVal::set(const ExVal& v)
{
  clear();
  kind_      =v.kind_;
  type_      =v.type_;
  string_    =v.string_;
  natural_   =v.natural_;
  real_      =v.real_;
  bool_      =v.bool_;
  fun_       =(v.fun_==NULL) ? NULL : v.fun_->copy();
  timer_     =v.timer_;
  scheduler_ =v.scheduler_;
  if (is_list()) { // can do this now that type_=v.type_
    list_=new ExNode*[natural_];
    for (int i=0; i<natural_; ++i) {
      list_[i]=v.list_[i];
      list_[i]->inc_ref();
    }
  }
  else list_=NULL;
}

ExVal::~ExVal()
{
  if (fun_!=NULL) fun_->deref();
  clear_list();
}

std::string
ExVal::toString() const
{
  if (type_=="mrs_string") { return string_; }
  else if (type_=="mrs_real") { return dtos(real_); }
  else if (type_=="mrs_natural") { return ltos(natural_); }
  else if (type_=="mrs_bool") { return btos(bool_); }
  else if (type_=="mrs_fun") { return "<mrs_fun>"; }
  else if (type_=="mrs_timer") { return "<mrs_timer>"; }
  else if (type_=="mrs_scheduler") { return "<mrs_scheduler>"; }
  else if (type_=="") { return "unknown value"; }
  return type_;
}

ExVal
ExVal::defaultExValue(std::string type)//{{{
{
  if (type=="mrs_string") return (std::string)"";
  if (type=="mrs_bool") return (bool)false;
  if (type=="mrs_natural") return (long)0;
  if (type=="mrs_real") return (double)0.0;
  if (type=="mrs_timer") { ExVal v((TmTimer**)NULL); return v; }
  if (type=="mrs_scheduler") { ExVal v((Scheduler**)NULL); return v; }
  return ExVal();
}//}}}

std::string
ExVal::getBaseType() const
{
  mrs_natural p=(mrs_natural)type_.find(' ');
  if (p<0) return type_;
  return type_.substr(0,p);
}

std::string
ExVal::getElemType() const
{
  if (type_=="mrs_string") {
    return "mrs_string";
  }
  else if (is_list()) {
    return type_.substr(0,type_.length()-5);
  }
  return "";
}

bool
ExVal::is_list() const
{ // whoa! that's crazy man..
  size_t len = type_.length();
  return (len>3)
         && (type_[len-4]=='l')
         && (type_[len-3]=='i')
         && (type_[len-2]=='s')
         && (type_[len-1]=='t');
}

bool
ExVal::is_seq() const
{
  return type_=="mrs_string"||is_list();
}

ExVal
ExVal::getSeqRange(int lidx, int ridx)
{
  if (!is_seq()) {
    return defaultExValue(getBaseType()); // obviously an error, but what to do?
  }
  if (lidx<0) lidx=0;

  mrs_natural len;
  if (is_list()) {
    len=natural_;
    if (len<=0||lidx>=len) { return ExVal(0,getType()); }

    if (ridx<lidx) ridx=lidx;
    else if (ridx>=len) ridx=len-1;
    mrs_natural new_len=ridx-lidx;

    ExNode** new_list=new ExNode*[new_len];
    int p=0;
    for (int i=lidx; i<ridx; ++i) {
      ExNode* e=list_[i];
      new_list[p]=e; p++;
      e->inc_ref();
    }
    return ExVal(new_len,(ExNode**)new_list);
  }
  else { // mrs_string
    len=(mrs_natural)string_.length();
    if (len<=0||lidx>=len) { return (std::string)""; }

    if (ridx<lidx) ridx=lidx;
    else if (ridx>=len) ridx=len-1;

    return (std::string)string_.substr(lidx,ridx-1);
  }
}

ExVal
ExVal::getSeqElem(int idx)
{
  if (is_list()) {
    if (idx>=natural_||idx<0) {
      MRSWARN("ExVal::getSeqElem  index exceeds list length");
      return defaultExValue(getBaseType());
    }
    return list_[idx]->eval();
  }
  else if (type_=="mrs_string") {
    if (idx>=0&&idx<=(int)string_.length())
      return (std::string)string_.substr(idx,1);
    return (std::string)"";
  }
  else {
    MRSWARN("ExVal::getSeqElem  element access on non-sequence type: "+type_);
    return defaultExValue(getBaseType());
  }
}

void
ExVal::setSeqElem(int idx, ExVal v)
{
  if (idx<0||idx>=natural_) {
    MRSWARN("ExVal::set[]  Index out of bounds");
    return;
  }
  list_[idx]->deref();
  list_[idx]=new ExNode(v);
}

namespace Marsyas {
std::ostream&
operator<<(std::ostream& o, ExVal& v)
{
  bool i_am_a_list = v.is_list();
  if (i_am_a_list) {
    o<<"[";
    for (int i=0; i<v.natural_; ++i) {
      ExVal x=v.list_[i]->eval();
      o<<x;
      if (i<v.natural_-1) { o<<", "; }
    }
    o<<"]";
  }
  else if (v.type_=="mrs_string") o<<"'"<<v.string_<<"'";
  else if (v.type_=="mrs_natural") o<<ltos(v.natural_);
  else if (v.type_=="mrs_real") o<<dtos(v.real_);
  else if (v.type_=="mrs_bool") o<<btos(v.bool_);
  return o;
}
}

ExVal
ExVal::append(const ExVal v) const
{
  if (!is_list()||!v.is_list()) {
    MRSWARN("ExVal::append  only sequence types may be appended: "+getType()+", "+v.getType());
    return *this;
  }
  if (getType()==" list"||v.getType()==" list"||getType()==v.getType()) {
    mrs_natural len=natural_+v.toNatural(); ExNode** elems=new ExNode*[len];
    int l=0;
    if (natural_>0) {
      for (int i=0; i<natural_; ++i,l++) {
        elems[l]=list_[i];
        elems[l]->inc_ref();
      }
    }
    if (v.toNatural()>0) {
      for (int i=0; i<v.toNatural(); ++i,l++) {
        elems[l]=v.list_[i];
        elems[l]->inc_ref();
      }
    }
    return ExVal(len,elems);
  }
  MRSWARN("ExVal::append  type mismatch in list concat: "+getType()+","+v.getType());
  return *this;
}
