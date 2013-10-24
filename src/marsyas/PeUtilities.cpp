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

#include <marsyas/PeUtilities.h>
#include <marsyas/marsystems/SoundFileSink.h>
#include <marsyas/marsystems/SoundFileSource.h>
#include <marsyas/NumericLib.h>

using std::ostringstream;
using std::abs;

using namespace Marsyas;

//[TODO] -> deprecate all this junk!!


// //online --> matrix
// int
// Marsyas::peaks2M(realvec& in, realvec& first, realvec& out, mrs_natural maxNbPeaks, mrs_natural *nbPkFrame, mrs_natural start)
// {
// 	int i,j,k,l=start,m=0;
//
// 	if(&first != NULL  && first(0)) //[WTF]
// 		for (i=0 ; i<maxNbPeaks ; ++i)
// 		{
// 			if(first(i) != 0.0)
// 			{
// 				for(k=0;k<nbPkParameters;k++)
// 					out(l, k) = first(k*maxNbPeaks+i);//[WTF]
// 				l++;
// 				m++;
// 			}
// 		}
// 		*nbPkFrame = m;
//
// 		for (j=0 ; j<in.getCols() ; j++)
// 			for (i=0 ; i<maxNbPeaks ; ++i)
// 		 {
// 			 if(in(i, j) != 0.0)
// 			 {
// 				 for(k=0;k<nbPkParameters;k++)
// 					 out(l, k) = in(k*maxNbPeaks+i, j);
// 				 l++;
// 			 }
// 		 }
// 			out.stretch(l, nbPkParameters);
// 			return l-start;
// }
//
// void
// Marsyas::peaks2V (realvec& in, realvec& last, realvec& out, mrs_natural maxNbPeaks, mrs_natural label)
// {
// 	mrs_natural i, j, k=0, start=0, iStart=0;
// 	mrs_natural frameIndex=-1, startIndex = (mrs_natural) in(0, pkFrame);
//
// 	out.setval(0);
//
// 	// on-line case with first init
// 	if(label == -1 && last(0))
// 	{
// 		start=1;
// 		startIndex +=1;
// 	}
// 	// if coming from a saved realvec first elts contain config so should be avoided
// 	if(in(0) == -1)
// 	{
// 		iStart = 1;
// 	}
//
// 	for (i=iStart ; i<in.getRows() ; ++i, k++)
// 	{
// 		if(frameIndex != in(i, 5))
// 		{
// 			frameIndex = (mrs_natural) in(i, 5);
// 			k=0;
// 		}
// 		if((!start || (start && in(i, 5) >= startIndex)))
// 			if(label < 0 || label == in(i, pkGroup))
// 				for (j=0 ; j<in.getCols() ; j++)
// 				{
// 					out(j*maxNbPeaks+k, frameIndex-startIndex) = in(i, j);
// 					/*	 if(peakFile)
// 					peakFile << in(i, j);*/
// 				}
// 	}
// 	//on-line case
// 	if(label == -1)
// 	{
// 		for (j=0 ; j<out.getRows() ; j++)
// 		{
// 			last(j) = out(j, out.getCols()-1);
// 		}
// 	}
// }
//
//
// void Marsyas::updateLabels(realvec& peakSet, realvec& conversion)
// {
// 	for (mrs_natural i=0 ; i<peakSet.getRows() ; ++i)
// 		peakSet(i, pkGroup) = conversion((mrs_natural) peakSet(i, pkGroup)+2);
// }
//
//
// void Marsyas::extractParameter(realvec&in, std::vector<realvec>& out, pkParameter type, mrs_natural kmax)
// {
// 	mrs_natural i, index, k=0;
// 	mrs_natural frameIndex=-1;
// 	mrs_natural startIndex = (mrs_natural)in(0, pkFrame);
// 	mrs_natural endIndex = (mrs_natural) in(in.getRows()-1, pkFrame);
// 	mrs_natural nbFrames = endIndex-startIndex+1;
// 	mrs_natural* l = new mrs_natural[nbFrames];
// 	realvec vec(kmax);
// 	vec.setval(0);
//
// 	// allocate vector of realvec
// 	for (i=0 ; i < nbFrames ; ++i)
// 	{
// 		out.push_back(vec);
// 		l[i]=0;
// 	}
//
// 	// fill vector
// 	for (i=0 ; i<in.getRows() ; ++i)
// 	{
// 		index = (mrs_natural) in(i, pkFrame) - startIndex;
// 		if( in(i, pkGroup) >= 0)
// 		{
// 		out[index](l[index]) = in(i, type);
// 		l[index]++;
// 		}
// 	}
// 	// stretch realvecs
// 	for (i=0 ; i < nbFrames ; ++i)
// 	{
// 		out[i].stretch(l[i]);
// 	}
//
// 	delete [] l;
// 	////out.setval(0);
// 	//for (i=0 ; i<in.getRows() ; ++i)
// 	//{
// 	//	if(frameIndex != in(i, pkFrame))
// 	//	{
// 	//		if(k)
// 	//		{
// 	//			vec.stretch(k);
// 	//			// put inside vector
// 	//			out.push_back(vec);
// 	//			vec.stretch(kmax);
// 	//		}
// 	//		else
// 	//			out.push_back(realvec());
// 	//		frameIndex = (mrs_natural) in(i, pkFrame);
// 	//		k=0;
// 	//	}
// 	//	if(in(i, pkFrame) == frameIndex)
// 	//	{
// 	//		vec(k++) = in(i, type);
// 	//	}
// 	//}
// 	//if(k)
// 	//{
// 	//	vec.stretch(k);
// 	//	// put inside vector
// 	//	out.push_back(vec);
// 	//}
// }
//
//
// mrs_real
// Marsyas::compareTwoPeakSets(realvec&f1, realvec&a1, realvec&f2, realvec&a2)
// {
// 	mrs_natural nb=0;
// 	mrs_real res=0;
// 	realvec i1(f1.getSize());
// 	realvec i2(f2.getSize());
// 	i1.setval(1);
// 	i2.setval(1);
//
// 	//cout << f1 << a1 << f2 << a2 ;
// 	// while the smallest set is not empty
// 	MATLAB_EVAL("Acc=[];");
// 	while (1)
// 	{
//
//
// 		//cout << i1<<i2;
// 		mrs_natural ind1=-1, ind2=0;
// 		mrs_real A1, A2, F1, F2;
// 		// minDiff = 10000000000000, maxVal = -100000000000000;
// 		mrs_real maxVal = 0, minDiff =100;
// 		// take the highest amplitude peak
// 		for (mrs_natural i=0 ; i<a1.getSize() ; ++i)
// 		{
// 			A1 = a1(i);
// 			if(i1(i) && A1 > maxVal)
// 			{
// 				maxVal= A1;
// 				ind1 = i;
// 			}
// 		}
// 		if(ind1 == -1) break;
//
// 		F1 = f1(ind1);
// 		A1 = a1(ind1);
// 		// find the closest frequency peak in the second set
// 		for (mrs_natural i=0 ; i<f2.getSize() ; ++i)
// 		{
// 			F2 = f2(i);
// 			if(i2(i) && abs(F1-F2) < minDiff)
// 			{
// 				minDiff= abs(F1-F2); // or product of fDiff and aDiff
// 				ind2 = i;
// 			}
// 		}
// 		A2 = a2(ind2);
// 		// acc the amplitude difference
// 		res += abs(A1-A2)*abs(F1-F2); // or product of fDiff and aDiff
// 		nb++;
// 		// remove the two peaks
// 		i1(ind1) = 0;
// 		i2(ind2) = 0;
//
// 		MATLAB_PUT(f1, "f1");
// 		MATLAB_PUT(a1, "a1");
// 		MATLAB_PUT(i1, "i1");
// 		MATLAB_PUT(i2, "i2");
// 		MATLAB_PUT(f2, "f2");
// 		MATLAB_PUT(a2, "a2");
// 		MATLAB_PUT(abs(A1-A2)*abs(F1-F2), "acc");
// 		MATLAB_EVAL("Acc=[Acc acc] ;subplot(2, 1, 1) ; plot(f1, a1.*i1, '*r', f2, a2.*i2, 'k+'); subplot(2, 1, 2); plot(Acc)");
//
// 	}
// 	return res/nb;
// }
//
// mrs_real
// Marsyas::compareTwoPeakSets2(realvec&f1, realvec&a1, realvec&f2, realvec&a2)
// {
// 	mrs_real res = 0;
// 	for (mrs_natural i = 0 ; i < f1.getSize() ; ++i)
// 		for (mrs_natural j = 0 ; j < f2.getSize() ; j++)
// 		{
// 			res += abs(f1(i)-f2(j))* abs(a1(i)-a2(j));
// 		}
// 		return res/(f1.getSize()*f2.getSize());
// }
//
// mrs_real
// Marsyas::compareTwoPeakSets3(realvec&f1, realvec&a1, realvec&f2, realvec&a2)
// {
// 	mrs_natural nb=0;
// 	mrs_real res=0;
// 	realvec i1(f1.getSize());
// 	realvec i2(f2.getSize());
// 	i1.setval(1);
// 	i2.setval(1);
//
// 	//cout << f1 << a1 << f2 << a2 ;
// 	// while the smallest set is not empty
// 	while (1)
// 	{
// 		//cout << i1<<i2;
// 		mrs_natural ind1=-1, ind2=0;
// 		// mrs_real minDiff = 10000000000000, minVal = 100000000000000;
//
// 		mrs_real minVal = 0;
// 		//
// 		//
// 		// look for the smallest couple
// 		for (mrs_natural i=0 ; i<a1.getSize() ; ++i)
// 			for (mrs_natural j=0 ; j<a2.getSize() ; j++)
// 			{
// 				mrs_real val = abs(f1(i)-f2(j));
// 				if(i1(i) && i2(j) && val < minVal)
// 				{
// 					minVal= val;
// 					ind1 = i;
// 					ind2 = j;
// 				}
// 			}
//
// 			if(ind1 == -1 || ind2 == -1) break;
//
// 			mrs_real val = abs(a1(ind1)-a2(ind2))*abs(f1(ind1)-f2(ind2));
// 			res += val; // or product of fDiff and aDiff
// 			nb++;
// 			// remove the two peaks
// 			i1(ind1) = 0;
// 			i2(ind2) = 0;
//
// 			MATLAB_PUT(f1, "f1");
// 			MATLAB_PUT(a1, "a1");
// 			MATLAB_PUT(i1, "i1");
// 			MATLAB_PUT(i2, "i2");
// 			MATLAB_PUT(f2, "f2");
// 			MATLAB_PUT(a2, "a2");
// 			MATLAB_EVAL("subplot(2, 1, 1) ; plot(f1, a1.*i1, '*r', f2, a2.*i2, 'k+'); subplot(2, 1, 2); plot(Acc)");
//
// 	}
// 	return res/nb;
// }
//
// mrs_real
// Marsyas::correlatePeakSets(realvec&f1, realvec&a1, realvec&f2, realvec&a2)
// {
// 	mrs_real res=0;
// 	realvec i1(f1.getSize());
// 	realvec i2(f2.getSize());
// 	i1.setval(1);
// 	i2.setval(1);
//
// 	//cout << f1 << a1 << f2 << a2 ;
// 	for (mrs_natural k=0; k<f1.getSize() ; k++)
// 	{
// 		//cout << i1<<i2;
// 		mrs_natural ind1=-1, ind2=-1;
// 		mrs_real minDiff = 2;
// 		// look for the couple closest in frequency
// 		for (mrs_natural i=0 ; i<f1.getSize() ; ++i)
// 			for (mrs_natural j=0 ; j<f2.getSize() ; j++)
// 				if(i1(i) && i2(j))
// 				{
// 					mrs_real fa = f1(i), fb = f2(j);
// 					if(fa>fb)
// 					{
// 						fa = f2(j);
// 						fb = f1(i);
// 					}
// 					mrs_real df = fb-fa;
// 					if(df>1-fb+fa)
// 						df = 1-fb+fa;
//
// 					if(df < minDiff)
// 					{
// 						minDiff= df;
// 						ind1 = i;
// 						ind2 = j;
// 					}
// 				}
//
//
//
//
// 				cout << i1;
// 				cout << i2;
//
//
//
// 				mrs_real val = a1(ind1)*a2(ind2)/(minDiff*minDiff+0.0000000000000001);
// 				cout << ind1 << " " << ind2 << " " << val << endl;
// 				res += val; // or product of fDiff and aDiff
// 				// remove the two peaks
// 				if(ind1 != -1)
// 				{
// 					i1(ind1) = 0;
// 					i2(ind2) = 0;
// 				}
// 				else
// 				{
// 					cout << i1 << i2;
// 				}
// 				/*		MATLAB_PUT(f1, "f1");
// 				MATLAB_PUT(a1, "a1");
// 				MATLAB_PUT(i1, "i1");
// 				MATLAB_PUT(i2, "i2");
// 				MATLAB_PUT(f2, "f2");
// 				MATLAB_PUT(a2, "a2");
// 				MATLAB_PUT(val, "acc");
// 				MATLAB_EVAL("plotHarmo");*/
// 				//	cout << ind1 <<" " << ind2<<endl;
// 	}
// 	return res/f1.getSize();
// }
//
// mrs_real
// Marsyas::cosinePeakSets(realvec&f1, realvec&a1, realvec&f2, realvec&a2, realvec&a3, realvec&a4, realvec&x1, realvec&x2, realvec&x3, realvec&x4, mrs_natural length)
// {
// 	mrs_natural index;
// 	mrs_real res1=0, res2=0, res3=0, res;
//
// 	x1.setval(0);
// 	x2.setval(0);
// 	x3.setval(0);
// 	x4.setval(0);
// 	// first discrete Harmonically Wrapped Spectrum
// 	for (mrs_natural i=0 ; i<f1.getSize() ; ++i)
// 	{
// 		index= (mrs_natural) fmod(floor(f1(i)*length+.5), length);
// 		x1(index) += a1(i);
// 		x3(index) += a3(i);
// 	}
// 	// second discrete Harmonically Wrapped Spectrum
// 	for (mrs_natural i=0 ; i<f2.getSize()  ; ++i)
// 	{
// 		index= (mrs_natural) fmod(floor(f2(i)*length+.5), length);
// 		//	cout << endl << "index " << index << endl;
// 		x2(index) += a2(i);
// 		x4(index) += a4(i);
// 	}
// 	// cosine metric
// 	for (mrs_natural i=0 ; i<x1.getSize()  ; ++i)
// 	{
// 		res1 += x1(i)*x2(i);
// 		res2 += x3(i)*x3(i);
// 		res3 += x4(i)*x4(i);
// 	}
// 	if (res2 && res3)
// 	res = res1/(sqrt(res2)*sqrt(res3));
// 	else
//     res = 0;
// 	/*if(res3==0)
// 	{
// 	cout << a4 ;
// 	cout << res1 << " " << res2 << " " << res3 << " " << x1.getSize() << endl;
//
// 	}*/
//
// 	//MATLAB_PUT(x1, "x1");
// 	//MATLAB_PUT(x2, "x2");
// 	//MATLAB_EVAL("plotHarmo");
//
// 	return res;
// }

