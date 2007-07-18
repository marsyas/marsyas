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
\class PeSimilarity
	\ingroup none
\brief similarities computation routines for peaks extraction project
*/

#include "PeSimilarity.h"
#include "PeUtilities.h"

using namespace std;
using namespace Marsyas;

void
Marsyas::similarityMatrix(realvec &data, realvec& m, string type, mrs_natural nbMaxSines, mrs_natural hSize, realvec& firstF, realvec& firstA, realvec& secondF, realvec& secondA, realvec & hmap)
{
	mrs_natural i, j;
	realvec vec(data.getRows());

	m.setval(1);

	for (i=0 ; i< (mrs_natural) type.length() ; i+=2)
	{
		/*cout << type[i] << endl;
		cout << m;*/
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
		case 'v':
			j=-3;
			break;
		case 's':
			j=-4;
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
		else if (j==-3)
		{
			harmonicityVirtanenCompute(data, m);
		}
		else if (j==-4)
		{
			harmonicitySrinivasanCompute(data, m, hmap);
		}
		else
		{// specific similarity computation
			std::vector<realvec> frequencySet_;
			std::vector<realvec> amplitudeSet_;

			realvec normData = data;
			//for(int j =0 ; j<normData.getRows() ; j++)
			//normData(j, pkAmplitude) = amplitude2dB(normData(j, pkAmplitude));



			// build frequency and amplitude matrices
			extractParameter(normData, frequencySet_, pkFrequency, nbMaxSines);
			//	normData.normSplMinMax(2);
			// unallow null amplitude
			//normData+= 0.00000000001;
			extractParameter(normData, amplitudeSet_, pkAmplitude, nbMaxSines);

			harmonicitySimilarityCompute(data, frequencySet_, amplitudeSet_, m, hSize, firstF, firstA, secondF, secondA);	
		}
	}
	// normalization in case the similarity values are bigger than one
	/*m/=m.maxval();
	for (i=0 ; i<m.getCols() ; i++)
	m(i, i) = 1 ;*/
}

void
Marsyas::similarityCompute(realvec &d, realvec& m)
{
	int i, j;

	// getSize ?? [ML]
	for(i=0 ; i<d.getCols() ; i++)
	{
		for(j=0 ; j<i ; j++)
		{
			// should be set adaptively [ML]
			mrs_real val = exp(-(d(i)-d(j))*(d(i)-d(j))/10);
			m(i, j) *= val;
			m(j, i) *= val;
		}
	}
}

void
Marsyas::harmonicWrapping(mrs_real peak1Freq, mrs_real peak2Freq, realvec& firstF, realvec& secondF)
{
	// shift frequencies
	firstF -= peak1Freq;
	secondF -= peak2Freq;

	// fundamental frequency estimate
	mrs_real hF = min(peak1Freq, peak2Freq);
	//mrs_real mhF = min(hF, abs(peak1Freq-peak2Freq));

	// wrap frequencies around fundamental freq estimate
	firstF /= hF;
	secondF /= hF;

	for (mrs_natural k=0 ; k<firstF.getSize() ; k++)
	{
		firstF(k)=fmod(firstF(k), 1);
		//if(firstF(k)<0)
		while(firstF(k)<0)//replacing "if" in case of strongly negative (=> multiple wraps)
			firstF(k)+=1;
	}
	for (mrs_natural k=0 ; k<secondF.getSize() ; k++)
	{
		secondF(k)=fmod(secondF(k), 1);
		//if(secondF(k)<0)
		while(secondF(k)<0) //replacing "if" in case of strongly negative (=> multiple wraps)
			secondF(k)+=1;
	}
}

void
Marsyas::harmonicitySimilarityCompute(realvec& data, std::vector<realvec>& fSet, std::vector<realvec>& aSet, realvec& m, mrs_natural hSize, 
									  realvec& firstF, realvec& firstA, realvec& secondF, realvec& secondA)
{
	mrs_natural i, j, startIndex = (mrs_natural) data(0, pkTime);

	realvec x1(hSize);
	realvec x2(hSize);
	realvec x3(hSize);
	realvec x4(hSize);
	//cout << data ;

	// similarity computing
	for(i=0 ; i<data.getRows() ; i++)
	{
		for(j=0 ; j<i ; j++)
		{
			mrs_real val=0;

			//convert indexes for this texture window (which start from 0)
			mrs_natural indexFirst = (mrs_natural) (data(i, pkTime)-startIndex);
			mrs_natural indexSecond = (mrs_natural) (data(j, pkTime)-startIndex);

			//get freq and amp vectors for both peak sets (i.e. first and second)
			firstF.stretch(fSet[indexFirst].getSize());
			firstA.stretch(aSet[indexFirst].getSize());
			firstF = fSet[indexFirst];
			firstA = aSet[indexFirst];
			secondF.stretch(fSet[indexSecond].getSize());
			secondA.stretch(aSet[indexSecond].getSize());
			secondF = fSet[indexSecond];
			secondA = aSet[indexSecond];

			/*
			MATLAB_PUT(data(i, pkFrequency), "sf1");
			MATLAB_PUT(data(j, pkFrequency), "sf2");
			MATLAB_PUT(firstF, "f1");
			MATLAB_PUT(firstA, "a1");
			MATLAB_PUT(secondF, "f2");
			MATLAB_PUT(secondA, "a2");
			*/

			//peaks i and j frequencies
			mrs_real iFreq = data(i, pkFrequency);
			mrs_real jFreq = data(j, pkFrequency);

			// weight the amplitudes
			//for (mrs_natural k=0 ; k<firstF.getSize() ; k++)
			//	firstA(k)*=harmonicWeighting(firstF(k), hF, harmonicityWeight_);
			//for (mrs_natural k=0 ; k<secondF.getSize() ; k++)
			//	secondA(k)*=harmonicWeighting(secondF(k), hF, harmonicityWeight_);


			//********************************************************************
			//						HWPS
			//********************************************************************

			//////////////////////////////////
			// calculate harmonic wrapping
			//////////////////////////////////
			harmonicWrapping(iFreq, jFreq, firstF, secondF);

			//////////////////////////////////
			// histogram and COSINE distance
			/////////////////////////////////
			val = cosinePeakSets(firstF,firstA, secondF,secondA, aSet[indexFirst], aSet[indexSecond], x1, x2, x3, x4, hSize);
			//val=exp(-val);

			//cout << data(i, pkFrequency) << " " <<data(j, pkFrequency) << " value: "<< val << " " << exp(val*val) <<endl;

			m(i, j) *= exp(val*val)/exp(1.0);
			m(j, i) *= exp(val*val)/exp(1.0);
		}
	}
}

mrs_real Marsyas::hVirtanen(mrs_real f1, mrs_real f2)
{
	// minimal f0 frequency in Hz
	mrs_real fMin = 50;
	mrs_real dist=MAXREAL;

	mrs_real r1 = floor(f1/fMin);
	mrs_real r2 = floor(f2/fMin);

	for (mrs_natural i=1 ; i<=r1 ; i++)
		for (mrs_natural j=1; j<=r2 ; j++)
		{
			mrs_real val = abs(log((f1/f2)/(i/j)));
			if (dist > val)
				dist=val;
		}

		return dist;
}

void
Marsyas::harmonicityVirtanenCompute(realvec& data, realvec& m)
{
	mrs_natural i, j;

	for (i=0 ; i<data.getRows() ; i++)
		for (j=0 ; j<i ; j++)
		{
			mrs_real sim = 1/(1+hVirtanen (data(i, pkFrequency), data(j, pkFrequency)));
			// should consider
			// mrs_real sim = exp(-hVirtanen (data(i, pkFrequency), data(j, pkFrequency))/exp(1);
			m(i, j) *= sim;
			m(j, i) *= sim;
		}

}

void
Marsyas::harmonicitySrinivasanCompute(realvec& data, realvec& m, realvec& hMap)
{
	mrs_natural i, j;

	if(!hMap.getSize())
		computeHarmonicityMap(hMap, 512); // 512



	for (i=0 ; i<data.getRows() ; i++)
		for (j=0 ; j<i ; j++)
		{
			//cout << data(i, pkBin) << " " << data(j, pkBin) << " " << hMap(data(i, pkBin), data(j, pkBin)) << endl;
			m(i, j) *= hMap(data(i, pkBin), data(j, pkBin));
			m(j, i) *= hMap(data(i, pkBin), data(j, pkBin));
		}
}

//                           similarity matrix, peak data, initial peak labels,       wanted N,                   initial N
mrs_real Marsyas::selectClusters(realvec &m, realvec &data, realvec &labels, mrs_natural wantedNbClusters, mrs_natural nbClusters)
{
	mrs_natural i, j, nbFound=0;
	mrs_real clusterDensity=0, outerClusterDensity=0;

	// intra class density
	realvec sValue(nbClusters);
	sValue.setval(0);
	// outer class density
	realvec sNValue(nbClusters);
	sNValue.setval(0);
	// numbers of peaks considered
	realvec sNb(nbClusters);
	sNb.setval(0);
	// numbers of peaks considered
	realvec sNNb(nbClusters);
	sNNb.setval(0);
	// newlabeling of the clusters (0/1)
	realvec newLabels(nbClusters);
	newLabels.setval(0);

	for(i=0 ; i<m.getRows() ; i++)
	{
		for(j=0 ; j<i ; j++)
		{		
			if(labels(i)==labels(j))
			{
				sValue((mrs_natural) labels(i))+= m(i, j);
				sNb((mrs_natural) labels(i))++;
			}	
			else
			{
				sNValue((mrs_natural) labels(i))+= m(i, j);
				sNNb((mrs_natural) labels(i))++;
			}
		}
	}
	// normalize by number of elements
	for(i=0; i<nbClusters ;i++)
		sValue(i) /= sNb(i);
	for(i=0; i<nbClusters ;i++)
		sNValue(i) /= sNNb(i);

	// normalize by outer density
	/*for(i=0; i<nbClusters ;i++)
	sValue(i) /= sNValue(i);*/

	/*MATLAB_PUT(sValue.maxval(), "val");
	MATLAB_EVAL("hold on ; plot(x, val, 'ko');");*/

// find the clusters with highest density
	while(nbFound<nbClusters-wantedNbClusters)
	{
		mrs_natural index=0;
		mrs_real value=MAXREAL;
		for(i=0; i<nbClusters ;i++)
		{
			if(sValue(i) != -1 && value>sValue(i))
			{
				value=sValue(i);
				index = i;
				clusterDensity += value;
			}
		}
		sValue(index) = -1;
		nbFound++;
	}
	clusterDensity/=nbClusters-wantedNbClusters;
	// compute outer Cluster density
	for(i=0; i<nbClusters ;i++)
//			if(sValue(i) == -1 )
				outerClusterDensity += sValue(i);
	outerClusterDensity/=wantedNbClusters;
	// propagate new labeling
	mrs_natural k=0;
	for(i=0; i<nbClusters ;i++)
		if(sValue(i) == -1)
			newLabels(i) =-1;
		else
			newLabels(i) = k;  // put k++ for unmerging the clusters in the same frame

	// cout << newLabels;
	for(i=0 ; i<m.getRows() ; i++)
		labels(i) = newLabels(labels(i));
	//cout << labels;
	return outerClusterDensity;///outerClusterDensity;
}
