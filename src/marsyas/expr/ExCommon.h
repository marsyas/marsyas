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
#ifndef MARSYAS_EX_COMMON_H
#define MARSYAS_EX_COMMON_H

#include <marsyas/common_header.h>

#include <string>
#include <iostream>
#include <sstream>

namespace Marsyas
{

/**
	\file
	\ingroup Scheduler
	\brief Common information for Ex - the expression compiler
	\author Neil Burroughs  inb@cs.uvic.ca
	\version 1.0
	\date    Jan 04, 2007
*/

enum {
  NONE=0,
  T_CONST,
  T_LIB,
  T_FUN, // a function call
  T_VAR,
  T_LIST,

  T_REAL,
  T_NATURAL,
  T_STR,
  T_BOOL,
  T_NAME,
  T_CNAME,
  T_COND, // conditional
  OP_BNEG,
  OP_MNEG,
  OP_ADD,
  OP_SUB,
  OP_MUL,
  OP_DIV,
  OP_MOD,
  OP_AND,
  OP_NE,
  OP_OR,
  OP_EQ,
  OP_GT,
  OP_LT,
  OP_GE,
  OP_LE,
  OP_LINK,
  OP_ASGN, // lchild=expr, rchild=nm
  OP_SETCTRL, // lchild=expr, rchild=nm
  OP_GETCTRL,
  OP_CONV
};

#define ExT_mrs_unit 1
#define ExT_mrs_bool 2
#define ExT_mrs_natural 4
#define ExT_mrs_real 8
#define ExT_mrs_string 16
#define ExT_mrs_timer 32
#define ExT_mrs_scheduler 64


/** \brief convert a marsyas type name to Ex type number
* \param tp a marsyas type string, ie "mrs_string"
* \return integer corresponding to the type as defined in the file ExCommon.h
*/
unsigned int ex_string_to_typeid(std::string tp);
/** \brief convert an ex type id to a marsyas type string
* \param tp ex type id, see file ExCommon.h
* \return marsyas type name corresponding to the input parameter
*/
std::string ex_typeid_to_string(unsigned int tp);

/** \brief convert a double to a string
* \param d a double value to convert
* \return string representing the supplied parameter
*/
std::string dtos(double d);

/** \brief convert a float to a string
* \param d a float value to convert
* \return string representing the supplied parameter
*/
std::string dtos(float d);

/** \brief convert a long to a string
* \param l a long value to convert
* \return string representing the supplied parameter
*/
std::string ltos(long l);

/** \brief convert a boolean to a string
* \param b a boolean value to convert
* \return string representing the supplied parameter, "true" or "false"
*/
std::string btos(bool b);

/** \brief convert a string to a long (integer)
* \param n a string representing an integer
* \return a long value corresponding to the input string
*/
long stol(std::string n);

/**
   \class ExRefCount
	 \ingroup Scheduler
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
  void op_ref() { std::cout<<"Obj<"<<this<<":"<<ref_count<<">\n"; }
};

}
#endif

