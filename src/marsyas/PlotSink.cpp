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

#include "PlotSink.h"
#include <iomanip> 

using namespace std;
using namespace Marsyas;

PlotSink::PlotSink(string name):MarSystem("PlotSink",name)
{
	counter_ = 0;
	addControls();
}

PlotSink::PlotSink(const PlotSink& a):MarSystem(a)
{
	ctrl_separator_ = getctrl("mrs_string/separator");
	ctrl_sequence_ = getctrl("mrs_bool/sequence");
	ctrl_filename_ = getctrl("mrs_string/filename");
}

PlotSink::~PlotSink()
{
}

MarSystem* 
PlotSink::clone() const 
{
	return new PlotSink(*this);
}

void 
PlotSink::addControls()
{
  addctrl("mrs_string/separator", ",", ctrl_separator_);
  addctrl("mrs_bool/sequence", true, ctrl_sequence_);
  addctrl("mrs_string/filename", "defaultfile", ctrl_filename_);
}

void 
PlotSink::myProcess(realvec& in, realvec& out)
{
  out = in;
  
  //if using MATLABengine, plot the input data in MATLAB
#ifdef MARSYAS_MATLAB
  // create in the matlab variable containing the data
  MATLAB_PUT(in, "in_data")
    // tentatively plot it
	  // MATLAB_EVAL("plot(in_data); axis([1 length(in_data) 0 1])");
	  MATLAB_EVAL("plot(in_data);");
  return;
#endif
  
  counter_++;
  
  if (ctrl_sequence_->isTrue()) 
    {
      //save current input to a sequence of numbered output files
      ostringstream oss;
      oss << ctrl_filename_->to<mrs_string>() <<
	setfill('0') << setw(4) << counter_ << ".plot";
      cout << "name = " << name_ << " " << oss.str() << endl;
      
      MRSMSG("Writing " << oss.str() << endl);
      in.write(oss.str());
    }
  else 
    {
      string sep =ctrl_separator_->to<mrs_string>();
      ostringstream oss;
      //output input content as a Marsyas Message (stdout by default)
      for (t = 0; t < inSamples_; t++)
	{
	  for (o=0; o < inObservations_; o++)
	    {
	      if (o < inObservations_ - 1)
		{
		  oss << out(o,t) << sep; 
		}
	      else
		{
		  oss << out(o,t);
		}
	    }
	  string s = oss.str(); 
	  MRSMSG(s << endl);
	}
    }
}


















