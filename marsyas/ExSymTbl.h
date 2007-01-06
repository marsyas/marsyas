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
    \class ExRecord.cpp
    \brief classes for managing the symbol table - functions and variables
    \author Neil Burroughs  inb@cs.uvic.ca
    \date Jan 1, 2007
*/
#ifndef __SYM_TBL_H__
#define __SYM_TBL_H__

#include <string>
#include <map>
#include <vector>
#include "ExVal.h"
#include "ExCommon.h"
#include "common.h"

namespace Marsyas
{

class ExNode;
class ExNode_Fun;

/**
   \class ExRefCount
   \brief convenient parent class for reference counted objects.
   \author Neil Burroughs  inb@cs.uvic.ca
   \version 1.0
   \date    Jan 01, 2007
*/
class ExRefCount {
private:
    int ref_count;
protected:
    ExRefCount() { ref_count=0; }
public:
    virtual ~ExRefCount() { }
    // reference counting
    void inc_ref() { ref_count++; }
    void deref() { --ref_count; if(ref_count<1) delete this; }
    int get_ref_count() { return ref_count; }
};
/**
   \class ExRecord
   \brief a symbol table node that symbolises a path component to a record.
          ie Real.cos(mrs_real) := ExRecord("Real",ExRecord("cos(mrs_real)"))
          A syntax is defined for adding functions as well as defining aliases
          to those functions. Adding and removing imports is also defined where
          an import is simply a shortcut that eliminates a portion of the start
          of the name.
   \author Neil Burroughs  inb@cs.uvic.ca
   \version 1.0
   \date    Jan 01, 2007
*/
class ExRecord : public ExRefCount {
private:
    int kind_;
    std::string name_;
    ExVal value_;
    bool reserved_;

    std::map<std::string,std::string> syms_aliases_;
    std::map<std::string,ExRecord*> syms_;

    std::vector<std::string> imports_;

private:
    void split_on(std::string p, char c, std::string& hd, std::string& tl, bool keep=false);
    void rsplit_on(std::string p, char c, std::string& hd, std::string& tl);
    ExRecord* find_sym(std::string nm);

public:
    ExRecord() : ExRefCount() {kind_=0;name_="";reserved_=false;}
    ExRecord(int kind) : ExRefCount() {kind_=kind;name_="";reserved_=false;}
    ExRecord(int kind, ExFun* fun, bool reserved);
    ExRecord(int kind, std::string name, ExVal& value, bool reserved);

    virtual ~ExRecord();

    std::string getType(std::string nm="");
    int getKind(std::string nm="");
    bool is_reserved(std::string nm="");

    void setValue(std::string path, ExVal& v);
    ExVal getValue(std::string path="");
    ExRecord* getRecord(std::string nm);
    ExFun* getFunctionCopy(std::string nm="");

    // addRecord expects the name of the symbol, for functions this includes
    // parameter type information used to differentiate it from other functions
    // aliases may be specified using the | symbol as so:
    // this "Real|R.log|ln(mrs_real)" adds the single symbol:
    //     Real.log(mrs_real)
    // and the aliases:
    //     R ~> Real
    //     ln ~> log
    // allowing the four possible ways of making the same call:
    //     Real.log(mrs_real)
    //     Real.ln(mrs_real)
    //     R.log(mrs_real)
    //     R.ln(mrs_real)
    // If parameters are to appear on the path then only one set is allowed and
    // must appear at the end as in the above examples.
    void addAliases(std::string path, std::string name);
    void addRecord(std::string path, ExRecord* sym);
    void addReserved(std::string path, ExFun* f);
    void addReserved(std::string path, ExVal v, std::string nm="", int kind=T_CONST);

    void import(std::string);
    void rmv_import(std::string);
};

}//namespace Marsyas

#endif

