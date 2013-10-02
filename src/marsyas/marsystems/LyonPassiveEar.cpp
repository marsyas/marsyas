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


#include "LyonPassiveEar.h"
#include "../common_source.h"

#include "Series.h"
#include "Filter.h"
#include "Cascade.h"
#include "HalfWaveRectifier.h"
#include <marsyas/basis.h>
#include <sstream>

using std::ostringstream;
using std::abs;

using namespace Marsyas;


namespace Marsyas
{
/**
\class LyonAgc
\ingroup Processing
\brief Automatic Gain control specific for Lyon's Passive Ear

- \b mrs_natural/numBands [wr] : number of filter channels in the passive ear

\see LyonPassiveEar
*/


class LyonAgc: public MarSystem
{
private:
  static mrs_real lyonEpsilonFromTauFS (mrs_real tau, mrs_real fs)
  {
    return 1.0 - exp (-1/tau/fs);
  }
  // copied and reformatted from auditory toolbox
  static void agc (const mrs_realvec input, mrs_realvec &output, mrs_realvec &state, mrs_real epsilon, mrs_real target, mrs_natural n)
  {
    mrs_natural	i;
    mrs_real f,
             stateLimit				= 1. - 1e-1;
    mrs_real oneMinusEpsOverThree	= (1.0 - epsilon)/3.0;
    mrs_real epsOverTarget			= epsilon/target;
    mrs_real prevState				= state(0);

    for ( i = 0; i < n-1; ++i)
    {
      output(i)	= fabs (input(i) * (1.0 - state(i)));
      f			= output(i) * epsOverTarget + oneMinusEpsOverThree * (prevState + state(i) + state(i+1));

      if (f > stateLimit)
        f = stateLimit;

      prevState	= state(i);
      state(i)	= f;
    }

    // do the last iteration with a different state input
    output(i)	= fabs (input(i) * (1.0 - state(i)));
    f			= output(i) * epsOverTarget + oneMinusEpsOverThree * (prevState + state(i) + state(i));

    if (f > stateLimit)
      f = stateLimit;

    state(i)	= f;
  };

  void myUpdate(MarControlPtr sender)
  {
    /// Use the default MarSystem setup with equal input/output stream format.
    MarSystem::myUpdate(sender);

    const mrs_natural	nStates	= 4;
    mrs_real			fs		= getctrl ("mrs_real/israte")->to<mrs_real>();
    mrs_natural			nBands	= getctrl ("mrs_natural/numBands")->to<mrs_natural>();

    // allocate vectors
    agcParms_.create(2, nStates);
    state_.create(nBands, nStates);
    tmpOut_.create(nBands, 1);

    // set targets and epsilons
    agcParms_(0,0)	= .0032;
    agcParms_(0,1)	= .0016;
    agcParms_(0,2)	= .0008;
    agcParms_(0,3)	= .0004;
    agcParms_(1,0)	= lyonEpsilonFromTauFS (.64, fs);
    agcParms_(1,1)	= lyonEpsilonFromTauFS (.16, fs);
    agcParms_(1,2)	= lyonEpsilonFromTauFS (.04, fs);
    agcParms_(1,3)	= lyonEpsilonFromTauFS (.01, fs);

    // initialize
    state_.setval (0);
  };

  mrs_realvec	agcParms_,
              state_,
              tmpOut_;

  friend class LyonPassiveEar;

public:
  LyonAgc(std::string name):MarSystem("LyonAgc", name)
  {
    addControls ();
  };
  ~LyonAgc() {};

  void addControls()
  {
    addctrl("mrs_natural/numBands", 1);
    setctrlState("mrs_natural/numBands", true);
  }

  MarSystem* clone() const
  {
    return new LyonAgc(*this);
  };

