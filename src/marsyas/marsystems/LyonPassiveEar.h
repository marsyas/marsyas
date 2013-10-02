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

#ifndef MARSYAS_LYONEAR_H
#define MARSYAS_LYONEAR_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
class Series;
class Filter;

/**
\class LyonPassiveEar
\ingroup Processing
\brief Lyon filterbank

Lyon Passive Ear computation. Based on
the code from the Auditory Toolbox by Malcolm Slaney.

Controls:
- \b mrs_natural/decimFactor [wr] : Decimation Factor - How much to decimate the model's output.
	Normally the cochlear model produces one output per channel at each sample time. This
	parameters allows the output to be decimated in time (using a filter to reduce aliasing.
	See the decimTauFactor parameter.)
- \b mrs_real/earQ [wr] : Quality Factor - The quality factor of a filter is a measure of its
	bandwidth. In this case it measures the ratio of the width of each band-pass filter at a
	point 3dB down from the maximum. Normally, critical band filters have a Q of about 8.
	Smaller values of earQ mean broader cochlear filters
- \b mrs_real/stepFactor [wr] :  Filter stepping factor - Each filter in a filter bank is
	overlapped by a fixed fraction given by this parameter. The default value is given by
	earQ/32. Thus normal filters (q=8) are overlapped by 25%.
- \b mrs_bool/channelDiffActive [wr] : Channel Difference Flag - Adjacent filter channels can be
	subtracted to further improve the model's frequency response. This parameter is a flag;
	non-zero values indicate the channel differences should be computed.
- \b mrs_bool/agcActive [wr] : Automatic Gain Control Flag - An automatic gain control is used
	to model neural adaptation. This ?ag turns the adaptation mechanism on and off.
	- \b mrs_real/decimTauFactor [wr] : Filter Decimation Tau Factor - When the output of the cochlear
	model is decimated, a low pass filter is applied to each channel to reduce the high frequency
	content and minimize aliasing. The ?lter’s time constant (tau) is set to the decimation
	factor multiplied by this argument. Larger values for the taufactor mean less high frequency
	information is passed.
- \b mrs_realvec/centerFreqs [r] : mid frequencies of the calculated filter bands.
*/


class LyonPassiveEar: public MarSystem
{
private:
  void addControls();
  void myUpdate(MarControlPtr sender);

  mrs_bool			setParametersIntern ();
  void				updateControlsIntern ();
  static mrs_realvec	lyonSecondOrderFilter (mrs_real midFreq, mrs_real q, mrs_real sRate);
  static mrs_real		lyonFreqResp (mrs_realvec firCoeffs, mrs_realvec iirCoeffs, mrs_real freq, mrs_real sRate, mrs_bool inDb = true);
  static mrs_real		lyonSetGain (mrs_realvec firCoeffs, mrs_realvec iirCoeffs, mrs_real newGain, mrs_real freq, mrs_real sRate);
  static Filter*		lyonCreateFilter (mrs_realvec firCoeffs, mrs_realvec iirCoeffs, mrs_string name);

  // control parameters
  mrs_real	fs_;					//!< sample rate

  mrs_natural decimFactor_;
  mrs_real	earQ_;
  mrs_real	stepFactor_;
  mrs_bool	channelDiffActive_;
  mrs_bool	agcActive_;
  mrs_real	decimTauFactor_;

  mrs_realvec centerFreqs_,
              tmpOut_,
              decimOut_;

  //internal variables and parameters
  mrs_natural currDecimState_;
  unsigned int numFilterChannels_;


  /// the actual filter cascade plus the post-processing (agc, channeldiff, etc.)
  Series      *passiveEar_;

public:
  LyonPassiveEar(std::string name);
  ~LyonPassiveEar();

  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif	 // MARSYAS_LYONEAR_H

