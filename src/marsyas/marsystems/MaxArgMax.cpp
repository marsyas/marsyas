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

#include "MaxArgMax.h"
#include <algorithm>

using std::cout;
using std::endl;

using std::ostringstream;
using std::max;
using std::min;

using namespace Marsyas;

MaxArgMax::MaxArgMax(mrs_string name):MarSystem("MaxArgMax",name)
{
  addControls();
}

MaxArgMax::~MaxArgMax()
{
}

MarSystem*
MaxArgMax::clone() const
{
  return new MaxArgMax(*this);
}

void
MaxArgMax::addControls()
{
  addctrl("mrs_natural/nMaximums", (mrs_natural)1);
  setctrlState("mrs_natural/nMaximums", true);

  //Added to avoid Fanout Crash (if used within Fanout) - requires equal number of columns in each line!
  addctrl("mrs_natural/fanoutLength", (mrs_natural)1);
  setctrlState("mrs_natural/fanoutLength", true);

  addctrl("mrs_natural/interpolation", (mrs_natural)0);
}

void
MaxArgMax::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)

  mrs_natural k = getctrl("mrs_natural/nMaximums")->to<mrs_natural>();
  mrs_natural fanoutLength_ = getctrl("mrs_natural/fanoutLength")->to<mrs_natural>();

  //Added to avoid Fanout Crash (if used within Fanout) - requires equal number of columns in each line!
  mrs_natural size = 2 * max(k, fanoutLength_);
  setctrl("mrs_natural/onSamples",  size);
  setctrl("mrs_natural/onObservations", getctrl("mrs_natural/inObservations"));
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));


}

void quadraticInterpolation(mrs_real *ix, mrs_real *iy, realvec& data)
{

  mrs_natural index = (mrs_natural) *ix;
  mrs_real d = (data(index-1)-data(index+1))/(2*(-2*data(index)+data(index-1)+data(index+1)));
  *ix += d;
  *iy -= d*(data(index-1)-data(index+1))/4;

}


void
MaxArgMax::myProcess(realvec& in, realvec& out)
{
  mrs_natural t,o;
  out.setval(MINREAL);
  mrs_natural k = getctrl("mrs_natural/nMaximums")->to<mrs_natural>();

  mrs_natural interpolationMode = getctrl("mrs_natural/interpolation")->to<mrs_natural>();

  for (o=0; o < inObservations_; o++)
  {
    for (t=0; t < inSamples_; t++)
    {
      mrs_real newmax = in(o,t);
      mrs_real newmax_i = t;

      //pair indexes = peak amplitude
      //odd indexes = peak argument

      for (ki=0; ki < k; ++ki)
      {
        if (newmax > out(o, 2*ki))
        {
          mrs_real oldmax = out(o, 2*ki);
          mrs_real oldmax_i = out(o,2*ki+1);
          out(o,2*ki) = newmax;
          out(o,2*ki+1) = newmax_i;
          newmax = oldmax;
          newmax_i = oldmax_i;
        }
      }
    }
    if(interpolationMode)
      for (ki=0; ki < k; ++ki)
      {
        mrs_real ix =  out(o,2*ki+1), iy =  out(o,2*ki);


        if ((ix != 0.0) &&  (iy != 0.0))
          quadraticInterpolation(&ix, &iy, in);
        out(o,2*ki) = iy;
        out(o,2*ki+1) = ix;
      }
  }



  //MATLAB_PUT(in, "Peaker_output");
  //MATLAB_PUT(out, "MaxPeaks");
}
