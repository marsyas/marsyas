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

#include "WekaSink.h"
#include "../common_source.h"

using namespace std;
using namespace Marsyas;

WekaSink::WekaSink(mrs_string name) : MarSystem("WekaSink",name)
{
  mos_ = NULL;
  stabilizingTicks_ = 0;
  addControls();
}

WekaSink::~WekaSink()
{
  // Close the output stream if required.
  if (mos_ != NULL)
  {
    mos_->close();
    delete mos_;
  }
}

WekaSink::WekaSink(const WekaSink& a) : MarSystem(a)
{
  mos_ = NULL;
  stabilizingTicks_ = 0;

  ctrl_regression_ = getControl("mrs_bool/regression");
  ctrl_putHeader_ = getControl("mrs_bool/putHeader");
  ctrl_labelNames_ = getControl("mrs_string/labelNames");
  ctrl_nLabels_ = getControl("mrs_natural/nLabels");
  ctrl_precision_ = getControl("mrs_natural/precision");
  ctrl_downsample_ = getControl("mrs_natural/downsample");
  ctrl_filename_ = getControl("mrs_string/filename");
  ctrl_currentlyPlaying_ = getControl("mrs_string/currentlyPlaying");
  ctrl_inject_ = getControl("mrs_bool/inject");
  ctrl_injectComment_ = getControl("mrs_string/injectComment");
  ctrl_injectVector_ = getControl("mrs_realvec/injectVector");
  ctrl_onlyStable_ = getControl("mrs_bool/onlyStable");
  ctrl_resetStable_ = getControl("mrs_bool/resetStable");
}

MarSystem*
WekaSink::clone() const
{
  return new WekaSink(*this);
}

void
WekaSink::addControls()
{
  addctrl("mrs_natural/precision", 6, ctrl_precision_);
  setctrlState("mrs_natural/precision", true);
  addctrl("mrs_string/filename", "weka.arff", ctrl_filename_);
  setctrlState("mrs_string/filename", true);
  addctrl("mrs_natural/nLabels", 2, ctrl_nLabels_);
  addctrl("mrs_natural/downsample", 1, ctrl_downsample_);
  setctrlState("mrs_natural/downsample", true);
  addctrl("mrs_string/labelNames", "Music,Speech", ctrl_labelNames_);
  setctrlState("mrs_string/labelNames", true);


  addctrl("mrs_bool/regression", false, ctrl_regression_);
  addctrl("mrs_string/currentlyPlaying", "", ctrl_currentlyPlaying_);
  addctrl("mrs_bool/putHeader", false, ctrl_putHeader_);
  setctrlState(ctrl_putHeader_, true);
  addctrl("mrs_bool/inject", false, ctrl_inject_);
  setctrlState(ctrl_inject_, true);
  addctrl("mrs_string/injectComment", "", ctrl_injectComment_);
  setctrlState(ctrl_injectComment_, true);
  addctrl("mrs_realvec/injectVector", realvec(), ctrl_injectVector_);
  setctrlState(ctrl_injectVector_, true);

  addctrl("mrs_bool/onlyStable", false, ctrl_onlyStable_);
  setctrlState(ctrl_onlyStable_, true);
  addctrl("mrs_bool/resetStable", false, ctrl_resetStable_);
}

