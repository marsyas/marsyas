/*
 *  Chroma.h
 *  testChroma
 *
 *  Created by tsunoo on 09/05/03.
 *  Copyright 2009 Emiru Tsunoo. All rights reserved.
 *
 */

#ifndef MARSYAS_CHROMA_H
#define MARSYAS_CHROMA_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
	\ingroup Analysis
	\brief Chroma Vector of input power spectrogram

	Controls:
	- \b mrs_real/samplingFreq : sets the sampligFreq of input signal (double of the nyquist frequency)
	- \b mrs_natural/lowOctNum : sets the lowest octave number to capture
	- \b mrs_natural/highOctNum : sets the highest octave number to capture
*/

class Chroma: public MarSystem
{
private:
  realvec m_;
  realvec freq_;
  realvec filter_;
  mrs_natural lowNum_;
  mrs_natural highNum_;

  MarControlPtr ctrl_samplingFreq_;
  MarControlPtr ctrl_lowOctNum_;
  MarControlPtr ctrl_highOctNum_;

  void addControls();
  void myUpdate(MarControlPtr sender);

public:
  realvec chord_;

  Chroma(std::string name);
  Chroma(const Chroma& a);
  ~Chroma();

  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

}
#endif
