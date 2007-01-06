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
   \class Expr
   \brief Expr encapsulates an evaluatable expression.
   \author Neil Burroughs  inb@cs.uvic.ca
   \version 1.0
   \date    Jan 4, 2007
*/
#ifndef __EXPR_H__
#define __EXPR_H__

#include <string>

namespace Marsyas
{

class ExNode;
/**
   \class Ex
   \brief a convenience class that encapsulates an expression string and knows
          how to parse that string to an ExNode expression tree.
*/
class Expr;
class Ex {
    std::string init_; std::string expr_;
public:
    Ex(std::string e) { init_=""; expr_=e; };
    Ex(std::string i, std::string e) { init_=i; expr_=e; };
    void parse(Expr* e, ExNode** init, ExNode** expr);
};
/**
   \class Rp
   \brief Rp is the same as Ex but is interpreted as an expression for
          deciding on repetition.
*/
class Rp : public Ex {
public:
    Rp(std::string e) : Ex(e) {};
    Rp(std::string i, std::string e) : Ex(i,e) {};
};

/**
   \class Expr
   \brief Expr encapsulates an evaluatable expression. 
*/
class ExRecord;
class MarSystem;
class VScheduler;
class TmTimer;
class Expr {
    friend class Ex;
    ExRecord* symbol_table_;
    ExNode* init_expr_; ExNode* expr_;
    ExNode* init_rept_;     ExNode* rept_;

    MarSystem* marsym_;  VScheduler* vsched_;  TmTimer* timer_;

protected:
    ExNode* parse(TmTimer** t, MarSystem* m, std::string expr);

public:
    Expr();
    Expr(MarSystem* msym, Ex e); // repeat_ evaluates to false
    Expr(MarSystem* msym, Ex e, Rp r);

    virtual ~Expr();

    virtual void eval();   // evaluate expression_ for side effects
    virtual bool repeat(); // evaluate repeat_ expression, return result

    void setVScheduler(VScheduler* v);
    void setTimer(TmTimer* t);
    void post();
};

};//namespace Marsyas

#endif

