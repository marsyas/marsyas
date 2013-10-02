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

#include "SCF.h"
#include "../common_source.h"

using namespace std;
using namespace Marsyas;

SCF::SCF(mrs_string name):MarSystem("SCF",name)
{
}

SCF::~SCF()
{
}

MarSystem*
SCF::clone() const
{
  return new SCF(*this);
}

void
SCF::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)
  MRSDIAG("SCF.cpp - SCF:myUpdate");

  //MPEG-7 audio standard:
  //assumes an 1/4 octave frequency resolution,
  //resulting in 24 frequency bands between 250Hz and 16kHz.
  //If the signal under analysis does not contain frequencies
  //above a determined value (e.g. due to signal sampling rate or
  //bandwidth limitations), the nr of bands should be reduced.

  mrs_natural i;

  //nrBands_ = getctrl("mrs_natural/nrbands");// can this be received as a control value?
  nrBands_ = 24;
  //can this parameter be dynamically modified, depending on the
  //sampling frequency?!?
  nrValidBands_ = nrBands_;

  setctrl(ctrl_onSamples_, 1);
  setctrl(ctrl_onObservations_, nrBands_);
  setctrl(ctrl_osrate_, ctrl_israte_);

  //features names
  mrs_string orig = ctrl_inObsNames_->to<mrs_string>();
  // remove final comma in name
  orig = orig.substr(0, orig.size()-1);
  ostringstream oss;
  for (i = 0; i < nrBands_; ++i)
    oss << "SCF_" + orig << i+1 << ",";
  setctrl(ctrl_onObsNames_, oss.str());

  edge_.create(nrBands_ + 1);
  bandLoEdge_.create(nrBands_);
  bandHiEdge_.create(nrBands_);

  //nominal band edges (Hz) -> reference = 1kHz (MPEG7)
  for(i = 0 ; i < nrBands_ + 1 ; ++i)
  {
    // 1/4 octave resolution (MPEG7)
    edge_(i) = 1000.0 * std::pow(2.0, (0.25 * (i - 8)));
  }
  // overlapped low and high band edges (Hz)
  for (i = 0; i < nrBands_; ++i)
  {
    bandLoEdge_(i) = edge_(i) * 0.95f; //band overlapping (MPEG7)
    bandHiEdge_(i) = edge_(i+1) * 1.05f; //band overlapping (MPEG7)
  }

  spectrumSize_ = ctrl_inObservations_->to<mrs_natural>();//PowerSpectrum returns N/2+1 spectral points
  //spectrumBinFreqs_.create(spectrumSize_);

  // spectrum sampling rate - not audio
  df_ = ctrl_israte_->to<mrs_real>();

  //calculate the frequency (Hz) of each FFT bin
  //for (mrs_natural k=0; k < spectrumSize_ ; k++)
  //	spectrumBinFreqs_(k) = (float) k * df_;

  //calculate FFT bin indexes for each band's edges
  il_.resize(nrBands_);
  ih_.resize(nrBands_);
  for(i = 0; i < nrBands_; ++i)
  {
    //round to nearest int (MPEG7)
    il_[i] = (mrs_natural)(bandLoEdge_(i)/df_ + 0.5f);
    ih_[i] = (mrs_natural)(bandHiEdge_(i)/df_ + 0.5f);

    //must verify if sampling rate is enough
    //for the specified nr of bands. If not,
    //reduce nr of valid freq. bands
    if(ih_[i] >= spectrumSize_) //if ih_[i] >= N/2+1 = spectrumSize_ = inObservations ...
    {
      nrValidBands_ = i;
      il_.resize(nrValidBands_);
      ih_.resize(nrValidBands_);
      break;
    }
  }
}

void
SCF::myProcess(realvec& in, realvec& out)
{
  mrs_natural i, k, bandwidth;
  double c, maxc;
  double aritMean ;

  //default SCF value = 1.0; (defines SCF=1.0 for silence)
  out.setval(1.0);

  //MPEG7 defines a grouping mechanism for the frequency bands above 1KHz
  //in order to reduce computational effort of the following calculation.
  //For now such grouping mechanism is not implemented...
  for(i = 0; i < nrValidBands_; ++i)
  {
    aritMean = 0.0;
    maxc = 0.0;
    bandwidth = ih_[i] - il_[i] + 1;
    for(k = il_[i]; k <= ih_[i]; k++)
    {
      c = in(k); //power spectrum coeff
      aritMean += c / bandwidth;
      if(c > maxc)
        maxc = c;
    }
    if (aritMean != 0.0)
    {
      out(i) = (float)(maxc/aritMean);
    }
    //else //mean power = 0 => silence...
    //  out(i) = 1.0; //as used for the SFM (MPEG-7)...
  }

  //for freq bands above the nyquist freq
  //return SFM value defined in MPEG7 for silence
  //for(i = nrValidBands_; i < nrBands_; ++i)
  //	out(i) = 1.0;
}
