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

#include "Confidence.h"
#include "../common_source.h"
#include <marsyas/FileName.h>


using std::ostringstream;
using std::cout;
using std::endl;
using std::setprecision;
using std::fixed;
using std::ios;



using namespace Marsyas;

Confidence::Confidence(mrs_string name):MarSystem("Confidence",name)
{
  print_ = false;
  forcePrint_ = false;
  csvOutput_ = false;

  predictions_ = 0;
  count_ = 0;
  write_=0;
  oriName_ = "MARSYAS_EMPTY";
  addControls();
}

Confidence::Confidence(const Confidence& a):MarSystem(a)
{
  ctrl_memSize_ = getctrl("mrs_natural/memSize");
  ctrl_nLabels_ = getctrl("mrs_natural/nLabels");
  count_ = 0;
  print_ = false;
  csvOutput_ = false;
  forcePrint_ = false;
  write_=0;
  oriName_ = "MARSYAS_EMPTY";
}

Confidence::~Confidence()
{
}


MarSystem*
Confidence::clone() const
{
  return new Confidence(*this);
}

void
Confidence::addControls()
{
  addctrl("mrs_natural/memSize", 40, ctrl_memSize_);
  addctrl("mrs_natural/nLabels", 2, ctrl_nLabels_);
  setctrlState("mrs_natural/nLabels", true);
  addctrl("mrs_string/labelNames", "Music,Speech");
  setctrlState("mrs_string/labelNames", true);
  addctrl("mrs_bool/print", false);
  setctrlState("mrs_bool/print", true);
  addctrl("mrs_bool/forcePrint", false);
  setctrlState("mrs_bool/forcePrint", true);
  addctrl("mrs_string/fileName", "MARSYAS_EMPTY");
  setctrlState("mrs_string/fileName", true);
  addctrl("mrs_natural/write", 0);
  setctrlState("mrs_natural/write", true);
  addctrl("mrs_natural/hopSize", 512);
  setctrlState("mrs_natural/hopSize", true);
  addctrl("mrs_bool/fileOutput", false);
  setctrlState("mrs_bool/fileOutput", true);
  addctrl("mrs_bool/csvOutput", false);
  setctrlState("mrs_bool/csvOutput", true);
}

void
Confidence::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)
  MRSDIAG("Confidence.cpp - Confidence:myUpdate");
  setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples"));
  setctrl("mrs_natural/onObservations", getctrl("mrs_natural/inObservations"));
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));

  num_labels_ = getctrl("mrs_natural/nLabels")->to<mrs_natural>();
  confidences_.stretch(num_labels_);
  gtconfidences_.stretch(num_labels_+1);

  mrs_string labelNames = getctrl("mrs_string/labelNames")->to<mrs_string>();

  labelNames_.clear();

  print_ = getctrl("mrs_bool/print")->to<mrs_bool>();
  forcePrint_ = getctrl("mrs_bool/forcePrint")->to<mrs_bool>();
  csvOutput_ = getctrl("mrs_bool/csvOutput")->to<mrs_bool>();

  for (mrs_natural i = 0; i < getctrl("mrs_natural/nLabels")->to<mrs_natural>(); ++i)
  {
    mrs_string labelName;
    mrs_string temp;

    labelName = labelNames.substr(0, labelNames.find(","));
    temp = labelNames.substr(labelNames.find(",")+1, labelNames.length());
    labelNames = temp;
    labelNames_.push_back(labelName);
  }

  if (getctrl("mrs_bool/fileOutput")->to<mrs_bool>())
  {
    if(getctrl("mrs_string/fileName")->to<mrs_string>().compare(oriName_))
    {
      if(write_)
      {
        outputFileSyn_.close();
        outputFileTran_.close();
      }
      oriName_ = getctrl("mrs_string/fileName")->to<mrs_string>();
      FileName Sfname(oriName_);
      mrs_string tmp = Sfname.nameNoExt() +"_synSeg.txt";
      //      getchar();
      outputFileSyn_.open(tmp.c_str(), ios::out);
      tmp = Sfname.nameNoExt() +"_tranSeg.txt";
      outputFileTran_.open(tmp.c_str(), ios::out);
      write_ = 1;
    }
  }
  hopDuration_ = getctrl("mrs_natural/inSamples")->to<mrs_natural>() / getctrl("mrs_real/osrate")->to<mrs_real>();
  nbFrames_ = -getctrl("mrs_natural/memSize")->to<mrs_natural>()+1;
  nbCorrectFrames_ = 0;
  lastLabel_ = "MARSYAS_EMPTY";
}

void
Confidence::myProcess(realvec& in, realvec& out)
{
  mrs_natural o,t;

  bool mute = ctrl_mute_->to<mrs_bool>();
  mrs_natural memSize = ctrl_memSize_->to<mrs_natural>();
  mrs_natural nLabels = ctrl_nLabels_->to<mrs_natural>();

  mrs_natural label;
  mrs_natural gtlabel;
  mrs_natural l;


  if (mute == false)
  {
    for (o=0; o < inObservations_; o++)
      for (t = 0; t < inSamples_; t++)
      {
        out(o,t) = in(o,t);
        if (o==0)
        {
          label = (mrs_natural)in(0,t);
          confidences_(label) = confidences_(label) + 1;
          gtlabel = (mrs_natural)in(1,t);
          if (gtlabel < 0) {
            gtlabel = num_labels_;
          }
          gtconfidences_(gtlabel) = gtconfidences_(gtlabel)+1;
        }
      }
    count_++;
    bool cond = ((count_ % memSize) == 0);

    if (cond || forcePrint_)
    {
      mrs_real max_conf = 0;
      mrs_natural max_l = 0;
      mrs_real max_gtconf = 0;
      mrs_natural max_gtl = 0;
      for (l=0; l < nLabels; l++)
      {
        mrs_real conf = ((confidences_(l)) / count_);
        if (conf > max_conf)
        {
          max_conf = conf;
          max_l = l;
        }
      }
      mrs_string ground_truth_text;
      for (l=0; l < nLabels+1; l++)
      {
        mrs_real gtconf = ((gtconfidences_(l)) / count_);
        //cout<<"gtconf "<<gtconf<<endl;
        if (gtconf > max_gtconf)
        {
          max_gtconf = gtconf;
          max_gtl = l;
        }
      }
      if (max_gtl < num_labels_) {
        ground_truth_text = labelNames_[max_gtl];
      } else {
        ground_truth_text = "---";
      }

      if (getctrl("mrs_bool/fileOutput")->to<mrs_bool>())
      {
        cout << "fileOutput" << endl;

        if (write_)
        {
          outputFileSyn_ << fixed << setprecision(3) << nbFrames_*hopDuration_ << "\t";
          outputFileSyn_ << setprecision(0) << labelNames_[max_l] << "\t" <<
                         ((confidences_(max_l) / count_)) * 100.0 << endl;

          if(lastLabel_ == "MARSYAS_EMPTY" || lastLabel_ != labelNames_[max_l])
          {
            outputFileTran_ << fixed << setprecision(3) << nbFrames_*hopDuration_ << "\t" << labelNames_[max_l] << endl;
            lastLabel_ = labelNames_[max_l];
          }
        }
      }
      else
      {
        if (print_)
        {
          if (max_l == max_gtl)
          {
            nbCorrectFrames_ ++;
          }

          if (csvOutput_)
          {
            cout << fixed << setprecision(3) << nbFrames_*hopDuration_ << "\t";
            cout << fixed << setprecision(3) << ((nbFrames_+memSize)*hopDuration_) - 0.001 << "\t";
            cout << fixed << setprecision(0) << labelNames_[max_l] << "\t";
            cout << fixed << setprecision(3) << ((confidences_(max_l) / count_)) << endl;
          } else {
            cout << fixed << setprecision(3) << nbFrames_*hopDuration_ << "\t";
            cout << fixed << setprecision(0) << "PR = " << labelNames_[max_l] << "\t" <<
                 ((confidences_(max_l) / count_)) * 100.0 << setprecision(4) << "\t" << nbCorrectFrames_ * 1.0 / (nbFrames_/memSize+1);
            cout << "\t GT = " << ground_truth_text << endl;
          }
        }

      }
      if (cond || forcePrint_)
      {
        count_ = 0;
      }

      confidences_.setval(0.0);
      gtconfidences_.setval(0.0);

    }
  }
  nbFrames_++;
}
