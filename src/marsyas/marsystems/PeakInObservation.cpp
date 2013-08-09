#include "PeakInObservation.h"

using std::ostringstream;
using namespace Marsyas;

PeakInObservation::PeakInObservation(mrs_string inName)
  :MarSystem("PeakInObservation",inName)
{
  addControls();
}

PeakInObservation::PeakInObservation(const PeakInObservation& inToCopy)
  :MarSystem(inToCopy)
{
  ctrl_HystLength_ = getctrl("mrs_natural/HystLength");
  ctrl_HystFactor_ = getctrl("mrs_real/HystFactor");

  HystLength_ = inToCopy.HystLength_;
  HystFactor_ = inToCopy.HystFactor_;
}

PeakInObservation::~PeakInObservation() {}

MarSystem* PeakInObservation::clone() const
{
  return new PeakInObservation(*this);
}

void PeakInObservation::addControls()
{
  addctrl("mrs_natural/HystLength",10,ctrl_HystLength_);
  addctrl("mrs_real/HystFactor",2.f,ctrl_HystFactor_);

  ctrl_HystLength_->setState(true);
  ctrl_HystFactor_->setState(true);

  HystLength_ = 10;
  HystFactor_ = 2.f;
}

void PeakInObservation::myUpdate(MarControlPtr inSender)
{
  MarSystem::myUpdate(inSender);

  if (ctrl_HystLength_->to<mrs_natural>() > 0 &&
      ctrl_HystFactor_->to<mrs_real>() > 1.f)
  {
    HystLength_ = ctrl_HystLength_->to<mrs_natural>();
    HystFactor_ = ctrl_HystFactor_->to<mrs_real>();
  } else
  {
    // Foutmelding
  }
}

void PeakInObservation::myProcess(realvec& inVec, realvec& outVec)
{
  // (!!) Should be simplified
  outVec.setval(0.f);

  //int nmin = 0;
  mrs_real vmin = inVec(0);
  int nmax = 0;
  mrs_real vmax = inVec(0);

  int nthresh = 0;
  bool theValid = true;
  bool theMaxFlag = true;

  for (mrs_natural n = 1; n < inVec.getSize(); n++) {
    if (theMaxFlag)
      if (inVec(n) > vmax) {
        // Zone 1: [min hysteresis, max]
        vmax = inVec(n);
        nmax = n;
        nthresh = n;
        theValid = true;

        vmin = vmax;
        //nmin = nmax;
      } else {
        if (inVec(n)<vmax/HystFactor_ && nmax!=0) {
          // Zone 3: [max hysteresis, min]

          if ((mrs_natural)n > nthresh + HystLength_) {
            // Maximum was WIDE ENOUGH
            if (theValid) {
              outVec(nmax) = vmax;
              theMaxFlag = false;
            } else {
              //Search for new maximum
              vmax = inVec(n);
              nmax = n;
              nthresh = n;
              theValid = true;

              vmin = vmax;
              //nmin = nmax;
            }

          } else {
            // Maximum was TOO SMALL
            if (inVec(n) < vmin) {
              vmin = inVec(n);
              //nmin = n;
            }
          }
        } else {
          // Zone 2: [max, max hysteresis]
          if (nthresh != (mrs_natural)n-1) {
            theValid = false;
            if ((mrs_natural)n > nthresh + HystLength_) {
              // Search for new maximum
              vmax = inVec(n);
              nmax = n;
              nthresh = n;
              theValid = true;

              vmin = vmax;
              //nmin = nmax;
            }
          } else
            nthresh = n;
        }
      }
    else if (inVec(n) < vmin) {
      vmin = inVec(n);
      //nmin = n;
    } else if (inVec(n) > vmin*HystFactor_) {
      vmax = inVec(n);
      nmax = n;
      nthresh = 0;

      vmin = vmax;
      //nmin = nmax;
      theValid = true;
      theMaxFlag = true;
    }
  }
}
