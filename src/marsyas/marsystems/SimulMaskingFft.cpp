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
#include "SimulMaskingFft.h"
#include <algorithm>
//#define MTLB_DBG_LOG

using std::min;
using std::max;
using namespace Marsyas;

static const mrs_natural h2bIdx	= 3;
static const mrs_real	lowFreq	= 80.;
static const mrs_real	upFreq	= 18000.;

static const mrs_real   truncTresh = 10;

static inline mrs_real IntPow (mrs_real a, mrs_natural b)
{
  if (b == 0)
    return 1.;
  mrs_real	dResult	= a;
  while (--b > 0)
    dResult *= a;
  return (dResult < 1e-30)? 0 : dResult;
}

SimulMaskingFft::SimulMaskingFft(mrs_string name):MarSystem("SimulMaskingFft", name)
{
  //Add any specific controls needed by SimulMaskingFft
  //(default controls all MarSystems should have
  //were already added by MarSystem::addControl(),
  //called by :MarSystem(name) constructor).
  //If no specific controls are needed by a MarSystem
  //there is no need to implement and call this addControl()
  //method (see for e.g. Rms.cpp)
  addControls();

  numBands_	= 0;
  freqBounds_	= 0;
  numBands_ = 0;

}

SimulMaskingFft::SimulMaskingFft(const SimulMaskingFft& a) : MarSystem(a)
{
  // For any MarControlPtr in a MarSystem
  // it is necessary to perform this getctrl
  // in the copy constructor in order for cloning to work
  ctrl_SimulMaskingFft_ = getctrl("mrs_real/SimulMaskingFft");
}

SimulMaskingFft::~SimulMaskingFft()
{
  if (freqBounds_)
    delete freqBounds_;
  freqBounds_	= 0;
}

MarSystem*
SimulMaskingFft::clone() const
{
  SimulMaskingFft *New	= new SimulMaskingFft(*this);

  if (this->numBands_ > 0)
  {
    New->freqBounds_	= new FrequencyBands_t [numBands_];
    New->ComputeTables ();
  }
  else
    New->freqBounds_	= 0;


  return New;
}

void
SimulMaskingFft::addControls()
{
  //Add specific controls needed by this MarSystem.
  addctrl("mrs_real/listeningLevelInDbSpl", 92.0);
  setctrlState("mrs_real/listeningLevelInDbSpl", true);
}

void
SimulMaskingFft::myUpdate(MarControlPtr sender)
{
  // no need to do anything SimulMaskingFft-specific in myUpdate
  MarSystem::myUpdate(sender);


  // compute audio samplerate, numBands, and normalization
  audiosrate_	= israte_*(mrs_real)(inObservations_-1)*2;
  barkRes_	= hertz2bark (lowFreq+israte_, h2bIdx)-hertz2bark (lowFreq,h2bIdx); // Delta f / N
  numBands_	= (mrs_natural)((hertz2bark (upFreq, h2bIdx) - hertz2bark (lowFreq, h2bIdx) + .5)/barkRes_);
  normFactor_	= (1<<15)*sqrt(8./3.)*2*20e-6*pow (10.,.05*getctrl("mrs_real/listeningLevelInDbSpl")->to<mrs_real>());

  // alloc memory
  if (numBands_ <= 0)
    return;
  processBuff_.stretch(inObservations_);
  processBuff_.setval (0);
  helpBuff_.stretch(inObservations_);
  helpBuff_.setval (0);
  outerEar_.stretch(inObservations_);
  outerEar_.setval (0);
  barkSpec_.stretch(numBands_);
  barkSpec_.setval (0);
  excPattern_.stretch(numBands_);
  excPattern_.setval (0);
  maskingThresh_.stretch(numBands_);
  maskingThresh_.setval (0);
  intNoise_.stretch(numBands_);
  intNoise_.setval (0);
  slopeSpread_.stretch(numBands_);
  slopeSpread_.setval (0);
  normSpread_.stretch(numBands_);
  normSpread_.setval (0);

  if (freqBounds_)
    delete freqBounds_;
  freqBounds_	= new FrequencyBands_t [numBands_];;

  ComputeTables ();
}


void
SimulMaskingFft::myProcess(realvec& in, realvec& out)
{
  for (mrs_natural t = 0; t < inSamples_; t++)
  {
    in.getCol(t, processBuff_);

    // normalize spectrum
    processBuff_	*= normFactor_;
    processBuff_	*= processBuff_;

    // weight with outer ear transfer function
    processBuff_	*= outerEar_;

    // compute bark spectrum
    GetBandLevels (freqBounds_, barkSpec_, false);

    // add internal noise
    barkSpec_	+= intNoise_;

    // compute spreading function
    CalcSpreading (barkSpec_, excPattern_);

    // apply  masking threshold
    excPattern_	*= maskingThresh_;

    // normalize input spectrum
    in.getCol(t, processBuff_);
    processBuff_	*= normFactor_;
    processBuff_	*= processBuff_;

    // compute difference
    ComputeDifference (out, processBuff_,  t);

  }
}


void
SimulMaskingFft::ComputeDifference (mrs_realvec &out, mrs_realvec &in, mrs_natural t)
{
  mrs_natural i;
  t = 0;

  for (i = 0; i < inObservations_; ++i)
    out(i,t) = 0;

  for (mrs_natural k = 0; k < numBands_; k++)
  {
    mrs_real   fLowFrac    = freqBounds_[k].fLowFreqBound/audiosrate_ * (inObservations_<<1),
               fHighFrac   = freqBounds_[k].fUpFreqBound/audiosrate_ *(inObservations_<<1);
    mrs_natural     iLowBin     = (mrs_natural)ceil (fLowFrac),
                    iHighBin    = (mrs_natural)floor(fHighFrac);
    for (i = iLowBin; i <= iHighBin; ++i)
    {
      if (excPattern_(k) <= 1./truncTresh*in(i))
        out(i,t)	= truncTresh;
      else if (excPattern_(k) >= truncTresh*in(i))
        out(i,t)	= 1./truncTresh;
      else
        out(i,t)	= in(i) / excPattern_(k);
    }
  }
#ifdef MARSYAS_MATLAB
#ifdef MTLB_DBG_LOG
  MATLAB_PUT(in, "a");
  MATLAB_PUT(out, "out");
  MATLAB_PUT(audiosrate_, "fs");
  MATLAB_PUT(normFactor_, "normFactor");
  MATLAB_EVAL("figure(1);clf ;semilogy((1:length(out))*fs/(2*length(out)),sqrt(a)/normFactor, (1:length(out))*fs/(2*length(out)),sqrt(a./out)/normFactor,'g'),grid on, axis([50 4000 1e-4 1])");
  //MATLAB_PUT(excPattern_, "exc");
  //MATLAB_EVAL("figure(2);clf ;plot(exc,'r'),grid on");
#endif
#endif
}

void
SimulMaskingFft::GetBandLevels (FrequencyBands_t *pFrequencyValues, mrs_realvec &bandLevels, mrs_bool bDezibel)
{

  for (mrs_natural i = 0; i < numBands_; ++i)
  {
    mrs_real   fLowFrac    = pFrequencyValues[i].fLowFreqBound/audiosrate_ * (inObservations_<<1),
               fHighFrac   = pFrequencyValues[i].fUpFreqBound/audiosrate_ *(inObservations_<<1);
    mrs_natural     iLowBin     = (mrs_natural)ceil (fLowFrac),
                    iHighBin    = (mrs_natural)floor(fHighFrac);

    fLowFrac            = iLowBin - fLowFrac;
    fHighFrac           = fHighFrac - iHighBin;
    bandLevels(i)   = fLowFrac * processBuff_(max(0L,iLowBin-1));
    bandLevels(i)  += fHighFrac * processBuff_(min((mrs_natural)(inObservations_ - .5),iHighBin+1));
    for (mrs_natural j = iLowBin; j < iHighBin; j++)
      bandLevels(i)  += processBuff_(j);
    if (bDezibel)
    {
      bandLevels(i)   = max ((mrs_real)bandLevels(i), (mrs_real)1e-20);
      bandLevels(i)   = 10./log(10.) * log ((bandLevels(i)));
    }
  }

  return;
}

