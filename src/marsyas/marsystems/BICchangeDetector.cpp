/*
** Copyright (C) 1998-2011 George Tzanetakis <gtzan@cs.uvic.ca>
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

#include "BICchangeDetector.h"
#include "Memory.h"
#include <marsyas/NumericLib.h>


using std::ostringstream;
using std::cout;
using std::endl;

using namespace Marsyas;

/**
    \brief default constructor

    \param name - the name of the change detector
    \return a BICchangeDetector
*/
BICchangeDetector::BICchangeDetector(mrs_string name):MarSystem("BICchangeDetector", name)
{
  prevDists_ = new Memory("cirMem");
  BICTick_ = 0;
  nfeats_ = 0;
  addControls();
  pdists_.create(nrPrevDists_);
}

/**
    \brief copy constructor

    \param a - the detector to be copied
    \return a BICchangeDetector
*/
BICchangeDetector::BICchangeDetector(const BICchangeDetector& a) : MarSystem(a)
{
  prevDists_ = new Memory(*(a.prevDists_));

  BICTick_ = 0;

  nfeats_ = 0;
  ctrl_reset_ = getctrl("mrs_bool/reset");
  ctrl_alpha1_= getctrl("mrs_real/alpha1");
  ctrl_lambda_= getctrl("mrs_real/lambda");
  ctrl_prevDists_= getctrl("mrs_natural/prevDists");
  ctrl_hopMS_ = getctrl("mrs_natural/hopMillis");
  nrPrevDists_ = ctrl_prevDists_->to<mrs_natural>();
  pdists_.create(nrPrevDists_);
  prev_change_time_ = 0.0;
}

BICchangeDetector::~BICchangeDetector()
{
  delete prevDists_;
}

MarSystem*

/**
    \brief clones using the copy constructor

    \return a copy of the BICchangeDetector
*/
BICchangeDetector::clone() const
{
  return new BICchangeDetector(*this);
}

/**
    \brief adds relevant controls

    add the following controls:
    mrs_bool/reset
    mrs_real/alpha1
    mrs_real/lambda

    a side effect is that the dynamic threshold and previous distances are reset.
*/
void
BICchangeDetector::addControls()
{
  addctrl("mrs_bool/reset", true, ctrl_reset_);
  setctrlState(ctrl_reset_, true);

  addctrl("mrs_real/alpha1", 0.4, ctrl_alpha1_);
  addctrl("mrs_real/lambda", 0.6, ctrl_lambda_);
  addctrl("mrs_natural/prevDists",3,ctrl_prevDists_);
  addctrl("mrs_natural/hopMillis",16,ctrl_hopMS_);

  dynThres_ = 0.0;
  prevDists_->updControl("mrs_natural/inSamples", 1);
  prevDists_->updControl("mrs_natural/inObservations", 1);
  nrPrevDists_ = getctrl("mrs_natural/prevDists")->to<mrs_natural>();
  prevDists_->updControl("mrs_natural/memSize", nrPrevDists_); //store 3 previous distances, for dynamic thresholding
  prev_change_time_ = 0.0;

}

/**
    \brief updates the controls

    segFrames_, nfeats_, and segHop_ are updated automatically to
    2/5 * inSamples, inObservations, and 1/5 * inSamples respectively

    If reset is true then the quasi-GMM and previous distances are reset.

    BICchangeDetector must receive as input a vector of feature frames as depicted bellow,
    which includes 4 speech sub-segments (C1, C2, C3, C4) that will be used for detecting
    speaker changes:
    |------------------------|
	C1        C2
    |----+----|----+----|
	|----+----|----+----|
	C3        C4
    |--->|
    hop

    For now, hop is set fixed as 1/5 of inSamples [!]

*/
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
  if(segFrames_ != ctrl_inSamples_->to<mrs_natural>()*2/5 ||
      nfeats_ != ctrl_inObservations_->to<mrs_natural>()) //hardcoded [!]
  {
    segFrames_ = ctrl_inSamples_->to<mrs_natural>()*2/5; // hardcoded [!]
    segHop_ = ctrl_inSamples_->to<mrs_natural>()*1/5; // hardcoded [!]


    hopSeconds_ = 0.001*segHop_*(mrs_real)ctrl_hopMS_->to<mrs_natural>();
    nfeats_ = ctrl_inObservations_->to<mrs_natural>();
    /* C1_.allocate(nfeats, segFrames_);
       C2_.allocate(nfeats, segFrames_);
       C3_.allocate(nfeats, segFrames_);
       C4_.allocate(nfeats, segFrames_);
    */
    // there is no allocate anymore in realvec
    // not sure how it compiled. Gustavo ?

    // cooplogic - this shouldn't be needed since it is
    //  done every time in myProcess
    // 		C1_.create(nfeats_, segFrames_);
    // 		C2_.create(nfeats_, segFrames_);
    // 		C3_.create(nfeats_, segFrames_);
    // 		C4_.create(nfeats_, segFrames_);

  }

  if(ctrl_reset_->to<bool>())
  {
    QGMMmodel_.resetModel();
    prevDists_->updControl("mrs_bool/reset", true);
    pdists_.setval(0.0);
    pIndex_ = 0;
    ctrl_reset_->setValue(false, NOUPDATE);
  }
}

