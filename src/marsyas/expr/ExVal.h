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

#ifndef MARSYAS_EX_VAL_H
#define MARSYAS_EX_VAL_H

#include <marsyas/sched/TmTimer.h>
#include <marsyas/sched/Scheduler.h>
#include <marsyas/common_header.h>

#include <string>
#include <iostream>
#include <cmath>

namespace Marsyas
{
/**
   \class ExVal
   \ingroup Scheduler
   \brief ExVal is the abstract value type for expression tree nodes. Every
   expression results in a ExVal of the basic types: string, natural,
   real, bool.
   \author Neil Burroughs  inb@cs.uvic.ca
   \version 1.0
   \date    Jan 04, 2007
*/
class ExNode;
class ExFun;
class ExNode_List;

class ExVal {
private:
  int kind_;
  std::string type_;
  std::string string_;
  mrs_natural natural_;
  mrs_real real_;
  mrs_bool bool_;

  ExFun* fun_;
  TmTimer** timer_;
  Scheduler** scheduler_;
  ExNode** list_; // use natural_ as length
protected:
  void clear();
  void clear_list();
  void setKindType(int k, std::string t) { kind_=k; type_=t; }
public:
  ExVal()                               {list_=NULL; fun_=NULL; clear();};
  ExVal(const std::string x)            {list_=NULL; fun_=NULL; set(x);};
  ExVal(double x)                     {list_=NULL; fun_=NULL; set(x);};
  ExVal(float x)                     {list_=NULL; fun_=NULL; set(x);};
//     ExVal(mrs_real x)                     {list_=NULL;fun_=NULL;set(x);};
  ExVal(mrs_natural x)                  {list_=NULL; fun_=NULL; set(x);};
  ExVal(mrs_bool x)                     {list_=NULL; fun_=NULL; set(x);};
  ExVal(ExFun* x)                       {list_=NULL; fun_=NULL; set((ExFun*)x);};
  ExVal(TmTimer** x)                    {list_=NULL; fun_=NULL; set((TmTimer**)x);};
  ExVal(Scheduler** x)                 {list_=NULL; fun_=NULL; set((Scheduler**)x);};
  ExVal(mrs_natural len, ExNode** xs, std::string t="")   {list_=NULL; fun_=NULL; set(len,(ExNode**)xs,t);}; // list
  ExVal(mrs_natural len, std::string t) {list_=NULL; fun_=NULL; set(len,(std::string)t);}; // empty list
  ExVal(const ExVal& x)                 {list_=NULL; fun_=NULL; set((ExVal&)x);};
  ExVal& operator=(const ExVal& x) {set(x); return*this;}
  virtual ~ExVal();

  std::string getType() const {return type_;};
  std::string getBaseType() const;
  std::string getElemType() const;
  bool is_list() const;
  bool is_seq() const;
  mrs_natural toNatural() const {return natural_;}
  mrs_real toReal() const {return real_;}
  mrs_bool toBool() const {return bool_;}
  ExFun* toFun() const {return fun_;}
  std::string toString() const;
  TmTimer** toTimer() const {return timer_;}
  Scheduler** toScheduler() const {return scheduler_;}

  ExVal getSeqRange(int lidx, int ridx);
  ExVal getSeqElem(int idx);
  void setSeqElem(int idx, ExVal v);
  ExVal append(const ExVal v) const;

  void set(ExFun* x);
  void set(const std::string x);
  void set(double x);
  void set(float x);
  void set(mrs_natural x);
  void set(mrs_bool x);
  void set(const ExVal& v);
  void set(TmTimer** t);
  void set(Scheduler** t);
  void set(mrs_natural len, ExNode** xs, std::string t="");
  void set(mrs_natural len, std::string t); // empty list
  static ExVal defaultExValue(std::string type);

#define LIST_CONCAT													\
		if (v1.is_list()&&v2.is_list()) { return v1.append(v2); }

#define T_BINOP(_T,_VAL,_OP,_CAST) if (v1.type_==_T) { return _CAST(v1._VAL _OP v2._VAL); }
#define S_BOP(_OP,_CAST) T_BINOP("mrs_string",string_,_OP,_CAST)
#define N_BOP(_OP,_CAST) T_BINOP("mrs_natural",natural_,_OP,_CAST)
#define R_BOP(_OP,_CAST) T_BINOP("mrs_real",real_,_OP,_CAST)
#define RMOD_BOP() if (v1.type_=="mrs_real") { return fmod(v1.real_,v2.real_); }
#define B_BOP(_OP,_CAST) T_BINOP("mrs_bool",bool_,_OP,_CAST)



// need to make non-inline version with MRSWARN

#define VAL_BINOP(_NAME,_WARN,_TESTS)									\
		friend inline ExVal _NAME(const ExVal& v1, const ExVal& v2)		\
		{																\
			_TESTS;														\
			((std::string)_WARN+"  Invalid types ~"+v1.getType()+","+v2.getType()); \
			return v1;													\
		};

  VAL_BINOP(operator==, "ExVal::op==", R_BOP(==,(bool)); N_BOP(==,(bool)); S_BOP(==,(bool)); B_BOP(==,(bool)));
  VAL_BINOP(operator!=, "ExVal::op!=", R_BOP(!=,(bool)); N_BOP(!=,(bool)); S_BOP(!=,(bool)); B_BOP(!=,(bool)));
  VAL_BINOP(operator<=, "ExVal::op<=", R_BOP(<=,(bool)); N_BOP(<=,(bool)); S_BOP(<=,(bool)); B_BOP(<=,(bool)));
  VAL_BINOP(operator< , "ExVal::op<" , R_BOP(< ,(bool)); N_BOP(< ,(bool)); S_BOP(< ,(bool)); B_BOP(< ,(bool)));
  VAL_BINOP(operator>=, "ExVal::op>=", R_BOP(>=,(bool)); N_BOP(>=,(bool)); S_BOP(>=,(bool)); B_BOP(>=,(bool)));
  VAL_BINOP(operator> , "ExVal::op>" , R_BOP(> ,(bool)); N_BOP(> ,(bool)); S_BOP(> ,(bool)); B_BOP(> ,(bool)));

  VAL_BINOP(operator+, "ExVal::op+", R_BOP(+,(mrs_real)); N_BOP(+,(mrs_natural)); S_BOP(+,(std::string)); LIST_CONCAT;);
  VAL_BINOP(operator-, "ExVal::op-", R_BOP(-,(mrs_real)); N_BOP(-,(mrs_natural))          );
  VAL_BINOP(operator*, "ExVal::op*", R_BOP(*,(mrs_real)); N_BOP(*,(mrs_natural))          );
  VAL_BINOP(operator/, "ExVal::op/", R_BOP(/,(mrs_real)); N_BOP(/,(mrs_natural))          );
  VAL_BINOP(operator%, "ExVal::op%", RMOD_BOP();          N_BOP(%,(mrs_natural))          );

  friend inline ExVal operator||(const ExVal& v1, const ExVal& v2) { return v1.bool_ || v2.bool_; };
  friend inline ExVal operator&&(const ExVal& v1, const ExVal& v2) { return v1.bool_ && v2.bool_; };
  friend std::ostream& operator<<(std::ostream& o, ExVal& v);

}; //class ExVal

class ExValTyped : public ExVal {
public:
  ExValTyped(int k, std::string t) : ExVal() { setKindType(k,t); };
  virtual ~ExValTyped() {}
};

}//namespace Marsyas

#endif

