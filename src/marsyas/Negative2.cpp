#include "Negative2.h"

using namespace std;
using namespace Marsyas;

Negative2::Negative2(string inName)
	:MarSystem("Negative2",inName){}

Negative2::~Negative2(){}

MarSystem* Negative2::clone() const
{
  return new Negative2(*this);
}

void Negative2::myProcess(realvec& inVec, realvec& outVec)
{
	for (t = 0; t < inSamples_; t++)
		for (o = 0; o < inObservations_; o++)
			outVec(o,t) = -inVec(o,t);
}
