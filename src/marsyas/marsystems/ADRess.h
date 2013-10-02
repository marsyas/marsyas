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

#ifndef MARSYAS_ADRESS_H
#define MARSYAS_ADRESS_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
	\ingroup Analysis
	\brief Azimuth Discrimination and Resynthesis (ADRess) - sound source separation.

	ADRess takes a stereo input (i.e. input is expected to be the output of a
	parallel of two Spectrum MarSystems, one for each stereo channel), and
	outputs the phase vector and frequency dependent nulls matrix for
	each channel using the following format:

	[bin_phases_l][AZl]
	[bin_phases_r][AZr]

	This algorithm was proposed by Dan Barry et al at the DAfX04,
	"Sound Source Separation: azimuth discrimination and resynthesis".
	The algorithm exploits the use of pan pot as a means to achieve image
	localisation within stereophonic recordings, assuming only an interaural
	intensity difference exists between left and right channels for a single
	source. A gain scaling and phase cancellation technique is then used to
	expose frequency dependent nulls across the azimuth domain, from which
	source separation and resynthesis may be  carried out.

	Controls:
	- \b mrs_natural/beta [w] : Sets the azimuth resolution
*/

class ADRess: public MarSystem
{
private:
  mrs_natural N4_, N2_;
  mrs_real rel_, iml_, rer_, imr_;
  mrs_real minAZr_;
  mrs_real minAZl_;
  mrs_real maxAZr_;
  mrs_real maxAZl_;

  MarControlPtr ctrl_beta_;

  void addControls();
  void myUpdate(MarControlPtr sender);

public:
  ADRess(std::string name);
  ADRess(const ADRess& a);
  ~ADRess();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

}

#endif

