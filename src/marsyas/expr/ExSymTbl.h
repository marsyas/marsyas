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

#ifndef MARSYAS_SYM_TBL_H
#define MARSYAS_SYM_TBL_H

#include <marsyas/expr/ExVal.h>
#include <marsyas/expr/ExCommon.h>
#include <marsyas/common_header.h>

#include <string>
#include <map>
#include <vector>
#include <cstddef>

namespace Marsyas
{
/**
	\file
	\ingroup Scheduler
	\brief Classes for managing the symbol table - functions and variables
	\author Neil Burroughs  inb@cs.uvic.ca
	\date Jan 4, 2007
*/

class ExNode;
class ExNode_Fun;

/**
	\brief A symbol table node that symbolises a path component to a record
	\author Neil Burroughs  inb@cs.uvic.ca
	\version 1.0
	\date    Jan 01, 2007

	A symbol table node that symbolises a path component to a record, i.e.:
	\code{.expr}
	Real.cos(mrs_real) := ExRecord("Real",ExRecord("cos(mrs_real)"))
	\endcode

	A syntax is defined for adding functions as well as defining aliases
	to those functions. Adding and removing imports is also defined where
	an import is simply a shortcut that eliminates a portion of the start
	of the name.
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
  ExRecord();
  ExRecord(int kind);
  ExRecord(int kind, ExFun* fun, bool reserved);
  ExRecord(int kind, std::string name, ExVal& value, bool reserved);

  virtual ~ExRecord();

  std::string getType(std::string nm="");
  std::string getElemType(std::string nm="");
  int getKind(std::string nm="");
  bool is_reserved(std::string nm="");
  std::size_t size() {return syms_.size();}

  void setValue(ExVal& v, std::string path="", int elem_pos=-1);
  ExVal getValue(std::string path="");
  ExRecord* getRecord(std::string nm);
  ExFun* getFunctionCopy(std::string nm="");
  bool is_list();
  bool is_seq();
  bool params_compare(std::string a, std::string b);
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
  ExRecord* rmvRecord(std::string path);
  void addReserved(std::string path, ExFun* f);
  void addReserved(std::string path, ExVal v, std::string nm="", int kind=T_CONST);

  void import(std::string);
  void rmv_import(std::string);
};

class ExSymTbl : public ExRefCount {
  /*** setup a naming scheme that prefixes an id for each variable name
  		 so that outputting bytecode is easier with variable names ***/
  std::vector<ExRecord*> rho_;
  ExRecord* curr_;
  unsigned int env_id;
public:
  ExSymTbl() : ExRefCount() {env_id=0; curr_=NULL;}
  virtual ~ExSymTbl();

  void block_open();
  void block_close();
  void addTable(ExRecord* r);

  std::size_t size() {return rho_.size();}
  void setValue(ExVal& v, std::string path);

  ExVal getValue(std::string path);
  ExRecord* getRecord(std::string nm);
  ExFun* getFunctionCopy(std::string nm);

  void addRecord(std::string path, ExRecord* sym);
  ExRecord* rmvRecord(std::string path);
  void addReserved(std::string path, ExFun* f);
  void addReserved(std::string path, ExVal v, std::string nm="", int kind=T_CONST);

  void import(std::string);
  void rmv_import(std::string);
};

}//namespace Marsyas

#endif

