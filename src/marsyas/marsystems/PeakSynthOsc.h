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

#ifndef MARSYAS_PESYNOSC_H
#define MARSYAS_PESYNOSC_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
\ingroup Processing
\brief Multiply with window (both length Nw) using modulus arithmetic;
fold and rotate windowed input into output array (FFT) (length N)
according to current input time (t).
*/


class PeakSynthOsc: public MarSystem
{
private:
  mrs_real factor_;
  mrs_natural delay_;
  mrs_real mulF_;
  mrs_real mulA_;
  mrs_natural pkGrp2Synth_;
  mrs_natural Nb_;
  mrs_natural nbH_;
  mrs_bool silence_;

  MarControlPtr ctrl_samplingFreq_;
  MarControlPtr ctrl_peakGroup2Synth_;
  MarControlPtr ctrl_isSilence_;
  MarControlPtr ctrl_synSize_;
  MarControlPtr ctrl_delay_;
  MarControlPtr ctrl_harmonize_;

  void addControls();
  void myUpdate(MarControlPtr sender);

  void sine(realvec& out, mrs_real f, mrs_real a, mrs_real p);

public:
  PeakSynthOsc(std::string name);
  PeakSynthOsc(const PeakSynthOsc&);
  ~PeakSynthOsc();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);

};

}//namespace Marsyas;

#endif










