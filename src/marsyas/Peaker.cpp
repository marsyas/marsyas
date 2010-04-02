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

#include "Peaker.h"

using namespace std;
using namespace Marsyas;

Peaker::Peaker(string name):MarSystem("Peaker",name)
{
	addControls();
}

Peaker::~Peaker()
{
}

MarSystem* 
Peaker::clone() const 
{
	return new Peaker(*this);
}

void
Peaker::addControls()
{
	addctrl("mrs_real/peakSpacing", 0.0);
	addctrl("mrs_real/peakStrength", 0.0);
	addctrl("mrs_natural/peakStart", (mrs_natural)0);
	addctrl("mrs_natural/peakEnd", (mrs_natural)0);
	addctrl("mrs_natural/interpolation", (mrs_natural)0);
	addctrl("mrs_real/peakGain", 1.0);
	addctrl("mrs_bool/peakHarmonics", false);
	addctrl("mrs_bool/rmsNormalize", false);
	addctrl("mrs_natural/peakNeighbors", 1);
}



void 
Peaker::myProcess(realvec& in, realvec& out)
{


	mrs_real peakSpacing;
	mrs_real peakStrength;
	mrs_real peakGain;
	mrs_bool peakHarmonics;
	mrs_bool rmsNormalize;
	
	mrs_natural peakStart;
	mrs_natural peakEnd;
	mrs_natural interpolationMode;
	mrs_natural peakNeighbors;
	

	peakSpacing = getctrl("mrs_real/peakSpacing")->to<mrs_real>();
	peakStrength = getctrl("mrs_real/peakStrength")->to<mrs_real>();
	peakStart = getctrl("mrs_natural/peakStart")->to<mrs_natural>();
	peakEnd = getctrl("mrs_natural/peakEnd")->to<mrs_natural>();
	interpolationMode = getctrl("mrs_natural/interpolation")->to<mrs_natural>();
	peakGain = getctrl("mrs_real/peakGain")->to<mrs_real>();
	peakHarmonics = getctrl("mrs_bool/peakHarmonics")->to<mrs_bool>();
	rmsNormalize = getctrl("mrs_bool/rmsNormalize")->to<mrs_bool>();
	peakNeighbors = getctrl("mrs_natural/peakNeighbors")->to<mrs_natural>();
	

	
	if (peakEnd == 0)
	  peakEnd = inSamples_;
	// FIXME This line defines an unused variable
	// mrs_real srate = getctrl("mrs_real/israte")->to<mrs_real>();

	out.setval(0.0);


	peakStrength = 0.0;
	
	

	for (o = 0; o < inObservations_; o++)
	{
		rms_ = 0.0;
		peakSpacing = (mrs_real)(peakSpacing * inSamples_);
		for (t=peakStart+1; t < peakEnd-1; t++)
		{
			rms_ += in(o,t) * in(o,t);
		} 
		if (rms_ != 0.0) 
			rms_ /= (peakEnd - peakStart);
		rms_ = sqrt(rms_);

		mrs_real max;
		mrs_natural maxIndex;

		bool peakFound = false;
		
		
		for (t=peakStart+1; t < peakEnd-1; t++)
		{
			peakFound = true;
			
			// peak has to be larger than neighbors 			
			for (int j = 1; j < peakNeighbors; j++)
			{
				if (in(o,t-j) >= in(o,t)) 
				{
					peakFound = false;
					break;
				}
				if (in(o,t+j) >= in(o,t))
				{
					peakFound = false;
					break;
				}
			}
			
			if ((in(o,t) <= 0.0) || (in(o,t) <= peakStrength * rms_))
			{
				peakFound = false;
			}
			
			if (peakFound) 
			{
				// check for another peak in the peakSpacing area
				max = in(o,t);
				maxIndex = t;
				
				
				for (int j=0; j < (mrs_natural)peakSpacing; j++)
				{
					if (t+j < peakEnd-1)
						if (in(o,t+j) > max)
						{ 
							max = in(o,t+j);
							maxIndex = t+j;
						}
				}
				
				t += (mrs_natural)peakSpacing;
				
				if (rmsNormalize)
				{
					out(o,maxIndex) = in(o,maxIndex) / rms_;
					if(interpolationMode && maxIndex > 0 && maxIndex < inSamples_)
					{
						out(o,maxIndex-1) = in(o,maxIndex-1) /rms_;
						out(o,maxIndex+1) = in(o,maxIndex+1) / rms_;
					}
				}
				else
				{
					out(o,maxIndex) = in(o,maxIndex);
					if(interpolationMode && maxIndex > 0 && maxIndex < inSamples_)
					{
						out(o,maxIndex-1) = in(o,maxIndex-1);
						out(o,maxIndex+1) = in(o,maxIndex+1);
					}
				}
				
			
				// peakNeighbors = 0;
				rms_ = 1.0;
				

				if (peakHarmonics)
				{
					twice_ = 2 * maxIndex;
					half_ = (mrs_natural) (0.5 * maxIndex + 0.5);
					triple_ = 3 * maxIndex;
					third_ = (mrs_natural) (0.33 * maxIndex + 0.5);
					mrs_real double_factor = 0.40;
					mrs_real triple_factor = 0.33;
					
					if (twice_ < (peakEnd - peakStart))
					{
						peakFound = true;
			
						// peak has to be larger than neighbors 			
						for (int j = 1; j < peakNeighbors; j++)
						{
							if (in(o,twice_-j) >= in(o,twice_)) 
							{
								peakFound = false;
								break;
							}
							if (in(o,twice_+j) >= in(o,twice_))
							{
								peakFound = false;
								break;
							}
						}
						
						
						if (peakFound)
						{
							out(o,maxIndex) *= (1-double_factor);
							out(o, maxIndex) +=  double_factor * (in(o,twice_)/rms_);
						}
						
					}
					
					/* 
					if (half_ < (peakEnd - peakStart))
					{
						peakFound = true;
						
						// peak has to be larger than neighbors 			
						for (int j = 1; j < peakNeighbors; j++)
						{
							if (in(o,half_-j) >= in(o,half_)) 
							{
								peakFound = false;
								break;
							}
							if (in(o,half_+j) >= in(o,half_))
							{
								peakFound = false;
								break;
							}
						}
						
						
						if (peakFound)
						{
							out(o, maxIndex) += (in(o, half_)/rms_);
							out(o, half_) = in(o,half_)/rms_ + 0.5 * out(o, maxIndex);
						}
						
						}
					*/ 
					
					
					if (triple_ < (peakEnd - peakStart))
					{
						peakFound = true;
			
						// peak has to be larger than neighbors 			
						for (int j = 1; j < peakNeighbors; j++)
						{
							if (in(o,triple_-j) >= in(o,triple_)) 
							{
								peakFound = false;
								break;
							}
							if (in(o,triple_+j) >= in(o,triple_))
							{
								peakFound = false;
								break;
							}
						}
						
						
						if (peakFound)
						{
							out(o, maxIndex) *= (1-triple_factor);
							out(o, maxIndex) = triple_factor * (in(o,triple_)/rms_ + 0.5);
						}
						
					}
					
					/* 
					if (third_ < (peakEnd - peakStart))
					{
						peakFound = true;
						
						// peak has to be larger than neighbors 			
						for (int j = 1; j < peakNeighbors; j++)
						{
							if (in(o,third_-j) >= in(o,third_)) 
							{
								peakFound = false;
								break;
							}
							if (in(o,third_+j) >= in(o,triple_))
							{
								peakFound = false;
								break;
							}
						}
						
						
						if (peakFound)
						{
							out(o, maxIndex) += (in(o, third_)/rms_);
							out(o, third_) = in(o,third_)/rms_ + 0.5 * out(o, maxIndex);
						}
						
					}
					*/ 
				}
				
				peakFound = true;
			}
			
		}
	}
}









