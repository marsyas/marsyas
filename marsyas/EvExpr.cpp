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
   \class EvExpr
   \brief Expression Event evaluates the supplied expression when prompted
          by the scheduler.
   \author Neil Burroughs  inb@cs.uvic.ca
   \version 1.0
   \date    Jan 5, 2007
*/

#include "EvExpr.h"
#include "MarSystem.h"
#include "VScheduler.h"

using namespace std;
using namespace Marsyas;

EvExpr::EvExpr(MarSystem* target, std::string e, std::string nm) : MarEvent("EvExpr",nm)
{
    expr_=new Expr(target,Ex(e));
}
EvExpr::EvExpr(MarSystem* target, Ex e, Rp r, std::string nm) : MarEvent("EvExpr",nm)
{
    expr_=new Expr(target,e,r);
}
EvExpr::EvExpr(MarSystem* target, ExFile ef, std::string nm) : MarEvent("EvExpr",nm)
{
    expr_=new Expr(target,ef);
}
EvExpr::~EvExpr()
{
    delete expr_;
}
void EvExpr::dispatch()
{
     expr_->eval();
}
EvExpr* EvExpr::clone()
{
    return new EvExpr(*this);
}
bool EvExpr::repeat()
{
    return expr_->repeat();
}
std::string EvExpr::repeat_interval()
{
    if (expr_->has_rate()) return expr_->repeat_interval();
    return repeat_.interval;
}

void EvExpr::updctrl(std::string cname, TmControlValue value)
{
    MRSWARN("EvExpr:updctrl(string,TmControlValue)  updctrl not supported");
}

