/*
 *  Delta.cpp
 *  testDelta
 *
 *  Created by tsunoo on 09/05/07.
 *  Copyright 2009 Emiru Tsunoo. All rights reserved.
 *
 */

#include "Delta.h"

using std::ostringstream;
using namespace Marsyas;

Delta::Delta(mrs_string name): MarSystem("Delta", name)
{
  addControls();
}

Delta::Delta(const Delta& a): MarSystem(a)
{
  ctrl_absolute_ = getctrl("mrs_bool/absolute");
  ctrl_sum_ = getctrl("mrs_bool/sum");
  ctrl_normalize_ = getctrl("mrs_bool/normalize");
  ctrl_normSize_ = getctrl("mrs_natural/normSize");
  ctrl_positive_ = getctrl("mrs_bool/positive");
}

Delta::~Delta()
{
}

MarSystem*
Delta::clone() const
{
  return new Delta(*this);
}

void 
Delta::addControls()
{
  addControl("mrs_bool/absolute", false, ctrl_absolute_);
  addControl("mrs_bool/sum", false, ctrl_sum_);
  addControl("mrs_bool/normalize", false, ctrl_normalize_);
  addControl("mrs_natural/normSize", 10, ctrl_normSize_);
  addControl("mrs_bool/positive", false, ctrl_positive_);
}

void 
Delta::myUpdate(MarControlPtr sender)
{
  mrs_natural o;
  (void)sender;
  absolute_ = ctrl_absolute_->to<mrs_bool>();
  sum_ = ctrl_sum_->to<mrs_bool>();
  normalize_ = ctrl_normalize_->to<mrs_bool>();
  size_ = ctrl_normSize_->to<mrs_natural>();
  positive_ = ctrl_positive_->to<mrs_bool>();

  if(sum_){
    ctrl_onObservations_->setValue(1, NOUPDATE);
  } else {
    ctrl_onObservations_->setValue(inObservations_, NOUPDATE);
  }
  ctrl_onSamples_->setValue(inSamples_, NOUPDATE);
  ctrl_osrate_->setValue(israte_, NOUPDATE);

  ostringstream oss;
  for(o=0; o<ctrl_onObservations_->to<mrs_natural>(); o++){
    oss << "Delta_" << o << ",";
  }
  ctrl_onObsNames_->setValue(oss.str(), NOUPDATE);
  
  // memory allocation
  if(normalize_ && inSamples_ == 1){
    delta_.create(inObservations_, size_);
  } else {
    delta_.create(inObservations_, inSamples_);
  }
  prevData_.create(inObservations_, 1);
  if(sum_){
    normVec_.create(1,size_);
  } else {
    normVec_.create(inObservations_, size_);
  }

}

void 
Delta::myProcess(realvec& in, realvec& out)
{
  mrs_natural i, j, k;
  if(inSamples_ > 0){
    for(j=0; j<onObservations_; j++){
      for(i=0; i<onSamples_; ++i){
	out(j,i) = 0.0;
      }
    }

    if(inSamples_ == 1){
      if(normalize_){
	for(j=0; j<inObservations_; j++){
	  for(i=0; i<size_-1; ++i){
	    delta_(j,i) = delta_(j,i+1);
	  }
	}
	for(j=0; j<inObservations_; j++){
	  delta_(j,size_-1) = in(j,0) - prevData_(j,0);
	}
      } else {
	for(j=0; j<inObservations_; j++){
	  delta_(j,0) = in(j,0) = prevData_(j,0);
	}
      }
    } else {
      for(j=0; j<inObservations_; j++){
	delta_(j,0) = in(j,0);
      }
      for(j=0; j<inObservations_; j++){
	for(i=1; i<inSamples_; ++i){
	  delta_(j,i) = in(j,i)-in(j,i-1);
	}
      }
    }
    
    if(positive_){
      for(j=0; j<inObservations_; j++){
	for(i=0; i<delta_.getCols(); ++i){
	  if(delta_(j,i) < 0){
	    delta_(j,i) = 0.0;
	  }
	}
      }
    } else if(absolute_){
      for(j=0; j<inObservations_; j++){
	for(i=0; i<delta_.getCols(); ++i){
	  if(delta_(j,i) < 0){
	    delta_(j,i) = -delta_(j,i);
	  }
	}
      }
    }

    if(inSamples_ == 1){
      for(j=0; j<inObservations_; j++){
	prevData_(j,0) = in(j,0);
      }
    }
    
    if(normalize_){
      if(inSamples_ == 1){
	if(sum_){
	  for(i=0; i<size_; ++i){
	    normVec_(0,i) = 0.0;
	  }
	  for(j=0; j<inObservations_; j++){
	    for(i=0; i<size_; ++i){
	      normVec_(0,i) += delta_(j,i);
	    }
	  }
	} else {
	  normVec_ = delta_;
	}
	normVec_.normObsMinMax();
	for(j=0; j<normVec_.getRows(); j++){
	  out(j,0) = normVec_(j,size_-1);
	}
      } else {
	for(k=0; k<size_/2; k++){
	  for(i=size_/2-k; i<size_; ++i){
	    if(sum_){
	      normVec_(0,i) = 0.0;
	      for(j=0; j<inObservations_; j++){
		normVec_(0,i) += delta_(j,i-size_/2+k);
	      }
	    } else {
	      for(j=0; j<inObservations_; j++){
		normVec_(j,i) = delta_(j,i-size_/2+k);
	      }
	    }
	  }
	  normVec_.normObsMinMax();
	  for(j=0; j<normVec_.getRows(); j++){
	    out(j,k) = normVec_(j,size_/2);
	  }
	}
	for(k=size_/2; k<inSamples_-size_/2; k++){
	  for(i=0; i<size_; ++i){
	    if(sum_){
	      normVec_(0,i) = 0.0;
	      for(j=0; j<inObservations_; j++){
		normVec_(0,i) += delta_(j,i+k-size_/2);
	      }
	    } else {
	      for(j=0; j<inObservations_; j++){
		normVec_(j,i) = delta_(j,i+k-size_/2);
	      }
	    }
	  }
	  normVec_.normObsMinMax();
	  for(j=0; j<normVec_.getRows(); j++){
	    out(j,k) = normVec_(j,size_/2);
	  }
	}
	for(k=inSamples_-size_/2; k<inSamples_; k++){
	  for(i=k-size_/2; i<inSamples_; ++i){
	    if(sum_){
	      normVec_(0,i+size_/2-k) = 0.0;
	      for(j=0; j<inObservations_; j++){
		normVec_(0,i+size_/2-k) += delta_(j,i);
	      }
	    } else {
	      for(j=0; j<inObservations_; j++){
		normVec_(j,i+size_/2-k) = delta_(j,i);
	      }
	    }
	  }
	}
      }
    } else {
      if(sum_){
	for(j=0; j<inObservations_; j++){
	  for(i=0; i<inSamples_; ++i){
	    out(0,i) += delta_(j,i);
	  }
	}
      } else {
	for(j=0; j<inObservations_; j++){
	  for(i=0; i<inSamples_; ++i){
	    out(j,i) = delta_(j,i);
	  }
	}
      }
    }
    
  }
}
	    
