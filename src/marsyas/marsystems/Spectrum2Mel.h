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

#ifndef MARSYAS_Spectrum2Mel_H
#define MARSYAS_Spectrum2Mel_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
	\class Spectrum2Mel
	\ingroup Analysis
	\brief Convert spectrum magnitude (e.g. output from PowerSpectrum MarSystem)
	into Mel frequency scale.

	In order to duplicate the mel matrix in Slaney's mfcc.m use the following parameters:
	- fftsize = 512
	- srate = 8000Hz
	- melBands = 40
	- bandWidth = 1.0
	- bandLowEdge = 133.33
	- bandHighEdge = 6855.5
	- constAmp = false

	Based in the fft2melmx.m MATLAB script by Dan Ellis:
	http://labrosa.ee.columbia.edu/projects/coversongs/

	Controls:
	- \b mrs_natural/melBands [w] : sets the number of Mel bands to output (default = 40, i.e. one per bark)
	- \b mrs_real/bandWidth [w] : sets the constant width of each band relative to standard Mel (default 1.0)
	- \b mrs_real/bandLowEdge [w] : sets the frequency (in Hz) of the lowest band edge (default 0.0, but 133.33 is a common standard (to skip LF))
	- \b mrs_real/bandHighEdge [w] : sets frequency in Hz of upper edge (default audio srate/2)
	- \b mrs_bool/htkMel [w] : use HTK's version of the mel curve, not Slaney's
	- \b mrs_bool/constAmp [w] : make integration windows peak at 1, not sum to 1

*/

class Spectrum2Mel: public MarSystem
{
private:
  realvec melMap_;

  MarControlPtr ctrl_melBands_;
  MarControlPtr ctrl_bandWidth_;
  MarControlPtr ctrl_bandLowEdge_;
  MarControlPtr ctrl_bandHighEdge_;
  MarControlPtr ctrl_htkMel_;
  MarControlPtr ctrl_constAmp_;

  mrs_natural pmelBands_;
  mrs_real pbandWidth_;
  mrs_real pbandLowEdge_;
  mrs_real pbandHighEdge_;
  bool phtkMel_;
  bool pconstAmp_;

  void addControls();
  void myUpdate(MarControlPtr sender);

public:
  Spectrum2Mel(std::string name);
  Spectrum2Mel(const Spectrum2Mel& a);
  ~Spectrum2Mel();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

}

#endif

