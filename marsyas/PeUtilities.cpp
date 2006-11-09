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
    \class PeUtilities
    \brief utilties for peaks extraction project
  
*/

#include "PeUtilities.h"

using namespace std;
using namespace Marsyas;



 int 
	 Marsyas::peaks2M (realvec& in, realvec& first, realvec& out, int maxNbPeaks)
 {
	 int i,j,k,l=0;

	 if(&first != NULL  && first(0))
	 	 for (i=0 ; i<maxNbPeaks ; i++)
		 {
			 if(first(i) != 0.0) 
			 {
				 for(k=0;k<nbPeaksParameters;k++)
					 out(l, k) = first(k*maxNbPeaks+i);
				 l++;
			 }
		 }
	 

	 for (j=0 ; j<in.getCols() ; j++)
		 for (i=0 ; i<maxNbPeaks ; i++)
		 {
			 if(in(i, j) != 0.0) 
			 {
				 for(k=0;k<nbPeaksParameters;k++)
					 out(l, k) = in(k*maxNbPeaks+i, j);
				 l++;
			 }
		 }
		 out.stretch(l, nbPeaksParameters);
		 return l;
 }

 void 
	 Marsyas::peaks2V (realvec& in, realvec& last, realvec& out, int maxNbPeaks)
 {
	 mrs_natural i, j, k=0, start=0;
	 mrs_natural frameIndex=-1, startIndex = (mrs_natural) in(0, 5);

	  out.setval(0);
	 if(&last && last(0))
	 {
		 start=1;
		 startIndex +=1;

	 }

	 for (i=0 ; i<in.getRows() ; i++, k++)
	 {
		 if(frameIndex != in(i, 5))
		 {
			 frameIndex = (mrs_natural) in(i, 5);
			 k=0;
		 }
		 if(!start || (start && in(i, 5) >= startIndex))
			 for (j=0 ; j<in.getCols() ; j++)
			 {
				 out(j*maxNbPeaks+k, frameIndex-startIndex) = in(i, j);
			/*	 if(peakFile)
					 peakFile << in(i, j);*/
			 }
	 }

	 if(&last)
	 {
		 for (j=0 ; j<out.getRows() ; j++)
		 {
			 last(j) = out(j, out.getCols()-1);
		 }
	 }
 }



 void Marsyas::extractParameter(realvec&in, realvec&out, pkParameter type)
{
 mrs_natural i, k=0, start=0;
 mrs_natural frameIndex=-1, startIndex = (mrs_natural) in(0, 5);

 for (i=0 ; i<in.getRows() ; i++, k++)
 {
   if(frameIndex != in(i, 5))
		 {
			 frameIndex = (mrs_natural) in(i, 5);
			 k=0;
		 }
		 if(!start || (start && in(i, 5) >= startIndex))
			 {
				 out(k, frameIndex-startIndex) = in(i, type);
			 }
 }
}


mrs_real
Marsyas::compareTwoPeakSets(realvec&f1, realvec&a1, realvec&f2, realvec&a2)
 {
	 mrs_real res=0;
// get the normalization values

// while the smallest set is not empty

	 // take the highest amplitude peak

	 // find the closest frequency peak in the second set

	 // acc the amplitude difference

	 // remove the two peaks

	 return res;
 }