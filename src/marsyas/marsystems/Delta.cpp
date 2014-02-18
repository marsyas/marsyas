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
}

Delta::Delta(const Delta& a): MarSystem(a)
{
}

MarSystem*
Delta::clone() const
{
  return new Delta(*this);
}

void
Delta::myUpdate(MarControlPtr)
{
  ctrl_onObservations_->setValue(inObservations_, NOUPDATE);
  ctrl_onSamples_->setValue(inSamples_, NOUPDATE);
  ctrl_osrate_->setValue(israte_, NOUPDATE);
  m_memory.create(inObservations_, 1);
}

void
Delta::myProcess(realvec& in, realvec& out)
{
  if (!inSamples_)
    return;

  mrs_natural last_sample = onSamples_ - 1;

  for(mrs_natural o = 0; o < onObservations_; o++)
  {
    out(o, 0) = in(o, 0) - m_memory(o);
  }

  for(mrs_natural s = 1; s < onSamples_; ++s)
  {
    for(mrs_natural o=0; o < onObservations_; o++)
    {
      out(o, s) = in(o, s) - in(o, s-1);
    }
  }

  for(mrs_natural o=0; o < onObservations_; o++)
  {
    m_memory(o) = in(o, last_sample);
  }
}

