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
\class BICchangeDetector
\brief detect changes between to succeeding segments using BIC

Detect changes between two succeeding segments using divergenceShape
(or BattacharyyaShape), and validation using BIC and quasi-GMM modeling

Implementation by Luis Gustavo Martins - lmartins@inescporto.pt
*/

#include "BICchangeDetector.h"
#include "Memory.h"

using namespace std;
using namespace Marsyas;

BICchangeDetector::BICchangeDetector(string name):MarSystem("BICchangeDetector", name)
{
	prevDists_ = new Memory("cirMem");
	addControls();
}

BICchangeDetector::BICchangeDetector(const BICchangeDetector& a) : MarSystem(a)
{
	prevDists_ = new Memory(*(a.prevDists_));
	
	ctrl_reset_ = getctrl("mrs_bool/reset");
	ctrl_alpha1_= getctrl("mrs_real/alpha1");
	ctrl_lambda_= getctrl("mrs_real/lambda");
}

BICchangeDetector::~BICchangeDetector()
{
	delete prevDists_;
}

MarSystem* 
BICchangeDetector::clone() const 
{
	return new BICchangeDetector(*this);
}

void 
BICchangeDetector::addControls()
{
	addctrl("mrs_bool/reset", true, ctrl_reset_);
	setctrlState(ctrl_reset_, true);

	addctrl("mrs_real/alpha1", 0.6, ctrl_alpha1_);
	addctrl("mrs_real/lambda", 0.8, ctrl_lambda_);

	dynThres_ = 0.0;
	prevDists_->updctrl("mrs_natural/inSamples", 1);
	prevDists_->updctrl("mrs_natural/inObservations", 1);
	nrPrevDists_ = 3; // hardcoded! [!]
	prevDists_->updctrl("mrs_natural/memSize", nrPrevDists_); //store 3 previous distances, for dynamic thresholding
}

void
BICchangeDetector::myUpdate(MarControlPtr sender)
{
	MarSystem::myUpdate(sender);

	//BICchangeDetector must receive as input a vector of feature frames as depicted bellow, 
	//which includes 4 speech sub-segments (C1, C2, C3, C4) that will be used for detecting 
	//speaker changes:
	// |------------------------|
	//      C1        C2
	// |----+----|----+----|					
	//      |----+----|----+----|
	//           C3        C4
	// |--->|
	//   hop
	//
	// For now, hop is set fixed as 1/5 of inSamples [!]
	//
	if(segFrames_ != ctrl_inSamples_->to<mrs_natural>()*2/5) //hardcoded [!]
	{
		segFrames_ = ctrl_inSamples_->to<mrs_natural>()*2/5; // hardcoded [!]
		segHop_ = ctrl_inSamples_->to<mrs_natural>()*1/5; // hardcoded [!]
		mrs_natural nfeats = ctrl_inObservations_->to<mrs_natural>();
		C1_.allocate(nfeats, segFrames_);
		C2_.allocate(nfeats, segFrames_);
		C3_.allocate(nfeats, segFrames_);
		C4_.allocate(nfeats, segFrames_);
	}

	if(ctrl_reset_->to<bool>())
	{
		QGMMmodel_.resetModel();
		prevDists_->updctrl("mrs_bool/reset", true);
		pdists_.setval(0.0);
		ctrl_reset_->setValue(false);
	}
}

void 
BICchangeDetector::myProcess(realvec& in, realvec& out)
{
	for(o=0; o < inObservations_; ++o)
	{
		//get segments => use pointers to "in" instead of copies?! [!]
		for(t=0; t < segFrames_ ; ++t)		
		{
			C1_(o, t) = in(o, t);
			C2_(o, t + segFrames_) = in(o, t + segFrames_);
			C3_(o, t + segHop_) = in(o, t + segHop_);
			C4_(o, t + segHop_ + segFrames_) = in(o, t + segHop_ + segFrames_);
		}

		//bypass input to output unchanged [!]
		for(t=0; t < inSamples_; ++t)
			out(o, t) = in(o, t);
	}

	//calculate covariance matrix for each segment
	C1_ = C1_.covariance();
	C2_ = C2_.covariance();
	C3_ = C3_.covariance();
	C4_ = C4_.covariance();

	//update current qGMM model, using the first sub-segment data, C1_
	QGMMmodel_.updateModel(C1_, segFrames_);

	//calculate divergenceShape between sub-segment pairs
	dist12_ = realvec::divergenceShape(C1_, C2_);
	dist34_ = realvec::divergenceShape(C3_, C4_);

	//calculate bhattacharyyaShape between sub-segment pairs => should be an option! [!]
	//mrs_real dist12 = realvec::bhattacharyyaShape(C1_, C2_);
	//mrs_real dist34 = realvec::bhattacharyyaShape(C3_, C4_);

	//calculate dynamic threshold, using the distance values from 
	//the previously stored sub-segment pairs
	// (NOTE:this implementation is slightly different from the one used in marsyas0.1
	//       implementation, where the mean calculation was performed only on the values 
	//       actually sent to the circular buffer (i.e. avoids calculating mean of an 
	//       "empty" circular buffer, as will happen here in the first process() calls...)
	pdists_ = prevDists_->getctrl("mrs_realvec/processedData")->to<mrs_realvec>(); // processedData: does this work?!?!?!?!?! [?]
	dynThres_ = pdists_.mean() * ctrl_alpha1_->to<mrs_real>();
	
	// get the values for the previous (i.e. left) and next (i.e. right) distances (in time)
	// so we can later check if the current distance is a local maximum (i.e. a peak)
	mrs_real distanceLeft = pdists_(pdists_.getSize()-1); //i.e. the previous distance value
	mrs_real distanceRight = dist34_; //i.e the next distance value
	//just to avoid "spurious" peaks in the audio stream borders (i.e. start and end of stream)...
	if(distanceLeft == 0.0)
		distanceLeft = dist12_;
	if(distanceRight == 0.0)
		distanceRight = dist12_;

	//store current distance(C1,C2) in the circular buffer for next iteration
	realvec newDist(1);
	newDist(0) = dist12_;
	prevDists_->process(newDist, pdists_);

	//check for a potential change (based only on distances!)
	// (i.e. distance is local maxima and is above the dynamic threshold)
	if(dist12_ > distanceRight && dist12_ > distanceLeft && dist12_ > dynThres_)
	{
		//if this a potential change point, validate it using BIC and the current model
		BICdist_ = QGMMmodel_.BICdistance(C2_, segFrames_, ctrl_lambda_->to<mrs_real>());
		
		//Apply BIC criteria
		if(BICdist_ > 0.0)
		{
			//BIC validated the change point!

			//reset current model, because we will now start a new one
			// (we could also store these models for future use - e.g. clustering)
			QGMMmodel_.resetModel();

			//do something to mark the detected change POINT:
			// - print a message
			// - write to a file
			// - generate a sound (e.g. a beep)
			// - ...
			mrs_real confidence = 1.0 - dynThres_/dist12_;
			cout << "Change detected, with confidence = " << confidence << endl; //get a timestamp!!!!!!
		}
		else
		{
			//BIC rejected potential change point
			//do something here? Probably not...
		}
	}
}








