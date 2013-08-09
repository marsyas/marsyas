/*
 *  Chroma.cpp
 *  testChroma
 *
 *  Created by tsunoo on 09/05/03.
 *  Copyright 2009 Emiru Tsunoo. All rights reserved.
 *
 */

#include "Chroma.h"

using std::ostringstream;
using namespace Marsyas;

Chroma::Chroma(mrs_string name): MarSystem("Chroma", name)
{
  addControls();
}

Chroma::Chroma(const Chroma& a): MarSystem(a)
{
  ctrl_samplingFreq_ = getctrl("mrs_real/samplingFreq");
  ctrl_lowOctNum_ = getctrl("mrs_natural/lowOctNum");
  ctrl_highOctNum_ = getctrl("mrs_natural/highOctNum");
}

Chroma::~Chroma()
{
}

MarSystem*
Chroma::clone() const
{
  return new Chroma(*this);
}

void
Chroma::addControls()
{
  addControl("mrs_real/samplingFreq", 44100.0, ctrl_samplingFreq_);
  addControl("mrs_natural/lowOctNum", 0, ctrl_lowOctNum_);
  addControl("mrs_natural/highOctNum", 8, ctrl_highOctNum_);
}

void
Chroma::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)
  ctrl_onObservations_->setValue(12, NOUPDATE);
  ctrl_onSamples_->setValue(inSamples_, NOUPDATE);
  ctrl_osrate_->setValue(ctrl_samplingFreq_->to<mrs_real>(), NOUPDATE);
  ostringstream oss;
  mrs_natural i, j, k;
  mrs_real tmpreal;
  for(mrs_natural o=0; o<ctrl_onObservations_->to<mrs_natural>(); o++) {
    oss << "Chroma_" << o << ",";
  }
  ctrl_onObsNames_->setValue(oss.str(), NOUPDATE);

  lowNum_ = ctrl_lowOctNum_->to<mrs_natural>();
  if(lowNum_ < 0) {
    lowNum_ = 0;
  }
  highNum_ = ctrl_highOctNum_->to<mrs_natural>();
  if(highNum_ > 8) {
    highNum_ = 8;
  }

  // memory allocation and initialization
  m_.create(9);
  freq_.create(inObservations_);
  filter_.create(14, inObservations_);
  chord_.create(14);
  chord_(1) = 261.625565; // C4
  chord_(2) = 277.182630; // C♯/D♭4
  chord_(3) = 293.664747; // D4
  chord_(4) = 311.126983; // D♯/E♭4
  chord_(5) = 329.627556; // E4
  chord_(6) = 349.228231; // F4
  chord_(7) = 369.994422; // F♯/G♭4
  chord_(8) = 391.995435; // G4
  chord_(9) = 415.304697; // G♯/A♭4
  chord_(10) = 440.000000; // A4
  chord_(11) = 466.163761; // A♯/B♭4
  chord_(12) = 493.883301; // B4
  chord_(0) = 0.5 * chord_(12);
  chord_(13) = 2.0 * chord_(1);

  for(i=0; i<9; ++i) {
    m_(i) = pow(2.0, (mrs_real)i-3.0);
  }
  for(i=0; i<inObservations_; ++i) {
    freq_(i) = ctrl_samplingFreq_->to<mrs_real>()*(mrs_real)i / (2.0*(mrs_real)(inObservations_-1));
  }

  // create filter
  for(i=1; i<13; ++i) {
    for(k=0; k<inObservations_-1; k++) {
      for(j=lowNum_; j<highNum_+1; j++) {
        if(freq_(k) < m_(j)*chord_(i) && freq_(k+1) >= m_(j)*chord_(i)) {
          filter_(i,k) += (freq_(k+1)-m_(j)*chord_(i)) / (freq_(k+1)-freq_(k));
          filter_(i,k+1) += (m_(j)*chord_(i)-freq_(k)) / (freq_(k+1)-freq_(k));
        }
        if((m_(j)*chord_(i)+m_(j)*chord_(i-1))/2.0 < freq_(k) && freq_(k) <= (m_(j)*chord_(i+1)+m_(j)*chord_(i))/2.0) {
          filter_(i,k) += 1.0;
        }
      }
    }
  }
  for(k=0; k<inObservations_; k++) {
    tmpreal = 0.0;
    for(i=1; i<13; ++i) {
      tmpreal += filter_(i,k);
    }
    if(tmpreal > 0) {
      for(i=1; i<13; ++i) {
        filter_(i,k) /= tmpreal;
      }
    }
  }

}

void
Chroma::myProcess(realvec& in, realvec& out)
{
  mrs_natural i, j, k;

  if(inSamples_ > 0) {
    for(j=0; j<12; j++) {
      for(i=0; i<inSamples_; ++i) {
        out(j,i) = 0;
      }
    }
    for(i=0; i<inSamples_; ++i) {
      for(j=1; j<13; j++) {
        for(k=0; k<inObservations_; k++) {
          out(j-1, i) += filter_(j,k)*in(k,i);//12.0;
        }
      }
    }
  }

}
