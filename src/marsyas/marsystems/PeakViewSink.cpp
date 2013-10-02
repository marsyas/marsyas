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

#include "PeakViewSink.h"
#include "../common_source.h"

#include <marsyas/peakView.h>


using std::ostringstream;
using std::ifstream;
using std::ios;
using std::endl;
using std::cout;
using std::istringstream;



using namespace Marsyas;

PeakViewSink::PeakViewSink(mrs_string name):MarSystem("PeakViewSink",name)
{
  count_= 0;
  tmpFilename_ = "defaultfile.tmp";
  addControls();
}

PeakViewSink::PeakViewSink(const PeakViewSink& a):MarSystem(a)
{
  count_ = a.count_;
  tmpFilename_ ="defaultfile.tmp";

  ctrl_accumulate2Disk_ = getctrl("mrs_bool/accumulate2Disk");
  ctrl_fs_ = getctrl("mrs_real/fs");
  ctrl_frameSize_ = getctrl("mrs_natural/frameSize");
  ctrl_filename_ = getctrl("mrs_string/filename");
  ctrl_done_ = getctrl("mrs_bool/done");
}

PeakViewSink::~PeakViewSink()
{
  if(tmpFile_.is_open())
    tmpFile_.close();
}

MarSystem*
PeakViewSink::clone() const
{
  return new PeakViewSink(*this);
}

void
PeakViewSink::addControls()
{
  addctrl("mrs_bool/done", false, ctrl_done_);
  ctrl_done_->setState(true);

  addctrl("mrs_string/filename", "defaultfile.peak", ctrl_filename_);

  addctrl("mrs_bool/accumulate2Disk", true, ctrl_accumulate2Disk_);
  addctrl("mrs_real/fs", 0.0, ctrl_fs_);
  addctrl("mrs_natural/frameSize", 0, ctrl_frameSize_);
}

void
PeakViewSink::done()
{
  mrs_natural t,o;
  if(ctrl_accumulate2Disk_->isTrue())
  {

    //read from tmp file and get rid of it (we're done with it ;-))
    if(tmpFile_.is_open())
    {
      //close the accumulated tmp data file (no more writting)
      tmpFile_.close();

      //open the tmp file for reading
      ifstream tmpFile;
      tmpFile.open(tmpFilename_.c_str(), ios::in);

      //read data from tmp file intomemory
      accumData_.create(inObservations_, count_);


      char myline[2048];

      for(t=0; t < count_; ++t)
      {
        tmpFile.getline(myline, 2048);
        istringstream iss(myline);
        for(o=0; o < inObservations_; ++o)
        {

          iss >> accumData_(o,t);
        }
      }

      //close and delete tmp file from disk
      tmpFile.close();
      // remove(tmpFilename_.c_str());
      count_ = 0;
    }
    else
      accumData_.create(0,0); //no data was accumulated in the temp file...
  }

  //save peak data into a .peak formated output file
  if(accumData_.getSize() != 0)
  {
    peakView accumDataView(accumData_);
    accumDataView.peakWrite(ctrl_filename_->to<mrs_string>(),
                            ctrl_fs_->to<mrs_real>(), ctrl_frameSize_->to<mrs_natural>());
  }

  //reset internal data
  accumData_.create(0, 0);
  ctrl_done_->setValue(false, NOUPDATE);
}

void
PeakViewSink::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)
  MRSDIAG("PeakViewSink.cpp - PeakViewSink:myUpdate");

  ctrl_onObservations_->setValue(ctrl_inObservations_, NOUPDATE);
  ctrl_onSamples_->setValue(ctrl_inSamples_, NOUPDATE);
  ctrl_osrate_->setValue(ctrl_israte_, NOUPDATE);
  ctrl_onObsNames_->setValue(ctrl_inObsNames_, NOUPDATE);

  if(ctrl_done_->isTrue())
    done();
}

void
PeakViewSink::myProcess(realvec& in, realvec& out)
{
  out = in;
  mrs_natural o,t;



  if(ctrl_accumulate2Disk_->isTrue())
  {



    //if a tmp file is not yet created, create one
    if(!tmpFile_.is_open())
    {
      tmpFilename_ = this->getType() + "_" + this->getName() + ".tmp";
      tmpFile_.open(tmpFilename_.c_str(), ios::out | ios::trunc);
      count_ = 0;
    }

    //accumulate data into a temp file
    for (t=0; t < inSamples_; t++)
    {
      for (o=0 ; o<inObservations_ ; o++)
        tmpFile_ << in(o, t) << " " ;
      tmpFile_ << endl;
    }
    count_ += inSamples_;
  }
  else
  {
    //accumulate input data into memory
    mrs_natural cols = accumData_.getCols();
    accumData_.stretch(inObservations_, cols+inSamples_);
    for (o=0; o < inObservations_; o++)
      for (t=0; t < inSamples_; t++)
      {
        accumData_(o, cols+t) = in(o, t);
      }
  }
}