  void myProcess(realvec& in, realvec& out)
  {
    // outer agc loop (compare file agc.c from the auditory toolbox)
    for (mrs_natural t = 0; t < inSamples_; t++)
    {
      mrs_natural i,
                  nStages = agcParms_.getCols (),
                  nRows	= in.getRows ();
      mrs_realvec state;
      in.getCol (t, tmpOut_);

      for (i = 0; i < nStages; ++i)
      {
        state_.getCol (i, state);
        agc (tmpOut_, tmpOut_, state, agcParms_(1,i), agcParms_(0,i), nRows);
        state_.setCol (i,state); // update agc state
      }
      out.setCol (t, tmpOut_);
    }
  };
};

/**
\class LyonChannelDiff
\ingroup Processing
\brief Channel Difference specific for Lyon's Passive Ear

- \b mrs_natural/numBands [wr] : number of filter channels in the passive ear

\see LyonChannelDiff
*/


class LyonChannelDiff: public MarSystem
{
private:

  void myUpdate(MarControlPtr sender)
  {

    /// Use the default MarSystem setup with equal input/output stream format.
    MarSystem::myUpdate(sender);

    numBands_	= getctrl ("mrs_natural/numBands")->to<mrs_natural>();

    //   allocate memory
    procBuf1_.create(numBands_-1,1);
    procBuf2_.create(numBands_-1,1);
  };

  mrs_realvec	procBuf1_,
              procBuf2_;
  mrs_natural	numBands_;
public:
  LyonChannelDiff(std::string name):MarSystem("LyonChannelDiff", name)
  {
    addControls ();
  };
  ~LyonChannelDiff() {};

  void addControls()
  {
    addctrl("mrs_natural/numBands", 1);
    setctrlState("mrs_natural/numBands", true);
  }

  MarSystem* clone() const
  {
    return new LyonChannelDiff(*this);
  };

  void myProcess(realvec& in, realvec& out)
  {

    mrs_natural t;
    for (t = 0; t < inSamples_; t++)
    {
      in.getSubMatrix (0,t, procBuf1_);
      in.getSubMatrix (1,t, procBuf2_);
      procBuf1_	-=procBuf2_;			// calc the difference over the filter channels
      out.setSubMatrix (1,t,procBuf1_);
      out(0,t)	= in(0,t);				// leave the first element the same
    }
  };
};

/**
\class LyonZeroOutPreEmph
\ingroup Processing
\brief set the first two filter channels (preemphasis) to zero

This MarSystem has no extra controls.

\see LyonPassiveEar
*/


class LyonZeroOutPreEmph: public MarSystem
{
private:

  void myUpdate(MarControlPtr sender)
  {

    /// Use the default MarSystem setup with equal input/output stream format.
    MarSystem::myUpdate(sender);
  };

public:
  LyonZeroOutPreEmph(std::string name):MarSystem("LyonZeroOutPreEmph", name)
  {
  };
  ~LyonZeroOutPreEmph() {};

  MarSystem* clone() const
  {
    return new LyonZeroOutPreEmph(*this);
  };

  void myProcess(realvec& in, realvec& out)
  {
    mrs_natural t,o;
    for (t = 0; t < inSamples_; t++)
    {
      out(0,t)	= 0;
      out(1,t)	= 0;
      for (o = 2; o < onObservations_; o++)
        out(o,t)	= in(o,t);
    }
  };
};
}//namespace Marsyas



////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

LyonPassiveEar::LyonPassiveEar(mrs_string name)
  : MarSystem("LyonPassiveEar", name),
    fs_ (0),
    currDecimState_(0),
    numFilterChannels_ (0),
    passiveEar_ (0)
{
  addControls();
}


LyonPassiveEar::~LyonPassiveEar()
{
  if (passiveEar_)
  {
    delete passiveEar_;
    passiveEar_	= 0;
  }
}

MarSystem*
LyonPassiveEar::clone() const
{
  LyonPassiveEar	*newEar = new LyonPassiveEar(*this);
  if (passiveEar_)
    newEar->passiveEar_ = (Series*)passiveEar_->clone ();

  return newEar;
}


