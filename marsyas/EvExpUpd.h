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
** ERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
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

#ifndef MARSYAS_EV_EXPUPD_H
#define MARSYAS_EV_EXPUPD_H

#include <string>
#include <iostream>

#include "MarControlValue.h"
#include "MarEvent.h"

namespace Marsyas
{

class MarSystem; // forward declaration

class EvExpUpd : public MarEvent {
protected:
    MarSystem* target_;
    std::string cname_;
    std::string expression_;
    struct Expr* exp_list;
    struct Expr* exp_tree;
    bool fail;

public:
    // Constructors
    EvExpUpd(std::string cname, std::string expr);
    EvExpUpd(MarSystem*, std::string cname, std::string expr);
    EvExpUpd(EvExpUpd& e);
    virtual ~EvExpUpd();

    // Set/Get methods
    std::string getCName() const;
    std::string getExpr() const;
    MarSystem* getTarget() const;

    void setCName(std::string cname);
    void setExpr(std::string expr);
    void setTarget(MarSystem* ms);
    void set(MarSystem* ms, std::string cname, std::string expr);

    // Event dispatch
    void dispatch();

    virtual EvExpUpd* clone();
    EvExpUpd& operator=(EvExpUpd& e);
    // the usual stream IO 
    friend std::ostream& operator<<(std::ostream&, MarEvent&);
    friend std::istream& operator>>(std::istream&, MarEvent&);

};

}//namespace Marsyas

#endif
