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

#ifndef MARSYAS_EV_EXPR_H
#define MARSYAS_EV_EXPR_H

#include <marsyas/sched/EvEvent.h>
#include <marsyas/sched/TmControlValue.h>
#include <marsyas/expr/Expr.h>
#include <marsyas/system/MarControl.h>

#include <string>
#include <iostream>

namespace Marsyas
{
/**
	\class EvExpr
	\ingroup Scheduler
	\brief Expression Event evaluates the supplied expression when prompted by the scheduler.
	\author Neil Burroughs  inb@cs.uvic.ca
	\version 1.0
	\date    Jan 5, 2007
*/

class MarSystem; // forward declaration
class Scheduler;

class EvExpr : public EvEvent {
protected:
  Scheduler* sched_;
  MarSystem* target_;
  Expr* expr_;

public:
  EvExpr(MarSystem* target, std::string e, std::string nm="Expr");
  EvExpr(MarSystem* target, Ex e, Rp r, std::string nm="Expr");
  EvExpr(MarSystem* target, ExFile ef, std::string nm="Expr");
  virtual ~EvExpr();

  void setTimer(TmTimer* t);

  // Event dispatch
  void dispatch();
  void updctrl(std::string cname, TmControlValue value);

  virtual EvExpr* clone();
  Expr* getExpression() { return expr_; }

  virtual bool repeat();
  virtual std::string repeat_interval();
  virtual void set_repeat(Repeat r) { repeat_=r; }

  // the usual stream IO
  friend std::ostream& operator<<(std::ostream&, EvEvent&);
  friend std::istream& operator>>(std::istream&, EvEvent&);
};

}//namespace Marsyas

#endif