void
BICchangeDetector::myProcess(realvec& in, realvec& out)
{

  // skip initial hops that have zeroes
  if (BICTick_ < 5)
  {
    BICTick_ ++;
    return;
  }




  mrs_natural o,t;
  // [!note!] if CX_ matrices are reused they need to be resized since
  // they meanwhile were assigned to covariance matrices (10x10)
  C1_.create(nfeats_, segFrames_);
  C2_.create(nfeats_, segFrames_);
  C3_.create(nfeats_, segFrames_);
  C4_.create(nfeats_, segFrames_);


  for(o=0; o < inObservations_; ++o)
  {
    //get segments => use pointers to "in" instead of copies?! [!]
    for(t=0; t < segFrames_ ; ++t)
    {
      C1_(o, t) = in(o, t);
      C2_(o, t) = in(o, t + segFrames_);
      C3_(o, t) = in(o, t + segHop_);
      C4_(o, t) = in(o, t + segHop_ + segFrames_);
    }

    //bypass input to output unchanged [!]
    for(t=0; t < inSamples_; ++t)
      out(o, t) = in(o, t);
  }





  //calculate covariance matrix for each segment
  realvec tmp;

  C1_.covariance(tmp);
  C1_ = tmp;

  C2_.covariance(tmp);
  C2_ = tmp;
  C3_.covariance(tmp);
  C3_ = tmp;
  C4_.covariance(tmp);
  C4_ = tmp;



  //update current qGMM model, using the first sub-segment data, C1_
  QGMMmodel_.updateModel(C1_, segFrames_);

  //calculate divergenceShape between sub-segment pairs
  dist12_ = NumericLib::divergenceShape(C1_, C2_);
  dist34_ = NumericLib::divergenceShape(C3_, C4_);

  //calculate bhattacharyyaShape between sub-segment pairs => should be an option! [!]
  //mrs_real dist12 = realvec::bhattacharyyaShape(C1_, C2_);
  //mrs_real dist34 = realvec::bhattacharyyaShape(C3_, C4_);

  //calculate dynamic threshold, using the distance values from
  //the previously stored sub-segment pairs
  // (NOTE:this implementation is slightly different from the one used in marsyas0.1
  //       implementation, where the mean calculation was performed only on the values
  //       actually sent to the circular buffer (i.e. avoids calculating mean of an
  //       "empty" circular buffer, as will happen here in the first process() calls...)
  //	pdists_ = prevDists_->getctrl("mrs_realvec/processedData")->to<mrs_realvec>(); // processedData: does this work?!?!?!?!?! [?]



  dynThres_ = pdists_.mean() * ctrl_alpha1_->to<mrs_real>();



  // get the values for the previous (i.e. left) and next (i.e. right) distances (in time)
  // so we can later check if the current distance is a local maximum (i.e. a peak)
  //	mrs_real distanceLeft = pdists_(pdists_.getSize()-1); //i.e. the previous distance value
  mrs_real distanceLeft = pdists_(pIndex_); //i.e. the previous distance value
  pIndex_ = (pIndex_ + 1) % nrPrevDists_;

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
  //		time_t currTime = ((mrs_real)BICTick_)*0.675;
  //time_t currTime = ((mrs_real)BICTick_-2)*hopSeconds_; // debug only


  mrs_real change_time = ((mrs_real)BICTick_-2) * hopSeconds_;


  //tm * currTm = gmtime(&currTime);  // for debug only


  if (dist12_ > distanceRight && dist12_ > distanceLeft && dist12_ > dynThres_)
  {

    mrs_real confidence = 1.0 - dynThres_/dist12_;



    //if this a potential change point, validate it using BIC and the current model
    BICdist_ = QGMMmodel_.BICdistance(C2_, segFrames_, ctrl_lambda_->to<mrs_real>());


// 	cout> << name_ << ": Potential change, with confidence " << confidence
// 		 << " at " << currTm->tm_hour << "h::"
// 		 << currTm->tm_min << "m::"
// 		 << currTm->tm_sec << "s" << endl;





    //Apply BIC criteria
    if (BICdist_ > 0.0)
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
      // 			mrs_real confidence = 1.0 - dynThres_/dist12_;
      // cout << " confirmed!";

      if (confidence > 0.0)
      {

        cout  << prev_change_time_ << "\t" << change_time << "\t" << confidence << endl;
        prev_change_time_ = change_time;


// 		  cout  << prev_change_time_ << "\t" << change_time << "\t" << confidence << endl;
// 		  prev_change_time_ = change_time;
      }

    }
    else
    {
      // cout << " UNCONFIRMED.";
      //BIC rejected potential change point
      //do something here? Probably not...
    }
    // cout << endl;
  }
  // 	cout << "TESTING TICKS: "
  // 	     << " at " << currTm->tm_hour << "h::"
  // 	     << currTm->tm_min << "m::"
  // 	     << currTm->tm_sec << "s"
  // 	     << endl;


  ++BICTick_;

}
