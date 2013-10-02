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

#include "ClassOutputSink.h"
#include "../common_source.h"


using std::ostringstream;
using std::ofstream;
using std::cout;
using std::endl;

using namespace Marsyas;


ClassOutputSink::ClassOutputSink(mrs_string name):MarSystem("ClassOutputSink", name)
{
  //type_ = "ClassOutputSink";
  //name_ = name;
  mos_ = NULL;

  addControls();
}

ClassOutputSink::~ClassOutputSink()
{
  if (mos_ != NULL)
    mos_->close();
  delete mos_;
}

MarSystem*
ClassOutputSink::clone() const
{
  return new ClassOutputSink(*this);
}

void
ClassOutputSink::addControls()
{
  addctrl("mrs_natural/memSize", 40);
  addctrl("mrs_natural/nLabels", 2);

  addctrl("mrs_string/filename", "mugle.mf");
  setctrlState("mrs_string/filename", true);
  setctrlState("mrs_natural/nLabels", true);
  addctrl("mrs_string/labelNames", "Music,Speech");
  setctrlState("mrs_string/labelNames", true);
  addctrl("mrs_bool/silent", true);
}

void
ClassOutputSink::putHeader()
{
  if ((filename_ != getctrl("mrs_string/filename")->to<mrs_string>()))
  {
    if (mos_ != NULL)
    {
      mos_->close();
      delete mos_;
      if (filename_ == "mugle.mf")
        remove(filename_.c_str());
    }


    filename_ = getctrl("mrs_string/filename")->to<mrs_string>();

    mos_ = new ofstream;
    mos_->open(filename_.c_str());

  }
}


void
ClassOutputSink::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)
  MRSDIAG("ClassOutputSink.cpp - ClassOutputSink:myUpdate");

  setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples"));
  setctrl("mrs_natural/onObservations", getctrl("mrs_natural/inObservations"));
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));

  mrs_string labelNames = getctrl("mrs_string/labelNames")->to<mrs_string>();

  labelNames_.clear();

  mrs_string temp;

  for (int i = 0; i < getctrl("mrs_natural/nLabels")->to<mrs_natural>(); ++i)
  {
    mrs_string labelName;


    labelName = labelNames.substr(0, labelNames.find(","));
    temp = labelNames.substr(labelNames.find(",")+1, labelNames.length());
    labelNames = temp;
    labelNames_.push_back(labelName);
  }
  count_ = 0;

  putHeader();
}


void
ClassOutputSink::myProcess(realvec& in, realvec& out)
{
  //checkFlow(in,out);

  mrs_natural p;
  //mrs_natural g;
  mrs_natural o,t;


  for (o=0; o < inObservations_; o++)
    for (t = 0; t < inSamples_; t++)
    {
      out(o,t) = in(o,t);
    }

  if (!getctrl("mrs_bool/silent")->isTrue())
  {
    for (o = 0; o < inObservations_; o++)
      for (t = 0; t < inSamples_; t++)
      {
        p = (mrs_natural)in(o,t);
        //g = (mrs_natural)in(1,t);
        if (o < inObservations_-1)
          cout << "Predicted: " << labelNames_[p] << endl;
        (*mos_) << labelNames_[p] << endl;
      }
  }

}