void Marsyas::synthNetCreate(MarSystemManager *mng, mrs_string outsfname, bool microphone, mrs_natural synType, bool residual)
{
  //create Shredder series
  MarSystem* postNet = mng->create("Series", "postNet");
  //	postNet->addMarSystem(mng->create("PeOverlapadd", "ob"));
  if (synType < 3)
  {
    if(synType == 0)
    {
      postNet->addMarSystem(mng->create("PeakSynthOsc", "pso"));
      postNet->addMarSystem(mng->create("Windowing", "wiSyn"));
    }
    else
    {
      // put a fake object for probing the series
      postNet->addMarSystem(mng->create("Gain", "fakeGain"));
      postNet->addMarSystem(mng->create("FlowCutSource", "fcs"));
      // put the original source
      if (microphone)
        postNet->addMarSystem(mng->create("AudioSource", "srcSyn"));
      else
        postNet->addMarSystem(mng->create("SoundFileSource", "srcSyn"));
      // set the correct buffer size
      postNet->addMarSystem(mng->create("ShiftInput", "siSyn"));
      // perform an FFT
      postNet->addMarSystem(mng->create("Spectrum", "specSyn"));
      // convert to polar
      postNet->addMarSystem(mng->create("Cartesian2Polar", "c2p"));
      // perform amplitude and panning change
      postNet->addMarSystem(mng->create("PeakSynthFFT", "psf"));
      // convert back to cartesian
      postNet->addMarSystem(mng->create("Polar2Cartesian", "p2c"));
      // perform an IFFT
      //	 postNet->addMarSystem(mng->create("PlotSink", "plot"));
      postNet->addMarSystem(mng->create("InvSpectrum", "invSpecSyn"));
      // postNet->addMarSystem(mng->create("PlotSink", "plot2"));
      postNet->addMarSystem(mng->create("Windowing", "wiSyn"));
    }

    postNet->addMarSystem(mng->create("OverlapAdd", "ov"));
  }
  else
  {
    postNet->addMarSystem(mng->create("PeakSynthOscBank", "pso"));
    // postNet->addMarSystem(mng->create("ShiftOutput", "so"));
  }

  postNet->addMarSystem(mng->create("Gain", "outGain"));

  MarSystem *dest;
  if (outsfname == "MARSYAS_EMPTY")
    dest = mng->create("AudioSink", "dest");
  else
  {
    dest = mng->create("SoundFileSink", "dest");
    //dest->updControl("mrs_string/filename", outsfname);
  }

  if(residual)
  {
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
    postNet->addMarSystem(mng->create("PeakResidual", "res"));

    MarSystem *destRes;
    if (outsfname == "MARSYAS_EMPTY")
      destRes = mng->create("AudioSink", "destRes");
    else
    {
      destRes = mng->create("SoundFileSink", "destRes");
      //dest->updControl("mrs_string/filename", outsfname);
    }
    postNet->addMarSystem(destRes);
  }
  else
    postNet->addMarSystem(dest);

  MarSystem* shredNet = mng->create("Shredder", "shredNet");
  shredNet->addMarSystem(postNet);

  mng->registerPrototype("PeSynthetize", shredNet);
}

