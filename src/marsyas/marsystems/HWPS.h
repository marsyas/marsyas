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

#ifndef MARSYAS_HWPS_H
#define MARSYAS_HWPS_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
	\class HWPS
	\ingroup Special
	\brief Calculates the Harmonic Wrapped Peak Similarity (HWPS) of input peaks

	Controls:
	- \b mrs_bool/calcDistance [w] :when true, outputs Harmonically Wrapped Peak Distance instead of the Similarity (i.e. out = 1-HWPS)
	- \b mrs_natural/histSize [w] : histogram number of bins when discretizing for cosine distance computation
*/

class HWPS: public MarSystem
{
private:
  // HWPS feature vector format:
  //[peakFreq, frameNumPeaks, pkSetFreq0,...,pkSetFreqN, pkSetAmp0,..., pkSetAmpN]'
  //where N+1=maxFrameNumPeaks
  enum pkFields {
    pkFreqIdx = 0,
    frameNumPeaksIdx = 1,
    pkSetFeatsIdx = 2
  };
  realvec vec_i_;
  realvec vec_j_;

  mrs_real pk_i_freq_;
  mrs_real pk_j_freq_;

  mrs_natural i_frameNumPeaks_;
  mrs_natural j_frameNumPeaks_;

  realvec pkSet_i_Freqs_;
  realvec pkSet_j_Freqs_;

  realvec pkSet_i_WrapFreqs_;
  realvec pkSet_j_WrapFreqs_;

  realvec pkSet_i_Amps_;
  realvec pkSet_j_Amps_;

  realvec histogram_i_;
  realvec histogram_j_;

  MarControlPtr ctrl_calcDistance_;
  MarControlPtr ctrl_histSize_;
  mrs_natural histSize_;

  void harmonicWrap(mrs_real peak1Freq, mrs_real peak2Freq,
                    realvec& peak1SetFreqs, realvec& peak2SetFreqs);
  void discretize(const realvec& peakSetWarpFreqs, const realvec& peakAmps,
                  const mrs_natural& histSize, realvec& resultHistogram);

  void addControls();
  void myUpdate(MarControlPtr sender);

public:
  HWPS(std::string name);
  HWPS(const HWPS& a);
  ~HWPS();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

}

#endif