void
SimulMaskingFft::CalcSpreading (mrs_realvec &bandLevels, mrs_realvec &result)
{
  // this is level dependent adapted from ITU-R BS.1387

  mrs_natural    iBarkj,                 // Masker
                 iBarkk;                 // Maskee

  mrs_real  fTmp1,
            fTmp2,
            fSlope,
            fScale      = sqrt(8./3.),
            fBRes       = barkRes_,//hertz2bark (.5*audiosrate_, h2bIdx)/numBands_,
            *pfEnPowTmp = processBuff_.getData (),
             *pfSlopeUp  = helpBuff_.getData ();
  mrs_real  *pfSlope    = slopeSpread_.getData (),
             *pfNorm     = normSpread_.getData ();

  // initialize pfResult
  result.setval(0);

  fSlope                      = exp ( -fBRes * 2.7 * 2.302585092994045684017991454684364207601101488628772976033);
  fTmp2						= 1.0 / (1.0 - fSlope);
  for (iBarkj = 0; iBarkj < numBands_; iBarkj++)
  {
    pfSlopeUp[iBarkj]       = pfSlope[iBarkj] * pow (bandLevels(iBarkj)*fScale,  .2 * fBRes);
    fTmp1    				= (1.0 - IntPow (fSlope, iBarkj+1)) * fTmp2;
    fTmp2    			    = (1.0 - IntPow(pfSlopeUp[iBarkj], numBands_ - iBarkj)) / (1.0 - pfSlopeUp[iBarkj]);
    if (bandLevels(iBarkj) < 1e-20)
    {
      pfSlopeUp[iBarkj]   = 0;
      pfEnPowTmp[iBarkj]  = 0;
      continue;
    }
    pfSlopeUp[iBarkj]       = exp (0.4 * log (pfSlopeUp[iBarkj]));
    pfEnPowTmp[iBarkj]      = exp (0.4 * log (bandLevels(iBarkj)/(fTmp1 + fTmp2 -1)));
  }
  fSlope                      = exp ( 0.4 * log (fSlope));

  // lower slope
  result(numBands_-1)     = pfEnPowTmp[numBands_-1];
  for (iBarkk = numBands_-2; iBarkk >= 0; iBarkk--)
    result(iBarkk)        = pfEnPowTmp[iBarkk] + result(iBarkk + 1) * fSlope;

  // upper slope
  for (iBarkj = 0; iBarkj < numBands_-1; iBarkj++)
  {
    fSlope                  = pfSlopeUp[iBarkj];
    fTmp1                   = pfEnPowTmp[iBarkj];
    for (iBarkk = iBarkj+1; iBarkk < numBands_; iBarkk++)
    {
      mrs_real  dTmp1       = fTmp1 * fSlope;
      fTmp1               = (dTmp1 < 1e-30)? 0 : (mrs_real)dTmp1;
      result(iBarkk)   += fTmp1;
    }
  }

  // normalization
  for (iBarkk = 0; iBarkk < numBands_; iBarkk++)
  {
    mrs_real  dTmp      = result(iBarkk);
    result(iBarkk)		= sqrt(dTmp) * dTmp * dTmp *pfNorm[iBarkk];
    //result(iBarkk)        = sqrt (result(iBarkk));
    //result(iBarkk)       *= result(iBarkk)*result(iBarkk)*result(iBarkk)*result(iBarkk);
    //result(iBarkk)       *= pfNorm[iBarkk];
  }
  return;
}

