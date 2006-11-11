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
#include "PeUtilities.h"

using namespace std;
using namespace Marsyas;

PeClust::PeClust(string name):MarSystem("PeClust", name)
{
 
	addControls();
kmax_ = 0;
	nbParameters_ = nbPeaksParameters; // 7
}

PeClust::PeClust(const PeClust& a) : MarSystem(a)
{
	ctrl_peakSet_ = getctrl("mrs_realvec/peakSet");
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
	setctrlState("mrs_natural/Clusters", true);
	addctrl("mrs_natural/Sinusoids", 1);
  setctrlState("mrs_natural/Sinusoids", true);
	addctrl("mrs_string/similarityType", "");
  setctrlState("mrs_string/similarityType", true);
	addctrl("mrs_realvec/peakSet", realvec(), ctrl_peakSet_);
  setctrlState("mrs_realvec/peakSet", true);
  addctrl("mrs_natural/hopSize", 512);
  setctrlState("mrs_natural/hopSize", true);
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
	/* inObservations_ = getctrl("mrs_natural/inObservations")->toNatural();
	 inSamples_ = getctrl("mrs_natural/inSamples")->toNatural();*/

	 nbParameters_ = inObservations_/kmax_;

	 data_.stretch(kmax_*inSamples_, nbParameters_);	 
	 lastFrame_.stretch(kmax_*nbParameters_);
	 lastFrame_.setval(0);
	 amplitudeSet_.stretch(kmax_, inSamples_);
	 frequencySet_.stretch(kmax_, inSamples_);
	 maxLabel_=0;
 }



 void
	 PeClust::similarityMatrix(realvec &data, realvec& m, string type)
 {
	 mrs_natural i, j;
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
			 j=-1;
			 break;
		 case 't':
			 j=5;
			 break;
		 default:
			 cout << "unrecognized peak parameter: " << type[i] << endl;
			 exit(1);
			 break;
		 }
		 if(j>=0)
		 {
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
	 else
	 {// specific similarity computation

		 // build frequency and amplitude matrices
	extractParameter(data, frequencySet_, pkFrequency);
	extractParameter(data, amplitudeSet_, pkAmplitude);
	harmonicitySimilarityCompute(data, frequencySet_, amplitudeSet_, m);	
	 }
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
	 PeClust::harmonicitySimilarityCompute(realvec& data, realvec& fSet, realvec& aSet, realvec& m)
 {
   mrs_natural i, j, k, startIndex = (mrs_natural) data(0, 5);

	 // similarity computing
	 for(i=0 ; i<nbPeaks_ ; i++)
	 {
		 for(j=0 ; j<i ; j++)
		 {
			 mrs_real fDiff = data(i, pkFrequency)-data(j, pkFrequency), val=0;
			 mrs_natural nbFirst=0, nbSecond=0, indexFirst = (mrs_natural) (data(i, 5)-startIndex), indexSecond = (mrs_natural) (data(j, 5)-startIndex);

			while(fSet(nbFirst++, indexFirst));
			while(fSet(nbSecond++, indexSecond));

			 realvec firstF(nbFirst, 1);
			 realvec firstA(nbFirst, 1);
			 realvec secondF(nbSecond, 1);
			 realvec secondA(nbSecond, 1);

			 // select the frame of the first peak
			 for (k=0 ; k<nbFirst ; k++)
			 {
firstF(k) = fSet(k, indexFirst);
firstA(k) = aSet(k, indexFirst);
			 }

			 // select the frame of the second and align the frequency
			 for (k=0 ; k<nbFirst ; k++)
			 {
secondF(k) = fSet(k, indexSecond)-fDiff;
secondA(k) = aSet(k, indexSecond);
			 }

			 // compare the two
			 /*MATLAB_PUT(firstF, "f1");
			 MATLAB_PUT(firstA, "a1");
			 MATLAB_PUT(secondF, "f2");
			 MATLAB_PUT(secondA, "a2");
			 MATLAB_EVAL("res=harmonicDistance(f1, a1, f2, a2);");
			 MATLAB_GET("res", val);*/

			 val = compareTwoPeakSets(firstF,firstA,secondF,secondA);
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
		 mrs_natural lastIndex = (mrs_natural) lastFrame_(5*kmax_);	
		 realvec oldLabels(kmax_);
		 realvec newLabels(kmax_);

		 conversion_.setval(-1);
		 newLabels.setval(-1);
		 mrs_natural nbInFrames=0;
		 for(i=0 ; i<nbPeaks_ ; i++)
			 if(data(i, 5) == lastIndex)
			 {
				 newLabels(i) = labels(i);
nbInFrames++;
			 }
oldLabels.setval(-1);
		 for(i=0 ; i<nbInFrames ; i++)
			 oldLabels(i) = lastFrame_(6*kmax_+i);

	 // oldLabels.dump();
	//	 newLabels.dump(); 
		 
		 while(1){
			 mrs_natural nbMax=0, nb, iMax=-1, jMax=-1;
			 // look for the biggest cluster in old		
			 for(i=0 ; i<= (mrs_natural) oldLabels.maxval() ; i++)
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
			 if(iMax > -1)
			 {
	

				 nbMax=0;
				 // look for the cluster in new with the highest intersection
				 for(i=0 ; i<= (mrs_natural) newLabels.maxval() ; i++)
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
				 if(jMax == -1)
					 break;
					// cout << iMax << " " << jMax << " " << newLabels.maxval() << endl;
				 //newLabels.dump();
				 // fill conversion table with oldLabel
				 conversion_(jMax) = iMax;
				 // remove old and new clusters
				 for(i=0 ; i<oldLabels.getSize() ; i++)
					 if(oldLabels(i) == iMax)
						 oldLabels(i) = -1;
				 for(i=0 ; i<newLabels.getSize() ; i++)
					 if(newLabels(i) == jMax)
						 newLabels(i) = -1;
			//	 newLabels.dump();
			 }
			 else
				 break;
		 }
		 // fill conversion table with new labels
		 mrs_real k = maxLabel_+1;
		 for (i=0 ; i<conversion_.getSize(); i++)
		 {
			 if(conversion_(i) == -1.0)
				 conversion_(i) = k++;
		 }
		 //conversion_.dump();
		 //labels.dump();
		 // convert labels
		 for(i=0 ; i<labels.getSize() ; i++)
			 if(labels(i) != -1.0 && conversion_( (mrs_natural) labels(i)) != -1.0)
				 labels(i) = conversion_((mrs_natural) labels(i));
		  labels.dump();
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
	 //checkFlow(in,out);

	 data_.stretch(kmax_*(inSamples_+1), nbParameters_);
	 nbPeaks_ = peaks2M(in, lastFrame_, data_, kmax_);
	
	 m_.stretch(nbPeaks_, nbPeaks_);
  // similarity matrix calculation
	similarityMatrix(data_, m_, similarityType_);

	// Ncut
	realvec labels(nbPeaks_);

	 MATLAB_PUT(m_, "m");
	 MATLAB_PUT(nbClusters_, "nb");
	 MATLAB_EVAL("[d, vec, val] = ncutW(m, nb);");
	 MATLAB_EVAL("d=d*(1:size(d, 2))';d=d-1;");
	 MATLAB_GET("d", labels);

 labeling(data_, labels);

	//  MATLAB_PUT(data_, "peaks");
	// MATLAB_EVAL("plotPeaks(peaks)");

	peaks2V(data_, lastFrame_, out, kmax_);

	// peaks storing
	const realvec& peakSet = ctrl_peakSet_->to<realvec> (); 
	int peaksSetSize = peakSet.getRows(), start;
	if(!peaksSetSize)
	{
// add synth infos
		ctrl_peakSet_->stretch(nbPeaks_+1, nbPeaksParameters);
		(**ctrl_peakSet_)(0,0) = -1;
		(**ctrl_peakSet_)(0,1) = ctrl_israte_->to<mrs_real>();
		(**ctrl_peakSet_)(0,2) = getctrl("mrs_natural/hopSize")->toNatural();
	start=1;
	}
	else
	{
		start=0;
	ctrl_peakSet_->stretch(peaksSetSize+nbPeaks_, nbPeaksParameters);
	}

	for (int i=0 ; i<nbPeaks_ ; i++)
		for (int j=0 ; j<nbPeaksParameters ; j++)
			(**ctrl_peakSet_)(peaksSetSize+i+start, j) = data_(i, j);
} 







	
