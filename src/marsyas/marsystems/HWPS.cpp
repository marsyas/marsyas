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

#include "HWPS.h"
#include <marsyas/NumericLib.h>
#include <algorithm>

using std::ostringstream;
using std::min;
using std::max;
using std::abs;

using namespace Marsyas;

HWPS::HWPS(mrs_string name):MarSystem("HWPS", name)
{
  addControls();
}

HWPS::HWPS(const HWPS& a) : MarSystem(a)
{
  ctrl_histSize_ = getctrl("mrs_natural/histSize");
  ctrl_calcDistance_ = getctrl("mrs_bool/calcDistance");
}

HWPS::~HWPS()
{
}

MarSystem*
HWPS::clone() const
{
  return new HWPS(*this);
}

void
HWPS::addControls()
{
  addctrl("mrs_bool/calcDistance", false, ctrl_calcDistance_);
  addctrl("mrs_natural/histSize", 20, ctrl_histSize_);
}

void
HWPS::harmonicWrap(mrs_real peak1Freq, mrs_real peak2Freq, realvec& peak1SetFreqs, realvec& peak2SetFreqs)
{

  // fundamental frequency estimate
  mrs_real hF;

  // Use the lowest in frequency highest amplitude
  // peak of the frames under consideration
  hF = min(peak1SetFreqs(0), peak2SetFreqs(0));

  // Original HWPS using the considered peaks for folding
  // hF = min(peak1Freq, peak2Freq);

  // mrs_real mhF = min(hF, abs(peak1Freq-peak2Freq));

  // shift frequencies
  peak1SetFreqs -= peak1Freq;
  peak2SetFreqs -= peak2Freq;


  /*
    MATLAB_PUT(peak1SetFreqs, "P1");
    MATLAB_PUT(peak2SetFreqs, "P2");
    MATLAB_EVAL("clf ; subplot(3, 1, 1);  hold ; stem(P1, A1); stem(P2, A2, 'r')");
  */

  // wrap frequencies around fundamental freq estimate
  peak1SetFreqs /= hF;
  peak2SetFreqs /= hF;

  for (mrs_natural k=0 ; k<peak1SetFreqs.getSize() ; k++)
  {
    peak1SetFreqs(k)=fmod(peak1SetFreqs(k), 1);
    //if(peak1SetFreqs(k)<0)
    while(peak1SetFreqs(k)<0)//replacing "if" in case of strongly negative (=> multiple wraps)
      peak1SetFreqs(k)+=1;
  }
  for (mrs_natural k=0 ; k<peak2SetFreqs.getSize() ; k++)
  {
    peak2SetFreqs(k)=fmod(peak2SetFreqs(k), 1);
    //if(peak2SetFreqs(k)<0)
    while(peak2SetFreqs(k)<0) //replacing "if" in case of strongly negative (=> multiple wraps)
      peak2SetFreqs(k)+=1;
  }
}

void
HWPS::discretize(const realvec& peakSetWrapFreqs, const realvec& peakAmps,
                 const mrs_natural& histSize, realvec& resultHistogram)
{
  mrs_natural index;

  resultHistogram.create(histSize);

  for (mrs_natural i=0 ; i<peakSetWrapFreqs.getSize() ; ++i)
  {
    index = (mrs_natural) fmod(floor(peakSetWrapFreqs(i)*histSize+.5), histSize);
    resultHistogram(index) += peakAmps(i);
  }
}

void
HWPS::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)
  if(inSamples_ > 1) {
    MRSWARN("HWPS::myUpdate - inSamples > 1 : only first column will be processed!");
  }

  ctrl_onObservations_->setValue(1, NOUPDATE);
  ctrl_onSamples_->setValue(1, NOUPDATE);
  ctrl_osrate_->setValue(ctrl_israte_, NOUPDATE); //[?]
  ctrl_onObsNames_->setValue("HWPS", NOUPDATE);

  //the input has the two vectors/matrices to process stacked vertically
  if(inObservations_ % 2 != 0) {
    MRSWARN("HWPS::myUpdate - input flow controls do not seem to be in a valid format!");
  }

  vec_i_.create(ctrl_inObservations_->to<mrs_natural>()/2);
  vec_j_.create(ctrl_inObservations_->to<mrs_natural>()/2);
}

