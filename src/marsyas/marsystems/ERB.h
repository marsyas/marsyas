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

#ifndef MARSYAS_ERB_H
#define MARSYAS_ERB_H

#include <marsyas/system/MarSystem.h>
#include "Fanout.h"

namespace Marsyas
{
/**
    \class ERB
	\ingroup Processing
    \brief ERB Filterbank

    ERB (Equivalent rectangular bandwith) filterbank. Based on
the code from the Auditory Toolbox by Malcolm Slaney.
*/


class ERB: public MarSystem
{
private:
  void addControls();
  void myUpdate(MarControlPtr sender);

  mrs_real lowFreq;
  mrs_real highFreq;
  mrs_real fs;
  mrs_real EarQ;
  mrs_real minBW;

  mrs_real A0, A2, B0;

  mrs_natural numChannels;
  mrs_natural order;

  realvec fcoefs;
  realvec centerFreqs;

  mrs_real E(mrs_real);
  mrs_real B(mrs_real);
  mrs_real B1(mrs_real, mrs_real);
  mrs_real B2(mrs_real);
  mrs_real A11(mrs_real, mrs_real);
  mrs_real A12(mrs_real, mrs_real);
  mrs_real A13(mrs_real, mrs_real);
  mrs_real A14(mrs_real, mrs_real);
  mrs_real gain(mrs_real, mrs_real);
  mrs_real abs(mrs_real, mrs_real);

  Fanout *filterBank;

public:
  ERB(std::string name);
  ~ERB();

  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif

