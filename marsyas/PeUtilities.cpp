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

#include "AudioSink.h"
#include "SoundFileSink.h"
#include "SoundFileSource.h"

using namespace std;
using namespace Marsyas;



int 
Marsyas::peaks2M (realvec& in, realvec& first, realvec& out, mrs_natural maxNbPeaks, mrs_natural *nbPkFrame)
{
	int i,j,k,l=0,m=0;

	if(&first != NULL  && first(0))
		for (i=0 ; i<maxNbPeaks ; i++)
		{
			if(first(i) != 0.0) 
			{
				for(k=0;k<nbPkParameters;k++)
					out(l, k) = first(k*maxNbPeaks+i);
				l++;
				m++;
			}
		}
		*nbPkFrame = m;

		for (j=0 ; j<in.getCols() ; j++)
			for (i=0 ; i<maxNbPeaks ; i++)
		 {
			 if(in(i, j) != 0.0) 
			 {
				 for(k=0;k<nbPkParameters;k++)
					 out(l, k) = in(k*maxNbPeaks+i, j);
				 l++;
			 }
		 }
			out.stretch(l, nbPkParameters);
			return l;
}

void 
Marsyas::peaks2V (realvec& in, realvec& last, realvec& out, mrs_natural maxNbPeaks, mrs_natural label)
{
	mrs_natural i, j, k=0, start=0, iStart=0;
	mrs_natural frameIndex=-1, startIndex = (mrs_natural) in(0, 5);

	out.setval(0);

	// on-line case with first init
	if(label == -1 && last(0))
	{
		start=1;
		startIndex +=1;
	}
	// if coming from a saved realvec first elts contain config so should be avoided
	if(in(0) == -1)
	{
		iStart = 1;
	}

	for (i=iStart ; i<in.getRows() ; i++, k++)
	{
		if(frameIndex != in(i, 5))
		{
			frameIndex = (mrs_natural) in(i, 5);
			k=0;
		}
		if((!start || (start && in(i, 5) >= startIndex)))
			if(label < 0 || label == in(i, pkGroup))
				for (j=0 ; j<in.getCols() ; j++)
				{
					out(j*maxNbPeaks+k, frameIndex-startIndex) = in(i, j);
					/*	 if(peakFile)
					peakFile << in(i, j);*/
				}
	}
	//on-line case
	if(label == -1)
	{
		for (j=0 ; j<out.getRows() ; j++)
		{
			last(j) = out(j, out.getCols()-1);
		}
	}
}


void Marsyas::updateLabels(realvec& peakSet, realvec& conversion)
{
	for (mrs_natural i=0 ; i<peakSet.getRows() ; i++)
		peakSet(i, pkGroup) = conversion((mrs_natural) peakSet(i, pkGroup)+2);
}


void Marsyas::extractParameter(realvec&in, std::vector<realvec>& out, pkParameter type, mrs_natural kmax)
{
	mrs_natural i, k=0;
	mrs_natural frameIndex=-1, startIndex = (mrs_natural) in(0, 5);
	realvec vec(kmax);
	//out.setval(0);
	for (i=0 ; i<in.getRows() ; i++, k++)
	{
		if(frameIndex != in(i, 5))
		{
			if(k)
			{
				vec.stretch(k);
				// put inside vector
				out.push_back(vec);
				vec.stretch(kmax);
			}
			// fix this [ML]
			/*else
				out.push_back(realvec());*/
			frameIndex = (mrs_natural) in(i, 5);
			k=0;
		}
		if(in(i, 5) >= startIndex)
			vec(k) = in(i, type);
	}
	if(k)
	{
		vec.stretch(k);
		// put inside vector
		out.push_back(vec);
	}
}


mrs_real
Marsyas::compareTwoPeakSets(realvec&f1, realvec&a1, realvec&f2, realvec&a2)
{
	mrs_natural nb=0;
	mrs_real res=0;
	realvec i1(f1.getSize());
	realvec i2(f2.getSize());
	i1.setval(1);
	i2.setval(1);

	//cout << f1 << a1 << f2 << a2 ;
	// while the smallest set is not empty
	MATLAB_EVAL("Acc=[];");
	while (1)
	{


		//cout << i1<<i2;
		mrs_natural ind1=-1, ind2=0;
		mrs_real A1, A2, F1, F2;
 // minDiff = 10000000000000, maxVal = -100000000000000; 
		mrs_real maxVal = 0, minDiff =100;
		// take the highest amplitude peak
		for (mrs_natural i=0 ; i<a1.getSize() ; i++)
		{
			A1 = a1(i);
			if(i1(i) && A1 > maxVal)
			{
				maxVal= A1;
				ind1 = i;
			}
		}
		if(ind1 == -1) break;

		F1 = f1(ind1);
		A1 = a1(ind1);
		// find the closest frequency peak in the second set
		for (mrs_natural i=0 ; i<f2.getSize() ; i++)
		{
			F2 = f2(i);
			if(i2(i) && abs(F1-F2) < minDiff)
			{
				minDiff= abs(F1-F2); // or product of fDiff and aDiff
				ind2 = i;
			}
		}
		A2 = a2(ind2);
		// acc the amplitude difference
		res += abs(A1-A2)*abs(F1-F2); // or product of fDiff and aDiff
		nb++;
		// remove the two peaks
		i1(ind1) = 0;
		i2(ind2) = 0;

		MATLAB_PUT(f1, "f1");
		MATLAB_PUT(a1, "a1");
		MATLAB_PUT(i1, "i1");
		MATLAB_PUT(i2, "i2");
		MATLAB_PUT(f2, "f2");
		MATLAB_PUT(a2, "a2");
		MATLAB_PUT(abs(A1-A2)*abs(F1-F2), "acc");
		MATLAB_EVAL("Acc=[Acc acc] ;subplot(2, 1, 1) ; plot(f1, a1.*i1, '*r', f2, a2.*i2, 'k+'); subplot(2, 1, 2); plot(Acc)");

	}
	return res/nb;
}

mrs_real
Marsyas::compareTwoPeakSets2(realvec&f1, realvec&a1, realvec&f2, realvec&a2)
{
	mrs_real res = 0;
	for (mrs_natural i = 0 ; i < f1.getSize() ; i++)
		for (mrs_natural j = 0 ; j < f2.getSize() ; j++)
		{
			res += abs(f1(i)-f2(j))* abs(a1(i)-a2(j));
		}
		return res/(f1.getSize()*f2.getSize());
}

mrs_real
Marsyas::compareTwoPeakSets3(realvec&f1, realvec&a1, realvec&f2, realvec&a2)
{
	mrs_natural nb=0;
	mrs_real res=0;
	realvec i1(f1.getSize());
	realvec i2(f2.getSize());
	i1.setval(1);
	i2.setval(1);

	//cout << f1 << a1 << f2 << a2 ;
	// while the smallest set is not empty
	while (1)
	{
		//cout << i1<<i2;
		mrs_natural ind1=-1, ind2=0;
		// mrs_real minDiff = 10000000000000, minVal = 100000000000000;
		 
		mrs_real minVal = 0;
		// 
		// 
		// look for the smallest couple
		for (mrs_natural i=0 ; i<a1.getSize() ; i++)
			for (mrs_natural j=0 ; j<a2.getSize() ; j++)
			{
				mrs_real val = abs(f1(i)-f2(j));
				if(i1(i) && i2(j) && val < minVal)
				{
					minVal= val;
					ind1 = i;
					ind2 = j;
				}
			}

			if(ind1 == -1 || ind2 == -1) break;

			mrs_real val = abs(a1(ind1)-a2(ind2))*abs(f1(ind1)-f2(ind2));
			res += val; // or product of fDiff and aDiff
			nb++;
			// remove the two peaks
			i1(ind1) = 0;
			i2(ind2) = 0;

			MATLAB_PUT(f1, "f1");
			MATLAB_PUT(a1, "a1");
			MATLAB_PUT(i1, "i1");
			MATLAB_PUT(i2, "i2");
			MATLAB_PUT(f2, "f2");
			MATLAB_PUT(a2, "a2");
			MATLAB_EVAL("subplot(2, 1, 1) ; plot(f1, a1.*i1, '*r', f2, a2.*i2, 'k+'); subplot(2, 1, 2); plot(Acc)");

	}
	return res/nb;
}

mrs_real
Marsyas::correlatePeakSets(realvec&f1, realvec&a1, realvec&f2, realvec&a2)
{
	mrs_real res=0;
	realvec i1(f1.getSize());
	realvec i2(f2.getSize());
	i1.setval(1);
	i2.setval(1);

	//cout << f1 << a1 << f2 << a2 ;
	for (mrs_natural k=0; k<f1.getSize() ; k++)
	{
		//cout << i1<<i2;
		mrs_natural ind1=-1, ind2=-1;
		mrs_real minDiff = 2; 
		// look for the couple closest in frequency
		for (mrs_natural i=0 ; i<f1.getSize() ; i++)
			for (mrs_natural j=0 ; j<f2.getSize() ; j++)
				if(i1(i) && i2(j))
				{
					mrs_real fa = f1(i), fb = f2(j);
					if(fa>fb)
					{
						fa = f2(j);
						fb = f1(i);
					}
					mrs_real df = fb-fa;
					if(df>1-fb+fa)
						df = 1-fb+fa;

					if(df < minDiff)
					{
						minDiff= df;
						ind1 = i;
						ind2 = j;
					}
				}




				cout << i1;
				cout << i2;



				mrs_real val = a1(ind1)*a2(ind2)/(minDiff*minDiff+0.0000000000000001);
				cout << ind1 << " " << ind2 << " " << val << endl;	
				res += val; // or product of fDiff and aDiff
				// remove the two peaks	
				if(ind1 != -1)
				{
					i1(ind1) = 0;
					i2(ind2) = 0;
				}
				else
				{
					cout << i1 << i2;
				}
				/*		MATLAB_PUT(f1, "f1");
				MATLAB_PUT(a1, "a1");
				MATLAB_PUT(i1, "i1");
				MATLAB_PUT(i2, "i2");
				MATLAB_PUT(f2, "f2");
				MATLAB_PUT(a2, "a2");
				MATLAB_PUT(val, "acc");
				MATLAB_EVAL("plotHarmo");*/
				//	cout << ind1 <<" " << ind2<<endl;
	}
	return res/f1.getSize();
}

mrs_real
Marsyas::cosinePeakSets(realvec&f1, realvec&a1, realvec&f2, realvec&a2, realvec&a3, realvec&a4, realvec&x1, realvec&x2, realvec&x3, realvec&x4, mrs_natural length)
{
	mrs_natural index;
	mrs_real res1=0, res2=0, res3=0, res;

	x1.setval(0);
	x2.setval(0);
	x3.setval(0);
	x4.setval(0);
	// first discrete Harmonically Wrapped Spectrum 
	for (mrs_natural i=0 ; i<f1.getSize() ; i++)
	{
		index= (mrs_natural) fmod(floor(f1(i)*length+.5), length);
		x1(index) += a1(i);
		x3(index) += a3(i);
	}
	// second discrete Harmonically Wrapped Spectrum 
	for (mrs_natural i=0 ; i<f2.getSize()  ; i++)
	{
		index= (mrs_natural) fmod(floor(f2(i)*length+.5), length);
		x2(index) += a2(i);
		x4(index) += a4(i);
	}
	// cosine metric
	for (mrs_natural i=0 ; i<x1.getSize()  ; i++)
	{
		res1 += x1(i)*x2(i);
		res2 += x3(i)*x3(i);
		res3 += x4(i)*x4(i);
	}

	res = res1/(sqrt(res2)*sqrt(res3));


	//MATLAB_PUT(x1, "x1");
	//MATLAB_PUT(x2, "x2");
	//MATLAB_EVAL("plotHarmo");

	return res;
}

void Marsyas::synthNetCreate(MarSystemManager *mng, string outsfname, bool microphone, mrs_natural synType)
{
	//create Shredder series
	MarSystem* postNet = mng->create("Series", "postNet");
//	postNet->addMarSystem(mng->create("PeOverlapadd", "ob"));

	 if(synType == 0)
	 {
	postNet->addMarSystem(mng->create("PeSynOsc", "pso"));
	 }
	 else
	 {

	 }

	postNet->addMarSystem(mng->create("OverlapAdd", "ov"));
	postNet->addMarSystem(mng->create("ShiftOutput", "so"));

	MarSystem *dest;
	if (outsfname == EMPTYSTRING) 
		dest = new AudioSink("dest");
	else
	{
		dest = new SoundFileSink("dest");
		//dest->updctrl("mrs_string/filename", outsfname);
	}
	MarSystem* fanout = mng->create("Fanout", "fano");
	fanout->addMarSystem(dest);
	MarSystem* fanSeries = mng->create("Series", "fanSeries");

	if (microphone) 
		fanSeries->addMarSystem(mng->create("AudioSource", "src2"));
	else 
		fanSeries->addMarSystem(mng->create("SoundFileSource", "src2"));

	fanSeries->addMarSystem(mng->create("Delay", "delay"));
	fanout->addMarSystem(fanSeries);

	postNet->addMarSystem(fanout);
	postNet->addMarSystem(mng->create("PeResidual", "res"));

	MarSystem *destRes;
	if (outsfname == EMPTYSTRING) 
		destRes = new AudioSink("destRes");
	else
	{
		destRes = new SoundFileSink("destRes");
		//dest->updctrl("mrs_string/filename", outsfname);
	}
	postNet->addMarSystem(destRes);

	MarSystem* shredNet = mng->create("Shredder", "shredNet");
	shredNet->addMarSystem(postNet);

	mng->registerPrototype("PeSynthetize", shredNet);
}

void
Marsyas::synthNetConfigure(MarSystem *pvseries, string sfName, string outsfname, string ressfname, mrs_natural Nw, 
													 mrs_natural D, mrs_natural S, mrs_natural accSize, bool microphone, mrs_natural synType, mrs_natural bopt, mrs_natural delay)
{
	pvseries->updctrl("PeSynthetize/synthNet/mrs_natural/nTimes", accSize);
  if(synType==0)
	{
	pvseries->updctrl("PeSynthetize/synthNet/Series/postNet/PeSynOsc/pso/mrs_natural/nbSinusoids", S);
	pvseries->updctrl("PeSynthetize/synthNet/Series/postNet/PeSynOsc/pso/mrs_natural/delay", Nw/2+1);
	pvseries->updctrl("PeSynthetize/synthNet/Series/postNet/PeSynOsc/pso/mrs_natural/synSize", D*2);
	}
	else
	{

	}
	pvseries->updctrl("PeSynthetize/synthNet/Series/postNet/ShiftOutput/so/mrs_natural/Interpolation", D);
	pvseries->updctrl("PeSynthetize/synthNet/Series/postNet/ShiftOutput/so/mrs_natural/WindowSize", Nw);      
	pvseries->updctrl("PeSynthetize/synthNet/Series/postNet/ShiftOutput/so/mrs_natural/Decimation", D);

	if (microphone) 
	{
		pvseries->updctrl("PeSynthetize/synthNet/Series/postNet/Fanout/fano/Series/fanSeries/AudioSource/src2/mrs_natural/inSamples", D);
		pvseries->updctrl("PeSynthetize/synthNet/Series/postNet/Fanout/fano/Series/fanSeries/AudioSource/src2/mrs_natural/inObservations", 1);
	}
	else
	{
		pvseries->updctrl("PeSynthetize/synthNet/Series/postNet/Fanout/fano/Series/fanSeries/SoundFileSource/src2/mrs_string/filename", sfName);
		pvseries->updctrl("PeSynthetize/synthNet/Series/postNet/Fanout/fano/Series/fanSeries/SoundFileSource/src2/mrs_natural/pos", 0);
		pvseries->updctrl("PeSynthetize/synthNet/Series/postNet/Fanout/fano/Series/fanSeries/SoundFileSource/src2/mrs_natural/inSamples", D);
		pvseries->updctrl("PeSynthetize/synthNet/Series/postNet/Fanout/fano/Series/fanSeries/SoundFileSource/src2/mrs_natural/inObservations", 1);
	}
	if (outsfname == EMPTYSTRING) 
		pvseries->updctrl("PeSynthetize/synthNet/Series/postNet/AudioSink/dest/mrs_natural/bufferSize", bopt);


	pvseries->updctrl("PeSynthetize/synthNet/Series/postNet/Fanout/fano/Series/fanSeries/Delay/delay/mrs_natural/delay", delay); // Nw+1-D
	pvseries->updctrl("PeSynthetize/synthNet/Series/postNet/Fanout/fano/SoundFileSink/dest/mrs_string/filename", outsfname);//[!]
	pvseries->updctrl("PeSynthetize/synthNet/Series/postNet/SoundFileSink/destRes/mrs_string/filename", ressfname);//[!]
}




mrs_real Marsyas::harmonicWeighting(mrs_real f, mrs_real h, mrs_real w)
{
	if(f < h)
		return 1;
	else
		return pow(harmonicWeightingBasic(f, h), -w/log(harmonicWeightingBasic(1,h)));
}


mrs_real Marsyas::harmonicWeightingBasic(mrs_real f, mrs_real h)
{
	return (1+cos(2*PI*f/h))/2;
}

void Marsyas::discrete2labels(realvec &labels, realvec& n, mrs_natural nbClusters, mrs_natural nbPeaks)
{
		for(mrs_natural i=0 ; i<nbPeaks ; i++)
		for(mrs_natural j=0 ; j<nbClusters ; j++)
			if(n(i*nbClusters+j) == 1)
				labels(i) = j;
}
