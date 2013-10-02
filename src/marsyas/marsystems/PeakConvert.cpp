/*
** Copyright (C) 1998-2005 George Tzanetakis <gtzan@cs.uvic.ca>
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


#include "PeakConvert.h"
#include "Peaker.h"
#include "MaxArgMax.h"
#include <marsyas/peakView.h>

#include <algorithm>

using std::ostringstream;
using std::abs;

using namespace Marsyas;

PeakConvert::PeakConvert(mrs_string name):MarSystem("PeakConvert",name)
{
  psize_ = 0;
  size_ = 0;
  nbParameters_ = peakView::nbPkParameters;
  skip_=0;
  frame_ = 0;
  N_ = 0;

  fundamental_ = 0.0;
  factor_ = 0.0;
  nbPeaks_ = 0;
  frameMaxNumPeaks_ = 0;

  useStereoSpectrum_ = false;

  peaker_ = new Peaker("Peaker");
  max_ = new MaxArgMax("MaxArgMax");

  addControls();
}

PeakConvert::PeakConvert(const PeakConvert& a) : MarSystem(a)
{
  psize_ = a.psize_;
  size_ = a.size_;
  nbParameters_ = a.nbParameters_;
  skip_ = a.skip_;
  frame_ = a.frame_;
  N_ = a.N_;

  fundamental_ = a.fundamental_;
  factor_ = a.factor_;
  nbPeaks_ = a.nbPeaks_;
  frameMaxNumPeaks_  = a.frameMaxNumPeaks_;

  useStereoSpectrum_ = a.useStereoSpectrum_;

  peaker_ = new Peaker("Peaker");
  max_ = new MaxArgMax("MaxArgMax");

  ctrl_totalNumPeaks_ = getctrl("mrs_natural/totalNumPeaks");
  ctrl_frameMaxNumPeaks_ = getctrl("mrs_natural/frameMaxNumPeaks");
}

PeakConvert::~PeakConvert()
{
  delete peaker_;
  delete max_;
}

MarSystem*
PeakConvert::clone() const
{
  return new PeakConvert(*this);
}

void
PeakConvert::addControls()
{
  addctrl("mrs_natural/frameMaxNumPeaks", 0);
  setctrlState("mrs_natural/frameMaxNumPeaks", true);

  addctrl("mrs_string/frequencyInterval", "MARSYAS_EMPTY");
  setctrlState("mrs_string/frequencyInterval", true);

  addctrl("mrs_natural/nbFramesSkipped", 0);
  setctrlState("mrs_natural/nbFramesSkipped", true);

  addctrl("mrs_bool/improvedPrecision", true);
  setctrlState("mrs_bool/improvedPrecision", true);

  addctrl("mrs_bool/picking", true);
  setctrlState("mrs_bool/picking", true);

  addctrl("mrs_natural/totalNumPeaks", 0, ctrl_totalNumPeaks_);
}

void
PeakConvert::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)

  //check the input to see if we are also getting stereo information
  //(N_ is the FFT size)
  if (fmod(inObservations_, 2.0) == 0.0)
  {
    //we just have the two shifted spectrums stacked vertically
    //(input has N + N observations)
    N_ = inObservations_/2;
    useStereoSpectrum_ = false;
  }
  else if(fmod(inObservations_-1, 2.5) == 0.0)
  {
    //we also have stereo spectrum info at the bottom
    //(input has N + N + N/2+1 observations)
    N_ = (mrs_natural)((inObservations_-1) / 2.5);
    useStereoSpectrum_ = true;
  }

  //if picking is disabled (==false), the number of sinusoids should be set
  //to the number of unique bins of the spectrums at the input (i.e. N/2+1)
  pick_ = getctrl("mrs_bool/picking")->to<mrs_bool>();
  if(!pick_ && ctrl_frameMaxNumPeaks_->to<mrs_natural>() == 0)
    frameMaxNumPeaks_ = N_/2+1; //inObservations_/4+1;
  else
    frameMaxNumPeaks_ = ctrl_frameMaxNumPeaks_->to<mrs_natural>();

  setctrl(ctrl_onSamples_, ctrl_inSamples_);
  setctrl(ctrl_onObservations_, frameMaxNumPeaks_*nbParameters_);
  setctrl(ctrl_osrate_, ctrl_israte_);

  ostringstream oss;
  for(mrs_natural j=0; j< nbParameters_; ++j) //j = param index
  {
    for (mrs_natural i=0; i < frameMaxNumPeaks_; ++i) //i = peak index
      oss << peakView::getParamName(j) << "_" << i+j*frameMaxNumPeaks_ << ",";
  }
  ctrl_onObsNames_->setValue(oss.str(), NOUPDATE);

  mrs_real timeSrate = israte_*(mrs_real)N_;//israte_*(mrs_real)inObservations_/2.0;

  size_ = N_/2+1;//inObservations_ /4 +1;
  if (size_ != psize_)
  {
    lastphase_.stretch(size_);
    phase_.stretch(size_);
    mag_.stretch(size_);
    magCorr_.stretch(size_);
    frequency_.stretch(size_);
    lastmag_.stretch(size_);
    lastfrequency_.stretch(size_);
    deltamag_.stretch(size_);
    deltafrequency_.stretch(size_);
    psize_ = size_;
  }

  factor_ = timeSrate / TWOPI;
  fundamental_ = israte_;

  skip_ = getctrl("mrs_natural/nbFramesSkipped")->to<mrs_natural>();
  prec_ = getctrl("mrs_bool/improvedPrecision")->to<mrs_bool>();

  if(getctrl("mrs_string/frequencyInterval")->to<mrs_string>() != "MARSYAS_EMPTY")
  {
    realvec conv(2);
    string2parameters(getctrl("mrs_string/frequencyInterval")->to<mrs_string>(), conv, '_'); //[!]
    downFrequency_ = (mrs_natural) floor(conv(0)/timeSrate*size_*2) ;
    upFrequency_ = (mrs_natural) floor(conv(1)/timeSrate*size_*2);
  }
  else
  {
    downFrequency_ = 0;
    upFrequency_ = size_;
  }
}

mrs_real
PeakConvert::lobe_value_compute(mrs_real f, mrs_natural type, mrs_natural size)
{
  mrs_real re ;

  // size par size-2 !!!
  switch (type)
  {
  case 1:
  {
    re= fabs (0.5*lobe_value_compute(f, 0, size)+
              0.25*lobe_value_compute(f-2.*PI/size, 0, size)+
              0.25*lobe_value_compute(f+2.*PI/size, 0, size))/size ;
    return fabs(re);
  }
  case 0:
    return (mrs_real) (f == 0) ? size : (sin(f*0.5*(size))/sin(f*0.5));
  default:
  {
    return 0.0 ;
  }
  }
}

/*
void
PeakConvert::getBinInterval(realvec& interval, realvec& index, realvec& mag) //[WTF] is this being used for anything?!?
{
	mrs_natural k=0, start=0, nbP=index.getSize();
	mrs_natural minIndex = 0;

	// getting rid of padding zeros
	while(start<index.getSize() && !index(start))
		start++;

	for(mrs_natural i=start ; i<nbP ; ++i, k++)
	{
		interval(2*k) = index(i)-1;
		interval(2*k+1) = index(i);
	}
}*/

void
PeakConvert::getShortBinInterval(realvec& interval, realvec& index, realvec& mag)
{
  mrs_natural k=0, start=0, nbP=index.getSize();
  mrs_natural minIndex = 0;

  // getting rid of padding zeros
  while(start<index.getSize() && !index(start))
    start++;

  for(mrs_natural i=start ; i<nbP ; ++i, ++k)
  {
    minIndex = 0;
    // look for the next valley location upward
    for (mrs_natural j= (mrs_natural)index(i) ; j<mag.getSize()-1 ; ++j)
    {
      if(mag(j) < mag(j+1))
      {
        minIndex = j;
        break;
      }
    }
// 		if(!minIndex) //arght!!! I hate using logic with integers!!! Makes code so difficult to read!!! [!]
// 		{
// 			cout << "pb while looking for bin intervals" << endl; //[?]
// 		}

    interval(2*k+1) = minIndex;

    // look for the next valley location downward
    for (mrs_natural j= (mrs_natural)index(i) ; j>1 ; --j)
    {
      if(mag(j) < mag(j-1))
      {
        minIndex = j;
        break;
      }
    }
// 		if(!minIndex) //arght!!! I hate using logic with integers!!! Makes code so difficult to read!!! [!]
// 		{
// 			cout << "pb while looking for bin intervals" << endl; //[?]
// 		}
    interval(2*k) = minIndex;
  }
}


void
PeakConvert::getLargeBinInterval(realvec& interval, realvec& index, realvec& mag)
{
  mrs_natural k=0, start=0, nbP=index.getSize();

  // handling the first case
  mrs_real minVal = HUGE_VAL;
  mrs_natural minIndex = 0;

  // getting rid of padding zeros
  while(!index(start))
    start++;

  for (mrs_natural j=0 ; j<index(start) ; j++) //is this foor loop like this?!?!?!?!?!?!?!?! [!]
  {
    if(minVal > mag(j))
    {
      minVal = mag(j);
      minIndex = j;
    }
  }
// 	if(!minIndex)
// 	{
// 		cout << "pb while looking for minimal bin intervals" << endl; //[WTF]
// 	}
  interval(0) = minIndex;

  for(mrs_natural i=start ; i<nbP-1 ; ++i, k++)
  {
    minVal = HUGE_VAL;
    minIndex = 0;
    // look for the minimal value among successive peaks
    for (mrs_natural j= (mrs_natural) index(i) ; j<index(i+1) ; j++) // is this for loop like this?!?!?! [?]
    {
      if(minVal > mag(j))
      {
        minVal = mag(j);
        minIndex = j;
      }
    }

// 		if(!minIndex)
// 		{
// 			cout << "pb while looking for bin intervals" << endl; //[WTF]
// 		}
    interval(2*k+1) = minIndex-1;
    interval(2*(k+1)) = minIndex;
  }

  // handling the last case
  minVal = HUGE_VAL;
  minIndex = 0;
  for (mrs_natural j= (mrs_natural)index(nbP-1) ; j<mag.getSize()-1 ; ++j)
  {
    if(minVal > mag(j))
    {
      minVal = mag(j);
      minIndex = j;
    }
    // consider stopping the search at the first valley
    if(minVal<mag(j+1))
      break;
  }
// 	if(!minIndex)
// 	{
// 		cout << "pb while looking for maximal bin intervals" << endl; //[WTF]
// 	}
  interval(2*(k)+1) = minIndex;
}

void
PeakConvert::myProcess(realvec& in, realvec& out)
{
  mrs_natural o;
  mrs_real a, c;
  mrs_real b, d;
  mrs_real phasediff;

  out.setval(0);
  peakView pkViewOut(out);

  for(mrs_natural f=0 ; f < inSamples_; ++f)
  {
    //we should avoid the first empty frames,
    //that will contain silence and consequently create
    //discontinuities in the signal, ruining the peak calculation!
    //only process if we have a full data vector (i.e. no zeros)
    if(frame_ >= skip_)
    {
      // handle amplitudes from shifted spectrums at input
      for (o=0; o < size_; o++)
      {
        if (o==0) //DC bins
        {
          a = in(0,f);
          b = 0.0;
          c = in(N_, f);
          d = 0.0;
        }
        else if (o == size_-1) //Nyquist bins
        {
          a = in(1, f);
          b = 0.0;
          c = in(N_+1, f);
          d = 0.0;
        }
        else //all other bins
        {
          a = in(2*o, f);
          b = in(2*o+1, f);
          c = in(N_+2*o, f);
          d = in(N_+2*o+1, f);
        }

        // computer magnitude value
        //mrs_real par = lobe_value_compute (0, 1, 2048); //[?]

        // compute phase
        phase_(o) = atan2(b,a);

        // compute precise frequency using the phase difference
        lastphase_(o)= atan2(d,c);
        if(phase_(o) >= lastphase_(o))
          phasediff = phase_(o)-lastphase_(o);
        else
          phasediff = phase_(o)-lastphase_(o)+TWOPI;
        if(prec_)
          frequency_(o) = phasediff * factor_ ;
        else
          frequency_(o) = o*fundamental_;

        // compute precise amplitude
        mag_(o) = sqrt((a*a + b*b))*2; //*4/0.884624;//*50/3); // [!]
        mrs_real mag = lobe_value_compute((o * fundamental_-frequency_(o))/factor_, 1, N_);
        magCorr_(o) = mag_(o)/mag;

        // computing precise frequency using the derivative method // use at your own risk	[?]
        /*	mrs_real lastmag = sqrt(c*c + d*d);
        mrs_real rap = (mag_(o)-lastmag)/(lastmag*2);
        f=asin(rap);
        f *= (getctrl("mrs_real/israte")->to<mrs_real>()*inObservations/2.0)/PI;
        */
        // rough frequency and amplitude
        //frequency_(o) = o * fundamental_;
        //magCorr_(o) = mag_(o);

        if(lastfrequency_(o) != 0.0)
          deltafrequency_(o) = (frequency_(o)-lastfrequency_(o))/(frequency_(o)+lastfrequency_(o));

        deltamag_(o) = (mag_(o)-lastmag_(o))/(mag_(o)+lastmag_(o));

        // remove potential peak if frequency too irrelevant
        if(abs(frequency_(o)-o*fundamental_) > 0.5*fundamental_)
          frequency_(o)=0.0;

        lastfrequency_(o) = frequency_(o);
        lastmag_(o) = mag_(o);
      }

      // select bins with local maxima in magnitude (--> peaks)
      realvec peaks_ = mag_;
      realvec tmp_;
      peaker_->updControl("mrs_real/peakStrength", 0.2);// to be set as a control [!]
      peaker_->updControl("mrs_natural/peakStart", downFrequency_);   // 0
      peaker_->updControl("mrs_natural/peakEnd", upFrequency_);  // size_
      peaker_->updControl("mrs_natural/inSamples", mag_.getCols());
      peaker_->updControl("mrs_natural/inObservations", mag_.getRows());
      peaker_->updControl("mrs_natural/onSamples", peaks_.getCols());
      peaker_->updControl("mrs_natural/onObservations", peaks_.getRows());
      if(pick_)
        peaker_->process(mag_, peaks_);
      else
      {
        //peaks_ = mag_;
        for (o = 0 ; o < downFrequency_ ; o++)
          peaks_(o)=0.0;
        for (o = upFrequency_ ; o < (mrs_natural)peaks_.getSize() ; ++o)
          peaks_(o)=0.0;
      }

      //discard bins whose frequency was set as irrelevant...
      for(o=0 ; o < size_ ; o++)
      {
        if(frequency_(o) == 0)
          peaks_(o) = 0.0;
      }

      // keep only the frameMaxNumPeaks_ highest amplitude local maxima
      if(ctrl_frameMaxNumPeaks_->to<mrs_natural>() != 0) //?????????????????? is this check needed?!? See myUpdate
      {
        tmp_.stretch(frameMaxNumPeaks_*2);
        max_->updControl("mrs_natural/nMaximums", frameMaxNumPeaks_);
      }
      else //?????????????????? is this check needed?!? See myUpdate
      {
        tmp_.stretch((upFrequency_-downFrequency_)*2);
        max_->updControl("mrs_natural/nMaximums", upFrequency_-downFrequency_);
      }
      max_->setctrl("mrs_natural/inSamples", size_);
      max_->setctrl("mrs_natural/inObservations", 1);
      max_->update();
      max_->process(peaks_, tmp_);

      nbPeaks_=tmp_.getSize()/2;
      realvec index_(nbPeaks_); //[!] make member to avoid reallocation at each tick!
      for (mrs_natural i=0 ; i<nbPeaks_ ; ++i)
        index_(i) = tmp_(2*i+1);
      realvec index2_ = index_;
      index2_.sort();

      // search for bins interval
      realvec interval_(nbPeaks_*2); //[!] make member to avoid reallocation at each tick!
      interval_.setval(0);
      if(pick_)
        getShortBinInterval(interval_, index2_, mag_);

      // fill output with peaks data
      /*
      MATLAB_PUT(mag_, "peaks");
      MATLAB_PUT(peaks_, "k");
      MATLAB_PUT(tmp_, "tmp");
      MATLAB_PUT(interval_, "int");
      MATLAB_EVAL("figure(1);clf;hold on ;plot(peaks);stem(k);stem(tmp(2:2:end)+1, peaks(tmp(2:2:end)+1), 'r')");
      MATLAB_EVAL("stem(int+1, peaks(int+1), 'k')");
      */

      interval_ /= N_*2;

      for (mrs_natural i = 0; i < nbPeaks_; ++i)
      {
        pkViewOut(i, peakView::pkFrequency, f) = frequency_((mrs_natural) index_(i));
        pkViewOut(i, peakView::pkAmplitude, f) = magCorr_((mrs_natural) index_(i));
        pkViewOut(i, peakView::pkPhase, f) = -phase_((mrs_natural) index_(i));
        pkViewOut(i, peakView::pkDeltaFrequency, f) = deltafrequency_((mrs_natural) index_(i));
        pkViewOut(i, peakView::pkDeltaAmplitude, f) = deltamag_((mrs_natural) index_(i));
        pkViewOut(i, peakView::pkFrame, f) = frame_;
        pkViewOut(i, peakView::pkGroup, f) = 0.0; //This should be -1!!!! [TODO]
        pkViewOut(i, peakView::pkVolume, f) = 1.0;
        pkViewOut(i, peakView::pkBinLow, f) = interval_(2*i);
        pkViewOut(i, peakView::pkBin, f) = index_(i);
        pkViewOut(i, peakView::pkBinHigh, f) = interval_(2*i+1);
        pkViewOut(i, peakView::pkTrack, f) = -1.0; //null-track ID

        if(useStereoSpectrum_)
          pkViewOut(i, peakView::pkPan, f) = in((mrs_natural)index_(i)+2*N_, f);
        else
          pkViewOut(i, peakView::pkPan, f) = 0.0;
      }
    }
    else //if not yet reached "skip" number of frames...
    {
      for(mrs_natural i=0; i< frameMaxNumPeaks_; ++i)
      {
        //pkViewOut(i, peakView::pkFrequency, f) = 0;
        //pkViewOut(i, peakView::pkAmplitude, f) = 0;
        //pkViewOut(i, peakView::pkPhase, f) = 0;
        //pkViewOut(i, peakView::pkDeltaFrequency, f) = 0;
        //pkViewOut(i, peakView::pkDeltaAmplitude, f) = 0;
        pkViewOut(i, peakView::pkFrame, f) = frame_;
        //pkViewOut(i, peakView::pkGroup, f) = -1;
        //pkViewOut(i, peakView::pkVolume, f) = 0;
        //pkViewOut(i, peakView::pkPan, f) = 0;
        //pkViewOut(i, peakView::pkBinLow, f) = 0;
        //pkViewOut(i, peakView::pkBin, f) = 0;
        //pkViewOut(i, peakView::pkBinHigh, f) = 0;
      }
    }
    frame_++;
  }

  //count the total number of existing peaks (i.e. peak freq != 0)
  ctrl_totalNumPeaks_->setValue(pkViewOut.getTotalNumPeaks());

  // MATLAB_PUT(out, "peaks");
  // MATLAB_PUT(frameMaxNumPeaks_, "k");
  // MATLAB_EVAL("figure(1);clf;plot(peaks(6*k+1:7*k));");

}