void
LyonPassiveEar::addControls()
{
  addctrl("mrs_natural/decimFactor", 1);
  addctrl("mrs_real/earQ", 8.0F);
  addctrl("mrs_real/stepFactor", 0.25F);
  addctrl("mrs_bool/channelDiffActive", true);
  addctrl("mrs_bool/agcActive", true);
  addctrl("mrs_real/decimTauFactor", 3.0F);

  addctrl("mrs_realvec/centerFreqs", centerFreqs_);

  setctrlState("mrs_natural/decimFactor", true);
  setctrlState("mrs_real/earQ", true);
  setctrlState("mrs_real/stepFactor", true);
  setctrlState("mrs_bool/channelDiffActive", true);
  setctrlState("mrs_bool/agcActive", true);
  setctrlState("mrs_real/decimTauFactor", true);

  setctrlState("mrs_realvec/centerFreqs", false);
}

void
LyonPassiveEar::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)

  if (!setParametersIntern ())
  {
    this->updateControlsIntern ();
    return;
  }

  MRSDIAG("LyonPassiveEar.cpp - LyonPassiveEar:myUpdate");

  //FilterBank creation
  // variable names more or less directly from matlab implementation...
  const mrs_real	Eb				= 1000.0;
  const mrs_real	EarZeroOffset	= 1.5;
  const mrs_real	EarSharpness	= 5.0;
  const mrs_real	EarPremphCorner	= 300.0;
  mrs_natural		i,numChans;
  ostringstream	name;
  mrs_realvec		a(3),
                b(3);		/// filter coeffs; we will use second order filters only - these are the coefficients
  mrs_real		lowFreq,
              topFreq			= .5 * getctrl ("mrs_real/israte")->to<mrs_real>();
  Cascade			*filterBank		= 0;

  //% Find top frequency, allowing space for first cascade filter.
  //	topf = fs/2.0;
  //  topf = topf - (sqrt(topf^2+Eb^2)/EarQ*StepFactor*EarZeroOffset)+ ...
  //	sqrt(topf^2+Eb^2)/EarQ*StepFactor;
  topFreq	= topFreq -
            (sqrt (sqr(topFreq) + sqr(Eb)) / earQ_ * stepFactor_ * EarZeroOffset) +
            sqrt (sqr(topFreq) + sqr(Eb)) / earQ_ * stepFactor_;

  //% Find place where CascadePoleQ < .5
  //	lowf = Eb/sqrt(4*EarQ^2-1);
  //  NumberOfChannels = floor((EarQ*(-log(lowf + sqrt(lowf^2 + Eb^2)) + ...
  //	log(topf + sqrt(Eb^2 + topf^2))))/StepFactor);
  lowFreq		= Eb / sqrt (4 * sqr(earQ_) - 1);
  numChans	= (mrs_natural)(floor ((earQ_ * (-log (lowFreq + sqrt (sqr(lowFreq) + sqr(Eb))) +
                                    log (topFreq + sqrt (sqr(Eb) + sqr(topFreq)))))/stepFactor_) + .1); // add .1 to ensure correct cast...

  //% Now make an array of CenterFreqs..... This expression was derived by
  //	% Mathematica by integrating 1/EarBandwidth(cf) and solving for f as a
  //	% function of filterctrl number.
  //	cn = 1:NumberOfChannels;
  //  CenterFreqs = (-((exp((cn*StepFactor)/EarQ)*Eb^2)/ ...
  //	(topf + sqrt(Eb^2 + topf^2))) + ...
  //	(topf + sqrt(Eb^2 + topf^2))./exp((cn*StepFactor)/EarQ))/2;
  centerFreqs_.create (numChans);
  for (i = 0; i < numChans; ++i)
    centerFreqs_(i) = (-((exp(((i+1)*stepFactor_)/earQ_) * sqr(Eb))/
                         (topFreq + sqrt(sqr(Eb) + sqr(topFreq)))) + (topFreq + sqrt(sqr(Eb) + sqr(topFreq)))/ exp(((i+1)*stepFactor_)/earQ_))*.5;

  // free memory if necessary...
  if (passiveEar_)
  {
    delete passiveEar_;
    passiveEar_	= 0;
  }
  // now create the processing chain
  passiveEar_ = new Series ("LyonPassiveEar");
  passiveEar_->addMarSystem (filterBank = new Cascade("LyonFilterBank"));
  passiveEar_->addMarSystem (new HalfWaveRectifier("hwr1"));
  passiveEar_->addMarSystem (new LyonZeroOutPreEmph("ZeroOut"));
  if (agcActive_)
    passiveEar_->addMarSystem (new LyonAgc("agc"));
  if (channelDiffActive_)
  {
    passiveEar_->addMarSystem (new LyonChannelDiff("differ"));
    passiveEar_->addMarSystem (new HalfWaveRectifier("hwr2"));
  }

  if (decimFactor_ > 1)
  {
    b(0)	= b(1) = 0;
    b(2)	= a(0) = 1;
    a(1)	= -2.0*(1-LyonAgc::lyonEpsilonFromTauFS (decimFactor_/fs_*decimTauFactor_, fs_));
    a(2)	= sqr(a(1)/(-2.0));
    b		*= lyonSetGain (b, a, 1.0, 0, fs_);
  }
  else
  {
    // otherwise set the filter to "bypass"
    b(0)	= a(0)	= 1;
    b(1)	= b(2)  = a(1)	= a(2)	= 0;
  }

  passiveEar_->addMarSystem (lyonCreateFilter (b,a,"decim"));

  //% Finally, let's design the front filters.
  //	front(1,:) = SetGain([0 1 -exp(-2*pi*EarPremphCorner/fs) 0 0], 1, fs/4, fs);
  //  topPoles = SecondOrderFilter(topf,CascadePoleQ(1), fs);
  //  front(2,:) = SetGain([1 0 -1 topPoles(2:3)], 1, fs/4, fs);
  b(0)	= a(1) = a(2) = 0;
  b(1)	= a(0) = 1;
  b(2)	= -exp(-TWOPI*EarPremphCorner/fs_);
  b		*= lyonSetGain (b, a, 1.0, fs_*.25, fs_);

  name.str("");
  name << "front_" << 0;
  filterBank->addMarSystem(lyonCreateFilter (b, a, name.str()));		// preemphasis 1

  b(0)	= 1;
  b(1)	= 0;
  b(2)	= -1;
  a		= lyonSecondOrderFilter (topFreq, centerFreqs_(0)/(sqrt(sqr(centerFreqs_(0)) + sqr(Eb))/earQ_), fs_);
  b		*= lyonSetGain (b, a, 1.0F, fs_*.25F, fs_);

  name.str("");
  name << "front_" << 1;
  filterBank->addMarSystem(lyonCreateFilter (b, a, name.str()));		// preemphasis 2

  for (i = 0; i < numChans; ++i)
  {
    mrs_real EarBandwidth, CascadeZeroCF, CascadeZeroQ, CascadePoleCF, CascadePoleQ;

    //% OK, now we can figure out the parameters of each stage filter.
    //	EarBandwidth = sqrt(CenterFreqs.^2+Eb^2)/EarQ;
    //  CascadeZeroCF = CenterFreqs +	EarBandwidth * StepFactor * EarZeroOffset;
    //  CascadeZeroQ = EarSharpness*CascadeZeroCF./EarBandwidth;
    //  CascadePoleCF = CenterFreqs;
    //  CascadePoleQ = CenterFreqs./EarBandwidth;
    EarBandwidth	= sqrt(sqr(centerFreqs_(i)) + sqr(Eb))/earQ_;
    CascadeZeroCF	= centerFreqs_(i) + EarBandwidth * stepFactor_ * EarZeroOffset;
    CascadeZeroQ	= EarSharpness * CascadeZeroCF / EarBandwidth;
    CascadePoleCF	= centerFreqs_(i);
    CascadePoleQ	= centerFreqs_(i)/EarBandwidth;

    // compute filter coeffs
    b	= lyonSecondOrderFilter (CascadeZeroCF, CascadeZeroQ, fs_);
    a	= lyonSecondOrderFilter (CascadePoleCF, CascadePoleQ, fs_);

    //% Now we can set the DC gain of each stage.
    //	dcgain(2:NumberOfChannels)=CenterFreqs(1:NumberOfChannels-1)./ ...
    //	CenterFreqs(2:NumberOfChannels);
    //  dcgain(1) = dcgain(2);
    //  for i=1:NumberOfChannels
    //	filters(i,:) = SetGain(filters(i,:), dcgain(i), 0, fs);
    //  end
    b	*= (i == 0) ? lyonSetGain (b, a, centerFreqs_(i)/centerFreqs_(i+1), 0, fs_) :
         lyonSetGain (b, a, centerFreqs_(i-1)/centerFreqs_(i), 0, fs_);

    // create the filter and add it
    name.str("");
    name << "filter_" << i;
    filterBank->addMarSystem(lyonCreateFilter (b, a, name.str()));
  }

  numFilterChannels_	= numChans + 2; // plus two front filters...

  this->updateControlsIntern ();

  // alloc some internal memory
  tmpOut_.create (numFilterChannels_, inSamples_);
  decimOut_.create (numFilterChannels_-2, inSamples_/decimFactor_); // integer division intentionally;
}

void
LyonPassiveEar::myProcess(realvec& in, realvec& out)
{
  if(getctrl("mrs_bool/mute")->to<mrs_bool>()) return;

  // check number of out observations
  mrs_natural	i,
              currCount		= -currDecimState_-1,
               numOutSamples	= (inSamples_+currDecimState_)/decimFactor_;// integer division intended

  MRSASSERT(currDecimState_ <= decimFactor_);

  // update the number of output samples if necessary
  if (onSamples_ != numOutSamples)
    updControl ("mrs_natural/onSamples", numOutSamples);

  decimOut_.stretch (numFilterChannels_-2, numOutSamples);

  // process the series
  passiveEar_->process(in, tmpOut_);

  // post-"processing": do the sample rate decimation and remove the pre-emphasis filters
  for (i = 0; i < numOutSamples; ++i)
  {
    mrs_realvec	decimTmp(numFilterChannels_-2,1);
    currCount	= currCount + decimFactor_;
    MRSASSERT(currCount <= inSamples_);
    tmpOut_.getSubMatrix (2, currCount, decimTmp);
    decimOut_.setSubMatrix (0, i, decimTmp);
  }
  currDecimState_	= inSamples_ - currCount-1;
  out	= decimOut_;
}


/// return true if the filterbank has to be (re-)created and set internal parameters
mrs_bool LyonPassiveEar::setParametersIntern ()
{
  mrs_bool	updateMe			= false;

  updateMe	|= (passiveEar_ == 0);

  // update controls
  if (decimFactor_		!= getctrl ("mrs_natural/decimFactor")->to<mrs_natural>())
  {
    updateMe			= true;
    decimFactor_		= getctrl ("mrs_natural/decimFactor")->to<mrs_natural>();
  }
  if (earQ_				!= getctrl ("mrs_real/earQ")->to<mrs_real>())
  {
    updateMe			= true;
    earQ_				= getctrl ("mrs_real/earQ")->to<mrs_real>();
  }
  if (stepFactor_			!= getctrl ("mrs_real/stepFactor")->to<mrs_real>())
  {
    updateMe			= true;
    stepFactor_			= getctrl ("mrs_real/stepFactor")->to<mrs_real>();
  }
  if (channelDiffActive_	!= getctrl ("mrs_bool/channelDiffActive")->to<mrs_bool>())
  {
    updateMe			= true;
    channelDiffActive_	= getctrl ("mrs_bool/channelDiffActive")->to<mrs_bool>();
  }
  if (agcActive_			!= getctrl ("mrs_bool/agcActive")->to<mrs_bool>())
  {
    updateMe			= true;
    agcActive_			= getctrl ("mrs_bool/agcActive")->to<mrs_bool>();
  }
  if (decimTauFactor_		!= getctrl ("mrs_real/decimTauFactor")->to<mrs_real>())
  {
    updateMe			= true;
    decimTauFactor_		= getctrl ("mrs_real/decimTauFactor")->to<mrs_real>();
  }
  if (fs_					!= getctrl ("mrs_real/israte")->to<mrs_real>())
  {
    updateMe			= true;
    fs_					= getctrl ("mrs_real/israte")->to<mrs_real>();
  }

  return updateMe;
}

