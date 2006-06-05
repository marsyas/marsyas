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


Filter::Filter(string name)
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
  addDefaultControls();
  
  norder_ = 2;
  dorder_ = 1;
  channels_ = 1;
  order_ = 2;
  ncoeffs_.create(norder_);
  dcoeffs_.create(dorder_);
  state_.create(channels_,order_-1);
  ncoeffs_(0) = 1.0f;
  dcoeffs_(0) = 1.0f;
  addctrl("realvec/ncoeffs", ncoeffs_);
  addctrl("realvec/dcoeffs", dcoeffs_);
  addctrl("real/fgain", fgain_);
  addctrl("natural/stateUpdate", natural(0));
  addctrl("realvec/state", state_);
  
  setctrlState("realvec/ncoeffs", true);
  setctrlState("realvec/dcoeffs", true);
  setctrlState("realvec/state", true);
  
  update();
}


void Filter::update()
{
  MRSDIAG("Filter.cpp - Filter:update");
  
  setctrl("natural/onSamples", getctrl("natural/inSamples"));
  setctrl("natural/onObservations", getctrl("natural/inObservations"));
  setctrl("real/osrate", getctrl("real/israte"));
  
  if (getctrl("realvec/ncoeffs").toVec().getSize() != norder_)
    {
      ncoeffs_.create(getctrl("realvec/ncoeffs").toVec().getSize());
      norder_ = ncoeffs_.getSize();
      order_ = (norder_ > dorder_) ? norder_ : dorder_;
      channels_ = getctrl("natural/inObservations").toNatural();
      state_.create(channels_,order_-1);
      setctrl("realvec/state", state_);
    }
  
  if (getctrl("realvec/dcoeffs").toVec().getSize() != dorder_)
    {

      dcoeffs_.create(getctrl("realvec/dcoeffs").toVec().getSize());
      dorder_ = dcoeffs_.getSize();
      order_ = (norder_ > dorder_) ? norder_ : dorder_;
      channels_ = getctrl("natural/inObservations").toNatural();
      state_.create(channels_,order_-1);
      setctrl("realvec/state", state_);
    }
  
  if (getctrl("natural/inObservations").toNatural() != channels_)
    {
      channels_ = getctrl("natural/inObservations").toNatural();
      state_.create(channels_,order_-1);
    }
  
  ncoeffs_ = getctrl("realvec/ncoeffs").toVec();
  dcoeffs_ = getctrl("realvec/dcoeffs").toVec();
  if (getctrl("natural/stateUpdate").toNatural()) 
    state_ = getctrl("realvec/state").toVec();
  
  real d0 = dcoeffs_(0);
  if (d0 != 1.0) {
    for (natural i = 0; i < dorder_; i++){
      dcoeffs_(i) /= d0;
    }
    
    for (natural i = 0; i < norder_; i++){
      ncoeffs_(i) /= d0;
    }
  }
  
  fgain_ = 1.0f;
  setctrl("real/fgain", 1.0f);
  
  defaultUpdate();
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
  
  natural i,j,c;
  natural size = in.getCols();
  natural stateSize = state_.getCols();
  natural channels = in.getRows();
  
  real gain = getctrl("real/fgain").toReal();
  
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

