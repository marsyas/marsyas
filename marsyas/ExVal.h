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
#ifndef __VAL_H__
#define __VAL_H__

#include <string>
#include <iostream>
#include <cmath>
#include "common.h"
#include "TmTimer.h"
#include "VScheduler.h"

namespace Marsyas
{

std::string dtos(double d);
std::string ltos(long l);
std::string btos(bool b);
long stol(std::string n);
std::string prep_string(std::string s);

class ExNode;
class ExFun;

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
    VScheduler** scheduler_;
    void clear();
public:
    ExVal(){fun_=NULL;clear();};
    ExVal(const std::string x){fun_=NULL;set(x);};
    ExVal(mrs_real x){fun_=NULL;set(x);};
    ExVal(mrs_natural x){fun_=NULL;set(x);};
    ExVal(mrs_bool x){fun_=NULL;set(x);};
    ExVal(ExFun* x){fun_=NULL;set(x);};
    ExVal(TmTimer** x){fun_=NULL;set(x);};
    ExVal(VScheduler** x){fun_=NULL;set(x);};
    ExVal(const ExVal& x){fun_=NULL;set(x);};
    ExVal& operator=(const ExVal& x){set(x);return*this;}
    virtual ~ExVal();

    std::string getType() const {return type_;};
    mrs_natural toNatural() const {return natural_;}
    mrs_real toReal() const {return real_;}
    mrs_bool toBool() const {return bool_;}
    ExFun* toFun() const {return fun_;}
    std::string toString() const;
    TmTimer** toTimer() const {return timer_;}
    VScheduler** toVScheduler() const {return scheduler_;}

    void set(ExFun* x);
    void set(const std::string x);
    void set(mrs_real x);
    void set(mrs_natural x);
    void set(mrs_bool x);
    void set(const ExVal& v);
    void set(TmTimer** t);
    void set(VScheduler** t);
    static ExVal defaultExValue(std::string type);

#define T_BINOP(_T,_VAL,_OP) if (v1.type_==_T) { return v1._VAL _OP v2._VAL; }
#define S_BOP(_OP) T_BINOP("mrs_string",string_,_OP)
#define N_BOP(_OP) T_BINOP("mrs_natural",natural_,_OP)
#define R_BOP(_OP) T_BINOP("mrs_real",real_,_OP)
#define RMOD_BOP() if (v1.type_=="mrs_real") { return fmod(v1.real_,v2.real_); }
#define B_BOP(_OP) T_BINOP("mrs_bool",bool_,_OP)
#define VAL_BINOP(_NAME,_WARN,_TESTS) \
friend inline ExVal _NAME(const ExVal& v1, const ExVal& v2) { \
    _TESTS; \
    MRSWARN(((std::string)_WARN+"  Invalid types ~"+v1.getType()+","+v2.getType())); \
    return v1; \
};

VAL_BINOP(operator==, "ExVal::op==", R_BOP(==); N_BOP(==); S_BOP(==);B_BOP(==));
VAL_BINOP(operator!=, "ExVal::op!=", R_BOP(!=); N_BOP(!=); S_BOP(!=);B_BOP(!=));
VAL_BINOP(operator<=, "ExVal::op<=", R_BOP(<=); N_BOP(<=); S_BOP(<=);B_BOP(<=));
VAL_BINOP(operator< , "ExVal::op<" , R_BOP(< ); N_BOP(< ); S_BOP(< );B_BOP(< ));
VAL_BINOP(operator>=, "ExVal::op>=", R_BOP(>=); N_BOP(>=); S_BOP(>=);B_BOP(>=));
VAL_BINOP(operator> , "ExVal::op>" , R_BOP(> ); N_BOP(> ); S_BOP(> );B_BOP(> ));

VAL_BINOP(operator+, "ExVal::op+", R_BOP(+);   N_BOP(+); S_BOP(+));
VAL_BINOP(operator-, "ExVal::op-", R_BOP(-);   N_BOP(-)          );
VAL_BINOP(operator*, "ExVal::op*", R_BOP(*);   N_BOP(*)          );
VAL_BINOP(operator/, "ExVal::op/", R_BOP(/);   N_BOP(/)          );
VAL_BINOP(operator%, "ExVal::op%", RMOD_BOP(); N_BOP(%)          );

friend inline ExVal operator||(const ExVal& v1, const ExVal& v2) { return v1.bool_ || v2.bool_; };
friend inline ExVal operator&&(const ExVal& v1, const ExVal& v2) { return v1.bool_ && v2.bool_; };

}; //class ExVal
}//namespace Marsyas

#endif