void LyonPassiveEar::updateControlsIntern ()
{
  passiveEar_->updControl("mrs_natural/inObservations", getctrl("mrs_natural/inObservations")->to<mrs_natural>());
  passiveEar_->updControl("mrs_natural/inSamples", getctrl("mrs_natural/inSamples")->to<mrs_natural>());
  passiveEar_->updControl("mrs_real/israte", getctrl("mrs_real/israte")->to<mrs_real>());

  ctrl_onSamples_->setValue(inSamples_/decimFactor_); // integer division intended
  ctrl_osrate_->setValue(israte_*1.0/decimFactor_);

  if (numFilterChannels_)
  {
    updControl ("mrs_realvec/centerFreqs", centerFreqs_);
    ctrl_onObservations_->setValue((numFilterChannels_-2)*getctrl("mrs_natural/inObservations")->to<mrs_natural>());

    passiveEar_->setctrl("mrs_natural/onObservations", getctrl("mrs_natural/onObservations")->to<mrs_natural>());
    if (agcActive_)
      passiveEar_->updControl("LyonAgc/agc/mrs_natural/numBands", numFilterChannels_);
    if (channelDiffActive_)
      passiveEar_->updControl("LyonChannelDiff/differ/mrs_natural/numBands", numFilterChannels_);
  }
}

