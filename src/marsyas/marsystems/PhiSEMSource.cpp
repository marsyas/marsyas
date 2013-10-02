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
	\class PhiSEMSource
	\ingroup Synthesis
	\brief Creates noise

	Controls:
	- \b mrs_natural/numObjects	: number of objects in the simulation
*/


#include "../common_source.h"
#include "PhiSEMSource.h"

using std::ostringstream;
using namespace Marsyas;

const mrs_real PhiSEMSource::MIN_ENERGY = 0.3;
const mrs_real PhiSEMSource::MAX_ENERGY = 2000;

PhiSEMSource::PhiSEMSource(mrs_string name)
  : MarSystem("PhiSEMSource", name) {
  addControls();
  temp_ = 0.0;
  shakeEnergy_ = 0.0;
  soundLevel_ = 0.0;
  sample_ = 0;
}

PhiSEMSource::PhiSEMSource(const PhiSEMSource& source)
  : MarSystem(source) {
  numObjects_ 	= getctrl("mrs_natural/numObjects");
  systemDecay_	= getctrl("mrs_real/systemDecay");
  soundDecay_	= getctrl("mrs_real/soundDecay");
  baseGain_	= getctrl("mrs_real/baseGain");
}

PhiSEMSource::~PhiSEMSource() {}

MarSystem*
PhiSEMSource::clone() const {
  return new PhiSEMSource(*this);
}

void PhiSEMSource::addControls() {
  //Add specific controls needed by this MarSystem.
  addctrl("mrs_natural/numObjects", 25, numObjects_);
  setctrlState("mrs_natural/numObjects", true);
  addctrl("mrs_real/systemDecay", 0.999, systemDecay_);
  setctrlState("mrs_real/systemDecay", true);
  addctrl("mrs_real/soundDecay", 0.95, soundDecay_);
  setctrlState("mrs_real/soundDecay", true);
  addctrl("mrs_real/baseGain", 20.0, baseGain_);
  setctrlState("mrs_real/baseGain", true);
}

void PhiSEMSource::myUpdate(MarControlPtr sender) {
  setctrl("mrs_natural/numObjects", 	getctrl("mrs_natural/numObjects"));
  setctrl("mrs_real/systemDecay",		getctrl("mrs_real/systemDecay"));
  setctrl("mrs_real/soundDecay",		getctrl("mrs_real/soundDecay"));
  setctrl("mrs_real/baseGain",		getctrl("mrs_real/baseGain"));
  MarSystem::myUpdate(sender);

  mrs_natural num = numObjects_->to<mrs_natural>();
  //gain_ = log(num) * baseGain_->to<mrs_real>() / (mrs_natural)num;
  gain_ = log(num+.0) / log(4.0) * 40.0 / (mrs_natural)num;
}

void PhiSEMSource::myProcess(realvec& /* in */, realvec& out) {
  mrs_natural t,o;
  for (o=0; o < inObservations_; o++ ) {
    for ( t=0; t < inSamples_; t++ ) {
      out(o,t) = computeSample();
    }
  }
}

mrs_real PhiSEMSource::computeSample() {
  mrs_real out = 0.0;
  mrs_natural numObjects	= numObjects_->to<mrs_natural>();
  mrs_natural samplePeriod = (mrs_natural) israte_ ;
  mrs_real systemDecay	= systemDecay_->to<mrs_real>();
  mrs_real soundDecay	= soundDecay_->to<mrs_real>();

  if ( temp_ < TWOPI ) {
    temp_ += (TWOPI / israte_ / 0.05);
    shakeEnergy_ += (1.0 - cos(temp_));
  }
  if ( (++sample_)%(samplePeriod / 4) == 0 ) {
    temp_ = 0;
    sample_ = 0;
  }

  //exponential system decay
  shakeEnergy_ *= systemDecay;

  if ( randomFloat(1024.0) < numObjects ) {
    soundLevel_ += gain_ * shakeEnergy_;
  }
  //output sound is just noise
  out = soundLevel_ * noiseTick();

  //expontential sound decay
  soundLevel_ *= soundDecay;

  return out;
}
/*
	if ( shakeEnergy_ > MIN_ENERGY ) {
		if ( temp < TWO_PI ) {
			temp += (TWO_PI / israte_ / 0.05);
			shakeEnergy += (1.0 - cos(temp));
		}
		if ( i%5050 == 0 ) {
			temp = 0;
		}

		//exponential system decay
		shakeEnergy *= systemDecay;

		if ( randomFloat(1024.0) < numObjects ) {
			soundLevel_ += gain_ * shakeEnergy;
		}
		//output sound is just noise
		out = soundLevel_ * noiseTick();

		//expontential sound decay
		soundLevel_ *= soundDecay;
	}
	return out;
}
*/

mrs_real
PhiSEMSource::noiseTick() {
  mrs_real temp = (mrs_real)(2.0 * rand() / (RAND_MAX + 1.0));
  temp -= 1.0;
  return temp;
}

int
PhiSEMSource::randomInt(int max) {
  int temp = (int)((float)max * (float)rand() / (RAND_MAX + 1.0));
  return temp;
}

mrs_real
PhiSEMSource::randomFloat(mrs_real max) {
  mrs_real temp = (mrs_real)(max * rand() / (RAND_MAX + 1.0));
  return temp;
}
