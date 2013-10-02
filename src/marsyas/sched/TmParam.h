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

#ifndef MARSYAS_TM_PARAM_H
#define MARSYAS_TM_PARAM_H

#include <marsyas/sched/TmControlValue.h>
#include <marsyas/common_header.h>

#include <string>
#include <iostream>

namespace Marsyas
{
/**
	\class TmParam
	\ingroup Scheduler
	\brief TmParam associates a name with a value and is used for setting parameters of timers
	\author Neil Burroughs  inb@cs.uvic.ca
*/

class TmParam
{
protected:
  std::string name_;
  TmControlValue valu_;

public:
  TmParam(std::string name, TmControlValue valu);
  TmParam(const TmParam&);

  std::string cname() {return name_;}
  TmControlValue value() {return valu_;}

  friend std::ostream& operator<<(std::ostream&, const TmParam&);
};

}//namespace Marsyas

#endif


