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
	nbParameters_ = nbPkParameters; // 7
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
	addctrl("mrs_realvec/similarityWeight", realvec());
	setctrlState("mrs_realvec/similarityWeight", true);
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
	similarityWeight_ = getctrl("mrs_realvec/similarityWeight")->toVec();
	/* inObservations_ = getctrl("mrs_natural/inObservations")->toNatural();
	inSamples_ = getctrl("mrs_natural/inSamples")->toNatural();*/

	nbParameters_ = inObservations_/kmax_;

	data_.stretch(kmax_*inSamples_, nbParameters_);	 
	lastFrame_.stretch(kmax_*nbParameters_);
	lastFrame_.setval(0);
	//amplitudeSet_.stretch(kmax_, inSamples_+1);
	//frequencySet_.stretch(kmax_, inSamples_+1);
	maxLabel_=0;

	harmonicityWeight_=0.01;
	harmonicitySize_=10;
}



void
PeClust::similarityMatrix(realvec &data, realvec& m, string type)
{
	mrs_natural i, j;
	realvec vec(nbPeaks_);

	m.setval(1);

	for (i=0 ; i< (mrs_natural) type.length() ; i+=2)
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
				vec.normObsMinMax();
				break;
			case 'b':
				if(type[i] == 'f')
					vec.apply(hertz2bark);
				else if (type[i] == 'a')
					vec.apply(amplitude2dB);
				else
			 {
				 cout << "unrecognized parameter normalization : " << type[i] << endl;
				 exit(1);
			 }
				vec.normObsMinMax();
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
			std::vector<realvec> frequencySet_;
			std::vector<realvec> amplitudeSet_;

			realvec normData = data;
			for(int j =0 ; j<normData.getRows() ; j++)
				//normData(j, pkAmplitude) = amplitude2dB(normData(j, pkAmplitude));

				

				// build frequency and amplitude matrices
				extractParameter(normData, frequencySet_, pkFrequency, kmax_);
				normData.normSplMinMax(2);
			extractParameter(normData, amplitudeSet_, pkAmplitude, kmax_);

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
PeClust::harmonicitySimilarityCompute(realvec& data, std::vector<realvec>& fSet, std::vector<realvec>& aSet, realvec& m)
{
	mrs_natural i, j, startIndex = (mrs_natural) data(0, 5);

	// similarity computing
	for(i=0 ; i<nbPeaks_ ; i++)
	{
		for(j=0 ; j<i ; j++)
		{
			mrs_real val=0, hF;
			mrs_natural indexFirst = (mrs_natural) (data(i, 5)-startIndex), indexSecond = (mrs_natural) (data(j, 5)-startIndex);

			realvec firstF = fSet[indexFirst];
			realvec firstA = aSet[indexFirst];
			realvec secondF = fSet[indexSecond];
			realvec secondA = aSet[indexSecond];


			MATLAB_PUT(data(i, pkFrequency), "sf1");
			MATLAB_PUT(data(j, pkFrequency), "sf2");
			MATLAB_PUT(firstF, "f1");
			MATLAB_PUT(firstA, "a1");
			MATLAB_PUT(secondF, "f2");
			MATLAB_PUT(secondA, "a2");

			// fundamentqal frequency estimates
			hF = min(data(i, pkFrequency), data(j, pkFrequency));
			// mrs_real mhF = min(hF, abs(data(i, pkFrequency)-data(j, pkFrequency)));
		
			// weight the amplitudes
			//for (mrs_natural k=0 ; k<firstF.getSize() ; k++)
			//	firstA(k)*=harmonicWeighting(firstF(k), hF, harmonicityWeight_);
			//for (mrs_natural k=0 ; k<secondF.getSize() ; k++)
			//	secondA(k)*=harmonicWeighting(secondF(k), hF, harmonicityWeight_);


			//cout << firstF;
			// align and wrap frequencies
			firstF-=data(i, pkFrequency);
			secondF-=data(j, pkFrequency);

			firstF/=hF;
			secondF/=hF;

			for (mrs_natural k=0 ; k<firstF.getSize() ; k++)
			{
				firstF(k)=fmod(firstF(k), 1);
				if(firstF(k)<0)
					firstF(k)+=1;
			}
			for (mrs_natural k=0 ; k<secondF.getSize() ; k++)
			{
				secondF(k)=fmod(secondF(k), 1);
				if(secondF(k)<0)
					secondF(k)+=1;
			}

			//	realvec v(100);
			//		for (mrs_natural k=0 ; k<v.getSize() ; k++)
			//v(k) = harmonicWeighting(k, 10, 0.1);
			//MATLAB_PUT(v, "v");
			//MATLAB_EVAL("clf ; plot(v)");

			// compare the two
			// cout <<firstF<<secondF;

	/*		MATLAB_PUT(firstF, "F1");
			MATLAB_PUT(firstA, "A1");
			MATLAB_PUT(secondF, "F2");
			MATLAB_PUT(secondA, "A2");*/
		//	MATLAB_EVAL("plotHarmo");

			// cout << data(i, pkFrequency) << " " <<data(j, pkFrequency) <<endl;
			/*if(firstF.getSize() < secondF.getSize())
				val = correlatePeakSets(firstF,firstA,secondF,secondA);
			else
				val = correlatePeakSets(secondF,secondA,firstF,firstA);*/

			val = cosinePeakSets(firstF,firstA, secondF,secondA, aSet[indexFirst], aSet[indexSecond], harmonicitySize_);

			//	val=exp(-val);
	//		cout << data(i, pkFrequency) << " " <<data(j, pkFrequency) << " value: "<< val <<endl;
			m(i, j) *= exp(val*val);
			m(j, i) *= exp(val*val);
		}
	}
}


void
PeClust::labeling(realvec& data, realvec& labels)
{
	int i, j;

	// !! do the labeling with amplitude priority
	// align labeling
	if(lastFrame_(0))
	{
		realvec conversion_(nbClusters_);
		mrs_natural lastIndex = (mrs_natural) lastFrame_(5*kmax_);	
		realvec oldLabels(kmax_);
		realvec newLabels(kmax_);
		realvec oldAmps(kmax_);
		realvec newAmps(kmax_);

		conversion_.setval(-1);
		newLabels.setval(-1);
		oldAmps.setval(0);
		newAmps.setval(0);
		mrs_natural nbInFrames=0;
		// labels sets for new
		for(i=0 ; i<nbPeaks_ ; i++)
			if(data(i, 5) == lastIndex)
			{
				newLabels(nbInFrames) = labels(i);
				newAmps(nbInFrames) = data(i, pkAmplitude);
				nbInFrames++;
			}
			// labels sets for old
			oldLabels.setval(-1);
			for(i=0 ; i<nbInFrames ; i++)
		 {
			 oldLabels(i) = lastFrame_(pkGroup*kmax_+i);
			 oldAmps(i) = lastFrame_(pkAmplitude*kmax_+i);
		 }
			//	 amps.dump();
			cout  << lastIndex << " " ;
			/* oldLabels.dump();
				 newLabels.dump(); */

			while(1){
				mrs_natural  iMax=-1, jMax=-1, i2Max=-1, nbMax1=0,nbMax2=0,nbMax3=0, nb, nb2=0;
				mrs_real ampMax1=0, ampMax2=0, ampMax3=0, amp, a2=0;
				// look for the highest amplitude cluster in old		
				for(i=0 ; i<= (mrs_natural) oldLabels.maxval() ; i++)
				{
					nb=0;
					amp=0;
					for(j=0 ; j<oldLabels.getSize() ; j++)
						if(oldLabels(j) == i)
						{
							nb++;
							amp+=oldAmps(j);
						}
					if(amp>ampMax1)
				 {
					 nbMax1 = nb;
					 ampMax1 = amp;
					 iMax = i;
				 }
				}
				if(iMax > -1)
				{
					// look for the cluster in new with the highest intersection
					for(i=0 ; i<= (mrs_natural) newLabels.maxval() ; i++)
				 {
					 mrs_natural n=0;
					 mrs_real a=0;
					 nb=0;
					 amp=0;
					 for(j=0 ; j<newLabels.getSize() ; j++)
						 if(newLabels(j) == i)
						 {
							 n++;
							 a+=newAmps(j);
						 if(oldLabels(j) == iMax)
						 {
							 nb++;
							 amp+=newAmps(j);
						 }
						 }
					 if(amp>ampMax2)
					 {
						 nb2=n;
						 a2=a;
						 nbMax2 = nb;
						 ampMax2 = amp;
						 jMax = i;
					 }
				 }
					if(jMax == -1)
						break;
					mrs_real av,nv,gv;
					if(ampMax1<a2)
						av = a2/ampMax1;
					else
						av = ampMax1/a2;
					if(nbMax1<nb2)
						nv = nb2/((mrs_real) nbMax1);
					else
						nv = nbMax1/((mrs_real) nb2);
					if(ampMax1/nbMax1<a2/nb2)
						gv = (a2/nb2)/(ampMax1/nbMax1);
					else
						gv = (ampMax1/nbMax1)/(a2/nb2);
					mrs_real diff = (abs(ampMax1-ampMax2)+abs(a2-ampMax2))/(ampMax1+a2);
					cout << diff << " " <<  2*ampMax2/(ampMax1+a2) << endl;
					

					
					// check if not used better elsewhere
					// or if two do not correspond put old to -1
					for(i=0 ; i<= (mrs_natural) oldLabels.maxval() ; i++)
				 {
					 nb=0;
					 amp=0;
					 for(j=0 ; j<oldLabels.getSize() ; j++)
						 if(newLabels(j) == jMax && oldLabels(j) == i)
						 {
							 nb++;
							 amp+=newAmps(j);
						 }
						 if(amp>ampMax3)
						 {
							 nbMax3 = nb;
							 ampMax3 = amp;
							 i2Max = i;
						 }
				 }
					
cout << iMax << " " << i2Max << endl << endl;
					if(iMax == i2Max && diff<0.4)
						{
					// if two correspond well
					// fill conversion table with oldLabel
					conversion_(jMax) = iMax;
					// remove new clusters
					for(i=0 ; i<newLabels.getSize() ; i++)
						if(newLabels(i) == jMax)
							newLabels(i) = -1;
						}
					// else only remove old  clusters
					for(i=0 ; i<oldLabels.getSize() ; i++)
						if(oldLabels(i) == iMax)
							oldLabels(i) = -1;
					// cout << iMax << " " << jMax << " " << newLabels.maxval() << endl;
					//newLabels.dump();
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
			//labels.dump();
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
	mrs_natural nbPeaksLastFrame;

	data_.stretch(kmax_*(inSamples_+1), nbParameters_);
	nbPeaks_ = peaks2M(in, lastFrame_, data_, kmax_, &nbPeaksLastFrame);
	if(nbPeaks_)
	{
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

		MATLAB_PUT(data_, "peaks");
		MATLAB_EVAL("plotPeaks(peaks)");

		peaks2V(data_, lastFrame_, out, kmax_);

		// peaks storing
		const realvec& peakSet = ctrl_peakSet_->to<realvec> (); 
		int peaksSetSize = peakSet.getRows(), start;
		if(!peaksSetSize)
		{
			// add synth infos
			ctrl_peakSet_->stretch(nbPeaks_+1, nbPkParameters);
			(**ctrl_peakSet_)(0,0) = -1;
			(**ctrl_peakSet_)(0,1) = ctrl_israte_->to<mrs_real>();
			(**ctrl_peakSet_)(0,2) = getctrl("mrs_natural/hopSize")->toNatural();
			start=1;

			for (int i=0 ; i<nbPeaks_ ; i++)
				for (int j=0 ; j<nbPkParameters ; j++)
					(**ctrl_peakSet_)(peaksSetSize+i+start, j) = data_(i, j);
		}
		else
		{
			start=nbPeaksLastFrame;
			ctrl_peakSet_->stretch(peaksSetSize+nbPeaks_-start, nbPkParameters);

			for (int i=0 ; i<nbPeaks_-nbPeaksLastFrame ; i++)
				// do not put peaks from the first frame
				for (int j=0 ; j<nbPkParameters ; j++)
					(**ctrl_peakSet_)(peaksSetSize+i, j) = data_(i+start, j);
		}
	}
} 








