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


#ifndef MARSYAS_OSCBANK_H
#define MARSYAS_OSCBANK_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
	\ingroup Synthesis
	\brief A bank of sine wave oscillators used for phasevocoding.
*/


class PvOscBank: public MarSystem
{
private:
  realvec lastamp_;
  realvec lastfreq_;
  realvec magnitudes_;
  realvec regions_;


  realvec index_;
  realvec table_;
  realvec temp_;

  mrs_natural L_;
  mrs_natural N_;
  mrs_natural Nw_;
  mrs_natural NP_;
  mrs_real P_;
  mrs_real PS_;

  mrs_real Iinv_;
  mrs_real Pinc_;
  mrs_real R_;
  mrs_natural I_;
  mrs_real S_;
  mrs_natural size_, psize_;
  mrs_real address_,paddress_, ppaddress_,f_, finc_, a_, ainc_;
  mrs_natural naddress_;
  mrs_real tableval_;

  MarControlPtr ctrl_regions_;
  MarControlPtr ctrl_peaks_;

  MarControlPtr ctrl_frequencies_;
  MarControlPtr ctrl_analysisphases_;
  MarControlPtr ctrl_phaselock_;
  MarControlPtr ctrl_onsetsAudible_;
  MarControlPtr ctrl_rmsIn_;

  int subband(int bin);
  bool isPeak(int bin, mrs_realvec& magnitudes, mrs_real maxAmp);

  void addControls();
  void myUpdate(MarControlPtr sender);

public:
  PvOscBank(std::string name);
  PvOscBank(const PvOscBank&);

  ~PvOscBank();
  MarSystem* clone() const;

  void process1(realvec& in, realvec& out);
  void myProcess(realvec& in, realvec& out);

};

}//namespace Marsyas

#endif






