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
\class PeClusters
\brief 

*/

#include "PeClusters.h"
#include "PeUtilities.h"
#include "FileName.h"

using namespace std;
using namespace Marsyas;



PeCluster::PeCluster()
{
	groundLabel = -1;
	oriLabel=-1;
	label=-1;
	histSize = 20;
}

PeCluster::~PeCluster()
{

}

void
PeCluster::init(realvec& peakSet, mrs_natural l)
{
	mrs_natural i;

	// set general informations
	start = MAXNATURAL;
	end=0;
	nbPeaks=0;
	for (i=0 ; i< peakSet.getRows() ; i++)
		if (peakSet(i, pkGroup) == l)
		{
			if(peakSet(i, pkTime) < start)
			{
				start = peakSet(i, pkTime);
			}
			if(peakSet(i, pkTime) > end)
			{
				end= peakSet(i, pkTime);
			}
			nbPeaks++;
		}

		length = end-start;
		envSize = (mrs_natural) length+1;
		oriLabel = l;
		label = l;
}

void 
PeCluster::computeAttributes(realvec& peakSet, mrs_natural l, string type)
{
		mrs_natural i, j, k;
	realvec set;
 
	set.stretch(nbPeaks, nbPkParameters);

	for (i=0, k=0 ; i< peakSet.getRows() ; i++)
		if (peakSet(i, pkGroup) == l)
		{
			for(j=0 ; j<nbPkParameters ; j++)
				set(k, j) = peakSet(i, j);
			k++;
		}

		// compute stats
		freqMean = set.getCol(pkFrequency).mean();
		freqStd = set.getCol(pkFrequency).std();
		ampMean = set.getCol(pkAmplitude).mean();
		ampStd = set.getCol(pkAmplitude).std();

		// compute envelopes
		frequencyEvolution.stretch(envSize);
		frequencyEvolution.setval(0);
		amplitudeEvolution.stretch(envSize);
		amplitudeEvolution.setval(0);
	
		for(i=0 ; i<nbPeaks ; i++)
			{ 
				frequencyEvolution(((mrs_natural) set(i, pkTime)-start)) += 
					set(i, pkFrequency);//*set(i, pkAmplitude);

				amplitudeEvolution(((mrs_natural) set(i, pkTime)-start)) += 
					set(i, pkAmplitude);
			}
	
			amplitudeHistogram.stretch(histSize);
			amplitudeHistogram.setval(0);
			frequencyHistogram.stretch(histSize);
			frequencyHistogram.setval(0);
			harmonicityHistogram.stretch(histSize);
			harmonicityHistogram.setval(0);
			// compute histograms
			/*for(i=0 ; i<histSize ; i++)
			{
				amplitudeHistogram((mrs_natural) floor(set(i, pkAmplitude)*histSize)) += 1;
				frequencyHistogram((mrs_natural) floor(set(i, pkFrequency)*histSize/22050)) += 1;
			}*/
	
			// compute similarities within cluster
}

mrs_natural 
PeCluster::getVecSize()
{
	return 3+6+2*envSize;
}

void 
PeCluster::toVec(realvec& vec)
{
	mrs_natural i=0, j;

	vec(i++) = oriLabel;
	vec(i++) = groundLabel;	
	vec(i++) = label;

	vec(i++) = start;
	vec(i++) = length;	
	vec(i++) = freqMean;
	vec(i++) = freqStd;
	vec(i++) = ampMean;
	vec(i++) = ampStd;

	for (j=0;j<envSize ; j++)
		vec(i++) = frequencyEvolution(j);
	for (j=0;j<envSize ; j++)
		vec(i++) = amplitudeEvolution(j);
}

mrs_natural 
PeCluster::getGroundThruth ()
{
	return groundLabel;
}

void 
PeCluster::setGroundThruth (mrs_natural l)
{
	groundLabel = l;
}

mrs_natural 
PeCluster::getOriLabel ()
{
	return oriLabel;
}

void PeCluster::setOriLabel (mrs_natural l)
{
	oriLabel = l;
}

mrs_natural PeCluster::getLabel ()
{
	return label;
}

void PeCluster::setLabel (mrs_natural l)
{
	label = l;
}


///////////////////////////////////////////////////////////

//////// Clusters STUFF

////////////////////////////////////////////////////////////

PeClusters::PeClusters(realvec &peakSet)
{
	// determine the number of clusters
	nbClusters=0;
	nbFrames=0;
	for (int i=0 ; i<peakSet.getRows() ; i++)
	{
		if(peakSet(i, pkGroup) > nbClusters)
			nbClusters = (mrs_natural) peakSet(i, pkGroup);
		if(peakSet(i, pkTime) > nbFrames)
			nbFrames = (mrs_natural) peakSet(i, pkTime);
	}
	nbClusters++;
	nbFrames++;
	// build the vector of clusters
	set = new PeCluster[nbClusters];
	for (int i=0 ; i<nbClusters ; i++)
	{
		set[i].init(peakSet, i);
	}
	conversion.stretch(nbClusters);
}

