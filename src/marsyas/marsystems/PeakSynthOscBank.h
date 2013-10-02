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

#ifndef MARSYAS_PeakSynthOscBank_H
#define MARSYAS_PeakSynthOscBank_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
\ingroup Synthesis
\brief A bank of sine wave oscillators used for pevocoding.

*/


class PeakSynthOscBank: public MarSystem
{
private:
  realvec lastamp_;
  realvec nextamp_;
  realvec lastfreq_;
  realvec nextfreq_;
  realvec index_;
  realvec nextindex_;
  realvec table_;
  mrs_natural nbH_;
  mrs_natural L_;
  mrs_natural N_;
  mrs_natural NP_;
  mrs_real P_;
  mrs_real Iinv_;
  mrs_real Pinc_;
  mrs_real R_;
  mrs_natural I_;
  mrs_real S_;
  mrs_natural size_, psize_;
  mrs_real address_,paddress_, ppaddress_,f_, finc_, a_, ainc_;
  mrs_natural naddress_;
  mrs_real tableval_;

  MarControlPtr ctrl_harmonize_;

  void addControls();
  void myUpdate(MarControlPtr sender);

public:
  PeakSynthOscBank(std::string name);
  PeakSynthOscBank(const PeakSynthOscBank&);

  ~PeakSynthOscBank();
  MarSystem* clone() const;

  void process1(realvec& in, realvec& out);
  void myProcess(realvec& in, realvec& out);

};

}//namespace Marsyas

#endif






