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

#ifndef MARSYAS_ADRESSSTEREOSPECTRUM_H
#define MARSYAS_ADRESSSTEREOSPECTRUM_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
   \class ADRessStereoSpectrum
   \ingroup Analysis
   \brief Takes the output of the ADRess (i.e. the panning-frequency maps)
   and outputs the panning coefficient for each spectral bin (N/2+1 bins).
*/

class ADRessStereoSpectrum: public MarSystem
{
private:
  mrs_natural N2_;
  mrs_natural beta_;
  mrs_real maxVal_;
  mrs_natural maxIndex_;

  void addControls();
  void myUpdate(MarControlPtr sender);

public:
  ADRessStereoSpectrum(std::string name);
  ADRessStereoSpectrum(const ADRessStereoSpectrum& a);
  ~ADRessStereoSpectrum();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

}

#endif

