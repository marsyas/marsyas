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
     \class Filter
     \brief Filter a Signal 

     Filters a Signal using the coefficients stored in a FilterCoeffs
object. The filter is implemented as a direct form II structure. This
is a canonical form which has the minimum number of delay elements.
*/

#include "Filter.h"

using namespace std;
using namespace Marsyas;

Filter::Filter(string name):MarSystem("Filter",name)
{
  type_ = "Filter";
  name_ = name;

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
  norder_ = 2;
  dorder_ = 1;
  channels_ = 1;
  order_ = 2;
  ncoeffs_.create(norder_);
  dcoeffs_.create(dorder_);
  state_.create(channels_,order_-1);
  ncoeffs_(0) = 1.0f;
  dcoeffs_(0) = 1.0f;
  addctrl("mrs_realvec/ncoeffs", ncoeffs_);
  addctrl("mrs_realvec/dcoeffs", dcoeffs_);
  addctrl("mrs_real/fgain", fgain_);
  addctrl("mrs_natural/stateUpdate", mrs_natural(0));
  addctrl("mrs_realvec/state", state_);
  
  setctrlState("mrs_realvec/ncoeffs", true);
  setctrlState("mrs_realvec/dcoeffs", true);
  setctrlState("mrs_realvec/state", true);
  
  //update(); //lmartins?!?!?: [!][?]
}


void Filter::localUpdate()
{
  MRSDIAG("Filter.cpp - Filter:localUpdate");
  
  setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples"));
  setctrl("mrs_natural/onObservations", getctrl("mrs_natural/inObservations"));
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));
  
  if (getctrl("mrs_realvec/ncoeffs").toVec().getSize() != norder_)
    {
      ncoeffs_.create(getctrl("mrs_realvec/ncoeffs").toVec().getSize());
      norder_ = ncoeffs_.getSize();
      order_ = (norder_ > dorder_) ? norder_ : dorder_;
      channels_ = getctrl("mrs_natural/inObservations").toNatural();
      state_.create(channels_,order_-1);
      setctrl("mrs_realvec/state", state_);
    }
  
  if (getctrl("mrs_realvec/dcoeffs").toVec().getSize() != dorder_)
    {

      dcoeffs_.create(getctrl("mrs_realvec/dcoeffs").toVec().getSize());
      dorder_ = dcoeffs_.getSize();
      order_ = (norder_ > dorder_) ? norder_ : dorder_;
      channels_ = getctrl("mrs_natural/inObservations").toNatural();
      state_.create(channels_,order_-1);
      setctrl("mrs_realvec/state", state_);
    }
  
  if (getctrl("mrs_natural/inObservations").toNatural() != channels_)
    {
      channels_ = getctrl("mrs_natural/inObservations").toNatural();
      state_.create(channels_,order_-1);
    }
  
  ncoeffs_ = getctrl("mrs_realvec/ncoeffs").toVec();
  dcoeffs_ = getctrl("mrs_realvec/dcoeffs").toVec();
  if (getctrl("mrs_natural/stateUpdate").toNatural()) 
    state_ = getctrl("mrs_realvec/state").toVec();
  
  mrs_real d0 = dcoeffs_(0);
  if (d0 != 1.0) {
    for (mrs_natural i = 0; i < dorder_; i++){
      dcoeffs_(i) /= d0;
    }
    
    for (mrs_natural i = 0; i < norder_; i++){
      ncoeffs_(i) /= d0;
    }
  }
  
  fgain_ = 1.0f;
  setctrl("mrs_real/fgain", 1.0f);
}

void 
Filter::write(string filename)
{
  ofstream os(filename.c_str());
  os << (*this) << endl;
}

void 
Filter::process(realvec& in, realvec& out)
{
  checkFlow(in,out);
  
  mrs_natural i,j,c;
  mrs_natural size = in.getCols();
  mrs_natural stateSize = state_.getCols();
  mrs_natural channels = in.getRows();
  
  mrs_real gain = getctrl("mrs_real/fgain").toReal();
  
  // State array holds the various delays for the difference equation 
  // of the filter. Similar implementation as described in the manual
  // for MATLAB Signal Processing Toolbox. State corresponds to 
  // the z, num_coefs to the b and denom_coefs to the a vector respectively
  // in_window is the input x(n) and out_window is the output y(n)
  
  if (norder_ == dorder_){
    for (c = 0; c < channels; c++) {
      for (i = 0; i < size; i++){
	out(c,i) = ncoeffs_(0) * in(c,i) + state_(c,0);	
	for (j = 0; j < stateSize - 1; j++){
	  state_(c,j) = ncoeffs_(j+1) * in(c,i) + state_(c,j+1) - dcoeffs_(j+1) * out(c,i);
	}
	state_(c,stateSize - 1) = ncoeffs_(order_-1) * in(c,i) - dcoeffs_(order_-1) * out(c,i);
      }
    }
    
  }
  else if (norder_ < dorder_){
    for (c = 0; c < channels; c++) {
      for (i = 0; i < size; i++){
	out(c,i) = ncoeffs_(0) * in(c,i) + state_(c,0);	
	for (j = 0; j < norder_ - 1; j++){
	  state_(c,j) = ncoeffs_(j+1) * in(c,i) + state_(c,j+1) - dcoeffs_(j+1) * out(c,i);
	}
	for (j = norder_ - 1; j < stateSize - 1; j++){
	  state_(c,j) = state_(c,j+1) - dcoeffs_(j+1) * out(c,i);
	}
	state_(c,stateSize - 1) = -dcoeffs_(order_ - 1) * out(c,i);
      }
    }
    
  }
  else {
    for (c = 0; c < channels; c++) {
      for (i = 0; i < size; i++){
	out(c,i) = ncoeffs_(0) * in(c,i) + state_(c,0);	
	for (j = 0; j < dorder_ - 1; j++){
	  state_(c,j) = ncoeffs_(j+1) * in(c,i) + state_(c,j+1) - dcoeffs_(j+1) * out(c,i);
	}
	for (j = dorder_ - 1; j < stateSize - 1; j++){
	  state_(c,j) = ncoeffs_(j+1) * in(c,i) + state_(c,j+1);
	}
	state_(c,stateSize - 1) = ncoeffs_(order_-1) * in(c,i);
      }
    }
    
  }
  out *= gain;
}

