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

#include "../common_source.h"
#include "Delay.h"

using std::ostringstream;
using std::vector;

using namespace Marsyas;

//#define MTLB_DBG_LOG

static inline mrs_natural	wrapCursor (mrs_natural unwrappedCursor, mrs_natural cursorMask)
{
  // add delay line length to have a sort-of dealing with negative indices as well; should be a while loop really
  return (unwrappedCursor + (cursorMask+1)) & cursorMask;
}

static inline mrs_real	getValue (mrs_natural obs, mrs_real index, mrs_realvec& buffer, mrs_natural cursorMask)
{
  mrs_natural integer = (mrs_natural)index + ((index < 0)? -1 : 0);
  mrs_real	frac	= index - integer;
  mrs_real	retVal	= buffer(obs, wrapCursor (integer, cursorMask));

  return retVal + frac * (buffer(obs, wrapCursor (integer+1, cursorMask))-retVal);
}

Delay::Delay(mrs_string name):MarSystem("Delay",name)
{

  delayInSamples_.create(0);
  writeCursor_	= 0;

  cursorMask_	= 1;

  addControls();
}


Delay::~Delay()
{
}


MarSystem*
Delay::clone() const
{
  return new Delay(*this);
}

Delay::Delay(const Delay& a) : MarSystem(a)
{
  addControls();
  maxDelayLengthInSamples_ = getctrl("mrs_real/maxDelaySamples")->to<mrs_real>(); // maximum delay in samples
  delayInSamples_	=  getctrl("mrs_realvec/delaySamples")->to<mrs_realvec>(); // delay in samples
}

void
Delay::addControls()
{
  mrs_realvec tmp(1);
  tmp(0)	= 0;
  addctrl("mrs_real/maxDelaySamples", 32.0); // maximum delay in samples
  addctrl("mrs_real/maxDelaySeconds", 0.0); // maximum delay in seconds
  addctrl("mrs_real/delaySamples", 0.0); // delay in samples
  addctrl("mrs_real/delaySeconds", 0.0); // delay in seconds
  addctrl("mrs_realvec/delaySamples", tmp); // delay in samples
  addctrl("mrs_realvec/delaySeconds", tmp); // delay in samples
  setctrlState("mrs_real/maxDelaySamples", true);
  setctrlState("mrs_real/maxDelaySeconds", true);
  setctrlState("mrs_real/delaySeconds", true);
  setctrlState("mrs_real/delaySamples", true);
  setctrlState("mrs_realvec/delaySeconds", true);
  setctrlState("mrs_realvec/delaySamples", true);
}


void
Delay::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)
  MRSDIAG("Delay.cpp - Delay:myUpdate");

  // for the following controls, check whether they have changed and update dependent controls accordingly
  if (samples2Seconds (maxDelayLengthInSamples_) != getctrl ("mrs_real/maxDelaySeconds")->to<mrs_real>())
  {
    maxDelayLengthInSamples_	= seconds2Samples (getctrl ("mrs_real/maxDelaySeconds")->to<mrs_real>());
    setctrl("mrs_real/maxDelaySamples", maxDelayLengthInSamples_);
    buffer_.stretch (getctrl("mrs_natural/inObservations")->to<mrs_natural>(),
                     nextPowOfTwo((mrs_natural(.1+ceil(maxDelayLengthInSamples_))+1)));
    buffer_.setval(0);
  }
  if (maxDelayLengthInSamples_ != getctrl ("mrs_real/maxDelaySamples")->to<mrs_real>())
  {
    maxDelayLengthInSamples_	= getctrl ("mrs_real/maxDelaySamples")->to<mrs_real>();
    setctrl("mrs_real/maxDelaySeconds", samples2Seconds (maxDelayLengthInSamples_));
    buffer_.stretch (getctrl("mrs_natural/inObservations")->to<mrs_natural>(),
                     nextPowOfTwo((mrs_natural(.1+ceil(maxDelayLengthInSamples_))+1)));
    buffer_.setval(0);
  }

  if (samples2Seconds (singleDelayInSamples_) != getctrl ("mrs_real/delaySeconds")->to<mrs_real>())
  {
    mrs_realvec tmp(1);
    singleDelayInSamples_	= seconds2Samples (getctrl ("mrs_real/delaySeconds")->to<mrs_real>());
    MRSASSERT(singleDelayInSamples_ >= 0);
    MRSASSERT(singleDelayInSamples_ <= maxDelayLengthInSamples_);

    setctrl("mrs_real/delaySamples", singleDelayInSamples_);

    // update vector
    tmp(0)					= singleDelayInSamples_;
    setctrl("mrs_realvec/delaySamples", tmp);
  }
  if (singleDelayInSamples_ != getctrl ("mrs_real/delaySamples")->to<mrs_real>())
  {
    mrs_realvec tmp(1);
    singleDelayInSamples_	= getctrl ("mrs_real/delaySamples")->to<mrs_real>();
    MRSASSERT(singleDelayInSamples_ >= 0);
    MRSASSERT(singleDelayInSamples_ <= maxDelayLengthInSamples_);

    setctrl("mrs_real/delaySeconds", samples2Seconds (singleDelayInSamples_));

    // update vector
    tmp(0)					= singleDelayInSamples_;
    setctrl("mrs_realvec/delaySamples", tmp);
  }

  if (delayInSamples_ != getctrl ("mrs_realvec/delaySamples")->to<mrs_realvec>())
  {
    delayInSamples_	= getctrl ("mrs_realvec/delaySamples")->to<mrs_realvec>();
    setctrl("mrs_realvec/delaySeconds", samples2Seconds (delayInSamples_));
  }
  if (samples2Seconds (delayInSamples_) != getctrl ("mrs_realvec/delaySeconds")->to<mrs_realvec>())
  {
    delayInSamples_	= seconds2Samples (getctrl ("mrs_realvec/delaySeconds")->to<mrs_realvec>());
    setctrl("mrs_realvec/delaySamples", delayInSamples_);
  }

  // allocate the delay line
  buffer_.stretch (getctrl("mrs_natural/inObservations")->to<mrs_natural>(),
                   nextPowOfTwo((mrs_natural(.1+ceil(maxDelayLengthInSamples_))+1)));
  cursorMask_	= buffer_.getCols () - 1;		// to ensure an efficient wrap around, buffer
  //length will be a power of two
  if (prevDelayInSamples_.getSize () != delayInSamples_.getSize ())
  {
    // only do this update if needed...
    ctrlIncrement_.stretch (delayInSamples_.getSize (), delayInSamples_.getCols ());
    prevDelayInSamples_	= delayInSamples_;

    // initialize the delay line
    buffer_.stretch (getctrl("mrs_natural/inObservations")->to<mrs_natural>(),
                     nextPowOfTwo((mrs_natural(.1+ceil(maxDelayLengthInSamples_))+1)));
    buffer_.setval(0);
    writeCursor_	= 0;

    // set output names
    vector<mrs_string> indiChannels	= stringSplit (getctrl("mrs_string/inObsNames")->to<mrs_string>(), ",");
    ostringstream	outNames;
    for (mrs_natural c = 0; c < getctrl("mrs_natural/inObservations")->to<mrs_natural>(); ++c)
      for (mrs_natural i = 0; i < delayInSamples_.getSize (); ++i)
      {
        outNames << indiChannels.at(c) << "-delay_" << i << ",";
      }
    setctrl("mrs_string/onObsNames", outNames.str());
  }


  setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples"));
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));
  setctrl("mrs_natural/onObservations",
          (mrs_natural) delayInSamples_.getSize () * getctrl("mrs_natural/inObservations")->to<mrs_natural>());

}


