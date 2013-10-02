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

#include <marsyas/Conversions.h>
#include <marsyas/realvec.h>

using std::ostringstream;
using namespace Marsyas;

mrs_real
Marsyas::pitch2hertz(mrs_real pitch) {
  return mrs_real(440.0 * pow(2.0, ((pitch - 69.0) / 12.0)));
}

mrs_real
Marsyas::hertz2pitch(mrs_real hz) {
  return (hz == 0.0) ? (mrs_real)0.0 : (mrs_real)(69.0 + 12.0 * (log(hz/440.0)/log(2.0)));
}

mrs_real
Marsyas::samples2hertz(mrs_natural samples, mrs_real srate) {
  return (samples <= 0)  ? (mrs_real)0.0 : (mrs_real) (srate * 1.0) / (samples);
}

mrs_real
Marsyas::samples2hertz(mrs_real samples, mrs_real srate) {
  return (samples <= 0.001)  ? (mrs_real)0.0 : (mrs_real) (srate * 1.0) / (samples);
}


mrs_natural
Marsyas::hertz2samples(mrs_real hz, mrs_real srate) {
  return (hz == 0.0) ? (mrs_natural)0 : (mrs_natural) (srate / hz);
}

/* convert a string representing time to number of samples base on the
given sample rate. Format "123.456#" where # is the time division.
Valid time divisions: { h, m, s, ms, us }.
On a format error,
Errors: -1 is returned. ie more than 1 decimal point, invalid time
division.
*/
mrs_natural
Marsyas::time2samples(mrs_string time, mrs_real srate) {
  //example times: { "10us", "10ms", "10s", "10m", "10h" }
  if (time=="") { return 0; }
  // calculate time value
  mrs_real samples=0;
  int i=0;
  int len=(int)time.length();
  bool decimal_point=false;
  mrs_real divisor = 10.0;
  for (i=0; i<len && (time[i]=='.' || (time[i]>='0' && time[i]<='9')); ++i) {
    if (decimal_point) {
      if (time[i]=='.') { return -1; }
      samples = samples + ((mrs_real)(time[i]-'0'))/divisor;
      divisor = divisor * 10.0;
    } else if (time[i]=='.') {
      decimal_point=true;
    } else {
      samples = samples * 10.0 + (time[i]-'0');
    }
  }
  //
  if (i<len) {
    char a=time[++i];
    if (i>=len) {
      if (a=='h') { // hours
        samples= 120.0*samples*srate;
      } else if (a=='m') { // minutes
        samples=  60.0*samples*srate;
      } else if (a=='s') { // seconds
        samples=       samples*srate;
      } else {
        return -1;
      }
    } else {
      char b=time[i];
      if ((i+1)>=len) {
        if (a=='u' && b=='s') { // micro-seconds
          samples= samples/1000000.0*srate;
        } else if (a=='m' && b=='s') { // milli-seconds
          samples= samples/1000.0*srate;
        } else {
          return -1;
        }
      }
    }
  }
  return (mrs_natural)samples;
}
mrs_natural
Marsyas::time2usecs(mrs_string time) {
  //example times: { "10us", "10ms", "10s", "10m", "10h" }
  if (time=="") { return 0; }
  // calculate time value
  mrs_real usecs=0;
  int i=0;
  int len=(int)time.length();
  bool decimal_point=false;
  mrs_real divisor = 10.0;
  for (i=0; i<len && (time[i]=='.' || (time[i]>='0' && time[i]<='9')); ++i) {
    if (decimal_point) {
      if (time[i]=='.') { return -1; }
      usecs = usecs + ((mrs_real)(time[i]-'0'))/divisor;
      divisor = divisor * 10.0;
    } else if (time[i]=='.') {
      decimal_point=true;
    } else {
      usecs = usecs * 10.0 + (time[i]-'0');
    }
  }
  //
  if (i<len) {
    char a=time[++i];
    if (i>=len) {
      if (a=='h') { // hours
        usecs *= 1000.0 * 1000.0 * 60.0 * 60.0;
      } else if (a=='m') { // minutes
        usecs *= 1000.0 * 1000.0 * 60.0;
      } else if (a=='s') { // seconds
        usecs *= 1000.0 * 1000.0;
      } else {
        return -1;
      }
    } else {
      char b=time[i];
      if ((i+1)>=len) {
        if (a=='u' && b=='s') { // micro-seconds
          ;
        } else if (a=='m' && b=='s') { // milli-seconds
          usecs *= 1000.0;
        } else {
          return -1;
        }
      }
    }
  }
  return (mrs_natural)usecs;
}

mrs_real Marsyas::amplitude2dB(mrs_real a)
{
  MRSASSERT (a > 0);
  return 20*log10(a);
}

mrs_real Marsyas::dB2amplitude(mrs_real a)
{
  return pow(10.0, a/20);
}

mrs_real
Marsyas::hertz2octs(mrs_real f, mrs_real middleAfreq)
{
  //adapted from Dan Ellis fft2chromamx.m MATLAB routine
  //
  // octs = hz2octs(freq, A440)
  // Convert a frequency in Hz into a real number counting
  // the octaves above A0. So hz2octs(440) = 4.0
  // Optional A440 specifies the Hz to be treated as middle A (default 440).
  // 2006-06-29 dpwe@ee.columbia.edu for fft2chromamx
  //
  // A4 = 440 Hz, so A0 = 440/16 Hz
  // octs = log(freq./(A440/16))./log(2);
  //
  return log(f/(middleAfreq/16.0))/log(2.0);
}

mrs_real Marsyas::hertz2bark(mrs_real f, mrs_natural mode)
{
  switch (mode)
  {
  case 0:
  default:
    return  6 * log(f/600 + sqrt(1+ (pow(f/600,2)))); // 6*asinh(f/600);

  case  1:  //  zwicker
    return  13 * atan  (0.00076*f)  +  3.5  *  atan  ((f*0.000133333333333333)*(f*0.000133333333333333));

  case  2:  //  terhardt
    return  13.3  *  atan  (0.00075*f);

  case  3:  //  schroeder
    return  7.0  *  log  (f*0.00153846153846154  +  sqrt  ((f*0.00153846153846154)*(f*0.00153846153846154)  +  1));  //  7*asinh  (fFrequency/650);
  }
}

mrs_real Marsyas::bark2hertz(mrs_real f, mrs_natural mode)
{
  switch  (mode)
  {
  case 0:
  case 1:
  default:
    return 600*sinh(f/6.0);

  case  2:  //  terhardt
    return  4*1000.0/3.0  *  tan  (f*0.075187969924812);
  case  3:  //  schroeder
    return  650.0  *  sinh  (f*0.142857142857143);
  }
}

mrs_real Marsyas::hertz2erb(mrs_real hz)
{
  return 21.4 * log10(1+ (hz / 229.0));
}

mrs_real Marsyas::erb2hertz(mrs_real erb)
{
  return (pow(10, (erb/21.4)) - 1.0) * 229.0;
}


mrs_real
Marsyas::hertz2mel(mrs_real f, bool htk)
{
  //  z = hertz2mel(f,htk)
  //  Convert frequencies f (in Hz) to mel 'scale'.
  //  Optional htk = 1 uses the mel axis defined in the HTKBook
  //  otherwise use Slaney's formula.
  //
  //  adapted from Dan Ellis fft2melmx.m MATLAB code

  if(htk)
  {
    return 2595.0 * log10(1.0 + f / 700.0);
  }
  else
  {
    // Mel fn to match Slaney's Auditory Toolbox mfcc.m
    mrs_real f_0 = 0.0; //133.33333;
    mrs_real f_sp = 200.0/3.0; //66.66667;
    mrs_real brkfrq = 1000.0;
    mrs_real brkpt  = (brkfrq - f_0)/f_sp;  //starting mel value for log region

    //the magic 1.0711703 which is the ratio needed to get from
    //1000 Hz to 6400 Hz in 27 steps, and is *almost* the ratio between
    //1000 Hz and the preceding linear filter center at 933.33333 Hz
    //(actually 1000/933.33333 = 1.07142857142857 and
    //exp(log(6.4)/27) = 1.07117028749447)
    mrs_real logstep = exp(log(6.4)/27.0);

    if(f < brkfrq)
      return (f - f_0) / f_sp; //linear
    else
      return brkpt + log(f / brkfrq) / log(logstep); //non-linear
  }
}

