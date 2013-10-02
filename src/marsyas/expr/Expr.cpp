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

#include <marsyas/expr/Expr.h>
#include <marsyas/expr/ExParser.h>
#include <marsyas/expr/ExScanner.h>
#include <marsyas/expr/ExSymTbl.h>
#include <marsyas/expr/ExNode.h>
#include <marsyas/system/MarSystem.h>
#include <marsyas/sched/Scheduler.h>
#include <marsyas/sched/TmTimer.h>

#include <fstream>

using std::ostringstream;
using namespace Marsyas;

void
Ex::parse(Expr* e, ExNode*& init, ExNode*& expr)
{
  ExScanner s;
  ExParser p(&(e->timer_),&s);

  // parse init expression
  if (init_!="") {
    s.setString(init_.c_str());
    p.Parse(e->sched_,e->marsym_,e->symbol_table_);
    init=p.getTree();
  }
  else init=NULL;

  if (expr_!="") {
    s.setString(expr_.c_str());
    p.Parse(e->sched_,e->marsym_,e->symbol_table_);
    expr=p.getTree();
  }
  else expr=NULL;
}

Expr::Expr()
{
  symbol_table_=NULL;
  init_expr_=NULL; expr_=NULL;
  rept_=NULL; rate_=NULL;
  marsym_=NULL;
  sched_=NULL;
  timer_=NULL;
  initialized_=false;
}
Expr::Expr(MarSystem* msym, Ex e)
{
  marsym_=msym;
  timer_=NULL;
  sched_=NULL;
  symbol_table_=new ExRecord();
  symbol_table_->inc_ref();
  e.parse(this,init_expr_,expr_);
  rept_=NULL;
  rate_=NULL;
  initialized_=false;
}
Expr::Expr(MarSystem* msym, Ex e, Rp r)
{
  set(msym,e,r);
}
Expr::Expr(MarSystem* msym, ExFile ef)
{
  Ex e=ef.getEx();
  Rp r=ef.getRp();
  set(msym,e,r);
}
void
Expr::set(MarSystem* m, Ex& e, Rp& r)
{
  marsym_=m;
  timer_=NULL;
  sched_=NULL;
  symbol_table_=new ExRecord();
  symbol_table_->inc_ref();
  e.parse(this,init_expr_,expr_);
  r.parse(this,rept_,rate_);
  if (rept_&&rept_->getEvalType()!="mrs_bool") {
    MRSWARN("Expr::  Repetition expression must evaluate to bool: "+rept_->getEvalType());
    rept_->deref(); rept_=NULL;
    if (rate_) rate_->deref(); rate_=NULL;
  }
  else if (rate_&&rate_->getEvalType()!="mrs_string") {
    MRSWARN("Expr::  Repetition rate expression must evaluate to string: "+rate_->getEvalType());
    if (rept_) rept_->deref(); rept_=NULL;
    if (rate_) rate_->deref(); rate_=NULL;
  }
  initialized_=false;
}

Expr::~Expr()
{
  symbol_table_->deref();
  delete expr_;
  delete init_expr_;
  delete rept_;
  delete rate_;
}

void
Expr::eval()
{
  if (expr_!=NULL)
    expr_->eval();
}

bool
Expr::repeat()
{
  return (rept_)&&(rept_->eval()).toBool();
}

std::string
Expr::repeat_interval()
{
  if (rate_) return (rate_->eval()).toString();
  return "__NULL";
}

void
Expr::setScheduler(Scheduler* v)
{
  sched_=v;
}

void
Expr::setTimer(TmTimer* t)
{
  timer_=t;
}

void
Expr::post()
{
  if (init_expr_&&!initialized_)
    init_expr_->eval();
  initialized_=true;
}

void
ExFile::read(std::string fname)
{
  std::ifstream from(fname.c_str());
  if (!from) { MRSWARN("ExFile::read  Cannot open file: "+fname); return; }
  std::string data;
  char buffer[256];
  // flags correspond to the 5 possible blocks, no block can be declared more than once
  bool flags[6] = { false };
  int pos=-1;
  std::string line;
  while (from.getline(buffer,256)) {
    if (buffer[0]=='#') {
      if (buffer[1]=='E'&&buffer[2]=='x') { // #ExInit: | #ExExpr:
        if (buffer[3]=='I'&&buffer[4]=='n'&&buffer[5]=='i'&&buffer[6]=='t'&&buffer[7]==':') {
          store(pos,data); data=""; pos=1;
          if (flags[pos]) { MRSWARN("ExFile::read  Double declaration of #ExInit: block"); }
        }
        else if (buffer[3]=='E'&&buffer[4]=='x'&&buffer[5]=='p'&&buffer[6]=='r'&&buffer[7]==':') {
          store(pos,data); data=""; pos=2;
          if (flags[pos]) { MRSWARN("ExFile::read  Double declaration of #ExExpr: block"); }
        }
      }
      else if (buffer[1]=='R'&&buffer[2]=='p') {
        if (buffer[3]=='E'&&buffer[4]=='x'&&buffer[5]=='p'&&buffer[6]=='r'&&buffer[7]==':') {
          // #RpExpr:
          store(pos,data); data=""; pos=3;
          if (flags[pos]) { MRSWARN("ExFile::read  Double declaration of #RpExpr: block"); }
        }
        else if (buffer[3]=='R'&&buffer[4]=='a'&&buffer[5]=='t'&&buffer[6]=='e'&&buffer[7]==':') {
          // #RpRate:
          store(pos,data); data=""; pos=4;
          if (flags[pos]) { MRSWARN("ExFile::read  Double declaration of #RpRate: block"); }
        }
      }
      else {
        MRSWARN("ExFile::read  Unknown macro # in ExFile");
      }
      flags[pos]=true;
    }
    else { data=data+buffer; }
  }
  if (!data.empty()) { store(pos,data); }
  from.close();
  file_read_=true;
}

void
ExFile::store(int pos, std::string data)
{
  switch(pos) {
  case 1: iex_=data; break;
  case 2: ex_=data; break;
  case 3: rp_=data; break;
  case 4: rr_=data; break;
  }
}
