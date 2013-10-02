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

#include "SMO.h"
#include "../common_source.h"

using namespace std;
using namespace Marsyas;

SMO::SMO(mrs_string name):MarSystem("SMO",name)
{
  //type_ = "SMO";
  //name_ = name;

  addControls();
}


SMO::~SMO()
{
}


MarSystem*
SMO::clone() const
{
  return new SMO(*this);
}

void
SMO::addControls()
{
  addctrl("mrs_string/mode", "train", modePtr_);
  addctrl("mrs_natural/nLabels", 1, nlabelsPtr_);
  setctrlState("mrs_natural/nLabels", true);
  weights_.create(1);
  addctrl("mrs_realvec/weights", weights_, weightsPtr_);
  addctrl("mrs_bool/done", false, donePtr_);
  setctrlState("mrs_bool/done", true);

}


void
SMO::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)
  MRSDIAG("SMO.cpp - SMO:myUpdate");

  ctrl_onSamples_->setValue(ctrl_inSamples_, NOUPDATE);
  ctrl_onObservations_->setValue(2, NOUPDATE);
  ctrl_osrate_->setValue(ctrl_israte_, NOUPDATE);


  mrs_natural inObservations = ctrl_inObservations_->to<mrs_natural>();
  // FIXME This variable is being defined but (possibly) not used.
  // mrs_natural nlabels = getctrl("mrs_natural/nLabels")->to<mrs_natural>();


  mrs_natural mcols = (getctrl("mrs_realvec/weights")->to<mrs_realvec>()).getCols();
  mrs_natural ncols = weights_.getCols();



  if (inObservations != mcols)
  {
    weights_.create(inObservations);
    updControl("mrs_realvec/weights", weights_);
  }


  if (inObservations != ncols)
  {
    weights_.create(inObservations);
  }

  mrs_string mode = getctrl("mrs_string/mode")->to<mrs_string>();
  if (mode == "predict")
  {
    weights_ = getctrl("mrs_realvec/weights")->to<mrs_realvec>();
  }
}


void
SMO::myProcess(realvec& in, realvec& out)
{
  mrs_natural t,o;
  mrs_string mode = modePtr_->to<mrs_string>();
  mrs_natural prediction = 0;
  mrs_real label;
  mrs_real thres;

  if (mode == "train")
  {
    for (t = 0; t < inSamples_; t++)
    {
      label = in(inObservations_-1, t);
      out(0,t) = (mrs_real) label;
      out(1,t) = (mrs_real) label;
    }

    weights_(0) = 0.4122;
    weights_(1) = -4.599;
    weights_(2) = -14.0203;
    weights_(3) = -6.2503;
    weights_(4) = -0.8447;
    weights_(5) = -2.0753;
    weights_(6) = 0.9826;
    weights_(7) = -4.1159;
    weights_(8) = -1.6985;
    weights_(9) = -1.1419;
    weights_(10) = 3.5605;
    weights_(11) = 1.9987;
    weights_(12) = 1.3641;
    weights_(13) = -6.412;
    weights_(14) = 7.7704;
    weights_(15) = 0.6565;
    weights_(16) = -0.3749;
    weights_(17) = -0.3507;
    weights_(18) = 2.5022;
    weights_(19) = 0.8658;
    weights_(20) = -2.6361;
    weights_(21) = 3.9029;
    weights_(22) = 0.4051;
    weights_(23) = -2.8185;
    weights_(24) = 2.4864;
    weights_(25) = -1.8054;
    weights_(26) = -2.7731;
    weights_(27) = 2.2423;
    weights_(28) = -2.1786;
    weights_(29) = -1.0741;
    weights_(30) = -0.5614;
    weights_(31) = -3.5967;
    weights_(32) = 7.7832;




    /* weights_(0) = -1.252;
    weights_(1) = 6.796;
    weights_(2) = -3.9419;
    weights_(3) = 2.3463;
    weights_(4) =  -3.6959;
    weights_(5) =  -4.5353;
    weights_(6) =  -3.5343;
    weights_(7) = 0.0114;
    weights_(8) = -5.0538;
    weights_(9) = -2.0138;
    weights_(10) = -1.8438;
    weights_(11) = 3.16;
    weights_(12) = 2.1316 ;
    weights_(13) =       1.6142 ;
    weights_(14) =      -4.4765 ;
    weights_(15) =       7.5799 ;
    weights_(16) =       0.9734 ;
    weights_(17) =       0.5425 ;
    weights_(18) =      -0.9018 ;
    weights_(19) =      -0.1296 ;
    weights_(20) =      -1.1898 ;
    weights_(21) =       2.7628 ;
    weights_(22) =      -2.7207 ;
    weights_(23) =       3.5209 ;
    weights_(24) =       0.8888 ;
    weights_(25) =      -3.8638 ;
    weights_(26) =       2.8184 ;
    weights_(27) =      -2.656  ;
    weights_(28) =      -2.7921 ;
    weights_(29) =       1.8606 ;
    weights_(30) =      -2.5113 ;
    weights_(31) =      -1.3537 ;
    weights_(32) =      -1.1434 ;
    weights_(33) =      -4.1955 ;
    weights_(34) = 3.9084;
    */

    /* weights_(0) = 0.9822;
    weights_(1) = 0.0614;
    weights_(2) = 1.74;
    weights_(3) = -1.0346;
    weights_(4) = 0.8395;
    weights_(5) = 1.2181;
    weights_(6) = 0.2218;
    weights_(7) = 1.092;
    weights_(8) = 0.2186;
    weights_(9) = -0.1633;
    weights_(10) = - 1.4334;
    */



    /*ights_(0) = 6.3694;
    weights_(1) = 1.3558;
    weights_(2) = -1.3705;
    weights_(3) = -7.7337;
    weights_(4) = 5.7892;
    weights_(5) = 2.2434;
    weights_(6) = 4.9089;
    weights_(7) = 2.2042;
    weights_(8) = -2.0502;
    */


  }



  if (mode == "predict")
  {
    for (t = 0; t < inSamples_; t++)
    {
      label = in(inObservations_-1, t);
      thres = 0.0;
      for (o = 0; o < inObservations_-1; o++)
      {

        thres += (weights_(o) * in(o,t));
      }
      thres += weights_(inObservations_-1);

      if (thres <= 0 )
      {
        prediction = 0;
      }

      else
      {
        prediction = 1;
      }

      out(0,t) = (mrs_real) prediction;
      out(1,t) = (mrs_real) label;
    }


  }

  if (donePtr_->to<mrs_bool>())
  {
    updControl(weightsPtr_, weights_);
  }


}












