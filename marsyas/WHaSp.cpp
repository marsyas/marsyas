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
	\class WHaSp
	\ingroup MarSystem
	\brief Calculate Wrapped Harmonically Spectrum (WHaSp)
	
	- \b mrs_natural/histSize [w] : set the discretization size when comparing HWPS spectrums (i.e. the histogram nr of bins).
	- \b mrs_natural/totalNumPeaks [w] : this control sets the total num of peaks at the input (should normally be linked with PeConvert similar control)
	- \b mrs_natural/frameMaxNumPeaks [w] : this control sets the maximum num of peaks per frame at the input (should normally be linked with PeConvert similar control)
*/

#include "WHaSp.h"
#include "peakView.h"
#include "PeFeatSelect.h"
#include "SimilarityMatrix.h"
#include "HWPS.h"

using namespace std;
using namespace Marsyas;

WHaSp::WHaSp(string name):MarSystem("WHaSp", name)
{
	addControls();
	createSimMatrixNet();
}

WHaSp::WHaSp(const WHaSp& a) : MarSystem(a)
{
	ctrl_histSize_ = getctrl("mrs_natural/histSize");
	ctrl_totalNumPeaks_ = getctrl("mrs_natural/totalNumPeaks");
	ctrl_frameMaxNumPeaks_ = getctrl("mrs_natural/frameMaxNumPeaks");
	
	createSimMatrixNet();
}

WHaSp::~WHaSp()
{
	delete HWPSnet_;
}

MarSystem* 
WHaSp::clone() const 
{
  return new WHaSp(*this);
}

void 
WHaSp::addControls()
{
	addctrl("mrs_natural/histSize", 20, ctrl_histSize_);
	addctrl("mrs_natural/totalNumPeaks",0, ctrl_totalNumPeaks_);
	addctrl("mrs_natural/frameMaxNumPeaks", 0, ctrl_frameMaxNumPeaks_);
}

void
WHaSp::createSimMatrixNet()
{
	HWPSnet_ = new Series("HWPSnet");

	//add a feat selector and 
	//set the features needed for HWPS
	MarSystem* peFeatSelect = new PeFeatSelect("peFeatSelect");
	peFeatSelect->updctrl("mrs_natural/selectedFeatures",
		PeFeatSelect::pkFrequency | PeFeatSelect::pkSetFrequencies| PeFeatSelect::pkSetAmplitudes);
	HWPSnet_->addMarSystem(peFeatSelect);

	//create a similarityMatrix MarSystem that uses the HPWS metric
	SimilarityMatrix* simMat = new SimilarityMatrix("simMat");
	simMat->addMarSystem(new HWPS("hwps"));

	HWPSnet_->addMarSystem(simMat);

	//link totalNumPeaks control to PeFeatSelect
	ctrl_totalNumPeaks_->linkTo(HWPSnet_->getctrl("PeFeatSelect/peFeatSelect/mrs_natural/totalNumPeaks"));
	//link frameMaxNumPeaks control to PeFeatSelect
	ctrl_frameMaxNumPeaks_->linkTo(HWPSnet_->getctrl("PeFeatSelect/peFeatSelect/mrs_natural/frameMaxNumPeaks"));

	//link histSize control to HWPS metric
	ctrl_histSize_->linkTo(HWPSnet_->getctrl("SimilarityMatrix/simMat/HWPS/hwps/mrs_natural/histSize"));
	ctrl_histSize_->setValue(20, NOUPDATE);
}

void
WHaSp::myUpdate(MarControlPtr sender)
{
	//output has the same flow format as input (see myProcess())
	MarSystem::myUpdate(sender);

	//pass input flow configuration to internal MarSystem
	HWPSnet_->setctrl("mrs_natural/inSamples",ctrl_inSamples_);
	HWPSnet_->setctrl("mrs_natural/inObservations", ctrl_inObservations_);
	HWPSnet_->setctrl("mrs_real/israte", ctrl_israte_);
	HWPSnet_->updctrl("mrs_string/inObsNames", ctrl_inObsNames_); //this calls update()
}

void 
WHaSp::myProcess(realvec& in, realvec& out)
{
	peakView inPkView(in);
	peakView outPkView(out);

	//copy input to output, so we can fill in
	//the WHASP values calculated bellow
	out = in;
	
	mrs_natural numPeaks = inPkView.getFrameNumPeaks();

	if(numPeaks > 0)
	{
		simMatrix_.create(numPeaks, numPeaks);
		
		HWPSnet_->process(in, simMatrix_);
		
		mrs_real maxDist = MINREAL;
		for(mrs_natural r=0; r<numPeaks; ++r)
		{
			outPkView(r, peakView::pkVolume) = 0;
			for(mrs_natural c=0; c < numPeaks; ++c)
			{
				if(r != c)
					outPkView(r, peakView::pkVolume) += simMatrix_(r, c); 
			}
			if(outPkView(r, peakView::pkVolume) > maxDist)
				maxDist = outPkView(r, peakView::pkVolume);
		}
		//filter peaks with Volumes below a defined threshold [!]
		mrs_real dist;
		for(mrs_natural i=0; i < numPeaks; ++i)
		{
			dist = outPkView(i, peakView::pkVolume)/maxDist;
			if(dist < 0.75)
				outPkView(i, peakView::pkAmplitude) = 0;
		}
	}
}







	
