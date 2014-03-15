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


#include "../common_source.h"

#include "PlotSink.h"
#include <iomanip>


using std::ostringstream;
using std::cout;
using std::endl;
using std::setw;
using std::setfill;

using namespace Marsyas;

PlotSink::PlotSink(mrs_string name):MarSystem("PlotSink",name)
{
  counter_ = 0;
  single_file_ = NULL;
  addControls();
}

PlotSink::PlotSink(const PlotSink& a):MarSystem(a)
{
  counter_ = 0;
  single_file_ = NULL;
  ctrl_messages_ = getctrl("mrs_bool/messages");
  ctrl_separator_ = getctrl("mrs_string/separator");
  ctrl_sequence_ = getctrl("mrs_bool/sequence");
  ctrl_single_file_ = getctrl("mrs_bool/single_file");
  ctrl_no_ticks_ = getctrl("mrs_bool/no_ticks");
  ctrl_filename_ = getctrl("mrs_string/filename");
  ctrl_matlab_ = getctrl("mrs_bool/matlab");
  ctrl_matlabCommand_ = getctrl("mrs_string/matlabCommand");
}

PlotSink::~PlotSink()
{
  if (ctrl_single_file_->isTrue() && single_file_ != NULL) {
    single_file_->close();
    delete single_file_;
    single_file_ = NULL;
  }
}

MarSystem*
PlotSink::clone() const
{
  return new PlotSink(*this);
}

void
PlotSink::addControls()
{
  addctrl("mrs_bool/messages", false, ctrl_messages_);
  addctrl("mrs_string/separator", ",", ctrl_separator_);
  addctrl("mrs_bool/sequence", true, ctrl_sequence_);
  addctrl("mrs_bool/single_file", false, ctrl_single_file_);
  addctrl("mrs_bool/no_ticks", false, ctrl_no_ticks_);
  setctrlState("mrs_bool/single_file", true);
  addctrl("mrs_string/filename", "", ctrl_filename_);
  setctrlState("mrs_string/filename", true);
  addctrl("mrs_bool/matlab", false, ctrl_matlab_);
  addctrl("mrs_string/matlabCommand",
          "plot("+type_+"_"+name_+"_indata);", ctrl_matlabCommand_);
}

void
PlotSink::myUpdate(MarControlPtr sender)
{
  // no change to network flow
  MarSystem::myUpdate(sender);

  if (single_file_ && (!ctrl_single_file_->isTrue() || ctrl_filename_->to<mrs_string>() != filename_))
  {
    single_file_->close();
    delete single_file_;
    single_file_ = NULL;
  }

  filename_ = ctrl_filename_->to<mrs_string>();

  if (!single_file_ && ctrl_single_file_->isTrue() &&
      !filename_.empty())
  {
    single_file_ = new std::ofstream(filename_.c_str());
  }
}

void
PlotSink::myProcess(realvec& in, realvec& out)
{
  out = in;
  mrs_natural t,o;

  //if using MATLABengine, plot the input data in MATLAB
#ifdef MARSYAS_MATLAB
  if(ctrl_matlab_->isTrue())
  {
    MATLAB_PUT(in, type_+"_"+name_+"_indata")
    MATLAB_EVAL(ctrl_matlabCommand_->to<mrs_string>());
  }
#endif

  counter_++;

  if (ctrl_sequence_->isTrue())
  {
    //save current input to a sequence of numbered output files
    ostringstream oss;
    oss << ctrl_filename_->to<mrs_string>() <<
        setfill('0') << setw(4) << counter_ << ".plot";
    cout << "name = " << name_ << " " << oss.str() << endl;
    in.write(oss.str());
  }

  if (ctrl_single_file_->isTrue() && single_file_) {
    for (o=0; o < inObservations_; o++) {
      for (t = 0; t < inSamples_; t++) {
        //(*single_file_) << counter_ << " " << t << " ";
        (*single_file_) << std::setprecision(20) << in(o,t);
        (*single_file_) << std::endl;
        //cout << in(o,t);
      }
    }
    if (ctrl_no_ticks_->isTrue()) {
    } else {
      (*single_file_) << std::endl;
    }
  }

  if(ctrl_messages_->isTrue())
  {
    mrs_string sep =ctrl_separator_->to<mrs_string>();
    //ostringstream oss;
    //output input content as a Marsyas Message (stdout by default)
//		for (t = 0; t < inSamples_; t++)
//		{
//			for (o=0; o < inObservations_; o++)
//			{
//				if (o < inObservations_ - 1)
//				{
//					oss << out(o,t) << sep;
//				}
//				else
//				{
//					oss << out(o,t);
//				}
//			}
//			mrs_string s = oss.str();
//			MRSMSG(s << endl);
//		}//FIXME: confirm that code below is correct and remove commented code above


    for (o=0; o < inObservations_; o++)
    {
      ostringstream oss;
      for (t = 0; t < inSamples_; t++)
      {
        if (t < inSamples_ - 1)
        {
          oss << out(o,t) << sep;
        }
        else
        {
          oss << out(o,t);
        }
      }
      mrs_string s = oss.str();
      MRSMSG(s << endl);
    }
  }
}
