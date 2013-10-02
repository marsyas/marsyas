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

#ifndef MARSYAS_ENHADRESS_H
#define MARSYAS_ENHADRESS_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
\class EnhADRess
\ingroup Analysis
\brief Azimuth Discrimination and Resynthesis (EnhADRess) implementation,
which takes a stereo input (i.e. input is expected to be the output of a
parallel of two Spectrum MarSystems, one for each stereo channel), and
outputs the magnitudes, phases and panning indexes for N/2+1 bins,
stacked vertically:

[Mag]
[Phases]
[Pan]

This enhanced version of the ADRess algorithm was proposed by Cooney et al,
"An Enhanced implemantation of the ADRess Music Source Separation Algorithm",
121st AES Convention, October 2006.

*/

class EnhADRess: public MarSystem
{
private:
  mrs_natural N4_, N2_;
  mrs_real rel_, iml_, rer_, imr_;
  mrs_real Lk_;
  mrs_real Rk_;
  mrs_real minLk_;
  mrs_real minRk_;
  mrs_real phaseL_;
  mrs_real phaseR_;
  mrs_real deltaPhase_;
  mrs_real azim_;

  void addControls();
  void myUpdate(MarControlPtr sender);

public:
  EnhADRess(std::string name);
  EnhADRess(const EnhADRess& a);
  ~EnhADRess();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

}

#endif

