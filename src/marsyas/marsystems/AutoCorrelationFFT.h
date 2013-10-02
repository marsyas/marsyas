/*
** Copyright (C) 2000 George Tzanetakis <gtzan@cs.princeton.edu>
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
#ifndef MARSYAS_AutoCorrelationFFT_H
#define MARSYAS_AutoCorrelationFFT_H

#include <marsyas/system/MarSystem.h>
#include <marsyas/fft.h>

namespace Marsyas
{
/**
    \class AutoCorrelationFFT
    \brief AutoCorrelation System based on FFT

*/
class AutoCorrelationFFT: public MarSystem
{


private:
  //Add specific controls needed by this MarSystem.
  void addControls();

  fft fft_;
  mrs_realvec temp_;

  //mrs_string resaModeOld_;
  void myUpdate(MarControlPtr sender);


public:
  mrs_real pitch();
  AutoCorrelationFFT(std::string name);
  AutoCorrelationFFT(const AutoCorrelationFFT& a);
  ~AutoCorrelationFFT();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);

};
}

#endif
