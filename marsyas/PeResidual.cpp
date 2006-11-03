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

/** 
    \class PeResidual
    \brief Compute the Reconstruction Signal-to-Noise Ratio
*/

#include "PeResidual.h"
#ifdef _MATLAB_ENGINE_
#include "MATLABengine.h"
#endif 
using namespace std;
using namespace Marsyas;

PeResidual::PeResidual(string name):MarSystem("PeResidual", name)
{
  
	addControls();
}


PeResidual::~PeResidual()
{
}

MarSystem* 
PeResidual::clone() const 
{
  return new PeResidual(*this);
}

void 
PeResidual::addControls()
{
  //Add specific controls needed by this MarSystem.
  addctrl("mrs_real/snr", 0.0);
}

 void
PeResidual::myUpdate()
{
	setctrl("mrs_natural/onSamples", 
		getctrl("mrs_natural/inSamples"));
	setctrl("mrs_natural/onObservations", 
		getctrl("mrs_natural/inObservations")->toNatural()/2);
	setctrl("mrs_real/osrate", 
		getctrl("mrs_real/israte"));
	setctrl("mrs_string/onObsNames", 
		getctrl("mrs_string/inObsNames"));
}

void 
PeResidual::myProcess(realvec& in, realvec& out)
{
  //checkFlow(in,out);

  mrs_real snr=0; 
  
  for (o=0; o < inObservations_/2; o++)
	{
		mrs_real tmpOri=0;
    mrs_real tmpSyn=0;
    for (t = 0; t < inSamples_; t++)
    {
			out(o,t) =  in(o,t)-in(o+1, t);
			tmpOri += in(o, t)*in(o, t);
			tmpSyn += out(o, t)*out(o, t);
    }
			tmpOri/=inSamples_;
			tmpSyn/=inSamples_;
			snr+= 10*log10 (tmpOri/tmpSyn);
	}
 
 setctrl("mrs_real/snr", snr);

 	#ifdef _MATLAB_ENGINE_
	 MATLAB->putVariable(in, "vec");

//	 MATLAB->evalString("figure(1);clf;plot(vec');");
	#endif
}







	