void
Delay::myProcess(realvec& in, realvec& out)
{
  mrs_natural k, numDelayLines = delayInSamples_.getSize ();
  mrs_natural o,t;
  // first, get the interpolated delay update (linear interpolation only)
  getLinearInterPInc (prevDelayInSamples_, delayInSamples_, ctrlIncrement_, inSamples_);

#ifdef MARSYAS_MATLAB
#ifdef MTLB_DBG_LOG
  MATLAB_PUT(in, "in");
  MATLAB_EVAL("figure(41),subplot(211),plot(in'),axis('tight'),grid on, title('in')");
#endif
#endif

  for (t = 0; t < inSamples_; t++)
  {
    for (o=0; o < inObservations_; o++)
    {
      // write new sample to buffer
      buffer_(o, writeCursor_)	= in(o,t);
      for (k = 0; k < numDelayLines; k++)
      {
        // read sample from buffer
        out(k+o*numDelayLines,t) = getValue (o, writeCursor_ - (prevDelayInSamples_(k) + t*ctrlIncrement_(k)), buffer_, cursorMask_);
      }

    }
    writeCursor_	= wrapCursor (++writeCursor_, cursorMask_);
  }

  prevDelayInSamples_	= delayInSamples_;


#ifdef MARSYAS_MATLAB
#ifdef MTLB_DBG_LOG
  MATLAB_PUT(out, "out");
  MATLAB_EVAL("figure(41),subplot(212),plot(out'),axis('tight'),grid on, title('out')");
#endif
#endif
}

void Delay::getLinearInterPInc (const mrs_realvec startVal, const mrs_realvec stopVal,
                                mrs_realvec &incVal, const mrs_natural numSamples)
{
  incVal	= (stopVal - startVal);
  incVal	/= (1.0*numSamples);
}

mrs_natural	Delay::nextPowOfTwo (mrs_natural value)
{
  mrs_natural    order = 0;

  while (value>>order)
    order++;

  if (!order)
    return value;

  if (!(value%(1<<(order-1))))
    order--;

  order	= (order < 1)? 1 : order;

  return (1<<(order));
}

mrs_real Delay::samples2Seconds (mrs_real samples)
{
  if (israte_ > 0)
    return samples / israte_;
  else
    return 0;
}
mrs_real Delay::seconds2Samples (mrs_real seconds)
{
  return seconds * israte_;
}
mrs_realvec Delay::samples2Seconds (mrs_realvec samples)
{
  for (mrs_natural i = 0; i < samples.getSize (); ++i)
    samples(i)	= samples(i)/ israte_;

  return samples;
}
mrs_realvec Delay::seconds2Samples (mrs_realvec seconds)
{
  for (mrs_natural i = 0; i < seconds.getSize (); ++i)
    seconds(i)	= seconds(i) * israte_;

  return seconds;
}