void
SimulMaskingFft::ComputeTables ()
{
  mrs_natural i;


  // outer ear transfer function
  {
    for (i = 0; i < inObservations_; ++i)
    {
      mrs_real dTmp;
      mrs_real fkFreq    = i * .5e-3 * audiosrate_ / inObservations_;
      if (fkFreq < 1e-10)
      {
        outerEar_(i)   = 0;
        continue;
      }
      dTmp	= -.2184 * pow (fkFreq, -.8);
      dTmp   += .65 * exp (-.6 * (fkFreq-3.3)*(fkFreq-3.3));
      //outerEar_(i)   = (dTmp > 1e-37)? (mrs_real)dTmp : 0;
      outerEar_(i)  = dTmp - 1e-4 * pow (fkFreq, 3.6);
      if (outerEar_(i) < -12)
        outerEar_(i) = 0;
      else
        outerEar_(i)   = pow (10.,outerEar_(i));
    }
  }

  // frequency bands and spreading
  {
    mrs_real fLowBark = hertz2bark (lowFreq, h2bIdx);
    mrs_real fMaxBark = hertz2bark (.5*audiosrate_-1, h2bIdx);
    for (i = 0; i < numBands_; ++i)
    {
      freqBounds_[i].fLowBarkBound  = min(fMaxBark,fLowBark + i*barkRes_);
      freqBounds_[i].fMidBark       = min((mrs_real)fMaxBark,(mrs_real)freqBounds_[i].fLowBarkBound + (mrs_real).5*barkRes_);
      freqBounds_[i].fUpBarkBound   = min(fMaxBark,freqBounds_[i].fLowBarkBound + barkRes_);

      freqBounds_[i].fLowFreqBound  = bark2hertz (freqBounds_[i].fLowBarkBound, h2bIdx);
      freqBounds_[i].fMidFreq       = bark2hertz (freqBounds_[i].fMidBark, h2bIdx);
      freqBounds_[i].fUpFreqBound   = bark2hertz (freqBounds_[i].fUpBarkBound, h2bIdx);
    }

    for (i = 0; i < numBands_; ++i)
    {
      mrs_natural     cBarkk;
      mrs_real   fAtt    = 1.0,
                 fNorm   = 1.0,
                 fSlope  = pow (10.0, -2.7 * barkRes_);

      slopeSpread_(i) = 24.0 + 230./freqBounds_[i].fMidFreq;
      slopeSpread_(i) = pow (10.,-0.1 * barkRes_ * slopeSpread_(i));

      processBuff_(i)    = 1.0;
      // lower slope
      for (cBarkk = i; cBarkk > 0; cBarkk--)
      {
        mrs_real  dTmp                = fAtt * fSlope;
        fAtt                        = (dTmp < 1e-30) ? 0 : (mrs_real) dTmp;

        processBuff_(cBarkk-1) = fAtt;                                             // nonnormalized masking threshold
        fNorm                      += fAtt;                                             // normalization factor
      }


      // initialize new
      fAtt                            = 1.0F;

      // upper slope
      for (cBarkk = i; cBarkk < numBands_-1; cBarkk++)
      {
        mrs_real  dTmp                = fAtt * slopeSpread_(i);
        fAtt                        = (dTmp < 1e-30) ? 0 : (mrs_real) dTmp;

        processBuff_(cBarkk+1) = fAtt;
        fNorm                      += fAtt;                                             // normalization factor
      }

      fNorm                           = 1.0/fNorm;

      // nonlinear superposition
      for (cBarkk = 0; cBarkk < numBands_; cBarkk++)
      {
        processBuff_(cBarkk)                  *= fNorm;
        normSpread_(cBarkk)    += pow (processBuff_(cBarkk), 0.4);
      }
    }
    for (i = 0; i < numBands_; ++i)
      normSpread_(i)  = pow (normSpread_(i), -2.5);            // resulting normalization pattern (eq. 19)
  }

  {
    // masking (eq.25)
    for ( i = 0; i <numBands_; ++i )
    {
      intNoise_(i)     = .1456 * pow (.001 * freqBounds_[i].fMidFreq, -0.8);    // noise in dB
      intNoise_(i)     = pow (10., intNoise_(i));                          // -> in energy domain
    }
  }

  {
    // masking (eq.25)
    mrs_real	v			= pow (.1, .3);
    for ( i = 0; i < 12.0/barkRes_; ++i )
      maskingThresh_(i)   = v;


    while (i < numBands_)
    {
      maskingThresh_(i)   = pow (.1, .025 * barkRes_ * i);
      ++i;
    }
  }

  return;
}
