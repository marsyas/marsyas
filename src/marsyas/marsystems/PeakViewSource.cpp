/*
** Copyright (C) 1998-2010 George Tzanetakis <gtzan@cs.uvic.ca>
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

#include "PeakViewSource.h"
#include <marsyas/peakView.h>

using std::ostringstream;
using namespace Marsyas;

PeakViewSource::PeakViewSource(mrs_string name):MarSystem("PeakViewSource", name)
{
  addControls();
  filename_ = "EMPTY_STRING";
  frameIdx_ = 0;
  numFrames_ = 0;
  frameSize_ = 0;
}

PeakViewSource::PeakViewSource(const PeakViewSource& a) : MarSystem(a)
{
  ctrl_filename_ = getctrl("mrs_string/filename");
  ctrl_pos_= getctrl("mrs_natural/pos");
  ctrl_size_ = getctrl("mrs_natural/size");
  ctrl_hasData_ = getctrl("mrs_bool/hasData");
  ctrl_frameMaxNumPeaks_ = getctrl("mrs_natural/frameMaxNumPeaks");
  ctrl_totalNumPeaks_ = getctrl("mrs_natural/totalNumPeaks");
  ctrl_nTimes_ = getctrl("mrs_natural/nTimes");
  ctrl_ignGroups_ = getctrl("mrs_bool/ignoreGroups");
  ctrl_noNegativeGroups_ = getctrl("mrs_bool/discardNegativeGroups");

  filename_ = a.filename_;
  frameIdx_ = a.frameIdx_;
  numFrames_ = a.numFrames_;
  frameSize_ = a.frameSize_;
}

PeakViewSource::~PeakViewSource()
{
}

MarSystem*
PeakViewSource::clone() const
{
  return new PeakViewSource(*this);
}

void
PeakViewSource::addControls()
{
  addctrl("mrs_string/filename", "", ctrl_filename_);
  setctrlState("mrs_string/filename", true);

  addctrl("mrs_natural/frameMaxNumPeaks", 0, ctrl_frameMaxNumPeaks_);
  addctrl("mrs_natural/totalNumPeaks", 0, ctrl_totalNumPeaks_);
  addctrl("mrs_natural/nTimes", 1, ctrl_nTimes_);

  addctrl("mrs_bool/hasData", false, ctrl_hasData_);
  addctrl("mrs_natural/size", 0, ctrl_size_);
  addctrl("mrs_natural/pos", 0, ctrl_pos_);

  addControl("mrs_bool/ignoreGroups", false, ctrl_ignGroups_);
  addControl("mrs_bool/discardNegativeGroups", false, ctrl_noNegativeGroups_);

}

void
PeakViewSource::defaultConfig()
{
  ctrl_onSamples_->setValue(1, NOUPDATE);
  ctrl_onObservations_->setValue(0, NOUPDATE);
  ctrl_osrate_->setValue(0.0, NOUPDATE);
  ctrl_onObsNames_->setValue(",", NOUPDATE);

  ctrl_pos_->setValue(0, NOUPDATE);
  ctrl_size_->setValue(0, NOUPDATE);
  ctrl_hasData_->setValue(false, NOUPDATE);
  peakData_.create(0);
  frameIdx_ = 0;
  numFrames_ = 0;
}

void
PeakViewSource::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)
  //check for a new filename (this is also true for the first call to myUpdate())
  // hack: disabled this so other control can be updated after setting the filename
  //if(ctrl_filename_->to<mrs_string>() != filename_)
  {
    //check if this is the first call to myUpdate()
    if(ctrl_filename_->to<mrs_string>().empty())
    {
      defaultConfig();
    }
    else //not the first call to myUpdate()
    {
      //try to read entire file into memory [!]
      peakView peakDataView(peakData_);
      if(peakDataView.peakRead(ctrl_filename_->to<mrs_string>()))
      {
        numFrames_ = peakDataView.getNumFrames();
        frameSize_ = peakDataView.getFrameSize();

        ctrl_frameMaxNumPeaks_->setValue (peakDataView.getFrameMaxNumPeaks());
        //mrs_natural frameMaxNumPeaks = peakDataView.getFrameMaxNumPeaks();

        ctrl_onSamples_->setValue(ctrl_nTimes_->to<mrs_natural>(), NOUPDATE);
        ctrl_onObservations_->setValue(ctrl_frameMaxNumPeaks_->to<mrs_natural>() * peakView::nbPkParameters, NOUPDATE);
        ctrl_osrate_->setValue(peakDataView.getFs(), NOUPDATE);
        ostringstream oss;
        for(mrs_natural j=0; j< peakView::nbPkParameters; ++j) //j = param index
        {
          for (mrs_natural i=0; i < ctrl_frameMaxNumPeaks_->to<mrs_natural>(); ++i) //i = peak index
            oss << peakView::getParamName(j) << "_" << i+j*ctrl_frameMaxNumPeaks_->to<mrs_natural>() << ",";
        }
        ctrl_onObsNames_->setValue(oss.str(), NOUPDATE);

        filename_ = ctrl_filename_->to<mrs_string>();
        frameIdx_ = 0;
        ctrl_size_->setValue(numFrames_*frameSize_, NOUPDATE);
        ctrl_pos_->setValue(0, NOUPDATE);
        ctrl_hasData_->setValue(true, NOUPDATE);
      }
      else //failed to load the file
      {
        MRSERR("PeakViewSource::myUpdate() : error opening file: " << filename_);
        defaultConfig();
      }
    }
  }

  //[TODO]: Rewind? Reset? Done? [!]
  //[TODO]: read directly from file and not load entire .peak file into memory
  //[TODO]: allow changing pos control
}

void
PeakViewSource::myProcess(realvec& in, realvec& out)
{
  (void) in;

  mrs_natural totalNumPeaks	= 0,
              frameMaxNumPeaks = ctrl_frameMaxNumPeaks_->to<mrs_natural>();
  mrs_natural nTimes			= ctrl_nTimes_->to<mrs_natural>();
  mrs_natural numRows			= peakData_.getRows();
  const mrs_bool ignoreGroups = 	ctrl_ignGroups_->to<mrs_bool>();


  for (mrs_natural f = 0; f < nTimes; f++)
  {
    //at each tick, output peaks for corresponding frame
    if(ctrl_hasData_->isTrue())
    {
      ctrl_pos_->setValue(frameIdx_*frameSize_);

      for(mrs_natural o=0; o < numRows; ++o)
      {
        mrs_real currData = peakData_(o, frameIdx_);
        out(o,f) = currData;
        if (o / frameMaxNumPeaks == peakView::pkFrequency)
          if (currData != 0)
            totalNumPeaks++;
        if (ignoreGroups && (o / frameMaxNumPeaks == peakView::pkGroup))
          if (currData < 0)
            out(o, f) = 0;

      }

      frameIdx_++;
      if(frameIdx_ == numFrames_)//if EOF
        ctrl_hasData_->setValue(false);
    }
  }
  if (ctrl_noNegativeGroups_->to<mrs_bool>())
  {
    discardNegativeGroups (out);
    totalNumPeaks = peakView(out).getTotalNumPeaks ();
  }

  ctrl_totalNumPeaks_->setValue(totalNumPeaks);
}

void PeakViewSource::discardNegativeGroups (mrs_realvec &output)
{
  peakView Out(output);
  mrs_natural numFrames  = Out.getNumFrames ();
  for (mrs_natural f = numFrames-1; f >= 0 ; f--)
  {
    mrs_natural frameNumPeaks	= Out.getFrameNumPeaks ();

    for (mrs_natural i = frameNumPeaks-1; i >= 0; i--)
    {
      if (Out(i,peakView::pkGroup, f) < 0)
        Out.removePeak(i, f);
    }
  }
}
