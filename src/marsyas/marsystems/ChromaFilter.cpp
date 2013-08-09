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


#include "ChromaFilter.h"
#include "Filter.h"
#include <sstream>


using std::ostringstream;
using std::stringstream;
using std::cout;
using std::endl;

using namespace Marsyas;

ChromaFilter::ChromaFilter(mrs_string name):MarSystem("ChromaFilter", name),
  filterBank (0)
{
  fs          = 0;
  octaves     = 0;
  freq        = 0;
  q           = 0;
  filterBank  = NULL;

  addControls();
}


ChromaFilter::~ChromaFilter()
{
  if (filterBank) delete filterBank;
}

MarSystem*
ChromaFilter::clone() const
{
  return new ChromaFilter(*this);
}


void
ChromaFilter::addControls()
{
  addctrl("mrs_natural/octaves", 2);
  addctrl("mrs_real/freq", 440.0f);
  addctrl("mrs_real/q", 1.0f);

  setctrlState("mrs_natural/octaves", true);
  setctrlState("mrs_real/freq", true);
  setctrlState("mrs_real/q", true);
}

void
ChromaFilter::myUpdate(MarControlPtr sender)
{
  (void) sender;
//  MRSDIAG("ChromaFilter.cpp - ChromaFilter:myUpdate");

  //FilterBank creation
  if (octaves != getctrl("mrs_natural/octaves")->to<mrs_natural>()) {
    octaves = getctrl("mrs_natural/octaves")->to<mrs_natural>();
    if (filterBank) delete filterBank;
    filterBank = new Fanout("filterBank");
    stringstream name;
    for(mrs_natural i = 0; i < 12*octaves+1; ++i) {
      name.str("");
      name << "filter_" << i;
      Filter* filter = new Filter(name.str());
      filterBank->addMarSystem(filter);
    }
    fcoefs.create(12*octaves+1, 6);
  }

  setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples")->to<mrs_natural>());
  setctrl("mrs_natural/onObservations", (12*octaves+1)*getctrl("mrs_natural/inObservations")->to<mrs_natural>());
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));


  //Coefficients computation
  fs = getctrl("mrs_real/israte")->to<mrs_real>();
  freq = getctrl("mrs_real/freq")->to<mrs_real>();
  q = getctrl("mrs_real/q")->to<mrs_real>();

  for (mrs_natural i = 0; i < 12*octaves+1; ++i) {

    mrs_real fc, r;

    fc = freq*pow(2.0, ((i-6*octaves)*100.0)/1200.0);
    r  = (q-PI*fc/fs)/q;

    fcoefs(i,0) = 1.0;
    fcoefs(i,1) = -2.0*r*cos(2*PI*fc/fs);
    fcoefs(i,2) = pow(r, 2.0);
    fcoefs(i,3) = (1-pow(r, 2.0))/2.0;
    fcoefs(i,4) = 0.0;
    fcoefs(i,5) = -fcoefs(i,3);
  }

  //Controls update
  stringstream channel,filter,ctrl;
  realvec b(1,3),a(1,3);

  filterBank->setctrl("mrs_natural/inSamples", getctrl("mrs_natural/inSamples")->to<mrs_natural>());
  filterBank->setctrl("mrs_natural/inObservations", getctrl("mrs_natural/inObservations")->to<mrs_natural>());
  filterBank->setctrl("mrs_natural/onObservations", getctrl("mrs_natural/onObservations")->to<mrs_natural>());
  filterBank->setctrl("mrs_real/israte", getctrl("mrs_real/israte")->to<mrs_real>());


  for(mrs_natural i = 0; i < 12*octaves+1; ++i) {

    // Low Filter
    filter.str("");
    filter << channel.str() << "Filter/filter_" << i << "/";
    ctrl.str("");
    ctrl << filter.str() << "mrs_natural/inSamples";
    filterBank->setctrl(ctrl.str(), getctrl("mrs_natural/inSamples")->to<mrs_natural>());
    ctrl.str("");
    ctrl << filter.str() << "mrs_natural/inObservations";
    filterBank->setctrl(ctrl.str(), getctrl("mrs_natural/inObservations")->to<mrs_natural>());
    ctrl.str("");
    ctrl << filter.str() << "mrs_natural/onObservations";
    filterBank->setctrl(ctrl.str(), getctrl("mrs_natural/inObservations")->to<mrs_natural>());
    ctrl.str("");
    ctrl << filter.str() << "mrs_real/israte";
    filterBank->setctrl(ctrl.str(), getctrl("mrs_real/israte")->to<mrs_real>());

    a(0) = fcoefs(i,0);
    a(1) = fcoefs(i,1);
    a(2) = fcoefs(i,2);
    b(0) = fcoefs(i,3);
    b(1) = fcoefs(i,4);
    b(2) = fcoefs(i,5);
    ctrl.str("");
    ctrl << filter.str() << "mrs_realvec/ncoeffs";
    filterBank->setctrl(ctrl.str(), b);
    ctrl.str("");
    ctrl << filter.str() << "mrs_realvec/dcoeffs";
    filterBank->setctrl(ctrl.str(), a);

    //  a.dumpDataOnly();
    //    cout << " ";
    //    b.dumpDataOnly();
    //    cout << endl;
  }
  //update the whole filter bank
  filterBank->update();

}

void
ChromaFilter::myProcess(realvec& in, realvec& out)
{
  //checkFlow(in,out);
  //lmartins: if (mute_) return;
  if(getctrl("mrs_bool/mute")->to<mrs_bool>()) return;

  filterBank->process(in, out);
}
