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

#ifndef MARSYAS_EXPR_H
#define MARSYAS_EXPR_H

#include <string>

namespace Marsyas
{
/**
	\file
	\ingroup Scheduler
	\author Neil Burroughs  inb@cs.uvic.ca
	\version 1.0
	\date Jan 01, 2007

	Expr encapsulates an evaluatable expression. Ex is a convenience
	class that encapsulates an expression string and can parse to a
	tree. Rp is the same as Ex but is interpreted as an expression for
	deciding on repetition.
*/

class ExNode;
class Expr;
/**
	\ingroup Scheduler
	\author Neil Burroughs  inb@cs.uvic.ca
	\version 1.0
	\date Jan 01, 2007
	\brief Convenience class that encapsulates an expression string and knows
		how to parse that string to an ExNode expression tree.
*/
class Ex {
  std::string init_; std::string expr_;
public:
  Ex(std::string e) { init_=""; expr_=e; };
  Ex(std::string i, std::string e) { init_=i; expr_=e; };
  void parse(Expr* e, ExNode*& init, ExNode*& expr);
};
/**
	\ingroup Scheduler
	\author Neil Burroughs  inb@cs.uvic.ca
	\version 1.0
	\date Jan 01, 2007
	\brief Rp works in a similar way to Ex except that the single parameter
		constructor is an expression that must evaluate to a boolean that
		determines if the event is to repeat while the two parameter
		constructor must be a boolean expression (if true then repeat)
		and an expression that evaluates to a string that specifies when
		the event is to repeat with respect to the current time and in a
		representation of time that is known to the timer (ie '1s' on sample count timer).
*/
class Rp : public Ex {
public:
  Rp(std::string e) : Ex(e,"") {};
  Rp(std::string e, std::string r) : Ex(e,r) {};
};
/**
	\ingroup Scheduler
	\author Neil Burroughs  inb@cs.uvic.ca
	\version 1.0
	\date Jan 01, 2007
	\brief Convenience class for placing Ex and Rp expressions in a separate file
		to be read at parse time. Expressions are separated by declaring block
		headers:
		\code{.expr}
		#ExInit:
		#ExExpr:
		#RpInit:
		#RpExpr:
		#RpRate:
		\endcode
*/
class ExFile {
  std::string iex_, ex_, rp_, rr_;
  bool file_read_;
  void read(std::string fname);
  void store(int pos, std::string data);
public:
  ExFile(std::string n) { file_read_=false; read(n); }
  Ex getEx() { return Ex(iex_,ex_); }
  Rp getRp() { return Rp(rp_,rr_); }
};

class ExRecord;
class MarSystem;
class Scheduler;
class TmTimer;

/**
	\ingroup Scheduler
	\author Neil Burroughs  inb@cs.uvic.ca
	\version 1.0
	\date Jan 01, 2007
	\brief Expr encapsulates an evaluatable expression.
*/
class Expr {
  friend class Ex;
  bool initialized_;
  ExRecord* symbol_table_;
  ExNode* init_expr_; ExNode* expr_;
  ExNode* rept_; ExNode* rate_;

  MarSystem* marsym_;  Scheduler* sched_;  TmTimer* timer_;

  void set(MarSystem* m, Ex& e, Rp& r);

public:
  Expr();
  Expr(MarSystem* msym, Ex e); // repeat_ evaluates to false
  Expr(MarSystem* msym, Ex e, Rp r);
  Expr(MarSystem* msym, ExFile e);

  virtual ~Expr();

  virtual void eval();   // evaluate expression_ for side effects
  virtual bool repeat(); // evaluate repeat_ expression, return result
  std::string repeat_interval();
  bool has_rate() { return rate_!=NULL; }

  void setScheduler(Scheduler* v);
  void setTimer(TmTimer* t);
  void post();
};

}//namespace Marsyas

#endif

