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
   \class Expr, Ex, Rp
   \brief Expr encapsulates an evaluatable expression. Ex is a convenience
          class that encapsulates an expression string and can parse to a
          tree. Rp is the same as Ex but is interpreted as an expression for
          deciding on repetition.
   \author Neil Burroughs  inb@cs.uvic.ca
   \version 1.0
   \date    Jan 01, 2007
*/
#include "Expr.h"
#include "ExParser.h"
#include "ExScanner.h"
#include "ExSymTbl.h"
#include "ExNode.h"
#include "MarSystem.h"
#include "VScheduler.h"
#include "TmTimer.h"

using namespace std;
using namespace Marsyas;

void Ex::parse(Expr* e, ExNode** init, ExNode** expr)
{
    ExScanner s;
    ExParser p(&(e->timer_),&s);

    // parse init expression
    if (init_!="") {
        s.setString(init_.c_str());
        p.Parse(e->vsched_,e->marsym_,e->symbol_table_);
        *init=p.getTree();
    } else *init=NULL;

    if (expr_!="") {
        s.setString(expr_.c_str());
        p.Parse(e->vsched_,e->marsym_,e->symbol_table_);
        *expr=p.getTree();
    } else *expr=NULL;
}
Expr::Expr()
{
    symbol_table_=NULL;
    init_expr_=NULL; expr_=NULL;
    init_rept_=NULL; rept_=NULL;
    marsym_=NULL;
    vsched_=NULL;
    timer_=NULL;
}
Expr::Expr(MarSystem* msym, Ex e)
{
    marsym_=msym;
    timer_=NULL;
    vsched_=NULL;
    symbol_table_=new ExRecord();
    symbol_table_->inc_ref();
    e.parse(this,&init_expr_,&expr_);
    rept_=new ExNode(false); // so that a call to Expr::repeat() works
    init_rept_=NULL;
}
Expr::Expr(MarSystem* msym, Ex e, Rp r)
{
    marsym_=msym;
    timer_=NULL;
    vsched_=NULL;
    symbol_table_=new ExRecord();
    symbol_table_->inc_ref();
    e.parse(this,&init_expr_,&expr_);
    r.parse(this,&init_rept_,&rept_);
}
Expr::~Expr()
{
    symbol_table_->deref();
    delete expr_;
    delete init_expr_;
    delete rept_;
    delete init_rept_;
}
void Expr::eval()
{
    if (expr_!=NULL) expr_->eval();
}
bool Expr::repeat()
{
    return (rept_->eval()).toBool();
}
void Expr::setVScheduler(VScheduler* v)
{
    vsched_=v;
}
void Expr::setTimer(TmTimer* t)
{
    timer_=t;
}
void Expr::post()
{
    if (init_expr_) init_expr_->eval();
    if (init_rept_) init_rept_->eval();
}

