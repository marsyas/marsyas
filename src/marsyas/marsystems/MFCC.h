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

#ifndef MARSYAS_MFCC_H
#define MARSYAS_MFCC_H

#include <marsyas/system/MarSystem.h>


namespace Marsyas
{
/**
	\ingroup Analysis
	\brief MFCC Mel-Frequency Cepstral Coefficients.
	Takes as input the N/2+1 spectrum Magnitude points output by PowerSpectrum.

	Mel-Frequency cepstral coefficients are features frequently
	used in Speech Recognition. The code is based on the corresponding
	function in the Auditory Toolbox by Malcolm Slaney.

	\see Spectrum, PowerSpectrum


	Controls:
	- \b mrs_natural/coefficients [w]: the number of cepstral coefficients
	to calculate.
*/


class MFCC: public MarSystem
{
private:

  /// Add specific controls needed by this MarSystem.
  void addControls();

  void myUpdate(MarControlPtr sender);

  mrs_real lowestFrequency_;
  mrs_natural linearFilters_;
  mrs_real linearSpacing_;
  mrs_natural logFilters_;
  mrs_real logSpacing_;
  mrs_natural totalFilters_;

  mrs_natural fftSize_, pfftSize_;
  mrs_natural samplingRate_, psamplingRate_;


  /// Number of cepstral coefficients.
  mrs_natural cepstralCoefs_;
  /// Previous number of cepstral coefficients (used for determining whether to update)
  mrs_natural pcepstralCoefs_;
  /// Default number of MFCC coefficients.
  static const int cepstralCoefs_default = 13;

  /// MarControlPtr for the number of cepstral coefficients.
  MarControlPtr ctrl_cepstralCoefs_;

  realvec freqs_;
  realvec lower_;
  realvec center_;
  realvec upper_;
  realvec triangle_heights_;

  realvec fftFreqs_;
  realvec mfccFilterWeights_;
  realvec mfccDCT_;
  realvec earMagnitude_;
  realvec fmagnitude_;

  // NEIL's filter weight speedup
  int* mfcc_offsets_;

  bool init_;

public:

  MFCC(std::string name);
  MFCC(const MFCC& a);
  ~MFCC();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif



