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

using namespace std;
using namespace Marsyas;

PeResidual::PeResidual(string name):MarSystem("PeResidual", name)
{
  
	addControls();
}

PeResidual::PeResidual(const PeResidual& a) : MarSystem(a)
{
//	ctrl_resVec_ = getctrl("mrs_realvec/resVec");
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
//	addctrl("mrs_realvec/resVec", realvec(), ctrl_resVec_);
//  setctrlState("mrs_realvec/peakSet", true);
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
		mrs_real tmpDiff=0;
		for (t = 0; t < inSamples_; t++)
		{
			out(o,t) =  in(o,t)-in(o+1, t);
			tmpSyn += in(o, t)*in(o, t);
			tmpDiff += out(o, t)*out(o, t);
			tmpOri += in(o+1, t)*in(o+1, t);
		}
		if(tmpDiff && tmpSyn)
		{
			tmpOri/=inSamples_;
			tmpSyn/=inSamples_;
			tmpDiff/=inSamples_;
			snr+= 10*log10 ((tmpOri+MINREAL)/tmpDiff);
		}
	}

 setctrl("mrs_real/snr", snr);

 /*mrs_natural resVecSize = ctrl_resVec_->to<realvec> ().getSize();
ctrl_resVec_->stretch(resVecSize+1);
(**ctrl_resVec_)(resVecSize) = snr;*/

 //   MATLAB_PUT(in, "vec");
 // MATLAB_EVAL("figure(1);clf;plot(vec');");
}







	
