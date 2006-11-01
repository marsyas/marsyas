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
	addctrl("mrs_string/similarityType", "");
}

 void
 PeClust::myUpdate()
 {
	 setctrl("mrs_natural/onSamples", 
		 getctrl("mrs_natural/inSamples"));
	 setctrl("mrs_natural/onObservations", 
		 getctrl("mrs_natural/inObservations"));
	 setctrl("mrs_real/osrate", 
		 getctrl("mrs_real/israte"));
 setctrl("mrs_string/onObsNames", 
	 getctrl("mrs_string/inObsNames"));

	 kmax_ = getctrl("mrs_natural/Sinusoids")->toNatural();
	 nbClusters_ = getctrl("mrs_natural/Clusters")->toNatural();

	 // string with the observations to consider for similarity computing
   similarityType_ = getctrl("mrs_string/similarityType")->toString();

	 nbParameters_ = inObservations_/kmax_;

	 data_.stretch(kmax_*inSamples_, nbParameters_);
	 lastFrame_.stretch(kmax_*nbParameters_);
	 lastFrame_.setval(0);
	 maxLabel_=0;
 }



 void PeClust::peaks2M (realvec& in, realvec& first, realvec& out)
 {
	 int i,j,k,l=0;

	 if(&first != NULL  && first(0))
	 	 for (i=0 ; i<kmax_ ; i++)
		 {
			 if(first(i) != 0.0) 
			 {
				 for(k=0;k<nbParameters_;k++)
					 out(l, k) = first(k*kmax_+i);
				 l++;
			 }
		 }
	 

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

 void PeClust::peaks2V (realvec& in, realvec& last, realvec& out)
 {
	 int i, j, k=0, start=0;
	 int frameIndex=-1, startIndex = in(0, 5);

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
			 frameIndex = in(i, 5);
			 k=0;
		 }
		 if(!start || (start && in(i, 5) >= startIndex))
			 for (j=0 ; j<in.getCols() ; j++)
			 {
				 out(j*kmax_+k, frameIndex-startIndex) = in(i, j);
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

 void
	 PeClust::similarityMatrix(realvec &data, realvec& m, string type)
 {
	 int i, j;
	 realvec vec(nbPeaks_);

	 m.setval(1);

	 for (i=0 ; i<type.length() ; i+=2)
	 {
		 switch(type[i])
		 {
		 case 'f':
			 j=0;
			 break;
		 case 'a':
			 j=1;
			 break;
		 case 'F':
			 j=2;
			 break;
		 case 'A':
			 j=3;
			 break;
		 case 'h':
			 j=0;
			 break;
		 case 't':
			 j=5;
			 break;
		 default:
			 cout << "unrecognized peak parameter: " << type[i] << endl;
			 exit(1);
			 break;
		 }
		 // select the parameter
		 for(int k=0 ; k<data.getRows() ; k++)
			 vec(k) = data(k, j);

		 // apply processing
		 switch(type[i+1])
		 {
		 case 'o':
			 break;
		 case 'l':
			 if(type[i] == 'f')
				 vec.apply(hertz2bark);
			 else if (type[i] == 'a')
				 vec.apply(amplitude2dB);
			 else
			 {
				 cout << "unrecognized parameter normalization : " << type[i] << endl;
			 exit(1);
			 }
			 break;
		 case 'n':
			 vec.normObs();
			 break;
		 case 'b':
			 vec.normObs();
			 break;
		 default:
			 cout << "unrecognized peak processing: " << type[i] << endl;
			 exit(1);
			 break;
		 }
// do the job
similarityCompute(vec, m);
	 }
 }

 void
	 PeClust::similarityCompute(realvec &d, realvec& m)
 {
	 int i, j;
// d.dump();
	 // similarity computing
	 for(i=0 ; i<nbPeaks_ ; i++)
	 {
		 for(j=0 ; j<i ; j++)
		 {
			 mrs_real val = exp(-(d(i)-d(j))*(d(i)-d(j)));
			 m(i, j) *= val;
			 m(j, i) *= val;
		 }
	 }
 }

 
 void
	 PeClust::labeling(realvec& data, realvec& labels)
 {
	 int i, j;


// align labeling
	if(lastFrame_(0))
	 {
     realvec conversion_(nbClusters_);
	   mrs_natural lastIndex = lastFrame_(5*kmax_);	
		 realvec oldLabels(kmax_);
	   realvec newLabels(kmax_);
	
		 conversion_.setval(0);
	
	 for(i=0 ; i<kmax_ ; i++)
		oldLabels(i) = lastFrame_(6*kmax_+i);
	 newLabels.setval(0);
	 for(i=0 ; i<nbPeaks_ ; i++)
		if(data(i, 5) == lastIndex)
			newLabels(i) = labels(i);
oldLabels.dump();
	 newLabels.dump(); 

	 while(1){
		 mrs_natural nbMax=0, nb, iMax=0, jMax=0;
		 // look for the biggest cluster in old		
		 for(i=1 ; i<= (mrs_natural) oldLabels.maxval() ; i++)
			{
				nb=0;
				for(j=0 ; j<oldLabels.getSize() ; j++)
					if(oldLabels(j) == i)
						nb++;
				if(nb>nbMax)
				{
					nbMax = nb;
					iMax = i;
				}
		 }
		 if(iMax)
		 {
			 nbMax=0;
			 // look for the cluster in new with the highest intersection
			 for(i=1 ; i<= (mrs_natural) newLabels.maxval() ; i++)
			 {
				 nb=0;
				 for(j=0 ; j<newLabels.getSize() ; j++)
					 if(oldLabels(j) == iMax && newLabels(j) == i)
						 nb++;
				 if(nb>nbMax)
				 {
					 nbMax = nb;
					 jMax = i;
				 }
			 }
			 if(!jMax)
			 cout <<jMax << newLabels.maxval() << endl;
			 newLabels.dump();
			 // fill conversion table with oldLabel
			 conversion_(jMax-1) = iMax;
			 // remove old and new clusters
			 for(i=0 ; i<oldLabels.getSize() ; i++)
				 if(oldLabels(i) == iMax)
					 oldLabels(i) = 0;
			 for(i=0 ; i<newLabels.getSize() ; i++)
				 if(newLabels(i) == jMax)
					 newLabels(i) = 0;
			 newLabels.dump();
		 }
		 else
			 break;
	 }
	// fill conversion table with new labels
	 mrs_real k = maxLabel_+1;
	 for (i=0 ; i<conversion_.getSize(); i++)
	 {
		 if(conversion_(i) == 0.0)
			 conversion_(i) = k++;
	 }
	 conversion_.dump();
	 //labels.dump();
	 // convert labels
	 for(i=0 ; i<labels.getSize() ; i++)
		 if(labels(i) != 0.0 && conversion_(labels(i)) != 0.0)
			 labels(i) = conversion_(labels(i)-1);
	// labels.dump();
	}
	// fill peaks data with clusters labels
	for (i=0 ; i<nbPeaks_ ; i++)
	{
		 data(i, 6) = labels(i);
	if(labels(i) > maxLabel_)
		maxLabel_ = labels(i);
	}
 }

 void 
	 PeClust::myProcess(realvec& in, realvec& out)
 {
	 checkFlow(in,out);

	 data_.stretch(kmax_*(inSamples_+1), nbParameters_);
	 peaks2M(in, lastFrame_, data_);
	
	 m_.stretch(nbPeaks_, nbPeaks_);
  // similarity matrix calculation
	similarityMatrix(data_, m_, similarityType_);

	// Ncut
	realvec labels(nbPeaks_);
#ifdef _MATLAB_ENGINE_
	 MATLAB->putVariable(m_, "m");
	 MATLAB->putVariable(nbClusters_, "nb");
	 MATLAB->evalString("[d, vec, val] = ncutW(m, nb);");
	 MATLAB->evalString("d=d*(1:size(d, 2))';");
	 MATLAB->getVariable("d", labels);
#endif 

	 labeling(data_, labels);


	 MATLAB_PUT(data_, "peaks");
	 //MATLAB_EVAL("plotPeaks(peaks)");

	peaks2V(data_, lastFrame_, out);

} 







	
