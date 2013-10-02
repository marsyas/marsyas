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

#include "Filter.h"
#include "../common_source.h"


using std::ostringstream;
using std::ofstream;
using std::endl;


using namespace Marsyas;

Filter::Filter(mrs_string name)
  :
  MarSystem("Filter",name),
  norder_(2),
  dorder_(1),
  channels_(1),
  order_(2),
  fgain_(1.0)
{
  ncoeffs_.create(norder_);
  dcoeffs_.create(dorder_);
  state_.create(channels_,order_-1);

  ncoeffs_(0) = 1.0;
  dcoeffs_(0) = 1.0;

  addControls();
}

Filter::~Filter()
{

}


MarSystem*
Filter::clone() const
{
  return new Filter(*this);
}

void
Filter::addControls()
{
  addctrl("mrs_realvec/ncoeffs", ncoeffs_);
  addctrl("mrs_realvec/dcoeffs", dcoeffs_);
  addctrl("mrs_real/fgain", fgain_);
  addctrl("mrs_natural/stateUpdate", mrs_natural(0));
  addctrl("mrs_realvec/state", state_);

  setctrlState("mrs_realvec/ncoeffs", true);
  setctrlState("mrs_realvec/dcoeffs", true);
  setctrlState("mrs_realvec/state", true);
}

void Filter::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)
  MRSDIAG("Filter.cpp - Filter:myUpdate");

  setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples"));
  setctrl("mrs_natural/onObservations", getctrl("mrs_natural/inObservations"));
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));

  ctrl_onObsNames_->setValue("Filter_" + ctrl_inObsNames_->to<mrs_string>() + "," , NOUPDATE);

  if (getctrl("mrs_realvec/ncoeffs")->to<mrs_realvec>().getSize() != norder_)
  {
    ncoeffs_.create(getctrl("mrs_realvec/ncoeffs")->to<mrs_realvec>().getSize());
    norder_ = ncoeffs_.getSize();
    order_ = (norder_ > dorder_) ? norder_ : dorder_;
    channels_ = getctrl("mrs_natural/inObservations")->to<mrs_natural>();
    state_.create(channels_,order_-1);
    setctrl("mrs_realvec/state", state_);
  }

  if (getctrl("mrs_realvec/dcoeffs")->to<mrs_realvec>().getSize() != dorder_)
  {

    dcoeffs_.create(getctrl("mrs_realvec/dcoeffs")->to<mrs_realvec>().getSize());
    dorder_ = dcoeffs_.getSize();
    order_ = (norder_ > dorder_) ? norder_ : dorder_;
    channels_ = getctrl("mrs_natural/inObservations")->to<mrs_natural>();
    state_.create(channels_,order_-1);
    setctrl("mrs_realvec/state", state_);
  }

  if (getctrl("mrs_natural/inObservations")->to<mrs_natural>() != channels_)
  {
    channels_ = getctrl("mrs_natural/inObservations")->to<mrs_natural>();
    state_.create(channels_,order_-1);
  }

  ncoeffs_ = getctrl("mrs_realvec/ncoeffs")->to<mrs_realvec>();
  dcoeffs_ = getctrl("mrs_realvec/dcoeffs")->to<mrs_realvec>();

  if (getctrl("mrs_natural/stateUpdate")->to<mrs_natural>())
    state_ = getctrl("mrs_realvec/state")->to<mrs_realvec>();

  mrs_real d0 = dcoeffs_(0);
  if (d0 != 1.0) {
    for (mrs_natural i = 0; i < dorder_; ++i) {
      dcoeffs_(i) /= d0;
    }

    for (mrs_natural i = 0; i < norder_; ++i) {
      ncoeffs_(i) /= d0;
    }
  }

  fgain_ = 1.0f;
  setctrl("mrs_real/fgain", 1.0);
}

void
Filter::write(mrs_string filename)
{
  ofstream os(filename.c_str());
  os << (*this) << endl;
}

