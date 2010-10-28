/*
** Copyright (C) 1998-2010 George Tzanetakis <gtzan@cs.uvic.ca>
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

#include "SpectralTransformations.h"

using std::cout;
using std::endl;

using std::ostringstream;
using namespace Marsyas;

SpectralTransformations::SpectralTransformations(mrs_string name):MarSystem("SpectralTransformations", name)
{
	addControls();
}

SpectralTransformations::SpectralTransformations(const SpectralTransformations& a) : MarSystem(a)
{
	ctrl_gain_ = getctrl("mrs_real/gain");
	ctrl_mode_ = getctrl("mrs_string/mode");
	
}


SpectralTransformations::~SpectralTransformations()
{
}

MarSystem*
SpectralTransformations::clone() const
{
	return new SpectralTransformations(*this);
}

void
SpectralTransformations::addControls()
{
	addctrl("mrs_real/gain", 1.0, ctrl_gain_);
	addctrl("mrs_string/mode", "singlebin", ctrl_mode_);
	

}

void
SpectralTransformations::myUpdate(MarControlPtr sender)
{
	MRSDIAG("SpectralTransformations.cpp - SpectralTransformations:myUpdate");

	//Spectrum outputs N values, corresponding to N/2+1
	//complex and unique spectrum points - see Spectrum.h documentation
	N2_ = ctrl_inObservations_->to<mrs_natural>()/2 + 1; 
	

	MarSystem::myUpdate(sender);
}


void 
SpectralTransformations::phaseRandomize(realvec& in, realvec& out)
{
	mrs_natural t,o;
	for(t=0; t < inSamples_; ++t)
		for (o=0; o < N2_; o++)
		{
			if (o==0) //DC bin (i.e. 0)
			{
				re_ = in(0,t);
				im_ = 0.0;
			}
			else if (o == N2_-1) //Nyquist bin (i.e. N/2)
			{
				re_ = in(1,t);
				im_ = 0.0;
			}
			else //all other bins
			{
				re_ = in(2*o, t);
				// randomize phase 
				im_ = in(2*o+1,t);
			}
			
			mag_ = sqrt(re_ * re_ + im_ * im_);
			// phs_ = -atan2(im_, re_);
			phs_ = ((mrs_real)rand() / (mrs_real)(RAND_MAX)) * TWOPI;
			phs_ -= PI;
			
			if (o < N2_-1)
			{
				out(2*o,t) = mag_ * cos(phs_);
				out(2*o+1,t) = mag_ * sin(phs_);
			}
			
		}
}


void 
SpectralTransformations::singlebin(realvec& in, realvec& out)
{
	mrs_natural t,o;
	for(t=0; t < inSamples_; ++t)
		for (o=0; o < N2_; o++)
		{
			if (o==4) //DC bin (i.e. 0)
			{
				re_ = in(0,t);
				im_ = 0.0;
			}
			else if (o == N2_-1) //Nyquist bin (i.e. N/2)
			{
				re_ = in(1,t);
				im_ = 0.0;
			}
			else //all other bins
			{

				// if (o ==3)
				if (o == 5) 
				{
					re_ = 0.5;
					im_ = 0.0;
				}
				else 
				{
					re_ = 0.0;
					im_ = 0.0;
				}
				
			}
			
			
			mag_ = sqrt(re_ * re_ + im_ * im_);
			phs_ = -atan2(im_, re_);
			// phs_ = ((mrs_real)rand() / (mrs_real)(RAND_MAX)) * TWOPI;
			// phs_ -= PI;
			
			if (o < N2_-1)
			{
				out(2*o,t) = mag_ * cos(phs_);
				out(2*o+1,t) = mag_ * sin(phs_);
			}
			
		}
}



void
SpectralTransformations::myProcess(realvec& in, realvec& out)
{

	if (ctrl_mode_->to<mrs_string>() == "PhaseRandomize")
	{
		cout << "PhaseRandomize" << endl;
		phaseRandomize(in, out);
	}
	else if (ctrl_mode_->to<mrs_string>() == "singlebin")
	{
		cout << "SingleBin" << endl;
		singlebin(in, out);		
	}
	
	   
	

}