void
Marsyas::synthNetConfigure(MarSystem *pvseries, mrs_string sfName, mrs_string outsfname, mrs_string ressfname, std::string panningInfo, mrs_natural nbChannels, mrs_natural Nw,
                           mrs_natural D, mrs_natural S, mrs_natural accSize, bool microphone, mrs_natural synType, mrs_natural bopt, mrs_natural delay, mrs_real fs, bool residual)
{
  // FIXME Unused parameters
  (void) nbChannels;
  (void) S;
  (void) Nw;

  pvseries->updControl("PeSynthetize/synthNet/mrs_natural/nTimes", accSize);

  if (synType < 3)
  {
    if(synType==0)
    {
      //pvseries->updControl("PeSynthetize/synthNet/Series/postNet/PeakSynthOsc/pso/mrs_natural/nbSinusoids", S);
      pvseries->updControl("PeSynthetize/synthNet/Series/postNet/PeakSynthOsc/pso/mrs_natural/delay", delay); // Nw/2+1
      pvseries->updControl("PeSynthetize/synthNet/Series/postNet/PeakSynthOsc/pso/mrs_natural/synSize", D*2);
      pvseries->updControl("PeSynthetize/synthNet/Series/postNet/PeakSynthOsc/pso/mrs_real/samplingFreq", fs);
      pvseries->updControl("PeSynthetize/synthNet/Series/postNet/Windowing/wiSyn/mrs_string/type", "Hanning");
    }
    else
    {
      // linking between the first slice and the psf
      pvseries->linkControl("PeSynthetize/synthNet/Series/postNet/mrs_realvec/processedData", "PeSynthetize/synthNet/Series/postNet/PeakSynthFFT/psf/mrs_realvec/peaks");
      //
      pvseries->updControl("PeSynthetize/synthNet/Series/postNet/Windowing/wiSyn/mrs_string/type", "Hanning");
      pvseries->updControl("PeSynthetize/synthNet/Series/postNet/FlowCutSource/fcs/mrs_natural/setSamples", D);
      pvseries->updControl("PeSynthetize/synthNet/Series/postNet/FlowCutSource/fcs/mrs_natural/setObservations", 1);
      // setting the panning mode mono/stereo
      pvseries->updControl("PeSynthetize/synthNet/Series/postNet/PeakSynthFFT/psf/mrs_natural/nbChannels", synType);
      pvseries->updControl("PeSynthetize/synthNet/Series/postNet/PeakSynthFFT/psf/mrs_string/panning", panningInfo);
      // setting the FFT size
      pvseries->updControl("PeSynthetize/synthNet/Series/postNet/ShiftInput/siSyn/mrs_natural/winSize", D*2);
      // setting the name of the original file
      if (microphone)
      {
        pvseries->updControl("PeSynthetize/synthNet/Series/postNet/AudioSource/srcSyn/mrs_natural/inSamples", D);
        pvseries->updControl("PeSynthetize/synthNet/Series/postNet/AudioSource/srcSyn/mrs_natural/inObservations", 1);
      }
      else
      {
        pvseries->updControl("PeSynthetize/synthNet/Series/postNet/SoundFileSource/srcSyn/mrs_string/filename", sfName);
        // pvseries->updControl("PeSynthetize/synthNet/Series/postNet/SoundFileSource/srcSyn/mrs_natural/pos", 0);
        pvseries->updControl("PeSynthetize/synthNet/Series/postNet/SoundFileSource/srcSyn/mrs_natural/onSamples", D);
        pvseries->updControl("PeSynthetize/synthNet/Series/postNet/SoundFileSource/srcSyn/mrs_natural/onObservations", 1);
      }
      // setting the synthesis starting time (default 0)
    }
  }
  else
    pvseries->updControl("PeSynthetize/synthNet/Series/postNet/PeakSynthOscBank/pso/mrs_natural/Interpolation", D);

  // pvseries->updControl("PeSynthetize/synthNet/Series/postNet/ShiftOutput/so/mrs_natural/Interpolation", D); //[WTF]

  if (outsfname == "MARSYAS_EMPTY")
    pvseries->updControl("PeSynthetize/synthNet/Series/postNet/AudioSink/dest/mrs_natural/bufferSize", bopt);

  if(residual)
  {
    pvseries->updControl("PeSynthetize/synthNet/Series/postNet/Fanout/fano/Series/fanSeries/Delay/delay/mrs_natural/delay", delay); // Nw+1-D

    if (microphone)
    {
      pvseries->updControl("PeSynthetize/synthNet/Series/postNet/Fanout/fano/Series/fanSeries/AudioSource/src2/mrs_natural/inSamples", D);
      pvseries->updControl("PeSynthetize/synthNet/Series/postNet/Fanout/fano/Series/fanSeries/AudioSource/src2/mrs_natural/inObservations", 1);
    }
    else
    {
      pvseries->updControl("PeSynthetize/synthNet/Series/postNet/Fanout/fano/Series/fanSeries/SoundFileSource/src2/mrs_string/filename", sfName);
      pvseries->updControl("PeSynthetize/synthNet/Series/postNet/Fanout/fano/Series/fanSeries/SoundFileSource/src2/mrs_natural/pos", 0);
      pvseries->updControl("PeSynthetize/synthNet/Series/postNet/Fanout/fano/Series/fanSeries/SoundFileSource/src2/mrs_natural/inSamples", D);
      pvseries->updControl("PeSynthetize/synthNet/Series/postNet/Fanout/fano/Series/fanSeries/SoundFileSource/src2/mrs_natural/inObservations", 1);
    }

    pvseries->updControl("PeSynthetize/synthNet/Series/postNet/Fanout/fano/SoundFileSink/dest/mrs_string/filename", outsfname);//[!]
    pvseries->updControl("PeSynthetize/synthNet/Series/postNet/SoundFileSink/destRes/mrs_string/filename", ressfname);//[!]
  }
  else
    pvseries->updControl("PeSynthetize/synthNet/Series/postNet/SoundFileSink/dest/mrs_string/filename", outsfname);//[!]

}

// mrs_real Marsyas::harmonicWeighting(mrs_real f, mrs_real h, mrs_real w)
// {
// 	if(f < h)
// 		return 1;
// 	else
// 		return pow(harmonicWeightingBasic(f, h), -w/log(harmonicWeightingBasic(1,h)));
// }
//
//
// mrs_real Marsyas::harmonicWeightingBasic(mrs_real f, mrs_real h)
// {
// 	return (1+cos(2*PI*f/h))/2;
// }
//
// //[WTF]
// void Marsyas::discrete2labels(realvec &labels, realvec& n, mrs_natural nbClusters, mrs_natural nbPeaks)
// {
// 	for(mrs_natural i=0 ; i<nbPeaks ; ++i)
// 		for(mrs_natural j=0 ; j<nbClusters ; j++)
// 			if(n(i*nbClusters+j) == 1)
// 				labels(i) = j;
// }
//
// void Marsyas::peakStore(realvec &peaks, mrs_string filename, mrs_real sf, mrs_natural hopSize)
// {
//   mrs_natural nbFrames_ = peaks.getCols();
//
// 	if(!nbFrames_) //i.e. if(nbFrames == 0)
// 	{
//      // search from a file  //[WTF]
//       peaks.read(filename) ;
// 	  peaks.transpose();
//       nbFrames_ = peaks.getCols();
// 	}
// 	realvec peakSetM_.create(peaks.getRows()/nbPkParameters*nbFrames_+1, nbPkParameters);
// 	peakSetM_(0, 0) = -1;
// 	peakSetM_(0, 1) =  sf;
// 	peakSetM_(0, 2) =  hopSize;
// 	peakSetM_(0, 3) =  peaks.getRows()/nbPkParameters;
// 	peakSetM_(0, 4) =  nbFrames_;
// 	peakSetM_(0, 5) = -1;
// 	peakSetM_(0, pkGroup) = -2; //isn't this overwritten below?!? [WTF]
// 	for (mrs_natural i=pkGroup ; i< nbPkParameters ; ++i)
// 		peakSetM_(0, i)=0;
// 	realvec tmp(1);
// 	tmp.setval(0);
// 	mrs_natural tmp2;
//
// 	peaks2M(peaks, tmp, peakSetM_, peaks.getRows()/nbPkParameters, &tmp2, 1);
//
// 	//cout << peakSetM_;
// 	//cout << sf << " " << peakSetM_(0, 1) << endl ;
//
// 	//cout << peaks.getSize() << endl;
// 	ofstream peakFile;
// 	peakFile.open(filename.c_str());
// 	if(!peakFile)
// 		cout << "Unable to open output Peaks File " << filename << endl;
// 	peakFile << peakSetM_;
// 	peakFile.close();
// }
//
// void Marsyas::peakLoad(realvec &peaks, mrs_string filename, mrs_real &fs, mrs_natural &nbSines, mrs_natural &nbFrames, mrs_natural &hopSize, bool tf_format)
// {
// 	realvec peakSet;
// 	peakSet.read(filename);
//
// 	fs  = peakSet(0, 1);
// 	hopSize = peakSet(0, 2);
// 	nbSines = peakSet(0, 3);
// 	nbFrames = peakSet(0, 4);
//
// 	peakSet(0, 5) = -1; //HACK!!!!! [!]
//
// 	if(tf_format)
// 	{
// 		peaks.stretch(nbSines*nbPkParameters, nbFrames);
// 		peaks.setval(0);
// 		peaks2V(peakSet, peaks, peaks, nbSines);
// 	}
// 	else
// 		peaks = peakSet;
// }
//
// /*
// mrs_real gaussian(mrs_real x, mrs_real v, mrs_real m)
// {
// 	return exp(-(x-m)*(x-m)/(2*v*v))/(v*sqrt(2*PI));
// }*/
//
//
// void Marsyas::computeHarmonicityMap(realvec& map, mrs_natural nbBins)
// {
// 	mrs_natural i, j;
// 	realvec tmp(nbBins, nbBins);
// 	map.stretch(nbBins, nbBins);
// 	map.setval(0);
//
// 	for (i=0 ; i<nbBins ; ++i)
// 		for (j=0 ; j<nbBins ; j++)
// 		{
// 			mrs_real val=0;
// 			if((i+1)%(j+1) == 0 || (j+1)%(i+1) == 0)
// 				val = 1;
// 			tmp(i, j) = val;
// 		}
//
// 		mrs_natural kSize=5, k, l;
// 		mrs_real sum=0;
// 		for (i=0 ; i<nbBins ; ++i)
// 			for (j=0 ; j<nbBins ; j++)
// 				for (k=max( (mrs_natural) 0, i-kSize); k<min(nbBins, i+kSize) ; k++)
// 					for (l=max((mrs_natural) 0, j-kSize) ; l<min(nbBins, j+kSize) ; l++)
// 					{
// 						mrs_real val1 = NumericLib::gaussian (i+1, (k+1)/10.0, k+1);
// 						mrs_real val2 = NumericLib::gaussian (j+1, (l+1)/10.0, l+1);
// 						map(i, j) += tmp(k, l)*val1*val2;
// 						sum += tmp(k, l)*val1*val2;
// 					}
// 					map/=sum;
// }
