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
    \class PeClust
    \brief Cluster peaks from peConvert

  
*/

#include "PeClust.h"

#ifdef _MATLAB_ENGINE_
#include "MATLABengine.h"
#endif 

using namespace std;
using namespace Marsyas;

PeClust::PeClust(string name):MarSystem("PeClust", name)
{
 
	addControls();

	nbParameters_ = 7;
}


PeClust::~PeClust()
{
}

MarSystem* 
PeClust::clone() const 
{
  return new PeClust(*this);
}

void 
PeClust::addControls()
{
  //Add specific controls needed by this MarSystem.
  addctrl("mrs_natural/Clusters", 2);
	addctrl("mrs_natural/Sinusoids", 1);
}

 void
 PeClust::localUpdate()
 {
	 setctrl("mrs_natural/onSamples", 
		 getctrl("mrs_natural/inSamples"));
	 setctrl("mrs_natural/onObservations", 
		 getctrl("mrs_natural/inObservations"));
	 setctrl("mrs_real/osrate", 
		 getctrl("mrs_real/israte"));
 setctrl("mrs_string/onObsNames", 
	 getctrl("mrs_string/inObsNames"));

	 kmax_ = getctrl("mrs_natural/Sinusoids").toNatural();
	 nbClusters_ = getctrl("mrs_natural/Clusters").toNatural();

	 // add a string with the observations to consider for similarity computing

	 nbParameters_ = inObservations_/kmax_;

	 data_.stretch(kmax_*inSamples_, nbParameters_);
}



 void PeClust::peaks2M (realvec& in, realvec& out)
 {
	 int i,j,k,l=0;

	 for (j=0 ; j<in.getCols() ; j++)
		 for (i=0 ; i<kmax_ ; i++)
		 {
			 if(in(i, j) != 0.0) 
			 {
				 for(k=0;k<nbParameters_;k++)
					 out(l, k) = in(k*kmax_+i, j);
				 l++;
			 }
		 }
		 out.stretch(l, nbParameters_);
		 nbPeaks_ = l;
 }

 void PeClust::peaks2V (realvec& in, realvec& out)
 {
	 int i, j, k=0;
	 int frameIndex=-1, startIndex = in(0, 5);

	 for (i=0 ; i<in.getRows() ; i++, k++)
	 {
		 if(frameIndex != in(i, 5))
		 {
			 frameIndex = in(i, 5);
			 k=0;
		 }
		 for (j=0 ; j<in.getCols() ; j++)
		 {
			 out(j*kmax_+k, frameIndex-startIndex) = in(i, j);
		 }
	 }
 }


 void 
	 PeClust::process(realvec& in, realvec& out)
 {
	 checkFlow(in,out);

	 // add last clustered frame
	 data_.stretch(kmax_*(inSamples_+1), nbParameters_);
	 peaks2M(in, data_);
	 
// similarity matrix calculation

// Ncut

// align labeling

// make it progressive plotting
#ifdef _MATLAB_ENGINE_
	 MATLAB->putVariable(data_, "peaks");
//	 MATLAB->evalString("plotPeaks(peaks)");
#endif 

	 peaks2V(data_, out);
out=in;
	 // remove first out frame

	 // store last out frame

 }







	
