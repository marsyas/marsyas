/*
** Copyright (C) 1998-2011 George Tzanetakis <gtzan@cs.uvic.ca>
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

#ifndef MARSYAS_ADRESSSPECTRUM_H
#define MARSYAS_ADRESSSPECTRUM_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
   \class ADRessSpectrum
   \ingroup Analysis
   \brief Takes the output of the ADRess (i.e. the stereo azimuth-frequency
   maps) and outputs a single channel spectrum of the part of the freq-azimuth
   plane selected by the d and H controls. This can then be
   inverse transformed back into time domain for resynthesis purposes.

   Controls:
   - \b mrs_real/d		[w] : value between 0.0~1.0, used for selecting the
   portion of the azimuth-frequency plane to be extracted
   - \b mrs_real/H   [w] : sets the azimuth subspace width
   (in percentage of total width of the azimuth plane)
*/

class ADRessSpectrum: public MarSystem
{
private:
  mrs_natural N2_;
  mrs_real re_, im_;
  mrs_natural beta_;

  MarControlPtr ctrl_d_;
  MarControlPtr ctrl_H_;

  void addControls();
  void myUpdate(MarControlPtr sender);

public:
  ADRessSpectrum(std::string name);
  ADRessSpectrum(const ADRessSpectrum& a);
  ~ADRessSpectrum();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

}

#endif

