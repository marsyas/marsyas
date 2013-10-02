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
#include <marsyas/expr/ExNode.h>

using std::ostringstream;
using namespace Marsyas;

void
Marsyas::loadlib_Real(ExRecord* st)
{
  st->addReserved("Real|R.abs(mrs_real)",new ExFun_RealAbs());
  st->addReserved("Real|R.cos(mrs_real)",new ExFun_RealCos());
  st->addReserved("Real|R.acos(mrs_real)",new ExFun_RealACos());
  st->addReserved("Real|R.cosh(mrs_real)",new ExFun_RealCosH());

  st->addReserved("Real|R.sin(mrs_real)",new ExFun_RealSin());
  st->addReserved("Real|R.asin(mrs_real)",new ExFun_RealASin());
  st->addReserved("Real|R.sinh(mrs_real)",new ExFun_RealSinH());

  st->addReserved("Real|R.tan(mrs_real)",new ExFun_RealTan());
  st->addReserved("Real|R.atan(mrs_real)",new ExFun_RealATan());

  st->addReserved("Real|R.log|ln(mrs_real)",new ExFun_RealLog());
  st->addReserved("Real|R.log2(mrs_real)",new ExFun_RealLog2());
  st->addReserved("Real|R.log10(mrs_real)",new ExFun_RealLog10());

  st->addReserved("Real|R.rand()",new ExFun_RealRand());

  st->addReserved("Real|R.sqrt(mrs_real)",new ExFun_RealSqrt());
#define _PI_VAL_ 3.14159265358979323846264338327950288419716939937510
#define _E_VAL_  2.7182818284590452353602874713526624977572470936999595749669676277240766303535
  st->addReserved("Real|R.e",(mrs_real)_E_VAL_);
  st->addReserved("Real|R.pi",(mrs_real)_PI_VAL_);
  st->addReserved("Real|R.pi2",(mrs_real)(_PI_VAL_ / 2.0));
  st->addReserved("Real|R.pi4",(mrs_real)(_PI_VAL_ / 4.0));
  st->addReserved("Real|R.dpr",(mrs_real)(360.0 / (2.0 * _PI_VAL_)));
  st->addReserved("Real|R.rpd",(mrs_real)(( 2.0 * _PI_VAL_) / 360.0));
}

void
Marsyas::loadlib_Natural(ExRecord* st)
{
  st->addReserved("Natural|N.abs(mrs_natural)",new ExFun_NaturalAbs());
  st->addReserved("Natural|N.rand()",new ExFun_NaturalRand());
  st->addReserved("Natural|N.rand(mrs_natural)",new ExFun_NaturalRandRange1());
  st->addReserved("Natural|N.rand(mrs_natural,mrs_natural)",new ExFun_NaturalRandRange2());
  st->addReserved("Natural|N.min(mrs_natural,mrs_natural)",new ExFun_NaturalMin());
  st->addReserved("Natural|N.max(mrs_natural,mrs_natural)",new ExFun_NaturalMax());
  st->addReserved("Natural|N.srand(mrs_natural)",new ExFun_NaturalSRand());
  st->addReserved("Natural|N.randmax",(mrs_natural)RAND_MAX);
}

void
Marsyas::loadlib_String(ExRecord* st)
{
  st->addReserved("String|S.len(mrs_string)",new ExFun_StrLen());
  st->addReserved("String|S.sub(mrs_string,mrs_natural,mrs_natural)",new ExFun_StrSub());
}

void
Marsyas::loadlib_Stream(ExRecord* st)
{
//    st->addReserved("Stream.op",new ExFun_StreamOutString("_fun","Stream.op")); // for lookup
  st->addReserved("Stream.op(mrs_string)",new ExFun_StreamOutString());
  st->addReserved("Stream.op(mrs_real)",new ExFun_StreamOutReal());
  st->addReserved("Stream.op(mrs_natural)",new ExFun_StreamOutNatural());
  st->addReserved("Stream.op(mrs_bool)",new ExFun_StreamOutBool());

  st->addReserved("Stream.opn(mrs_string)",new ExFun_StreamOutNString());
  st->addReserved("Stream.opn(mrs_real)",new ExFun_StreamOutNReal());
  st->addReserved("Stream.opn(mrs_natural)",new ExFun_StreamOutNNatural());
  st->addReserved("Stream.opn(mrs_bool)",new ExFun_StreamOutNBool());
}

void
Marsyas::loadlib_List(ExRecord* st)
{
  st->addReserved("List.len(mrs_list)",new ExFun_ListLen());
}

void
Marsyas::load_symbols(ExRecord* st)
{
  loadlib_Real(st);
  loadlib_Natural(st);
  loadlib_String(st);
  loadlib_Stream(st);
  loadlib_List(st);
}

void
Marsyas::loadlib_timer(ExRecord* st, TmTimer** tmr)
{
  st->addReserved("Timer|Tmr.cur",(TmTimer**)tmr,"Timer.cur",T_VAR);
  st->addReserved("Timer|Tmr.prefix(mrs_timer)",new ExFun_TimerGetPrefix());
  st->addReserved("Timer|Tmr.name(mrs_timer)",new ExFun_TimerGetName());
  st->addReserved("Timer|Tmr.type(mrs_timer)",new ExFun_TimerGetType());
  st->addReserved("Timer|Tmr.time(mrs_timer)",new ExFun_TimerGetTime());
  st->addReserved("Timer|Tmr.upd(mrs_timer,mrs_string,mrs_real)",new ExFun_TimerUpdReal());
  st->addReserved("Timer|Tmr.upd(mrs_timer,mrs_string,mrs_natural)",new ExFun_TimerUpdNatural());
  st->addReserved("Timer|Tmr.upd(mrs_timer,mrs_string,mrs_string)",new ExFun_TimerUpdString());
  st->addReserved("Timer|Tmr.upd(mrs_timer,mrs_string,mrs_bool)",new ExFun_TimerUpdBool());
  st->addReserved("Timer|Tmr.ival(mrs_timer,mrs_string)",new ExFun_TimerIntrvlSize());
}
/*
void Marsyas::loadlib_timer(ExRecord* st, VScheduler** tmr)
{
    st->addReserved("Scheduler|Sched.tmr(mrs_scheduler,mrs_string)",new ExFun_SchedulerFind("mrs_timer","Scheduler.tmr(mrs_scheduler,mrs_string)"));
//    st->addReserved("Scheduler|Sched.addtmr(mrs_scheduler,mrs_string,mrs_string)",new ExFun_SchedulerAddTimer("mrs_bool","Scheduler.addtmr(mrs_scheduler,mrs_string,mrs_string)"));
//    st->addReserved("Scheduler|Sched.post(mrs_scheduler,mrs_timer,mrs_event)",new ExFun_
}
*/
ExNode::ExNode() : ExRefCount()
{
  init();
}

ExNode::ExNode(int k, std::string t) : ExRefCount()
{
  init();
  setKind(k);
  setType(t);
}

ExNode::ExNode(int k, std::string t, ExVal v) : ExRefCount()
{
  init();
  setKind(k);
  setType(t);
  value=v;
}

ExNode::ExNode(ExVal v) : ExRefCount()
{
  init();
  setKind(T_CONST); // what about list types
  setType(v.getType());
//    std::cout<<"ExNode<"<<getType()<<">\n";
  value=v;
}

ExNode::ExNode(const ExNode& v) : ExRefCount()
{
  init();
  setType(v.getType());
  setKind(v.getKind());
  val_str=v.val_str;
  value=v.value;
  next=NULL;
}

void
ExNode::init()
{
  next=NULL;
  inc_ref();
}

bool
ExNode::is_const()
{
  return (getKind()==T_CONST);
}

bool
ExNode::is_list() const
{
  std::string humuhumunukunukuapuaa=getType();
  // whoa! that's crazy man..
  size_t len = humuhumunukunukuapuaa.length();
  return (len>3)
         && (humuhumunukunukuapuaa[len-4]=='l')
         && (humuhumunukunukuapuaa[len-3]=='i')
         && (humuhumunukunukuapuaa[len-2]=='s')
         && (humuhumunukunukuapuaa[len-1]=='t');
}

bool
ExNode::is_seq() const
{
  return getType()=="mrs_string"||is_list();
}

ExNode::~ExNode()
{
  if (next) next->deref();
}

ExNode*
ExNode::copy()
{
  return new ExNode(*this);
}

std::string
ExNode::getType() const
{
//    return (getKind()==T_CONST) ? value.getType() : type;
  return type;
}

std::string
ExNode::getEvalType() const
{
  if (next==NULL) return getType();
  ExNode* e=next; while (e->next!=NULL) { e=e->next; }
  return e->getType();
}

void
ExNode::setType(const std::string t)
{
  type=t;
}

std::string
ExNode::getElemType() const
{
  int r=(int)type.rfind(' ');
  return type.substr(0,r);
}

void
ExNode::setKind(const int k)
{
  kind=k;
}

std::string
ExNode::toString()
{
  std::string o = oot();
  if (next!=NULL) o=o+", "+next->toString();
  return o;
}

std::string
ExNode::oot()
{
  return value.toString();
}

ExVal
ExNode::eval()
{
  ExVal v = calc(); // evaluate for side-effects
  if (next!=NULL) { return next->eval(); }
  return v;
}

ExVal
ExNode::getSeqRange(int lidx, int ridx)
{
  return value.getSeqRange(lidx,ridx);
}

ExVal
ExNode::getSeqElem(int idx)
{
  return value.getSeqElem(idx);
}

void
ExNode::setSeqElem(int idx, ExVal v)
{
  value.setSeqElem(idx, v);
}

/*** ExFun **************************************************************/
void
ExFun::setParams(ExNode* ps)
{
  bool evaluatable=true;
  num_params=0;
  if (ps!=NULL) {
    ExNode* p=ps;
    for (; p!=NULL; num_params++,p=p->next);
    params=new ExNode*[num_params];
    int i=0;
    while(i<num_params) {
      ExNode* param=ps; ps=ps->next; param->next=NULL; // sever the linked list to avoid eval() list
      std::string pt_i=param_types[i];
      std::string pst=param->getType();
      if (pt_i!=pst) {
        if (pt_i=="mrs_real") {
          if (pst=="mrs_natural") { param=new ExNode_NaturalToReal(param); }
        }
        else if (pt_i=="mrs_natural") {
          if (pst=="mrs_real") { param=new ExNode_RealToNatural(param); }
        }
      }
      params[i]=param;
      if (param->getKind()!=T_CONST) { evaluatable=false; }
      ++i;
    }
  }
  const_params=evaluatable;
}

void
ExFun::setParamTypes(std::string t)
{
  int s=(int)t.find('('); if (s<0) return;
  int e=(int)t.rfind(')');
  t=t.substr(s+1,e-s-1);
  while (t!="") {
    int x=(int)t.find(',');
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
  for (int i=0; i<num_params; ++i) { params[i]->deref(); }
  delete [] params;
}

bool
ExFun::is_const()
{
  return (getKind()==T_CONST||((getKind()==T_FUN)&&is_pure&&const_params));
}

void
ExFun::setSignature(const std::string s)
{
  signature=s;
  setParamTypes(s);
}
