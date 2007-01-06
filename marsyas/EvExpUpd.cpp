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

/**
   \class EvExpUpd
   \brief EvExpUpd is a sample of an expression that can be evaluated then
          used to update a control
*/
#if 0

#include "EvExpUpd.h"
#include "MarSystem.h"
#include "Expression.h"

using namespace std;
using namespace Marsyas;

EvExpUpd::EvExpUpd(string cname, string expr) : MarEvent("EvExpUpd","ExpUpd")
{
    exp_list=NULL;
    set(NULL,cname,expr);
}
EvExpUpd::EvExpUpd(MarSystem* target, string cname, string expr) : MarEvent("EvExpUpd","ExpUpd")
{
    exp_list=NULL;
    set(target,cname,expr);
}
EvExpUpd::EvExpUpd(EvExpUpd& e) : MarEvent("EvExpUpd","ExpUpd")
{
    if (e.exp_list!=NULL) { exp_list=e.exp_list; e.exp_list->usage_count++; }
    else { setExpr(e.expression_); }
}

EvExpUpd::~EvExpUpd()
{
    if (exp_list!=NULL) { clear_expr_list(exp_list); }
}

string EvExpUpd::getCName() const { return cname_; }
string EvExpUpd::getExpr() const { return expression_; }
MarSystem* EvExpUpd::getTarget() const { return target_; }

void EvExpUpd::setCName(string cname) { cname_=cname; }

void EvExpUpd::setExpr(string expr) {
    // delete an existing parsed expression
    clear_expr_list(exp_list);
    // clear expression failed flag, and store expression string
    fail=false; expression_=expr;
    // get the list of tokens
    exp_list = lex(&fail,expression_);
    // attempt to parse into an expression tree
    if (!fail) { exp_tree = parse(&fail,exp_list); }
    // evaluate to find errors
    if (!fail) { Val v = eval(&fail,target_,exp_tree); }
    if (fail) { clear_expr_list(exp_list); exp_list=NULL; }
    else { exp_list->usage_count++; }
}

void EvExpUpd::setTarget(MarSystem* ms) { target_=ms; }
void EvExpUpd::set(MarSystem* ms, string cname, string expr)
{
    target_=ms;
    cname_=cname;
    setExpr(expr);
}

void EvExpUpd::dispatch()
{
    // don't dispatch if no MarSystem or the expression failed
    if (target_ !=NULL && !fail) {
        Val v = eval(&fail,target_,exp_tree);
        if (!fail) {
            if (v.isreal) { target_->updctrl(cname_,v.r); }
            else { target_->updctrl(cname_,v.n); }
        }
    }
}

EvExpUpd* EvExpUpd::clone() { return new EvExpUpd(*this); }

EvExpUpd& EvExpUpd::operator=(EvExpUpd& e) {
    setExpr(e.expression_);
    return *this;
}
#endif
