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

/**
   \class PlotSink
   \brief PlotSink: Text output sink

*/

#include "PlotSink.h"
#include <iomanip> 

using namespace std;
using namespace Marsyas;

PlotSink::PlotSink(string name):MarSystem("PlotSink",name)
{
  //type_ = "PlotSink";
  //name_ = name;

  counter_ = 0;

	addControls();
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
  addctrl("mrs_string/separator", ",");
  addctrl("mrs_bool/sequence", true);
  addctrl("mrs_string/outputFilename", "marsyas");
}

void 
PlotSink::myProcess(realvec& in, realvec& out)
{
  checkFlow(in,out);
  
  
  mrs_natural nObservations = getctrl("mrs_natural/inObservations")->toNatural();
  mrs_natural nSamples = getctrl("mrs_natural/inSamples")->toNatural();

  string sep = getctrl("mrs_string/separator")->toString();
  bool seq = getctrl("mrs_bool/sequence")->toBool();
  string outName = getctrl("mrs_string/outputFilename")->toString();
  counter_++;
  
  if (!seq) 
    {
      for (t = 0; t < nSamples; t++)
	{
	  
	  for (o=0; o < nObservations; o++)
	    {
	      out(o,t) = in(o,t);
	      if (o < nObservations - 1) 
		cout << out(o,t) << sep;
	      else
		cout << out(o,t);
	    }
	  cout << endl;
	}
    }
  else 
    {
      // copy input to output 
      for (t = 0; t < nSamples; t++)
	for (o=0; o < nObservations; o++)
	  out(o,t) = in(o,t);
      
      ostringstream oss;
      oss << outName << setfill('0') << setw(4) << counter_ << ".plot";
      cout << "Writing " << oss.str() << endl;
      in.write(oss.str());
    }
  
  
}
















	
	
