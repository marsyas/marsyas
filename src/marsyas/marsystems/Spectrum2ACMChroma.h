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

#ifndef MARSYAS_SPECTRUM2ACMCHROMA_H
#define MARSYAS_SPECTRUM2ACMCHROMA_H

#include <marsyas/system/MarSystem.h>
#include <vector>

namespace Marsyas
{
/**
\ingroup	Processing Analysis
\brief		Transform an (amplitude) spectrum to a chroma profile
\author		Matthias Varewyck
\date		2009 05 18
\todo
- Check whether parameters are independent on input sample rate (~= 8kHz)
- Include check if provided (amplitude) spectrum was correctly computed
- Include correct error handling (conform Marsyas rules)
- Let the lowest F0 be higher than the diapason
- Introduce a highest F0 additional to a lowest F0
- Add "fan in combinator" to simplify actual chroma profile computation

This class implements the conversion of an (amplitude) spectrum to a chroma profile as described in
"a novel chroma representation of polyphonic music based on multiple pitch tracking techniques"
which was presented at the 16th ACM International Conference on Multimedia.

The method computes the set of pitches that best explains the salient frequencies present in the
(amplitude) spectrum.  The method results in (1) a chroma profile and (2) a chord evidence.
(1) The chroma profile represents the normalized pitches after they were folded to one octave
(2) The chord evidence represents the probability that a chord occurs in the examined frame.
For an example, see the function "toy_with_chroma" in the mudbox.

Controls:
- \b mrs_natural/NrOfHarmonics [rw] : adjust the number of harmonics in F0 detection (should be >0)
- \b mrs_real/F0Weight [rw] : adjust the balance between F0 and its harmonics (between 0 and 1)
- \b mrs_real/LowestF0 [rw] : adjust the lowest F0 to be detected (between 0 and Nyquist frequency)
- \b mrs_real/ChordEvidence [r] : store the chord evidence of the last examined frame
*/

class marsyas_EXPORT Spectrum2ACMChroma: public MarSystem
{
public:
  Spectrum2ACMChroma(mrs_string inName);
  Spectrum2ACMChroma(const Spectrum2ACMChroma& inToCopy);

  ~Spectrum2ACMChroma();

  MarSystem* clone() const;

  void addControls();
  void myUpdate(MarControlPtr inSender);
  void myProcess(realvec& inSpectrum, realvec& outChroma);

private:
  MarSystem* Spectrum2ACMChromaNet_;

  // Pointers to MarControllers: contain MOST RECENT values
  MarControlPtr ctrl_NrOfHarmonics_;
  MarControlPtr ctrl_F0Weight_;
  MarControlPtr ctrl_LowestF0_;

  // Member variables: values SINCE LAST UPDATE
  mrs_natural NrOfHarmonics_;		// Number of harmonics in harmonic sum
  mrs_real F0Weight_;				// Relative weight of F0 in pitch evidence
  mrs_real LowestF0_;				// Lowest candidate F0 (in Hz)

  // Static member variables
  static const mrs_natural MedianLength_ = 80;	// Length of median filter (in bins)
  static const mrs_natural NotesPerOctave_ = 12;	// Number of notes per octave

// 			static const mrs_real RaiseFactor_;				// Factor to raise background spectrum with
// 			static const mrs_real Hysteresis_;				// Hysteresis in peak detection function
// 			static const mrs_real Tolerance_;				// Relative tolerance for harmonic analysis
// 			static const mrs_real Attenuation_;				// Attenuation of higher order harmonics
// 			static const mrs_real PowerOfAmplitude_;		// Power of amplitude (0.5 for loudness)
// 			static const mrs_real Diapason_;				// Frequency of middle A (NN69) (in Hz)
};

}	// End namespace

#endif
