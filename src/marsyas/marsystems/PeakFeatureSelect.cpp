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

#include "PeakFeatureSelect.h"
#include <marsyas/peakView.h>

using std::ostringstream;
using namespace Marsyas;

PeakFeatureSelect::PeakFeatureSelect(mrs_string name):MarSystem("PeakFeatureSelect", name)
{
  addControls();
}

PeakFeatureSelect::PeakFeatureSelect(const PeakFeatureSelect& a) : MarSystem(a)
{
  ctrl_selectedFeatures_ = getctrl("mrs_natural/selectedFeatures");
  ctrl_totalNumPeaks_ = getctrl("mrs_natural/totalNumPeaks");
  ctrl_frameMaxNumPeaks_ = getctrl("mrs_natural/frameMaxNumPeaks");

  selectedFeatures_ = 0;
  frameMaxNumPeaks_ = 0;
  numFeats_ = 0;
}

PeakFeatureSelect::~PeakFeatureSelect()
{
}

MarSystem*
PeakFeatureSelect::clone() const
{
  return new PeakFeatureSelect(*this);
}

void
PeakFeatureSelect::addControls()
{
  addctrl("mrs_natural/selectedFeatures", 0, ctrl_selectedFeatures_);
  ctrl_selectedFeatures_->setState(true);

  addctrl("mrs_natural/totalNumPeaks", 0, ctrl_totalNumPeaks_);
  ctrl_totalNumPeaks_->setState(true);

  addctrl("mrs_natural/frameMaxNumPeaks", 0, ctrl_frameMaxNumPeaks_);
  ctrl_frameMaxNumPeaks_->setState(true);

  selectedFeatures_ = 0;
  frameMaxNumPeaks_ = 0;

  numFeats_ = 0;
}

void
PeakFeatureSelect::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)
  if(ctrl_selectedFeatures_->to<mrs_natural>() != selectedFeatures_ ||
      ctrl_frameMaxNumPeaks_->to<mrs_natural>() != frameMaxNumPeaks_)
  {
    selectedFeatures_ = ctrl_selectedFeatures_->to<mrs_natural>();
    frameMaxNumPeaks_ = ctrl_frameMaxNumPeaks_->to<mrs_natural>();

    //determine the number of selected features to output, per peak
    numFeats_ = 0;
    ostringstream oss;
    if(selectedFeatures_ & PeakFeatureSelect::pkFrequency)
    {
      numFeats_++;
      oss << "pkFrequency,";
    }
    if(selectedFeatures_ & PeakFeatureSelect::pkAmplitude)
    {
      numFeats_++;
      oss << "pkAmplitude,";
    }
    if(selectedFeatures_ & PeakFeatureSelect::pkDeltaFrequency)
    {
      numFeats_++;
      oss << "pkDeltaFrequency,";
    }
    if(selectedFeatures_ & PeakFeatureSelect::pkDeltaAmplitude)
    {
      numFeats_++;
      oss << "pkDeltaAmplitude,";
    }
    if(selectedFeatures_ & PeakFeatureSelect::pkFrame)
    {
      numFeats_++;
      oss << "pkFrame,";
    }
    if(selectedFeatures_ & PeakFeatureSelect::pkPan)
    {
      numFeats_++;
      oss << "pkPan,";
    }
    //-----------------------------------------------------
    if(selectedFeatures_ & (PeakFeatureSelect::pkSetFrequencies |
                            PeakFeatureSelect::pkSetAmplitudes |
                            PeakFeatureSelect::pkSetFrames))
    {
      numFeats_++;
      oss << "frameNumPeaks,";
    }
    if(selectedFeatures_ & PeakFeatureSelect::pkSetFrequencies)
    {
      for(mrs_natural i = 0; i < frameMaxNumPeaks_; ++i)//These obsNames are incorrect when frameNumPeaks < maxFrameNumPeaks!! [TODO]
        oss << "pk_"<< i << "_Frequency,";
      numFeats_ += frameMaxNumPeaks_;
    }
    if(selectedFeatures_ & PeakFeatureSelect::pkSetAmplitudes)
    {
      for(mrs_natural i = 0; i < frameMaxNumPeaks_; ++i) //These obsNames are incorrect when frameNumPeaks < maxFrameNumPeaks!! [TODO]
        oss << "pk_"<< i << "_Amplitude,";
      numFeats_ += frameMaxNumPeaks_;
    }
    if(selectedFeatures_ & PeakFeatureSelect::pkSetFrames)
    {
      for(mrs_natural i = 0; i < frameMaxNumPeaks_; ++i) //These obsNames are incorrect when frameNumPeaks < maxFrameNumPeaks!! [TODO]
        oss << "pk_"<< i << "_Frame,";
      numFeats_ += frameMaxNumPeaks_;
    }
    if(numFeats_ == 0)
      oss << ",";

    ctrl_onObsNames_->setValue(oss.str(), NOUPDATE);
  }

  ctrl_onSamples_->setValue(ctrl_totalNumPeaks_->to<mrs_natural>(), NOUPDATE);
  ctrl_onObservations_->setValue(numFeats_, NOUPDATE);
  ctrl_osrate_->setValue(ctrl_israte_, NOUPDATE);
}

void
PeakFeatureSelect::myProcess(realvec& in, realvec& out)
{
  peakView inPeakView(in);

  //get the total number of peaks in input texture window,
  //and reconfigure output flow controls (i.e. call update())
  //ctrl_frameMaxNumPeaks_->setValue(inPeakView.getFrameMaxNumPeaks(), NOUPDATE);
  //ctrl_totalNumPeaks_->setValue(inPeakView.getTotalNumPeaks()); //update is called here!

  //if there is at least one peak at the input, and at least one selected feature
  //fill the output realvec (otherwise there is no need to do anything else)
  if(ctrl_totalNumPeaks_->to<mrs_natural>() > 0 && ctrl_selectedFeatures_->to<mrs_natural>() != 0)
  {
    //////////////////////////////////////////////////////////////////////////
    //fill the output realvec with the selected feature vector for each peak
    //////////////////////////////////////////////////////////////////////////
    mrs_natural peak_index = 0;
    for(mrs_natural f=0; f < inPeakView.getNumFrames(); ++f)//frame loop
    {
      mrs_natural frameNumPeaks = inPeakView.getFrameNumPeaks(f);
      for(mrs_natural p=0; p<frameNumPeaks; ++p)//iterate over peaks in this frame (if any)
      {
        mrs_natural feat_index = 0;

        if(selectedFeatures_ & PeakFeatureSelect::pkFrequency)
        {
          out(feat_index, peak_index) = inPeakView(p, peakView::pkFrequency, f);
          if(selectedFeatures_ & PeakFeatureSelect::barkPkFreq)
          {
            out(feat_index, peak_index) = hertz2bark(out(feat_index, peak_index));
          }
          feat_index++;
        }
        if(selectedFeatures_ & PeakFeatureSelect::pkAmplitude)
        {
          out(feat_index, peak_index) = inPeakView(p, peakView::pkAmplitude, f);
          if(selectedFeatures_ & PeakFeatureSelect::dBPkAmp)
          {
            out(feat_index, peak_index) = amplitude2dB(out(feat_index, peak_index));
          }
          feat_index++;
        }
        if(selectedFeatures_ & PeakFeatureSelect::pkDeltaFrequency)
        {
          out(feat_index, peak_index) = inPeakView(p, peakView::pkDeltaFrequency, f);
          if(selectedFeatures_ & PeakFeatureSelect::barkPkFreq)
          {
            out(feat_index, peak_index) = hertz2bark(out(feat_index, peak_index));
          }
          feat_index++;
        }
        if(selectedFeatures_ & PeakFeatureSelect::pkDeltaAmplitude)
        {
          out(feat_index, peak_index) = inPeakView(p, peakView::pkDeltaAmplitude	, f);
          if(selectedFeatures_ & PeakFeatureSelect::dBPkAmp)
          {
            out(feat_index, peak_index) = amplitude2dB(out(feat_index, peak_index));
          }
          feat_index++;
        }
        if(selectedFeatures_ & PeakFeatureSelect::pkFrame)
        {
          out(feat_index, peak_index) = inPeakView(p, peakView::pkFrame, f);
          feat_index++;
        }
        if(selectedFeatures_ & PeakFeatureSelect::pkPan)
        {
          out(feat_index, peak_index) = inPeakView(p, peakView::pkPan, f);
          feat_index++;
        }
        //-----------------------------------------------------------------------------
        if(selectedFeatures_ & (PeakFeatureSelect::pkSetFrequencies |
                                PeakFeatureSelect::pkSetAmplitudes |
                                PeakFeatureSelect::pkSetFrames))
        {
          out(feat_index, peak_index) = frameNumPeaks;
          feat_index++;
        }
        if(selectedFeatures_ & PeakFeatureSelect::pkSetFrequencies)
        {
          //for each peak, just get all the feats of the peaks in the same frame
          for(mrs_natural i=0; i < frameNumPeaks; ++i)
          {
            out(feat_index, peak_index) = inPeakView(i, peakView::pkFrequency, f);
            feat_index++;
          }
        }
        if(selectedFeatures_ & PeakFeatureSelect::pkSetAmplitudes)
        {
          //for each peak, just get all the feats of the peaks in the same frame
          for(mrs_natural i=0; i < frameNumPeaks; ++i)
          {
            out(feat_index, peak_index) = inPeakView(i, peakView::pkAmplitude, f);
            feat_index++;
          }
        }
        if(selectedFeatures_ & PeakFeatureSelect::pkSetFrames)
        {
          //for each peak, just get all the feats of the peaks in the same frame
          for(mrs_natural i=0; i < frameNumPeaks; ++i)
          {
            out(feat_index, peak_index) = inPeakView(i, peakView::pkFrame, f);
            feat_index++;
          }
        }
        //move on to next peak
        peak_index++;
      }
    }
  }
}
