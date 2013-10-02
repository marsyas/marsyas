
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

#include "LSP.h"
#include "../common_source.h"

#include <marsyas/NumericLib.h>

#include <algorithm>
#include <vector>

// #define _MATLAB_LSP_


using std::ostringstream;
using std::vector;
using std::polar;

using namespace Marsyas;

LSP::LSP(mrs_string name):MarSystem("LSP",name)
{
  addControls();
}

LSP::~LSP()
{
}

MarSystem*
LSP::clone() const
{
  return new LSP(*this);
}

void
LSP::addControls()
{
  addctrl("mrs_natural/order", (mrs_natural)10);//read-only control
  addctrl("mrs_real/gamma", (mrs_real)1.0);
}

void
LSP::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)
  MRSDIAG("LSP.cpp - LSP:myUpdate");

  order_ = getctrl("mrs_natural/inObservations")->to<mrs_natural>() - 2;
  setctrl("mrs_natural/order", order_);//read-only control

  setctrl("mrs_natural/onObservations", order_);
  setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples"));
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));

  //LSP features names
  ostringstream oss;
  for (mrs_natural i = 0; i < order_; ++i)
    oss << "LSP_" << i+1 << ",";
  setctrl("mrs_string/onObsNames", oss.str());
}

void
LSP::myProcess(realvec& in, realvec& out)
{
  NumericLib numLib;

  mrs_real gamma = getctrl("mrs_real/gamma")->to<mrs_real>();
  vector<mrs_real> ak(order_);

  if( gamma != 1.0)
    for(mrs_natural j = 0; j < order_ ; j++)
    {
      ak[j] = in(j)*pow((mrs_real)gamma, (mrs_real)j+1);//*(-1.0);//apply pole-shifting
    }
  else
    for(mrs_natural j = 0; j < order_ ; j++)
    {
      ak[j] = in(j);//*(-1.0); //no pole-shifting applied
    }

  vector<mrs_complex> P(order_+2);
  vector<mrs_complex> Q(order_+2);
  vector<mrs_complex> Proots(order_+1);
  vector<mrs_complex> Qroots(order_+1);

  P[order_+1] = polar(1.0, 0.0);
  Q[order_+1] = polar(1.0, 0.0);
  for(mrs_natural k = 0; k < order_; k++)
  {
    P[order_-k] = polar((double)(ak[k] + ak[order_-1-k]), 0.0);
    Q[order_-k] = polar((double)(ak[k] - ak[order_-1-k]), 0.0);
  }
  P[0] = polar(1.0, 0.0);
  Q[0] = polar(-1.0, 0.0);

  if (!numLib.polyRoots(P, false, order_+1, Proots)) {
    //P has only real coefs => complexCoefs = false
    MRSERR("LSP::myProcess() - numerical error in polynomial root calculation!");
  }
  if(!numLib.polyRoots(Q, false, order_+1, Qroots)) {
    //Q has only real coefs => complexCoefs = false
    MRSERR("LSP::myProcess() - numerical error in polynomial root calculation!");
  }

  mrs_real phase;
  vector<mrs_real> out_vec;
  for(mrs_natural k = 0; k <= order_; k++)
  {
    phase = arg(Proots[k]);
    if((phase > 0) && (phase < PI))
    {
      out_vec.push_back(phase);
    }
  }
  for(mrs_natural k = 0; k <= order_; k++)
  {
    phase = arg(Qroots[k]);
    if((phase > 0) && (phase < PI))
    {
      out_vec.push_back(phase);
    }
  }
  sort(out_vec.begin(), out_vec.end()); //sorts LSP freqs into ascending order

  //output sorted LSP frequencies
  for(mrs_natural i = 0; i < order_; ++i)
    out(i) = out_vec[i];

#ifdef _MATLAB_LSP_
  MATLAB_PUT(order_, "LSP_order");
  MATLAB_PUT(in, "LSP_in");
  MATLAB_PUT(P, "LSP_P");
  MATLAB_PUT(Q, "LSP_Q");
  MATLAB_PUT(Proots, "LSP_Proots");
  MATLAB_PUT(Qroots, "LSP_Qroots");
  MATLAB_PUT(out_vec, "LSP_out1");
  MATLAB_PUT(out, "LSP_out2");
  MATLAB_EVAL("LSP_test(LSP_order, LSP_in, LSP_P, LSP_Q, LSP_Proots, LSP_Qroots, LSP_out1, LSP_out2);");
#endif
}
