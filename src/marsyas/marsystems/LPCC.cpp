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

#include "LPCC.h"
#include "../common_source.h"

using std::ostringstream;
using namespace Marsyas;

LPCC::LPCC(mrs_string name):MarSystem("LPCC",name)
{
  addControls();
}

LPCC::LPCC(const LPCC& a) : MarSystem(a)
{
  ctrl_order_ = getctrl("mrs_natural/order");
}

LPCC::~LPCC()
{
}

MarSystem*
LPCC::clone() const
{
  return new LPCC(*this);
}

void
LPCC::addControls()
{
  //read-only
  addctrl("mrs_natural/order", 1, ctrl_order_);
}

void
LPCC::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)
  MRSDIAG("LPCC.cpp - LPCC:myUpdate");

  ctrl_onSamples_->setValue(ctrl_inSamples_);
  ctrl_osrate_->setValue(ctrl_israte_, NOUPDATE);

  // output nr of observations (i.e. LPCC coefficients) is equal
  // to the number of LPC coefs (LPC coeffs - 1 pitch coeff - 1 power)
  mrs_natural order = ctrl_inObservations_->to<mrs_natural>() - 2;
  ctrl_order_->setValue(order, NOUPDATE);
  ctrl_onObservations_->setValue(order, NOUPDATE);

  //LPCC features names
  ostringstream oss;
  for (mrs_natural i = 0; i < ctrl_order_->to<mrs_natural>(); ++i)
    oss << "LPCC_" << i+1 << ",";
  ctrl_onObsNames_->setValue(oss.str(), NOUPDATE);

  tmp_.create(ctrl_onObservations_->to<mrs_natural>()+1, ctrl_onSamples_->to<mrs_natural>());
}

void
LPCC::myProcess(realvec& in, realvec& out)
{
  mrs_real sum;
  mrs_natural order = ctrl_order_->to<mrs_natural>();

//************************************************************************
// Based on:
// http://www.mathworks.com/access/helpdesk/
// help/toolbox/dspblks/index.html?/access/helpdesk/help/toolbox/dspblks/
// lpctofromcepstralcoefficients.html
//************************************************************************
  tmp_.setval(0.0);
  tmp_(0) = -log(in(order+1)); //[!][?]
  for (mrs_natural m = 1; m <= order; m++)
  {
    sum = 0.0;
    for (mrs_natural k=1; k <= m-1; k++)
      sum = sum + (mrs_real)(m-k) * in(k-1) * tmp_(m-k);
    tmp_(m) = +in(m-1) + sum / (mrs_real)m;

    out(m-1) = tmp_(m);//drop c0 => output only [c1 c2 c3 ... cp]
  }

  //MATLAB_PUT(in, "LPCC_in");
  //MATLAB_PUT(out, "LPCC_out");
  //MATLAB_EVAL("LPCC_test");
}