mrs_real
Marsyas::mel2hertz(mrs_real z, bool htk)
{
  //   f = mel2hz(z, htk)
  //   Convert 'mel scale' frequencies into Hz
  //   Optional htk = 1 means use the HTK formula
  //   else use the formula from Slaney's mfcc.m
  //
  //	 Adapted from Dan Ellis fft2melmx.m MATLAB code

  if(htk)
  {
    return 700.0 * (pow(10.0, z/2595.0) - 1.0);
  }
  else
  {
    mrs_real f_0 = 0.0; //133.33333;
    mrs_real f_sp = 200.0 / 3.0; //66.66667;
    mrs_real brkfrq = 1000.0;
    mrs_real brkpt  = (brkfrq - f_0)/f_sp; //starting mel value for log region

    //the magic 1.0711703 which is the ratio needed to get from 1000 Hz
    //to 6400 Hz in 27 steps, and is *almost* the ratio between 1000 Hz
    //and the preceding linear filter center at 933.33333 Hz
    //(actually 1000/933.33333 = 1.07142857142857 and
    //exp(log(6.4)/27) = 1.07117028749447)
    mrs_real logstep = exp(log(6.4)/27.0);

    if(z < brkpt)
      return f_0 + f_sp * z;
    else
      return brkfrq * exp(log(logstep)*(z-brkpt));
  }
}

// Returns the frequency in Hz, represented by a PowerSpectrum bin number
// (as returned from the PowerSpectrum Marsystem).
// * bin is the bin number
// * nyquistFreq is the Nyquist frequency (the sampling rate / 2)
// * framerate is the samplerate / total # bins (usually we can pass in israte_)
//
// For example, if the nyquistFreq = 4000, the original sample rate was 8000Hz,
// and we are using a framesize of 8 samples (8 FFT bins), then we should pass
// in framerate=1000(Hz). Then there are 5 PowerSpect bins ((8/2)+1)
// corresponding to frequencies as follows:
// bin=0: returns 0
// bin=1: returns 4000   (+/- 4000 Hz)
// bin=2: returns 1000   (+/- 1000 Hz)
// bin=3: returns 2000   (+/- 2000 Hz)
// bin=4: returns 3000   (+/- 3000 Hz)
mrs_real
Marsyas::bin2hertz(mrs_natural bin, mrs_real nyquistFreq, mrs_real framerate)
{
  if (bin == 0)
    return 0;
  if (bin == 1)
    return nyquistFreq;
  else
    return (bin-1) * framerate;
}

mrs_natural Marsyas::powerOfTwo(mrs_real v)
{
  mrs_natural n=1, res=0;
  while(res < v)
  {
    res = (mrs_natural) pow(2.0, n+.0);
    n++;
  }
  return res;
}

void
Marsyas::string2parameters(mrs_string s, realvec &v, char d)
{
  mrs_natural i =0, pos=0, newPos=0;
  mrs_string tmp;
  while(newPos != -1 )
  {
    newPos = (mrs_natural) s.find_first_of(&d, pos, 1);
    tmp = s.substr(pos, newPos);
    v(i++) = atof(tmp.c_str());
    pos = newPos+1;
  }
}
