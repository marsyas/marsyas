/*
** Copyright (C) 1998-2005 George Tzanetakis <gtzan@cs.uvic.ca>
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

#include <marsyas/sched/EvExpr.h>
#include <marsyas/sched/Scheduler.h>
#include <marsyas/system/MarSystem.h>

using std::ostringstream;
using namespace Marsyas;

EvExpr::EvExpr(MarSystem* target, std::string e, std::string nm) : EvEvent("EvExpr",nm)
{
  expr_=new Expr(target,Ex(e));
}

EvExpr::EvExpr(MarSystem* target, Ex e, Rp r, std::string nm) : EvEvent("EvExpr",nm)
{
  expr_=new Expr(target,e,r);
}

EvExpr::EvExpr(MarSystem* target, ExFile ef, std::string nm) : EvEvent("EvExpr",nm)
{
  expr_=new Expr(target,ef);
}

EvExpr::~EvExpr()
{
  delete expr_;
}

void
EvExpr::setTimer(TmTimer* t)
{
  EvEvent::setTimer(t);
//	if (getType()=="EvExpr") {
//		EvExpr* e = dynamic_cast<EvExpr*>(event_);
//		if (e!=NULL) {
//			Expr* x=e->getExpression();
  if (expr_!=NULL) {
    expr_->setTimer(timer_);
    expr_->post(); // evaluate init expressions
  }
//		}
//	}
}

void
EvExpr::dispatch()
{
  expr_->eval();
}

EvExpr*
EvExpr::clone()
{
  return new EvExpr(*this);
}

bool
EvExpr::repeat()
{
  return expr_->repeat();
}

std::string
EvExpr::repeat_interval()
{
  if (expr_->has_rate()) return expr_->repeat_interval();
  return repeat_.getInterval();
}

void
EvExpr::updctrl(std::string cname, TmControlValue value)
{
  (void) cname; (void) value; // FIXME These values are unused
  MRSWARN("EvExpr:updControl(string,TmControlValue)  updctrl not supported");
}
