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

#include "PeakerOnset.h"
#include "../common_source.h"

using namespace std;
using namespace Marsyas;

PeakerOnset::PeakerOnset(mrs_string name):MarSystem("PeakerOnset", name)
{
  addControls();

  prevValue_ = 0.0;
  t_ = 0;
}

PeakerOnset::PeakerOnset(const PeakerOnset& a) : MarSystem(a)
{
  ctrl_lookAheadSamples_ = getctrl("mrs_natural/lookAheadSamples");
  ctrl_threshold_ = getctrl("mrs_real/threshold");
  ctrl_onsetDetected_ = getctrl("mrs_bool/onsetDetected");
  ctrl_confidence_ = getctrl("mrs_real/confidence");

  prevValue_ = a.prevValue_;
  t_ = a.t_;
}

PeakerOnset::~PeakerOnset()
{
}

MarSystem*
PeakerOnset::clone() const
{
  return new PeakerOnset(*this);
}

void
PeakerOnset::addControls()
{
  addctrl("mrs_natural/lookAheadSamples", 0, ctrl_lookAheadSamples_);
  addctrl("mrs_real/threshold", 0.0, ctrl_threshold_);
  addctrl("mrs_bool/onsetDetected", false, ctrl_onsetDetected_);
  addctrl("mrs_real/confidence", 0.0, ctrl_confidence_);
}

void
PeakerOnset::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)
  MRSDIAG("PeakerOnset.cpp - PeakerOnset:myUpdate");

  ctrl_onSamples_->setValue(1, NOUPDATE);
  if(inObservations_ > 1)
  {
    MRSWARN("PeakerOnset::myUpdate() - inObservations is bigget than 1. This MarSystem only takes the first observation into consideration...");
  }
  ctrl_onObservations_->setValue(1, NOUPDATE);
  ctrl_osrate_->setValue(ctrl_israte_, NOUPDATE);
  ctrl_onObsNames_->setValue("onset_confidence");

  if(inSamples_ < 1 + 2 * ctrl_lookAheadSamples_->to<mrs_natural>())
  {
    cout << "inSamples_ = " << inSamples_ << endl;
    cout << "lookAhead = " << ctrl_lookAheadSamples_->to<mrs_natural>() << endl;
    MRSWARN("PeakerOnset::myUpdate() - inSamples is too small for specified onsetWinSize: onset detection not possible to be performed!");
    ctrl_lookAheadSamples_->setValue(0, NOUPDATE);
  }
}

void
PeakerOnset::myProcess(realvec& in, realvec& out)
{
  mrs_natural o,t;
  (void) o;
  ctrl_onsetDetected_->setValue(false);
  ctrl_confidence_->setValue(0.0);
  out.setval(0.0);

  mrs_natural w = ctrl_lookAheadSamples_->to<mrs_natural>();

  if(w == 0)
    return;

  //point to check for an onset
  mrs_natural checkPoint = inSamples_-1-w;
  mrs_real checkPointValue = in(checkPoint);
  mrs_bool isOnset = true;

  //check first condition
  mrs_natural interval = mrs_natural(2.0/3.0*w);
  //for (t = inSamples_-1; t >= inSamples_-1-2*w ; t--)
  for(t=checkPoint-interval; t <= checkPoint+interval; t++)
  {
    if(checkPointValue < in(t))
    {
      isOnset = false;
      MRSDIAG("PeakerOnset::myProcess() - Failed 1st condition!");
      break;
    }
  }

// 	//new check proposed by Fabien Gouyon
// 	mrs_real ww = w/2;
// 	mrs_real maxVal = MINREAL;
// 	for(t = inSamples_-1-ww; t > checkPoint; t--)
// 	{
// 		if(in(t) > maxVal)
// 		{
// 			maxVal = in(t);
// 		}
// 		else
// 		{
// 			isOnset = false;
// 			//cout << "failed 1st condition!" << endl;
// 			break;
// 		}
// 	}
// 	maxVal = MINREAL;
// 	for(t = inSamples_-1-2*ww; t < checkPoint; t++)
// 	{
// 		if(in(t) > maxVal)
// 		{
// 			maxVal = in(t);
// 		}
// 		else
// 		{
// 			isOnset = false;
// 			//cout << "failed 1st condition!" << endl;
// 			break;
// 		}
// 	}

  /* Last version (by lgmartins) -> corrected (below) for not being strict to the window size defined by the precede ShiftInput
  	//check second condition
  	mrs_real m = 0.0;
  	for(t=0; t < inSamples_; t++)
  		m += in(t);
  	m /= inSamples_;
  */

  mrs_natural mul = 3; //multiplier proposed in Dixon2006
  mrs_real m = 0.0;
  for(t=checkPoint-(mul*w); t < inSamples_; t++)
    m += in(t);
  m /= (w*4+1);

  //checkPoint value should be higher than the window mean and mean should
  //be a significant value (otherwise we most probably are in a silence segment,
  //and we do not want to detect onsets on segments!)
  if(checkPointValue <= (m * ctrl_threshold_->to<mrs_real>()) || m < 10e-20)
  {
    isOnset = false;
    MRSDIAG("PeakerOnset::myProcess() - Failed 2nd condition!");
  }

  //third condition from Dixon2006 (DAFx paper) is not implemented
  //since it was found on that paper that its impact is minimal...

  if(isOnset)
  {
    ctrl_onsetDetected_->setValue(true);
    //ctrl_confidence_->setValue(1.0); //[!] must still find a way to output a confidence...
    ctrl_confidence_->setValue(checkPointValue/100.0); // ad-hoc value which should still have more meaning than a pure 1.0 vs. 0.0.
    out.setval(1.0);
    MRSDIAG("PeakerOnset::myProcess() - Onset Detected!");
  }

  //used for toy_with_onsets.m (DO NOT DELETE! - COMMENT INSTEAD)

  //t_++;

  //if(t_ == 0)
  //	MATLAB_PUT(in, "PeakerOnset_inWIN");
  //MATLAB_PUT(in, "PeakerOnset_in");
  //if(t_ <= 431)
  //	MATLAB_EVAL("PK_TS2 = [PK_TS2 PeakerOnset_in]");
  //MATLAB_EVAL("plot(PK_TS, 'r');");
  //MATLAB_EVAL("plot(FluxTS); hold on; plot(PK_TS, 'r');");
  //MATLAB_EVAL("plot(PeakerOnset_in,'r');hold on; plot(ShiftInput_out); hold off");
  //MATLAB_PUT(out,"PeakerOnset_out");
  //MATLAB_EVAL("onsetTS = [onsetTS, PeakerOnset_out];");
}