/// compute second order filter coefficients
mrs_realvec LyonPassiveEar::lyonSecondOrderFilter (mrs_real midFreq, mrs_real q, mrs_real sRate)
{
  //function filts = SecondOrderFilter(f,q,fs)
  //cft = f'/fs;
  //rho = exp(- pi * cft ./ q');
  //		  theta = 2 * pi * cft .* sqrt(1-1 ./(4*q'.^2));
  //		  filts = [ ones(size(rho)) -2*rho.*cos(theta) rho.^2 ];
  mrs_realvec	result (3);
  mrs_real cft	= midFreq / sRate,
            rho		= exp (-PI * cft / q);

  result(0)	= 1;
  result(1)	= -2 * rho * cos (TWOPI * cft * sqrt (1 - 1.0/(4*sqr(q))));
  result(2)	= sqr(rho);
  return result;
}

/// compute magnitude at a specific freq from a second order filter
mrs_real LyonPassiveEar::lyonFreqResp (mrs_realvec firCoeffs, mrs_realvec iirCoeffs, mrs_real freq, mrs_real sRate, mrs_bool inDb)
{
  //function mag=FreqResp(filter,f,fs)
  //	cf = exp(i*2*pi*f/fs);
  //mag = (filter(3) + filter(2)*cf + filter(1)*cf.^2) ./ ...
  //	(filter(5) + filter(4)*cf + cf.^2);
  //mag = 20*log10(abs(mag));
  mrs_complex cf	= mrs_complex (cos (TWOPI * freq / sRate), sin (TWOPI * freq / sRate));
  mrs_complex mag	= (firCoeffs(2) + firCoeffs(1) * cf + firCoeffs(0) * (cf*cf)) / (iirCoeffs(2) + iirCoeffs(1) * cf + (cf*cf));
  mrs_real	res = sqrt (sqr(mag.real ()) + sqr (mag.imag ()));

  return inDb? 20.0/log(10.0) * log (res) : res;
}

/// adjust the filter gain
mrs_real LyonPassiveEar::lyonSetGain (mrs_realvec firCoeffs, mrs_realvec iirCoeffs, mrs_real newGain, mrs_real freq, mrs_real sRate)
{
  //function filter = SetGain(filter, desired, f, fs)
  //	oldGain = 10^(FreqResp(filter, f, fs)/20);
  //filter(1:3) = filter(1:3)*desired/oldGain;

  return newGain / lyonFreqResp (firCoeffs, iirCoeffs, freq, sRate, false);
}

/// create a new filter
Filter*		LyonPassiveEar::lyonCreateFilter (mrs_realvec b, mrs_realvec a, mrs_string name)
{
  Filter *filter = new Filter(name);
  filter->setctrl("mrs_realvec/ncoeffs", b);
  filter->setctrl("mrs_realvec/dcoeffs", a);

  return filter;
}
