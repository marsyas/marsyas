/*
** Copyright (C) 1998-2006 George Tzanetakis <gtzan@cs.princeton.edu>
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

#include "KNNClassifier.h"
#include "../common_source.h"

using std::ostringstream;
using namespace Marsyas;

KNNClassifier::KNNClassifier(mrs_string name):MarSystem("KNNClassifier",name)
{
  prev_mode_ = "predict";
  addControls();
}


KNNClassifier::~KNNClassifier()
{
}


MarSystem*
KNNClassifier::clone() const
{
  return new KNNClassifier(*this);
}

void
KNNClassifier::addControls()
{
  addctrl("mrs_string/mode", "train");
  addctrl("mrs_natural/nLabels", 1);
  setctrlState("mrs_natural/nLabels", true);
  trainSet_.create((mrs_natural)1,(mrs_natural)1);
  addctrl("mrs_natural/grow", 1);
  addctrl("mrs_natural/k", 1);
  k_ = 1;
  addctrl("mrs_realvec/trainSet", trainSet_);
  addctrl("mrs_natural/nPoints", 0);
  addctrl("mrs_bool/done", false);
  addctrl("mrs_natural/nPredictions", 1);
  setctrlState("mrs_natural/nPredictions", true);
  setctrlState("mrs_bool/done", true);
}


void
KNNClassifier::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)
  MRSDIAG("KNNClassifier.cpp - KNNClassifier:myUpdate");

  nPredictions_ = getctrl("mrs_natural/nPredictions")->to<mrs_natural>();
  setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples"));
  setctrl("mrs_natural/onObservations", (mrs_natural) nPredictions_ + 1);
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));

  inObservations_ = getctrl("mrs_natural/inObservations")->to<mrs_natural>();
  grow_ = getctrl("mrs_natural/grow")->to<mrs_natural>();
  nPoints_ = getctrl("mrs_natural/nPoints")->to<mrs_natural>();
  k_ = getctrl("mrs_natural/k")->to<mrs_natural>();
  mrs_string mode = getctrl("mrs_string/mode")->to<mrs_string>();

  if (mode == "train")
  {
    if (inObservations_ != trainSet_.getCols())
    {
      trainSet_.stretch(1, getctrl("mrs_natural/inObservations")->to<mrs_natural>());
      setctrl("mrs_realvec/trainSet", trainSet_);
    }
  }



  if (mode == "predict")
  {
    trainSet_.create(getctrl("mrs_realvec/trainSet")->to<mrs_realvec>().getRows(),
                     getctrl("mrs_realvec/trainSet")->to<mrs_realvec>().getCols());
    trainSet_ = getctrl("mrs_realvec/trainSet")->to<mrs_realvec>();
  }


  if (getctrl("mrs_bool/done")->to<mrs_bool>())
  {
    setctrl("mrs_bool/done", false);
    setctrl("mrs_realvec/trainSet", trainSet_);
  }
}


void
KNNClassifier::myProcess(realvec& in, realvec& out)
{
  //checkFlow(in,out);

  mrs_real v;
  mrs_string mode = getctrl("mrs_string/mode")->to<mrs_string>();
  mrs_real label;
  mrs_natural nlabels = getctrl("mrs_natural/nLabels")->to<mrs_natural>();
  mrs_natural prediction;
  int x, y;
  int p;
  mrs_natural o,t;

  if ((prev_mode_ == "predict")&&(mode == "train"))
  {

    // reset
    for (p = 0; p < nPoints_; p++)
    {
      for (o=0; o < inObservations_-1; o++)
        trainSet_(p,o) = 0.0;
    }
    nPoints_ = 0;
  }


  if (mode == "train")
  {
    for (t = 0; t < inSamples_; t++)
    {
      label = in(inObservations_-1, t);

      if (nPoints_ == grow_)
      {

        // expontentially stretch trainSet_ realvec
        grow_ = 2*grow_;
        trainSet_.stretch(grow_, inObservations_);
        updControl("mrs_natural/grow", grow_);
      }

      for (o=0; o < inObservations_; o++)
      {
        // store all observations for instance t
        trainSet_(nPoints_,o) = in(o,t);
      }
      out(0,t) = label;
      out(1,t) = label;

      // update number of points
      nPoints_= nPoints_ +1;
      updControl("mrs_natural/nPoints", nPoints_);
    }
  }



  if (mode == "predict")
  {

    // Calculate Distances for each Point
    for (t = 0; t < inSamples_; t++)
    {
      label = in(inObservations_-1, t);

      realvec Distance;
      Distance.create(nPoints_);

      realvec kMin;
      kMin.create(k_,2);

      realvec kSmallest;
      kSmallest.create(nlabels);

      for (p = 0; p < nPoints_; p++)
      {
        mrs_real sum = 0;
        for (o=0; o < inObservations_-1; o++)
        {
          v = in(o,t);
          v = (v - trainSet_(p,o));
          sum += v*v;
        }
        Distance(p) = sum;
      }


      // Find k smallest distances

      // initialize kMin RealVec
      mrs_real kmaxV = Distance(0); // max value initialization
      int kmaxI = 0; // max Index initialization

      for (x=0; x < k_; x++)
      {
        kMin(x, 0) = Distance(0); // Distance Value
        kMin(x, 1) = 0; // Label
      }


      for (y=0; y < nPoints_; y++)
      {

        if (Distance(y) < kmaxV)
        {
          mrs_real kmaxV_t = 0.0;
          int kmaxI_t = 1;

          kMin(kmaxI,0) = Distance(y); // value
          kMin(kmaxI,1) = trainSet_(y, inObservations_-1); // label

          // Now find Max Value in kMin RealVec
          for (x=0; x < k_; x++)
          {
            kmaxV_t = kMin(0,0);
            kmaxI_t = 0;
            if (kMin(x,0) > kmaxV_t)
            {
              kmaxV_t = kMin(x,0);
              kmaxI_t = x;
            }
          }
          kmaxV = kmaxV_t;
          kmaxI = kmaxI_t;
        }
      }


      // Now find biggest number of values for label in kMin
      for (x=0; x< k_; x++)
      {
        // based on the label of the min point, incement kSmallest
        kSmallest((int)kMin(x, 1))++;
      }


      // find largest value in kSmallest
      mrs_real max = kSmallest(0);
      int maxI = 0;
      for (x=0; x<nlabels; x++)
      {
        if (kSmallest(x) > max)
        {
          max = kSmallest(x);
          maxI = x;
        }
      }
      prediction = maxI;
      out(0,t) = (mrs_real)prediction;
      if (nPredictions_ >= 1)
        for (x=0; x < nPredictions_; x++)
          out(x,t) = kMin(x,1);

      out(onObservations_-1,t) = label;
    }
  }



  prev_mode_ = mode;
}










