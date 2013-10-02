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

#ifndef MARSYAS_F0ANALYSIS_H
#define MARSYAS_F0ANALYSIS_H

#include <marsyas/system/MarSystem.h>
#include <functional>

namespace Marsyas
{
/**
\ingroup	Analysis
\brief		Compute F0s in input spectrum
\author		Matthias Varewyck
\date		20090518

Controls:
- \b mrs_natural NrOfHarmonics [rw] : nr. of harmonics taken into account (excl. F0)
- \b mrs_real F0Weight [rw] : balance between F0 and higher harmonics
- \b mrs_real Attenuation [rw] : attenuation of higher harmonics
- \b mrs_real Tolerance_ [rw] : tolerance for harmonics to be assigned to F0
- \b mrs_real LowestFo_ [rw] : lowest possible F0
- \b mrs_real ChordEvidence_ [r] : evidence that input spectrum includes chord
*/

class F0Analysis: public MarSystem
{
public:
  F0Analysis(mrs_string inName);
  F0Analysis(const F0Analysis& inToCopy);

  ~F0Analysis();

  MarSystem* clone() const;

  void addControls();
  void myUpdate(MarControlPtr inSender);
  void myProcess(realvec& inVec, realvec& outVec);

private:
  // Pointer to MarControllers
  MarControlPtr ctrl_SampleRate_;
  MarControlPtr ctrl_NrOfHarmonics_;
  MarControlPtr ctrl_F0Weight_;
  MarControlPtr ctrl_Attenuation_;
  MarControlPtr ctrl_Tolerance_;
  MarControlPtr ctrl_LowestF0_;
  MarControlPtr ctrl_Compression_;

  // Member variable
  mrs_real SampleRate_;		// Sample rate of the input vectors
  mrs_natural NrOfHarmonics_;	// Nr. of harmonics taken into account (excl. F0)
  mrs_real F0Weight_;			// Balance between F0 and higher harmonics
  mrs_real Attenuation_;		// Attenuation of higher harmonics
  mrs_real Tolerance_;		// Tolerance for harmonics to be assigned to F0
  mrs_real LowestF0_;			// Lowest possible F0
  mrs_real Compression_;

  mrs_real ChordEvidence_;	// Evidence that last input spectrum includes chord

  // Help functions
  typedef std::map<double,double,std::less<double> > FreqMap;		// freq -> float
  typedef std::map<double,double,std::greater<float> > HarmMap;	// harm sum -> freq
  typedef std::map<double,std::vector<double>,std::less<double> > F2Fs;

  bool FindCandidateF0s(const realvec& inPeaks,
                        HarmMap& outHarmSums, F2Fs& outF0ToFks) const;
  bool SelectUnrelatedF0s(const realvec& inPeaks, const HarmMap inHarmSums,
                          const F2Fs& inF0ToFks, realvec& outNoteEvidences);

  mrs_real ComputePowerOfF0(const FreqMap inPeaks, const F2Fs& inF0ToFks, double inF0) const;
  mrs_real ComputePowerOfInput(const FreqMap inPeaks) const;
  mrs_real ComputePowerOfHyp(const FreqMap inPeaks, const F2Fs& inF0ToFks,
                             realvec& inNoteEvidence) const;
};

}

#endif
