/*
** Copyright (C) 1998-2006 George Tzanetakis <gtzan@cs.uvic.ca>
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

#ifndef MARSYAS_CHROMAFILTER_H
#define MARSYAS_CHROMAFILTER_H

#include <marsyas/system/MarSystem.h>
#include "Fanout.h"

namespace Marsyas
{
/**
	\class ChromaFilter
	\ingroup Processing
	\brief Tuning Estimation Filterbank
*/


class ChromaFilter: public MarSystem
{
private:
  void addControls();
  void myUpdate(MarControlPtr sender);

  mrs_real freq;
  mrs_real fs;
  mrs_real q;

  mrs_natural octaves;

  realvec fcoefs;

  Fanout *filterBank;

public:
  ChromaFilter(std::string name);
  ~ChromaFilter();

  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif

