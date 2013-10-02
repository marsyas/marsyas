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
#include "RealvecSink.h"

using namespace std;
using namespace Marsyas;

RealvecSink::RealvecSink(mrs_string name):MarSystem("RealvecSink",name)
{
  //type_ = "RealvecSink";
  //name_ = name;
  oriName_ = "MARSYAS_EMPTY";
  count_= 0;
  write_ = 0 ;
  addControls();
}

RealvecSink::RealvecSink(const RealvecSink& a):MarSystem(a)
{
  count_ = 0;
  write_ = 0 ;
  oriName_ = "MARSYAS_EMPTY";
  ctrl_data_ = getctrl("mrs_realvec/data");
}


RealvecSink::~RealvecSink()
{
}


MarSystem*
RealvecSink::clone() const
{
  return new RealvecSink(*this);
}

void
RealvecSink::addControls()
{
  addctrl("mrs_bool/done", false);
  setctrlState("mrs_bool/done", true);
  addctrl("mrs_realvec/data", realvec(), ctrl_data_);
  setctrlState("mrs_real/israte", true);
  addctrl("mrs_string/fileName", "MARSYAS_EMPTY");
  setctrlState("mrs_string/fileName", true);
}


void
RealvecSink::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)
  MRSDIAG("RealvecSink.cpp - RealvecSink:myUpdate");

  setctrl("mrs_natural/onObservations", getctrl("mrs_natural/inObservations")->to<mrs_natural>());
  setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples")->to<mrs_natural>());
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte")->to<mrs_real>());
  ctrl_onObsNames_->setValue(ctrl_inObsNames_->to<mrs_string>(), NOUPDATE);

  if( getctrl("mrs_bool/done")->isTrue()) {
    if(write_)
    {
      // closing output file
      outputFile_.close();
      // copy to tmp
      mrs_string tmp  = oriName_.c_str();
      tmp+="tmp";
      ofstream out;
      out.open(tmp.c_str(), ios::out);
      ifstream in;
      in.open(oriName_.c_str(), ios::in);
      out << in.rdbuf();
      in.close();
      out.close();

      //reopen
      out.open(oriName_.c_str(), ios::out);
      // print header
      out << "# MARSYAS mrs_realvec" << endl;
      out << "# Size = " << inObservations_*count_ << endl << endl;
      out << endl;


      out << "# type: matrix" << endl;
      out << "# rows: " << count_ << endl;
      out << "# columns: " << inObservations_ << endl;
      // fill core
      in.open(tmp.c_str(), ios::in);
      out << in.rdbuf();
      in.close();
      // remove tmp file
#ifdef MARSYAS_WIN32
      _unlink(tmp.c_str());
#else
      unlink(tmp.c_str());
#endif
      // write bottom
      out << endl;
      out << "# Size = " << inObservations_*count_ << endl;
      out << "# MARSYAS mrs_realvec" << endl;
      out.close();
    }
    else
    {
      MarControlAccessor acc(ctrl_data_, NOUPDATE);
      realvec& data = acc.to<mrs_realvec>();
      data.stretch(0);
    }
    count_=0;
    setctrl("mrs_bool/done", false);
  }


  if(getctrl("mrs_string/fileName")->to<mrs_string>().compare(oriName_))
  {
    if(write_)
      outputFile_.close();
    oriName_ = getctrl("mrs_string/fileName")->to<mrs_string>();
    outputFile_.open(oriName_.c_str(), ios::out);
    write_ = 1;
  }
}

void
RealvecSink::myProcess(realvec& in, realvec& out)
{
  mrs_natural o,t;
  out=in;

  if(write_)
  {
    for (t=0; t < inSamples_; t++)
    {
      for (o=0 ; o<inObservations_ ; o++)
        outputFile_ << in(o, t) << " " ;
      outputFile_ << endl;
    }
  }
  else
  {
    MarControlAccessor acc(ctrl_data_);
    realvec& data = acc.to<mrs_realvec>();
    data.stretch(inObservations_, count_+inSamples_);

    for (o=0; o < inObservations_; o++)
      for (t=0; t < inSamples_; t++)
        data(o, count_+t) = in(o, t);

    //out.dump();
  }

  count_+=inSamples_;
}
