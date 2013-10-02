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

#ifndef MARSYAS_SCF_H
#define MARSYAS_SCF_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
\ingroup Analysis
\brief Spectral Crest Factor

Spectral Crest Factor is a feature set very similar to the
Spectral Flatness Measure defined in the MPEG-7 audio standard.
Although it is not defined in the MPEG7 standard, its calculation
is here based on the same definitions used for the MPEG7 SFM:
1/4 octave frequency resolution, resulting in 24 frequency bands between
250Hz and 16kHz.
Based on code provided by Luis Gustavo Martins <lmartins@inescporto.pt>
*/


class SCF: public MarSystem
{
private:
  void myUpdate(MarControlPtr sender);

  mrs_natural nrBands_;
  mrs_natural nrValidBands_;

  realvec edge_;
  realvec bandLoEdge_;
  realvec bandHiEdge_;

  mrs_natural spectrumSize_;
  mrs_real df_;
  //realvec spectrumBinFreqs_;
  std::vector<mrs_natural> il_;
  std::vector<mrs_natural> ih_;

public:
  SCF(std::string name);
  ~SCF();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif




