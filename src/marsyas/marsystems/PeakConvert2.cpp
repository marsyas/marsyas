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
#include <algorithm>

#include "../common_source.h"
#include <marsyas/common_header.h>
#include "PeakConvert2.h"
#include "Peaker.h"
#include "MaxArgMax.h"
#include "SimulMaskingFft.h"
#include <marsyas/peakView.h>


//#define MTLB_DBG_LOG
//#define ORIGINAL_VERSION
//#define LOG2FILE


#ifdef LOG2FILE
#include <iomanip>
static std::ofstream pFDbgFile;
static const std::string kDbgFilePath = "d:/temp/peaks.new.txt";
#endif



using std::ostringstream;
using std::min;
using std::max;
using std::abs;


using namespace Marsyas;

static const mrs_real gaussianStd = 0.42466090014401;	// results in output of .5 for input of .5

static void FreqSmear (mrs_realvec &spectrum)
{
  mrs_natural length = spectrum.getSize ();
  mrs_real buf[3]		= {0,0,0},
                      coeff[3]	= {.25, .5, .25};

  spectrum(0) = spectrum(length-1) = 0;

  for (mrs_natural i = 0; i < length-1; i++)
  {
    buf[(i+1)%3]	= spectrum(i+1);
    spectrum(i)		= coeff[0]*buf[(i-1+3)%3] + coeff[1]*buf[(i)%3] + coeff[2]*buf[(i+1)%3];
  }

  return;
}
mrs_real
PeakConvert2::GaussianPdf (mrs_real x, mrs_real std)
{
  return exp (-(x*x)/(2*(std*std)));// / sqrt (TWOPI*std);
}

mrs_real	princArg (mrs_real phase)
{
  mrs_real	fx = phase + PI;
  return PI + (fx + TWOPI*floor (fx*(-1.0/TWOPI)));
}

PeakConvert2::PeakConvert2(mrs_string name):MarSystem("PeakConvert2",name),
  peaker_(0),
  max_(0),
  masking_(0)
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
  instFreqHopSize_ = 1;

  useStereoSpectrum_ = false;

  peaker_		= new Peaker("Peaker");
  max_		= new MaxArgMax("MaxArgMax");
  masking_	= new SimulMaskingFft("masking");

  addControls();
}

PeakConvert2::PeakConvert2(const PeakConvert2& a) : MarSystem(a)
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
  hopSize_ = a.hopSize_;
  instFreqHopSize_	= 1;

  useStereoSpectrum_ = a.useStereoSpectrum_;

  peaker_		= (Peaker*)a.peaker_->clone ();
  max_		= (MaxArgMax*)a.max_->clone ();
  masking_	= (SimulMaskingFft*)a.masking_->clone ();

  ctrl_totalNumPeaks_ = getctrl("mrs_natural/totalNumPeaks");
  ctrl_frameMaxNumPeaks_ = getctrl("mrs_natural/frameMaxNumPeaks");

#ifdef LOG2FILE
  pFDbgFile.open (kDbgFilePath.c_str (), std::ios::out);
#endif
}

PeakConvert2::~PeakConvert2()
{
  delete peaker_;
  delete max_;
  if (masking_)
    delete masking_;
#ifdef LOG2FILE
  pFDbgFile.close ();
#endif
}

MarSystem*
PeakConvert2::clone() const
{
  return new PeakConvert2(*this);
}

void
PeakConvert2::addControls()
{
  realvec tmp(3);
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

  addctrl("mrs_natural/hopSize", 1);
  setctrlState("mrs_natural/hopSize", true);

  addctrl("mrs_real/probabilityTresh" , .5);
  setctrlState("mrs_real/probabilityTresh", true);

  addctrl("mrs_natural/totalNumPeaks", 0, ctrl_totalNumPeaks_);

#ifdef ORIGINAL_VERSION
  addctrl("mrs_bool/useMasking", false);
  setctrlState("mrs_bool/useMasking", true);

  tmp(0) = 0;	// probability weight for peak being a masker
  tmp(1) = 0; // probability weight for peak being stationary
  tmp(2) = 1;	// probability weight for peak being tonal
  addctrl("mrs_realvec/peakProbabilityWeight", tmp);
  setctrlState("mrs_realvec/peakProbabilityWeight", true);

  addctrl( "mrs_real/peakSmearingTimeInS" , .0);	// check with other hopsizes
  setctrlState( "mrs_real/peakSmearingTimeInS", true);
#else
  addctrl("mrs_bool/useMasking", true);
  setctrlState("mrs_bool/useMasking", true);


  tmp(0) = 1;	// probability weight for peak being a masker
  tmp(1) = 1; // probability weight for peak being stationary
  tmp(2) = 1;	// probability weight for peak being tonal

  addctrl("mrs_realvec/peakProbabilityWeight", tmp);
  setctrlState("mrs_realvec/peakProbabilityWeight", true);

  addctrl( "mrs_real/peakSmearingTimeInS" , 0.03);	// check with other hopsizes
  setctrlState( "mrs_real/peakSmearingTimeInS", true);
#endif
}

void
PeakConvert2::myUpdate(MarControlPtr sender)
{
  //(void) sender;  //suppress warning of unused parameter(s)
  MarSystem::myUpdate (sender);

  hopSize_	= getctrl ("mrs_natural/hopSize")->to<mrs_natural>();
  mrs_real timeSrate = israte_*(mrs_real)N_;//israte_*(mrs_real)inObservations_/2.0;
  //check the input to see if we are also getting stereo information
  //(N_ is the FFT size)
  if (fmod(inObservations_, 2.0) == 0.0)
  {
    //we just have the two shifted spectra stacked vertically
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
  size_ = N_/2+1;//inObservations_ /4 +1;


  skip_ = getctrl("mrs_natural/nbFramesSkipped")->to<mrs_natural>();
  prec_ = getctrl("mrs_bool/improvedPrecision")->to<mrs_bool>();
  pick_ = getctrl("mrs_bool/picking")->to<mrs_bool>();
  if(getctrl("mrs_string/frequencyInterval")->to<mrs_string>() != "MARSYAS_EMPTY")
  {
    realvec conv(2);
    string2parameters(getctrl("mrs_string/frequencyInterval")->to<mrs_string>(), conv, '_'); //[!]
    downFrequency_ = (mrs_natural) floor(conv(0)/timeSrate*size_*2) ;
    upFrequency_ = min(size_,(mrs_natural) floor(conv(1)/timeSrate*size_*2));
  }
  else
  {
    downFrequency_ = 0;
    upFrequency_ = size_;
  }


  //if picking is disabled (==false), the number of sinusoids should be set
  //to the number of unique bins of the spectrums at the input (i.e. N/2+1)
  if(!pick_ /*&& ctrl_frameMaxNumPeaks_->to<mrs_natural>() == 0*/)
  {
    //frameMaxNumPeaks_	= N_/2+1; //inObservations_/4+1;
    //downFrequency_		= 0;
    //upFrequency_		= size_;
    frameMaxNumPeaks_	= upFrequency_-downFrequency_;
  }
  else
    frameMaxNumPeaks_ = ctrl_frameMaxNumPeaks_->to<mrs_natural>();

  setctrl(ctrl_onSamples_, ctrl_inSamples_);
  setctrl(ctrl_onObservations_, frameMaxNumPeaks_*nbParameters_);
  setctrl(ctrl_osrate_, ctrl_israte_);

  ostringstream oss;
  for(mrs_natural j=0; j< nbParameters_; ++j) //j = param index
  {
    for (mrs_natural i=0; i < frameMaxNumPeaks_; i++) //i = peak index
      oss << peakView::getParamName(j) << "_" << i+j*frameMaxNumPeaks_ << ",";
  }
  ctrl_onObsNames_->setValue(oss.str(), NOUPDATE);

  if (getctrl("mrs_real/peakSmearingTimeInS")->to<mrs_real>() == 0 || !pick_)
    lpCoeff_	= 0;
  else
    lpCoeff_	= exp(-2.2/(timeSrate/hopSize_*getctrl("mrs_real/peakSmearingTimeInS")->to<mrs_real>()));

  if (size_ != psize_)
  {
    tmpBuff_.stretch(inObservations_);
    lastphase_.stretch(size_);
    phase_.stretch(size_);
    mag_.stretch(size_);
    masked_.stretch(size_,1);
    lpPeakerRes_.stretch(size_,1);
    magCorr_.stretch(size_);
    frequency_.stretch(size_);
    lastmag_.stretch(size_);
    lastfrequency_.stretch(size_);
    deltamag_.stretch(size_);
    deltafrequency_.stretch(size_);
    psize_ = size_;

    lpPeakerRes_.setval (1.);
  }

  factor_ = timeSrate / TWOPI / instFreqHopSize_;
  fundamental_ = israte_;

  peakProb_.stretch (3,1);
  peakProbWeight_	= getctrl("mrs_realvec/peakProbabilityWeight")->to<mrs_realvec>();
  if (peakProbWeight_.getRows () > peakProbWeight_.getCols ())
    peakProbWeight_.transpose ();
  peakProbWeight_	/= peakProbWeight_.sum ();

  // init lastfreq to avoid distance inconsistencies
  for (mrs_natural k = 0; k < size_; k++)
    lastfrequency_(k)	= k*fundamental_;
}

mrs_real
PeakConvert2::lobe_value_compute(mrs_real f, mrs_natural type, mrs_natural size)
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

void
PeakConvert2::getShortBinInterval(realvec& interval, realvec& index, realvec& mag)
{
  const unsigned int maxLobeWidth = 6;
  unsigned int	nbP=index.getSize();
  unsigned int minIndex = 0,
               endLoop,
               length = mag.getSize ();

  // we could also use the instantaneous frequency here...?

  for(unsigned int i=0 ; i<nbP ; i++)
  {
    unsigned int idx = (unsigned int)(index(i)+.1);

    if (idx <= 0)
      continue;

    endLoop		= min(length,idx + maxLobeWidth);
    minIndex	= endLoop;
    // look for the next valley location upward
    for (unsigned int j = idx ; j < endLoop ; j++)
    {
      if(mag(j) < mag(j+1))
      {
        minIndex = j;
        break;
      }
    }

    interval(2*i+1) = minIndex;

    endLoop		= max((unsigned int)0,idx - maxLobeWidth);
    minIndex	= endLoop;

    // look for the next valley location downward
    for (unsigned int j= idx ; j > endLoop ; j--)
    {
      if(mag(j) < mag(j-1))
      {
        minIndex = j;
        break;
      }
    }

    interval(2*i) = minIndex;
  }
}


void PeakConvert2::ComputeMasking (realvec& in)
{
  (void) in; // [!] what was this supposed to do?
  masking_->updControl ("mrs_natural/inObservations", size_);
  masking_->updControl ("mrs_natural/inSamples", 1);
  masking_->updControl ("mrs_real/israte", israte_);

  mag_.transpose();
  masking_->myProcess (mag_,masked_);
  mag_.transpose();
}

void PeakConvert2::ComputeMagnitudeAndPhase (mrs_realvec in)
{
  mrs_real a, c;
  mrs_real b, d;
  mrs_real phasediff;
  for (mrs_natural o=0; o < size_; o++)
  {
    if (o==0) //DC bins
    {
      a = in(0);
      b = 0.0;
      c = in(N_);
      d = 0.0;
    }
    else if (o == size_-1) //Nyquist bins
    {
      a = in(1);
      b = 0.0;
      c = in(N_+1);
      d = 0.0;
    }
    else //all other bins
    {
      a = in(2*o);
      b = in(2*o+1);
      c = in(N_+2*o);
      d = in(N_+2*o+1);
    }

    if (o < downFrequency_ || o > upFrequency_)
    {
      frequency_(o)	= 0;
      mag_(o)			= sqrt((a*a + b*b))*2;
      continue;
    }
    if ( a == .0 || c == .0)
    {
      frequency_(o) = o*fundamental_;
    }
    else
    {
      if(prec_ && pick_)
      {
        mrs_real Omega = TWOPI*o*instFreqHopSize_/N_;    // now works with hopsizes != 1 as well  (AL)

        // compute phase
        phase_(o)		= atan2(b,a);

        // compute precise frequency using the phase difference
        lastphase_(o)	= atan2(d,c);
        phasediff		= princArg(phase_(o)-lastphase_(o) - Omega) + Omega;
        frequency_(o)	= abs(phasediff * factor_ );
      }
      else
        frequency_(o) = o*fundamental_;
    }


    // compute precise amplitude
    mag_(o) = sqrt((a*a + b*b))*2;
    if (pick_)
    {
      mrs_real mag = lobe_value_compute((o * fundamental_-frequency_(o))/factor_, 1, N_);
      magCorr_(o) = mag_(o)/mag;
    }
    else
    {
      magCorr_(o) = mag_(o);
    }
    //mrs_real freq = frequency_(o);

    if(frequency_(o) != 0.0)
    {
      const mrs_natural searchRange	= 8;
      mrs_natural	k,
                  kd	= o,
                   ku	= o,
                    kEndSearch;
      mrs_real	diff[2];

      // find closest preceding frequency
      kEndSearch = max ((mrs_natural)0, o-searchRange);
      for (k = o-1; k > kEndSearch; k--)
      {
        diff[0]	= abs(frequency_(o) - lastfrequency_(k));
        diff[1] = abs(frequency_(o) - lastfrequency_(kd));
        if (diff[0] < diff[1])
          kd	= k;
      }
      kEndSearch = min (size_, o+searchRange);
      for (k = o+1; k < kEndSearch; k++)
      {
        diff[0]	= abs(frequency_(o) - lastfrequency_(k));
        diff[1] = abs(frequency_(o) - lastfrequency_(ku));
        if (diff[0] < diff[1])
          ku	= k;
      }
      diff[0]	= abs(frequency_(o) - lastfrequency_(kd));
      diff[1] = abs(frequency_(o) - lastfrequency_(ku));
      k = (diff[0] < diff[1])? kd : ku;

      deltafrequency_(o) = (lastfrequency_(k) == 0)? .0 : log10(lastfrequency_(k)/frequency_(o));
      //deltafrequency_(o) = (frequency_(o)-lastfrequency_(o))/(frequency_(o)+lastfrequency_(o));
    }
    else
      deltafrequency_(o) = .0;

    mrs_real lastmag	= lastmag_(o);
    if (o > 0)
      lastmag		= max(lastmag_(o-1), lastmag);
    if (o < size_-1)
      lastmag		= max(lastmag_(o+1), lastmag);

    if (mag_(o) > 0)
      deltamag_(o)		= (mag_(o)-lastmag)/mag_(o);
    else if (lastmag > 0)
      deltamag_(o)		= (mag_(o)-lastmag)/lastmag;
    else
      deltamag_(o)		= 0;
  }
  lastfrequency_	= frequency_;
  lastmag_		= mag_;
}

void PeakConvert2::ComputePeaker (mrs_realvec in, realvec& out)
{
#ifdef ORIGINAL_VERSION
  peaker_->updControl("mrs_real/peakStrength", 0.2);// to be set as a control [!]
#else
  peaker_->updControl("mrs_real/peakStrength",1e-1);
  peaker_->updControl("mrs_real/peakStrengthRelMax" ,1e-2);
  peaker_->updControl("mrs_real/peakStrengthAbs",1e-10 );
  peaker_->updControl("mrs_real/peakStrengthTreshLpParam" ,0.95);
  peaker_->updControl("mrs_real/peakStrengthRelThresh" , 1.);
#endif

  peaker_->updControl("mrs_real/peakSpacing", 2e-3);   // 0
  peaker_->updControl("mrs_natural/peakStart", downFrequency_);   // 0
  peaker_->updControl("mrs_natural/peakEnd", upFrequency_);  // size_
  peaker_->updControl("mrs_natural/inSamples", in.getCols());
  peaker_->updControl("mrs_natural/inObservations", in.getRows());
  peaker_->updControl("mrs_natural/onSamples", out.getCols());
  peaker_->updControl("mrs_natural/onObservations", out.getRows());

  peaker_->process(in, out);
}

void
PeakConvert2::myProcess(realvec& in, realvec& out)
{
  mrs_natural o,i;
  out.setval(0);
  peakView pkViewOut(out);

  const mrs_bool useMasking	= getctrl("mrs_bool/useMasking")->to<mrs_bool>();
  const mrs_real probThresh	= getctrl("mrs_real/probabilityTresh")->to<mrs_real>();

  max_->updControl("mrs_natural/nMaximums", frameMaxNumPeaks_);

  max_->setctrl("mrs_natural/inSamples", size_);
  max_->setctrl("mrs_natural/inObservations", 1);
  max_->update();
  tmp_.stretch(frameMaxNumPeaks_*2);

  for(mrs_natural f=0 ; f < inSamples_; ++f)
  {
    //we should avoid the first empty frames,
    //that will contain silence and consequently create
    //discontinuities in the signal, ruining the peak calculation!
    //only process if we have a full data vector (i.e. no zeros)
    if(frame_ >= skip_)
    {
      // get pair of ffts
      in.getCol (f, tmpBuff_);

      // compute magnitude, phase, and instantaneous frequency
      this->ComputeMagnitudeAndPhase (tmpBuff_);

      // compute masking threshold
      if (useMasking && pick_)
        ComputeMasking (tmpBuff_);
      else
        masked_.setval(10.);

      // select bins with local maxima in magnitude (--> peaks)
      peaks_ = mag_;
      if(pick_)
        this->ComputePeaker (mag_, peaks_);
      else
      {
        for (o = 0 ; o < downFrequency_ ; o++)
          peaks_(o)=0.0;
        for (o = upFrequency_ ; o < (mrs_natural)peaks_.getSize() ; o++)
          peaks_(o)=0.0;
      }

      if (lpCoeff_ > 0)
        FreqSmear (lpPeakerRes_);

      //compute the probability of a peak being a peak
      for(o=0 ; o < size_ ; o++)
      {
        if (peaks_(o) <= 0)
        {
          frequency_(o)		= .0;
          //lastmag_(o)		= .0;
          lastfrequency_(o)	= .0;
          // time smearing if no new peak
          lpPeakerRes_(o)	*=lpCoeff_;
          continue;
        }
#ifdef ORIGINAL_VERSION
        // probability of peak being a masker
        peakProb_(0)	= 0;
        // probability of peak being stationary
        peakProb_(1)	= 0;
        // probability of peak being tonal
        peakProb_(2)	= (abs(frequency_(o)/fundamental_-o) > .5)? 0 : 1;
#else
        // probability of peak being a masker
        peakProb_(0)	= max((mrs_real).1, (mrs_real).5 * (mrs_real)(log10(masked_(o)) +1.));
        // probability of peak being stationary
        peakProb_(1)	= max((mrs_real).1, (mrs_real)lpPeakerRes_(o));
        // probability or peak being tonal
        peakProb_(2)	= GaussianPdf (frequency_(o)/fundamental_-o, gaussianStd);
#endif

        // reset lpPeakerRes with peaker results
        lpPeakerRes_(o)	= 1;

        peakProb_ *= peakProbWeight_;
        if ((peakProb_.sum() < probThresh) && pick_)
        {
          peaks_(o)		= .0;
          frequency_(o)	= .0;
          //lastmag_(o)		= .0;
          lastfrequency_(o)	= .0;
        }
      }

      // keep only the frameMaxNumPeaks_ highest amplitude local maxima
      tmp_.setval(0.);
      max_->process(peaks_, tmp_);

      nbPeaks_=tmp_.getSize()/2;
      realvec index_(nbPeaks_); //[!] make member to avoid reallocation at each tick!
      for (i=0 ; i<nbPeaks_ ; i++)
        index_(i) = tmp_(2*i+1);

      // search for bins interval
      realvec interval_(nbPeaks_*2); //[!] make member to avoid reallocation at each tick!
      interval_.setval(0);
      if(pick_)
        getShortBinInterval(interval_, index_, mag_);
      else
      {
        for (i=0 ; i<nbPeaks_ ; i++)
          interval_(2*i+1) = index_(i);
      }

#ifdef LOG2FILE
      for (i=0 ; i<nbPeaks_ ; i++)
      {
        mrs_real value = frequency_((mrs_natural) (index_(i)+.1));
        pFDbgFile << std::scientific << std::setprecision(4) << value << "\t";
      }
      pFDbgFile << std::endl;
#endif
#ifdef MARSYAS_MATLAB
#ifdef MTLB_DBG_LOG
      MATLAB_PUT(mag_, "peaks");
      MATLAB_PUT(peaks_, "k");
      MATLAB_PUT(tmp_, "tmp");
      MATLAB_PUT(interval_, "int");
      MATLAB_PUT(frequency_, "freq");
//			MATLAB_EVAL("figure(1);clf;hold on ;plot(peaks);stem(k);stem(tmp(2:2:end)+1, peaks(tmp(2:2:end)+1), 'r')");
//			MATLAB_EVAL("stem(int+1, peaks(int+1), 'k')");
      MATLAB_EVAL("figure(1);hold on ;stem(freq(tmp(2:2:end)+1), peaks(tmp(2:2:end)+1), 'r');hold off");
#endif
#endif


      // fill output with peaks data
      interval_ /= N_;

      for (i = 0; i < nbPeaks_; i++)
      {
        mrs_natural index = (mrs_natural) (index_(i)+.1);
        pkViewOut(i, peakView::pkFrequency, f) = frequency_(index);
        pkViewOut(i, peakView::pkAmplitude, f) = magCorr_(index);
        pkViewOut(i, peakView::pkPhase, f) = -phase_(index);
        pkViewOut(i, peakView::pkDeltaFrequency, f) = deltafrequency_(index);
        pkViewOut(i, peakView::pkDeltaAmplitude, f) = /*abs*/(deltamag_(index));
        pkViewOut(i, peakView::pkFrame, f) = frame_;
        pkViewOut(i, peakView::pkGroup, f) = 0.;//(pick_)?-1.:0.; //This should be -1!!!! [TODO]
        pkViewOut(i, peakView::pkVolume, f) = 1.0;
        pkViewOut(i, peakView::pkBinLow, f) = interval_(2*i);
        pkViewOut(i, peakView::pkBin, f) = index_(i);
        pkViewOut(i, peakView::pkBinHigh, f) = interval_(2*i+1);
        pkViewOut(i, peakView::pkTrack, f) = -1.0; //null-track ID

        MRSASSERT((index_(i) <= interval_(2*i)) || (interval_(2*i+1) <= index_(i)));

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
}
