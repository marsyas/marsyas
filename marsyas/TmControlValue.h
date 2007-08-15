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

#ifndef __TMCONTROLVALUE__
#define __TMCONTROLVALUE__

#include <string> 
#include <iostream>

#include "common.h"
namespace Marsyas
{
/** 
    \class TmControlValue
	\ingroup none
    \brief TmControlValue is a simple parameter passing mechanism for updtimer
*/

class MarSystem;

enum {
    tmcv_null=0,
    tmcv_real,
    tmcv_natural,
    tmcv_string,
    tmcv_bool,
    tmcv_vec,
    tmcv_marsystem
};

// define an enum for marsystem type that doesn't class with the mar_* types
//#define mar_marsystem 8192

class TmControlValue
{
protected:
  int type_;

  mrs_real r;
  mrs_natural n;
  bool b;
  std::string s;
  MarSystem* ms;

public:
  TmControlValue();
  TmControlValue(const TmControlValue&);
//  MarControlValue& operator=(const MarControlValue& a);
  TmControlValue(float re);
  TmControlValue(double re);
  TmControlValue(int ne);
  TmControlValue(long int ne);
  TmControlValue(std::string st);
  TmControlValue(const char *);
  TmControlValue(bool be);
  TmControlValue(MarSystem* m);

  mrs_real toReal();
  mrs_natural toNatural();
  bool toBool();
  std::string toString();
  MarSystem* toMarSystem();

  int getType();
  std::string getSType();
  friend std::ostream& operator<<(std::ostream&, const TmControlValue&);
};

}//namespace Marsyas

#endif
 
 