void
WekaSink::putHeader(mrs_string inObsNames)
{
  //updctrl(ctrl_putHeader_, false);
  ctrl_putHeader_->setValue(true);

  // Only write the header when we are dealing with a new file, i.e. when
  // the filename setting differs from the filename we were (previously)
  // writing to.
  if ((filename_ != ctrl_filename_->to<mrs_string>()))
  {
    // Close the previously used output file if needed and cleanup.
    if (mos_ != NULL)
    {
      mos_->close();
      delete mos_;
      // TODO: do something about this ugly hack.
      if (filename_ == "weka.arff")
      {
        remove(filename_.c_str());
      }
    }

    // Set the current filename to the new value.
    filename_ = ctrl_filename_->to<mrs_string>();

    // Open a new output stream.
    mos_ = new ofstream;
    mos_->open(filename_.c_str());

    // General header stuff.
    (*mos_) << "% Created by Marsyas" << endl;
    (*mos_) << "@relation " << filename_ << endl;

    // The number of attributes is one less than the number of input
    // observations because we assume the last observation is for the label?
    // TODO: why this assumption? What if a use case requires two labels per
    // feature vector or no labels?
    // There is no such assumption is the WEKA ARFF format anyway.
    mrs_natural nAttributes = ctrl_inObservations_->to<mrs_natural>() - 1;
    mrs_natural nLabels = ctrl_nLabels_->to<mrs_natural>();

    // Print the attribute names.
    // TODO: this is could be done way more elegant
    // (e.g. using a 'split()' or 'explode()' function).
    mrs_natural i;
    for (i =0; i < nAttributes; ++i)
    {
      mrs_string inObsName;
      mrs_string temp;
      inObsName = inObsNames.substr(0, inObsNames.find(","));
      temp = inObsNames.substr(inObsNames.find(",") + 1, inObsNames.length());
      inObsNames = temp;
      // TODO: what's the point of using an extra ostringstream here?
      ostringstream oss;
      // oss << "attribute" << i;
      (*mos_) << "@attribute " << inObsName << " real" << endl;
    }

    // The attribute for the label.
    if (!ctrl_regression_->isTrue())
    {
      (*mos_) << "@attribute output {";
      // TODO: this could be done way more elegant
      // (e.g. with a 'join()' or 'implode()' function).
      for (i=0; i < nLabels; ++i)
      {
        // TODO: what's the point of using an extra ostringstream here?
        ostringstream oss;
        // oss << "label" << i;
        oss << labelNames_[i];
        (*mos_) << oss.str();
        if (i < nLabels - 1)
        {
          (*mos_) << ",";
        }
        // (*mos_) << "@attribute output {music,speech}" << endl;
      }
      (*mos_) << "}" << endl;
    }
    else
    {
      (*mos_) << "@attribute output real" << endl;
    }

    // End of header, now we are ready for outputting the data.
    (*mos_) << "\n\n@data" << endl;
  }
}

void
WekaSink::myUpdate(MarControlPtr sender)
{
  MRSDIAG("WekaSink.cpp - WekaSink:myUpdate");

  MarSystem::myUpdate(sender);

  // (Re)build the list of label names.
  mrs_string labelNames = ctrl_labelNames_->to<mrs_string>();

  labelNames_.clear();
  // TODO: this could be done way more elegant
  // (e.g. by using a split() or explode() function).
  for (int i = 0; i < ctrl_nLabels_->to<mrs_natural>(); ++i)
  {
    mrs_string labelName;
    mrs_string temp;
    labelName = labelNames.substr(0, labelNames.find(","));
    temp = labelNames.substr(labelNames.find(",") + 1, labelNames.length());
    labelNames = temp;
    labelNames_.push_back(labelName);
  }

  downsample_ = ctrl_downsample_->to<mrs_natural>();
  ctrl_israte_->setValue(israte_ / downsample_, NOUPDATE);



  // If not muted: write the header with the observation names.
  if (!ctrl_mute_->isTrue())
  {
    mrs_string onObsNames = ctrl_onObsNames_->to<mrs_string>();
    putHeader(onObsNames);
  }




  // Optional injecting of additional data.
  // TODO: this should be refactored together with the printing
  // from WekaSink::myProcess().
  if (!ctrl_mute_->isTrue())
  {
    if (ctrl_inject_->isTrue())
    {
      (* mos_) << ctrl_injectComment_->to<mrs_string>() << endl;
      (* mos_) << "% srate " << israte_ << endl;
      ctrl_inject_->setValue(false, NOUPDATE);
      MarControlAccessor acc_injectVector(ctrl_injectVector_);
      realvec& injectVector = acc_injectVector.to<mrs_realvec>();

      for (mrs_natural j=0; j < injectVector.getSize() - 1; j++)
      {
        (*mos_) << fixed << setprecision(precision_) << injectVector(j) << ",";
      }
      // TODO: the following assumes that the last item is a label, which is
      // not always true, see ctrl_regression.
      int label = (int)injectVector(injectVector.getSize() - 1);
      // TODO: what's the point of all those ostringstreams?
      ostringstream oss;
      oss << labelNames_[label];
      (*mos_) << oss.str();
      (*mos_) << endl;
    }
  }
  precision_ = ctrl_precision_->to<mrs_natural>();



  // initalize the downsample count
  count_ = 0;
}

void
WekaSink::myProcess(realvec& in, realvec& out)
{
  mrs_natural o,t;
  // If muted: just copy input to output.
  if (ctrl_mute_->isTrue())
  {
    for (o=0; o < inObservations_; o++)
    {
      for (t = 0; t < inSamples_; t++)
      {
        out(o,t) =  in(o,t);
      }
    }
    return;
  }

  mrs_bool print_line;
  if (ctrl_onlyStable_->isTrue())
  {
    stabilizingTicks_++;
    // under normal circumstances, do we print it?
    // use <= because we just incremented it.
    if (stabilizingTicks_ <= ctrl_inStabilizingDelay_->to<mrs_natural>()) {
      print_line = false;
    } else {
      print_line = true;
    }
    // what about special circumstances?
    if (ctrl_resetStable_->isTrue()) {
      stabilizingTicks_ = 0;
      // end of file
      if (ctrl_currentlyPlaying_->to<mrs_string>() == prev_playing_) {
        print_line = false;
      }
    }
  } else {
    print_line = true;
  }

  // Counter for handling the decimation (see ctrl_downsample).

  mrs_natural label_class = 0;


  for (t = 0; t < inSamples_; t++)
  {
    // Add a comment about the current input file.
    if (ctrl_currentlyPlaying_->to<mrs_string>() != prev_playing_)
    {
      (*mos_) << "% filename " << ctrl_currentlyPlaying_->to<mrs_string>() << endl;
      (*mos_) << "% srate " << israte_ << endl;
      prev_playing_ = ctrl_currentlyPlaying_->to<mrs_string>();
    }

    // round value, in case of weird floating-point effects
    label_class = (mrs_natural) (in(inObservations_ - 1, t) + 0.5);

    // Output all but last feature values.
    // TODO: this should be refactored together with the injection stuff from
    // WekaSink::myUpdate().
    for (o=0; o < inObservations_; o++)
    {
      out(o,t) = in(o,t);
      if ((label_class >= 0) || (ctrl_regression_->isTrue()))
      {
        if (o < inObservations_ - 1)
        {
          if ((count_ % downsample_) == 0)
          {
            if (print_line)
            {
              if ( out(o,t) != out(o,t) )	// Jen's NaN check for MIREX 05
              {
                // (*mos_) << fixed << setprecision(precision_) << 0. << ",";
                // DO NOT OUTPUT FEATURES
                // (*mos_) << fixed << setprecision(precision_) << 0. << ",";
                //notPrint = true;
                (*mos_) << "?" << ",";
              }
              else
              {
                (*mos_) << fixed << setprecision(precision_) << out(o,t) << ",";
                //notPrint = false;
              }
            }
          }
        }
      }
    }

    // Output last value (e.g. as label).
    ostringstream oss;
    if ((count_ % downsample_) == 0)
    {
      if (print_line)
      {
        if (!ctrl_regression_->isTrue())
        {
          if (label_class >= 0)
          {
            //  if (!notPrint)
            //{
            if (label_class >= (mrs_natural)labelNames_.size())
            {
              MRSWARN("WekaSink: label number is too big");
              oss << "non-label";
            }
            else
            {
              oss << labelNames_[label_class];
            }
            (*mos_) << oss.str();
            (*mos_) << endl;
          }
          //  else
          //{
          //  cout << "skipping instance" << endl;
          //}
          //}
        }
        else
        {
          (*mos_) << in(inObservations_ - 1, t);
          (*mos_) << endl;
        }
      }
    }
  }
  count_++;
}
