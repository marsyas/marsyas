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

#ifndef MARSYAS_PESYNFFT_H
#define MARSYAS_PESYNFFT_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
	\ingroup Processing
    \brief  perform amplitude modification of Power Spectrum using Peaks Info

*/


class PeakSynthFFT: public MarSystem
{
private:

  MarControlPtr ctrl_Peaks_;
  MarControlPtr ctrl_NbChannels_;

  mrs_real fgVolume_ ;
  mrs_real fgPanning_ ;
  mrs_real bgVolume_ ;
  mrs_real bgPanning_;
  realvec mask_;
  realvec lastMask_;

  void addControls();
  void myUpdate(MarControlPtr sender);
  void generateMask(mrs_natural type);
  void lpfMask();
public:
  PeakSynthFFT(std::string name);
  PeakSynthFFT(const PeakSynthFFT& a);
  ~PeakSynthFFT();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif
