#include "ConstQFiltering.h"

using std::ostringstream;
using namespace Marsyas;

ConstQFiltering::ConstQFiltering(mrs_string name):MarSystem("ConstQFiltering", name)
{
  addControls();
}

ConstQFiltering::ConstQFiltering(const ConstQFiltering& a):MarSystem(a)
{
  ctrl_qValue_ = getctrl("mrs_real/qValue");
  ctrl_lowFreq_ = getctrl("mrs_real/lowFreq");
  ctrl_highFreq_ = getctrl("mrs_real/highFreq");
  ctrl_width_ = getctrl("mrs_natural/width");
  ctrl_channels_ = getctrl("mrs_natural/channels");
  ctrl_time_ = getctrl("mrs_realvec/time");
  ctrl_freq_ = getctrl("mrs_realvec/freq");
}

ConstQFiltering::~ConstQFiltering()
{
}

MarSystem*
ConstQFiltering::clone() const
{
  return new ConstQFiltering(*this);
}

void
ConstQFiltering::addControls()
{
  addControl("mrs_real/qValue", 60.0, ctrl_qValue_);
  addControl("mrs_real/lowFreq", 60.0, ctrl_lowFreq_);
  addControl("mrs_real/highFreq", 6000.0, ctrl_highFreq_);
  addControl("mrs_natural/width", 2048, ctrl_width_);
  addControl("mrs_natural/channels", 256, ctrl_channels_);
  addControl("mrs_realvec/time", time_, ctrl_time_);
  addControl("mrs_realvec/freq", freq_, ctrl_freq_);
}

void ConstQFiltering::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)

  mrs_natural h, i;
  mrs_real f, bw;
  mrs_real lowFreq_, highFreq_;
  mrs_natural width_, channels_;
  mrs_real qValue_;

  lowFreq_ = ctrl_lowFreq_->to<mrs_real>();
  highFreq_ = ctrl_highFreq_->to<mrs_real>();
  width_ = ctrl_width_->to<mrs_natural>();
  channels_ = ctrl_channels_->to<mrs_natural>();
  qValue_ = ctrl_qValue_->to<mrs_real>();

  time_.create(width_);
  freq_.create(channels_);

  updControl("mrs_natural/onSamples", width_);
  updControl("mrs_natural/onObservations", channels_*2);
  updControl("mrs_real/osrate", israte_);

  fil_.create(channels_,width_);
  fshift_.create(channels_);
  for(h=0; h<channels_; h++) {
    freq_(h) = exp(log(lowFreq_)+(log(highFreq_)-log(lowFreq_))/(double)(channels_-1)*(double)h);
    bw = freq_(h)/(double)qValue_;
    fshift_(h) = (int)(freq_(h)/(israte_/(double)inSamples_));
    for(i=0; i<width_/2; ++i) {
      f = (double)(i+fshift_(h))/(double)inSamples_*israte_;
      fil_(h,i) = exp(-(f-freq_(h))*(f-freq_(h))/(2.0*bw*bw));
    }
    for(i=width_/2; i<width_; ++i) {
      f = (double)(i+fshift_(h)-width_)/(double)inSamples_*(double)israte_;
      fil_(h,i) = exp(-(f-freq_(h))*(f-freq_(h))/(2.0*bw*bw));
    }
  }
  for(i=0; i<width_; ++i) {
    time_(i) = (double)inSamples_/width_/israte_*i*1000.0;
  }

  spec1_.create(inSamples_,1);
  spec2_.create(width_*2,1);

  ctrl_time_->setValue(time_);
  ctrl_freq_->setValue(freq_);
}

void
ConstQFiltering::myProcess(realvec& in, realvec& out)
{
  mrs_natural h,i;
  mrs_real *tmp;
  //mrs_real lowFreq_, highFreq_;
  mrs_natural width_, channels_;
  //mrs_real qValue_;

  //lowFreq_ = ctrl_lowFreq_->to<mrs_real>();
  //highFreq_ = ctrl_highFreq_->to<mrs_real>();
  width_ = ctrl_width_->to<mrs_natural>();
  channels_ = ctrl_channels_->to<mrs_natural>();
  //qValue_ = ctrl_qValue_->to<mrs_real>();

  if(inSamples_>0) {
    for(i=0; i<inSamples_; ++i) {
      spec1_(i,0) = in(0,i);
    }
    tmp = spec1_.getData();
    fft1_.rfft(tmp, inSamples_/2, FFT_FORWARD);
    for(h=0; h<channels_; h++) {
      for(i=0; i<width_*2; ++i) {
        spec2_(i,0) = 0.0;
      }
      for(i=0; i<width_/2 && fshift_(h)+i<inSamples_/2; ++i) {
        spec2_(2*i,0) = fil_(h,i)*spec1_((mrs_natural)(2*(fshift_(h)+i)),0);
        spec2_(2*i+1,0) = fil_(h,i)*spec1_((mrs_natural)(2*(fshift_(h)+i)+1),0);
      }
      for(i=width_-1; i>=width_/2 && fshift_(h)+i-width_>=0; i--) {
        spec2_(2*i,0) = fil_(h,i)*spec1_((mrs_natural)(2*(fshift_(h)+i-width_)),0);
        spec2_(2*i+1,0) = fil_(h,i)*spec1_((mrs_natural)(2*(fshift_(h)+i-width_)+1),0);
      }

      tmp = spec2_.getData();
      fft2_.cfft(tmp, width_, FFT_INVERSE);
      for(i=0; i<width_; ++i) {
        out(2*h,i) = spec2_(2*i,0)*cos(fshift_(h)/width_*i) - spec2_(2*i+1,0)*sin(fshift_(h)/width_*i);
        out(2*h+1,i) = spec2_(2*i,0)*sin(fshift_(h)/width_*i) + spec2_(2*i+1,0)*cos(fshift_(h)/width_*i);
      }
    }
  }
}
