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
		peakSet(i, pkGroup) = conversion((mrs_natural) peakSet(i, pkGroup));
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




void Marsyas::synthNetCreate(MarSystemManager *mng, string outsfname, bool microphone)
{
	//create Shredder series
	MarSystem* postNet = mng->create("Series", "postNet");
	postNet->addMarSystem(mng->create("PeOverlapadd", "ob"));
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
													 mrs_natural D, mrs_natural S, mrs_natural accSize, bool microphone, mrs_natural bopt, mrs_natural delay)
{
	pvseries->updctrl("Shredder/synthNet/mrs_natural/nTimes", accSize);
	pvseries->updctrl("Shredder/synthNet/Series/postNet/PeOverlapadd/ob/mrs_natural/hopSize", D);
	pvseries->updctrl("Shredder/synthNet/Series/postNet/PeOverlapadd/ob/mrs_natural/nbSinusoids", S);
	pvseries->updctrl("Shredder/synthNet/Series/postNet/PeOverlapadd/ob/mrs_natural/delay", Nw/2+1);
	pvseries->updctrl("Shredder/synthNet/Series/postNet/ShiftOutput/so/mrs_natural/Interpolation", D);
	pvseries->updctrl("Shredder/synthNet/Series/postNet/ShiftOutput/so/mrs_natural/WindowSize", Nw);      
	pvseries->updctrl("Shredder/synthNet/Series/postNet/ShiftOutput/so/mrs_natural/Decimation", D);

	if (microphone) 
	{
		pvseries->updctrl("Shredder/synthNet/Series/postNet/Fanout/fano/Series/fanSeries/AudioSource/src2/mrs_natural/inSamples", D);
		pvseries->updctrl("Shredder/synthNet/Series/postNet/Fanout/fano/Series/fanSeries/AudioSource/src2/mrs_natural/inObservations", 1);
	}
	else
	{
		pvseries->updctrl("Shredder/synthNet/Series/postNet/Fanout/fano/Series/fanSeries/SoundFileSource/src2/mrs_string/filename", sfName);
		pvseries->updctrl("Shredder/synthNet/Series/postNet/Fanout/fano/Series/fanSeries/SoundFileSource/src2/mrs_natural/pos", 0);
		pvseries->updctrl("Shredder/synthNet/Series/postNet/Fanout/fano/Series/fanSeries/SoundFileSource/src2/mrs_natural/inSamples", D);
		pvseries->updctrl("Shredder/synthNet/Series/postNet/Fanout/fano/Series/fanSeries/SoundFileSource/src2/mrs_natural/inObservations", 1);
	}
	if (outsfname == EMPTYSTRING) 
		pvseries->updctrl("Shredder/synthNet/Series/postNet/AudioSink/dest/mrs_natural/bufferSize", bopt);


	pvseries->updctrl("Shredder/synthNet/Series/postNet/Fanout/fano/Series/fanSeries/Delay/delay/mrs_natural/delay", delay); // Nw+1-D
	pvseries->updctrl("Shredder/synthNet/Series/postNet/Fanout/fano/SoundFileSink/dest/mrs_string/filename", outsfname);//[!]
	pvseries->updctrl("Shredder/synthNet/Series/postNet/SoundFileSink/destRes/mrs_string/filename", ressfname);//[!]
}
