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

#ifndef MARSYAS_PEFEATSELECT_H
#define MARSYAS_PEFEATSELECT_H

#include <marsyas/system/MarSystem.h>
#include <sstream>

namespace Marsyas
{
/**
	\class PeakFeatureSelect
	\ingroup Processing
	\brief This MarSystem takes a realvec with peak information (using peakView)
	and allows to select which peak features should be sent to the output.
	Used for similarity computations on peaks.

	Controls:
	- \b mrs_natural/selectedFeatures [rw] : bit field used to select peaks features to output
	- \b mrs_natural/totalNumPeaks [w] : this control sets the total num of peaks at the input (should normally be linked with PeakConvert similar control)
	- \b mrs_natural/frameMaxNumPeaks [w] : this control sets the maximum num of peaks per frame at the input (should normally be linked with PeakConvert similar control)
*/

class PeakFeatureSelect: public MarSystem
{
private:
  mrs_natural selectedFeatures_;
  MarControlPtr ctrl_selectedFeatures_;

  MarControlPtr ctrl_totalNumPeaks_;

  MarControlPtr ctrl_frameMaxNumPeaks_;
  mrs_natural frameMaxNumPeaks_;

  mrs_natural numFeats_;

  void addControls();
  void myUpdate(MarControlPtr sender);

public:
  //bit enum for feature selection
  enum features {
    pkFrequency = 1,
    pkAmplitude = 2,
    pkFrame = 4,
    pkSetFrequencies = 8,
    pkSetAmplitudes = 16,
    pkSetFrames = 32,
    barkPkFreq = 64,
    dBPkAmp = 128,
    pkPan = 256,
    pkDeltaFrequency = 512,
    pkDeltaAmplitude = 1024
  };//should we also add standardization options here?!? [?]

  PeakFeatureSelect(std::string name);
  PeakFeatureSelect(const PeakFeatureSelect& a);
  ~PeakFeatureSelect();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

}

#endif



