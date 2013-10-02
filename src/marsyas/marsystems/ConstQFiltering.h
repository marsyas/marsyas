#ifndef MARSYAS_CONSTQFILTERING_H
#define MARSYAS_CONSTQFILTERING_H

#include <marsyas/system/MarSystem.h>
#include <marsyas/fft.h>

namespace Marsyas
{
/**
   \class ConstQFiltering
   \ingroup MarSystem
   \brief Calculate wavelet transform

   Controls:
   - \b mrs_real/qValue        [w] : sets Q value
   - \b mrs_real/lowFreq       [w] : sets lower limit of frequencya
   - \b mrs_real/highFreq      [w] : sets higher limit of frequency
   - \b mrs_natural/width      [w] : sets number of frames (maximum bandwidth)
   - \b mrs_natural/channels   [w] : sets number of channels
   - \b mrs_string/mode        [w] : sets mode, forward or backward
   - \b mrs_realvec/time       [r] : corresponds to time (ms)
   - \b mrs_realvec/freq       [r] : corresponding frequency

 */

class ConstQFiltering: public MarSystem
{
private:
  realvec fil_;
  realvec fshift_;
  realvec spec1_;
  realvec spec2_;
  realvec time_;
  realvec freq_;
  mrs_string mode_;

  fft fft1_;
  fft fft2_;

  MarControlPtr ctrl_qValue_;
  MarControlPtr ctrl_lowFreq_;
  MarControlPtr ctrl_highFreq_;
  MarControlPtr ctrl_width_;
  MarControlPtr ctrl_channels_;
  MarControlPtr ctrl_time_;
  MarControlPtr ctrl_freq_;
  MarControlPtr ctrl_mode_;

  void addControls();
  void myUpdate(MarControlPtr sender);

public:
  ConstQFiltering(std::string name);
  ConstQFiltering(const ConstQFiltering& a);
  ~ConstQFiltering();

  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};
}

#endif