void
Filter::myProcess(realvec& in, realvec& out)
{
  //checkFlow(in,out);

  mrs_natural i,j,c;
  mrs_natural size = in.getCols();
  mrs_natural stateSize = state_.getCols();
  mrs_natural channels = in.getRows();

  mrs_real gain = getctrl("mrs_real/fgain")->to<mrs_real>();

  // State array holds the various delays for the difference equation
  // of the filter. Similar implementation as described in the manual
  // for MATLAB Signal Processing Toolbox. State corresponds to
  // the z, num_coefs to the b and denom_coefs to the a vector respectively
  // in_window is the input x(n) and out_window is the output y(n)

  //dcoeffs_/=10;


  // state_.setval(0);
  if (norder_ == dorder_) {
    for (c = 0; c < channels; ++c) {
      for (i = 0; i < size; ++i) {
        out(c,i) = ncoeffs_(0) * in(c,i) + state_(c,0);
        for (j = 0; j < stateSize - 1; j++)
        {
          state_(c,j) = ncoeffs_(j+1) * in(c,i) + state_(c,j+1) - dcoeffs_(j+1) * out(c,i);
        }
        state_(c,stateSize - 1) = ncoeffs_(order_-1) * in(c,i) - dcoeffs_(order_-1) * out(c,i);
      }
    }
  }
  else if (norder_ < dorder_) {
    for (c = 0; c < channels; ++c) {
      for (i = 0; i < size; ++i) {
        out(c,i) = ncoeffs_(0) * in(c,i) + state_(c,0);
        for (j = 0; j < norder_ - 1; j++)
        {
          state_(c,j) = ncoeffs_(j+1) * in(c,i) + state_(c,j+1) - dcoeffs_(j+1) * out(c,i);
        }
        for (j = norder_ - 1; j < stateSize - 1; j++)
        {
          state_(c,j) = state_(c,j+1) - dcoeffs_(j+1) * out(c,i);
        }
        state_(c,stateSize - 1) = -dcoeffs_(order_ - 1) * out(c,i);
      }
    }
  }
  else {
    for (c = 0; c < channels; ++c) {
      for (i = 0; i < size; ++i) {
        out(c,i) = ncoeffs_(0) * in(c,i) + state_(c,0);
        for (j = 0; j < dorder_ - 1; j++)
        {
          state_(c,j) = ncoeffs_(j+1) * in(c,i) + state_(c,j+1) - dcoeffs_(j+1) * out(c,i);
        }
        for (j = dorder_ - 1; j < stateSize - 1; j++)
        {
          state_(c,j) = ncoeffs_(j+1) * in(c,i) + state_(c,j+1);
        }
        state_(c,stateSize - 1) = ncoeffs_(order_-1) * in(c,i);
      }
    }
  }
  out *= gain;


  //		MATLAB_PUT(in, "Filter_in");
  //	 	MATLAB_PUT(out, "Filter_out");
  //	 	MATLAB_PUT(ncoeffs_, "ncoeffs_");
  //	 	MATLAB_PUT(dcoeffs_, "dcoeffs_");
  //	 	MATLAB_EVAL("MAT_out = filter(ncoeffs_, dcoeffs_, Filter_in)");
  //
  //	 	MATLAB_EVAL("spec_in = abs(fft(Filter_in));");
  //	 	MATLAB_EVAL("spec_out = abs(fft(Filter_out));");
  //	 	MATLAB_EVAL("spec_mat = abs(fft(MAT_out));");
  //
  //	 	MATLAB_EVAL("subplot(2,1,1);plot(Filter_in);hold on; plot(Filter_out, 'r'); plot(MAT_out, 'g');hold off");
  //	 	MATLAB_EVAL("subplot(2,1,2);plot(spec_in(1:end/2));hold on; plot(spec_out(1:end/2),'r');plot(spec_mat(1:end/2),'g');hold off;");
  //	 	MATLAB_EVAL("h = abs(fft([1 -.97], length(Filter_in)));");
  //	 	MATLAB_EVAL("hold on; plot(h(1:end/2), 'k'); hold off");
  //		//MATLAB_GET("MAT_out", out)
  //
}