void
HWPS::myProcess(realvec& in, realvec& out)
{
  mrs_natural o;
  //get the two stacked vectors from the input
  for(o=0; o < inObservations_/2; ++o)
  {
    vec_i_(o) = in(o,0);
    vec_j_(o) = in(o+inObservations_/2,0);
  }

  //parse data from the input vectors.
  //format:
  //[peakFreq, frameNumPeaks, pkSetFreq0,...,pkSetFreqN, pkSetAmp0,..., pkSetAmpN, dummyPad, dummyPad, ...]'
  //where N = frameNumPeaks-1, and dummyPad are dummy values used for filling the vector when frameNumPeaks < maxFrameNumPeaks.
  pk_i_freq_ = vec_i_(HWPS::pkFreqIdx); //peak i frequency
  pk_j_freq_ = vec_j_(HWPS::pkFreqIdx); //peak j frequency
  i_frameNumPeaks_ = (mrs_natural)vec_i_(HWPS::frameNumPeaksIdx); //peakSet i frameNumPeaks
  j_frameNumPeaks_ = (mrs_natural)vec_j_(HWPS::frameNumPeaksIdx); //peakSet j frameNumPeaks

  //get i peaksets freqs and amplitudes
  pkSet_i_Freqs_.stretch(i_frameNumPeaks_);
  pkSet_i_Amps_.stretch(i_frameNumPeaks_);
  for(o=0; o < i_frameNumPeaks_; ++o)
  {
    pkSet_i_Freqs_(o) = vec_i_(o+pkSetFeatsIdx);
    pkSet_i_Amps_(o) = vec_i_(o+pkSetFeatsIdx+i_frameNumPeaks_);
  }
  //get j peaksets freqs and amplitudes
  pkSet_j_Freqs_.stretch(j_frameNumPeaks_);
  pkSet_j_Amps_.stretch(j_frameNumPeaks_);
  for(o=0; o < j_frameNumPeaks_; ++o)
  {
    pkSet_j_Freqs_(o) = vec_j_(o+pkSetFeatsIdx);
    pkSet_j_Amps_(o) = vec_j_(o+pkSetFeatsIdx+j_frameNumPeaks_);
  }

  //perform harmonic wrapping
  pkSet_i_WrapFreqs_ = pkSet_i_Freqs_;
  pkSet_j_WrapFreqs_ = pkSet_j_Freqs_;
  harmonicWrap(pk_i_freq_, pk_j_freq_, pkSet_i_WrapFreqs_, pkSet_j_WrapFreqs_);

  /*
    MATLAB_PUT(pkSet_i_WrapFreqs_, "P1");
    MATLAB_PUT(pkSet_j_WrapFreqs_, "P2");
    MATLAB_PUT(pkSet_i_Amps_, "A1");
    MATLAB_PUT(pkSet_j_Amps_, "A2");
    MATLAB_EVAL("subplot(3, 1, 2);  hold ; stem(P1, A1); stem(P2, A2, 'r')");
  */

  //create histograms for both peaks
  histSize_ = ctrl_histSize_->to<mrs_natural>();
  discretize(pkSet_i_WrapFreqs_, pkSet_i_Amps_, histSize_, histogram_i_);
  discretize(pkSet_j_WrapFreqs_, pkSet_j_Amps_, histSize_, histogram_j_);

  /*
    MATLAB_PUT(histogram_i_, "H1");
    MATLAB_PUT(histogram_j_, "H2");
    MATLAB_EVAL("subplot(3, 1, 3); bar([H1; H2]')");
  */

  if(ctrl_calcDistance_->isTrue())
  {
    //return the cosine DISTANCE between the two histograms and we get the HWPDistance!
    out(0) = NumericLib::cosineDistance(histogram_i_, histogram_j_);
  }
  else
  {
    //return the cosine SIMILARITY between the two histograms and we get the HWPSimilarity!
    out(0) = 1.0 - NumericLib::cosineDistance(histogram_i_, histogram_j_);
  }
}