PeClusters::~PeClusters(){
	delete [] set;
}
void 
PeClusters::attributes(realvec &peakSet)
{
for (int i=0 ; i<nbClusters ; i++)
		set[i].computeAttributes(peakSet, i, "");
}

void
PeClusters::getVecs(realvec& vecs)
{
	mrs_natural maxSize=0;
	for (int i=0 ; i<nbClusters ; i++)
	if(set[i].getVecSize()>maxSize)
	{
maxSize = set[i].getVecSize();
	}
	vecs.stretch(nbClusters, maxSize);
	vecs.setval(0);

	for (int i=0 ; i<nbClusters ; i++)
	{
		realvec vec(set[i].getVecSize());
		set[i].toVec(vec);
		for (int j=0 ; j<vec.getSize() ; j++)
			vecs(i, j) = vec(j);
	}
}


realvec&
PeClusters::getConversionTable()
{
	for (int i=0 ; i<nbClusters ; i++)
	{
		conversion(i) = set[i].label;
	}
	return conversion;
}

void
PeClusters::selectBefore(mrs_real val)
{
	for (int i=0 ; i<nbClusters ; i++)
	{
		if(set[i].start < val)
			set[i].label = 0;
		else
			set[i].label = -1;
	}
}

void
PeClusters::selectGround()
{
	for (int i=0 ; i<nbClusters ; i++)
	{
		set[i].label = set[i].groundLabel;
	}
}

void
PeClusters::synthetize(realvec &peakSet, string fileName, string outFileName, mrs_natural Nw, mrs_natural D, mrs_natural S, mrs_natural bopt, mrs_natural residual)
{
	cout << "Synthetizing Clusters" << endl;
	MarSystemManager mng;

	synthNetCreate(&mng, fileName, 0);
	MarSystem* pvseries = mng.create("Series", "pvseries");
	MarSystem *peSynth = mng.create("PeSynthetize", "synthNet");

	MarSystem *peSource = mng.create("RealvecSource", "peSource");
	pvseries->addMarSystem(peSource);


	pvseries->addMarSystem(peSynth);

	// convert peakSet in frame form
	realvec pkV(S*nbPkParameters, nbFrames);

	// for all clusters
	for (int i=0 ; i<nbClusters ; i++)
		if(set[i].label != -1)
		{
			// cout << set[i].label << endl;
			// label peak set
			pkV.setval(0);
			// peaks2V(peakSet, NULL, pkV, S, i);
			// Changed by gtzan in order to compile trunk in Linux/OS X 
			// NULL CAN NOT BE A realvec& 
			peaks2V(peakSet, peakSet, pkV, S, i);

			pvseries->setctrl("RealvecSource/peSource/mrs_realvec/data", pkV);
			pvseries->setctrl("RealvecSource/peSource/mrs_real/israte", peakSet(0, 1));

			// configure synthesis
			FileName FileName(outFileName);
			string path = FileName.path();
			string name = FileName.nameNoExt();
			string ext = FileName.ext();

			ostringstream ossi;
			ossi << i;
			string ground = "";
			if(residual)
				ground = "Grd";

			// string outsfname = path + name + "_" +  itoa(i, tmp, 10) + "." + ext;
			// string fileResName = path + name + "Res_" + itoa(i, tmp, 10) + "." + ext;

			// Changed by gtzan to compile trunk under Linux 
			string outsfname = path + name + ground + "_" +  ossi.str() + "." + ext;
			string fileResName = path + name + ground + "Res_" + ossi.str() + "." + ext;

			synthNetConfigure (pvseries, fileName, outsfname, fileResName, Nw, D, S, 1, 0, bopt, Nw+1-D); //  nbFrames

			mrs_natural nbActiveFrames=0;
			mrs_real Snr=0;
			while(1)
			{
				// launch synthesis
				pvseries->tick();

				// get snr and decide ground thruth
				if(residual)
				{
					const mrs_real snr = pvseries->getctrl("PeSynthetize/synthNet/Series/postNet/PeResidual/res/mrs_real/snr")->toReal();
					//	cout << " SNR : "<< snr << endl;
					if(snr != -80)
					{
						nbActiveFrames++;
						Snr+=snr;
					}
				}
				if(pvseries->getctrl("RealvecSource/peSource/mrs_bool/done")->toBool())
					break;
			}
			if(residual)
			{
				Snr/=nbActiveFrames;
				if(Snr)
					Snr = 10*log10(pow(10, Snr)-1);
				else
					Snr = -80;
				cout << " SNR for cluster " << i << " : "<< Snr << " " << nbActiveFrames << endl;
				if(Snr > -3)
					set[i].groundLabel = 0;
				else
					set[i].groundLabel = -1;

			}	
		}
}
