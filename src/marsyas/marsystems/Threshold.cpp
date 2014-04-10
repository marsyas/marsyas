#include "Threshold.h"

namespace Marsyas {

Threshold::Threshold(const string & name):
  MarSystem("Threshold", name)
{
  addControl("mrs_real/value", 0.0);
  setControlState("mrs_real/value", true);
}

Threshold::Threshold(const Threshold & other):
  MarSystem(other)
{
}

void Threshold::myUpdate( MarControlPtr )
{
  m_threshold = getControl("mrs_real/value")->to<mrs_real>();

  ctrl_onObservations_->setValue(1, NOUPDATE);
  ctrl_onSamples_->setValue(inSamples_, NOUPDATE);
  ctrl_osrate_->setValue(israte_, NOUPDATE);
}

void Threshold::myProcess( realvec & in, realvec & out )
{
  if (!inSamples_ || !inObservations_)
    return;

  for(mrs_natural s = 0; s < inSamples_; ++s)
  {
    mrs_natural sum = 0;
    for(mrs_natural o=0; o < inObservations_; o++)
    {
      if (in(o,s) > m_threshold)
        ++sum;
    }
    out(0,s) = sum;
  }
}

}
