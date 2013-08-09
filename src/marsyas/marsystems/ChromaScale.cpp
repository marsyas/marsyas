/*
 *  ChromaScale.cpp
 *  testMood
 *
 *  Created by tsunoo on 09/09/10.
 *  Copyright 2009 Emiru Tsunoo. All rights reserved.
 *
 */

#include "ChromaScale.h"

using std::ostringstream;
using namespace Marsyas;

ChromaScale::ChromaScale(mrs_string name): MarSystem("ChromaScale", name)
{
  addControls();
}

ChromaScale::ChromaScale(const ChromaScale& a): MarSystem(a)
{
}

ChromaScale::~ChromaScale()
{
}

MarSystem*
ChromaScale::clone() const
{
  return new ChromaScale(*this);
}

void ChromaScale::addControls()
{
}

void ChromaScale::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)
  ctrl_onObservations_->setValue(6, NOUPDATE);
  ctrl_onSamples_->setValue(inSamples_, NOUPDATE);
  ctrl_osrate_->setValue(israte_, NOUPDATE);
  ostringstream oss;

  for(mrs_natural o=0; o<ctrl_onObservations_->to<mrs_natural>(); o++) {
    oss << "ChromaScale_" << o << ",";
  }
  ctrl_onObsNames_->setValue(oss.str(), NOUPDATE);
}

void
ChromaScale::myProcess(realvec& in, realvec& out)
{
  mrs_natural i, j, k, tmp;
  mrs_real s, s12;

  if(inSamples_ > 0) {
    for(i=0; i<inSamples_; ++i) {
      for(j=0; j<6; j++) {
        out(j,i) = 0.0;
      }
      s = 0.0;
      for(k=0; k<inObservations_; k++) {
        s += in(k,i);
      }
      s12 = s/12.0;
      if(s > 0) {
        for(j=0; j<6; j++) {
          for(k=0; k<inObservations_; k++) {
            tmp = k+j+1;
            if(tmp >= inObservations_) {
              tmp -= inObservations_;
            }
            out(j,i) += (s12-in(k,i))*(s12-in(tmp,i));
          }
          out(j,i) /= s*s;
        }
      }
    }
  }
}
