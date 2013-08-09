#include "Signum.h"

using namespace std;
using namespace Marsyas;

Signum::Signum(mrs_string inName)
  :MarSystem("Signum",inName)
{
  addControls();
}

Signum::Signum(const Signum& inToCopy)
  :MarSystem(inToCopy) {}

Signum::~Signum() {}

MarSystem* Signum::clone() const
{
  return new Signum(*this);
}

void Signum::addControls() {}

void Signum::myUpdate(MarControlPtr inSender)
{
  MarSystem::myUpdate(inSender);
}

void Signum::myProcess(realvec& inVec, realvec& outVec)
{
  mrs_natural t,o;
  for (o=0; o<inObservations_; o++)
    for (t=0; t<inSamples_; t++)
      outVec(o,t) = inVec(o,t)>0?1:0;
}
