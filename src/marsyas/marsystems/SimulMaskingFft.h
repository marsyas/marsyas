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

#ifndef MARSYAS_SimulMaskingFft_H
#define MARSYAS_SimulMaskingFft_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
\class SimulMaskingFft
\ingroup Processing Basic
\brief Return only the power spectrum components above the masking threshold

This code originates from https://feapi.svn.sourceforge.net/svnroot/feapi/trunk/FEAPI/examples/plugin/FEAPIExamplePluginLoudness/FEAPIExamplePluginLoudness.cpp
and has been modified. It is based on the psycho-acoustic model in ITU-R BS.1387.

The input is a power spectrum, the output is the difference of the input spectrum and
the masking threshold

Controls:
- mrs_real listeningLevelInDbSpl: since the masking depends on the level, this impacts the masking threshold
*/


class SimulMaskingFft: public MarSystem
{
private:
  //Add specific controls needed by this MarSystem.
  void addControls();
  MarControlPtr ctrl_SimulMaskingFft_;
  void myUpdate(MarControlPtr sender);

  struct FrequencyBands_t
  {
    mrs_real    fLowFreqBound,
                fUpFreqBound,
                fMidFreq,
                fLowBarkBound,
                fUpBarkBound,
                fMidBark;
  };

  void GetBandLevels (FrequencyBands_t *pFrequencyValues, mrs_realvec &bandLevels, mrs_bool bDezibel);
  void CalcSpreading (mrs_realvec &bandLevels, mrs_realvec &result);
  void ComputeTables ();
  void ComputeDifference (mrs_realvec &out, mrs_realvec &in, mrs_natural t);

  mrs_real	normFactor_,
            audiosrate_,
            barkRes_;
  mrs_natural	numBands_;
  mrs_realvec processBuff_,
              barkSpec_,
              excPattern_,
              helpBuff_,
              outerEar_,
              maskingThresh_,
              intNoise_,
              slopeSpread_,
              normSpread_;
  FrequencyBands_t	*freqBounds_;
public:
  SimulMaskingFft(std::string name);
  SimulMaskingFft(const SimulMaskingFft& a);
  ~SimulMaskingFft();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif
