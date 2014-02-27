#include "Ratio.h"

#include <cmath>
#include <string>

using std::string;

namespace Marsyas {

Ratio::Ratio(std::string name): MarSystem("Ratio", name)
{
  addControl("mrs_string/mode", string());
}

void Ratio::myUpdate(MarControlPtr)
{
  const string & mode_str = getControl("mrs_string/mode")->to<string>();
  if (mode_str == "log")
    m_mode = log;
  else if (mode_str == "log10")
    m_mode = log10;
  else
    m_mode = raw;

  setControl("mrs_natural/onObservations", 1);
  setControl("mrs_natural/onSamples", inSamples_);
}

void Ratio::myProcess(realvec& in, realvec& out)
{
  mrs_real ratio;

  if (inObservations_ < 2)
  {
    double ratio;
    switch (m_mode)
    {
    case raw:
      ratio = 1.0; break;
    case log:
    case log10:
      ratio = 0.0; break;
    }

    for(mrs_natural s = 0; s < inSamples_; ++s)
    {
      out(0,s) = ratio;
    }

    return;
  }

  switch (m_mode)
  {
  case raw:
    for(mrs_natural s = 0; s < inSamples_; ++s)
    {
      ratio = in(0,s);
      for(mrs_natural o = 1; o < inObservations_; ++o)
        ratio /= in(o,s);
      out(0,s) = ratio;
    }
    break;
  case log:
    for(mrs_natural s = 0; s < inSamples_; ++s)
    {
      ratio = in(0,s);
      for(mrs_natural o = 1; o < inObservations_; ++o)
        ratio = in(o,s);
      out(0,s) = std::log(ratio);
    }
    break;
  case log10:
    for(mrs_natural s = 0; s < inSamples_; ++s)
    {
      ratio = in(0,s);
      for(mrs_natural o = 1; o < inObservations_; ++o)
        ratio /= in(o,s);
      out(0,s) = std::log10(ratio);
    }
    break;
  }
}

} // namespace Marsyas
