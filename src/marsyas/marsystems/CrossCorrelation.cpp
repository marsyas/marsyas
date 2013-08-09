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

#include "CrossCorrelation.h"


using std::ostringstream;
using std::cout;
using std::endl;
using std::abs;

using namespace Marsyas;

CrossCorrelation::CrossCorrelation(mrs_string name):MarSystem("CrossCorrelation",name)
{
  myfft_ = NULL;
  addControls();
}

// destructor
CrossCorrelation::~CrossCorrelation()
{
  delete myfft_;
}

// copy constructor
CrossCorrelation::CrossCorrelation(const CrossCorrelation& a):MarSystem(a)
{
  myfft_ = NULL;
  ctrl_mode_ = getctrl("mrs_string/mode");
}

void
CrossCorrelation::addControls()
{
  // cross correlation modes: "general", "phat", "ml"
  addctrl("mrs_string/mode","general",ctrl_mode_);
}

MarSystem*
CrossCorrelation::clone() const
{
  return new CrossCorrelation(*this);
}

void
CrossCorrelation::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)
  delete myfft_; //[!]
  myfft_ = new fft();//[!]

  setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples"));
  setctrl("mrs_natural/onObservations", getctrl("mrs_natural/inObservations")->to<mrs_natural>() - 1);
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));

  scratch_.create(getctrl("mrs_natural/onSamples")->to<mrs_natural>());
  scratch1_.create(getctrl("mrs_natural/onSamples")->to<mrs_natural>());
  scratch2_.create(getctrl("mrs_natural/onSamples")->to<mrs_natural>());

  // for sub loop (used to get averaged fft in maximum likelihood mode)
  sub_scratch1_.create(getctrl("mrs_natural/onSamples")->to<mrs_natural>());
  sub_scratch2_.create(getctrl("mrs_natural/onSamples")->to<mrs_natural>());
}

void
CrossCorrelation::myProcess(realvec& in, realvec& out)
{
  // this cross correlation will take in N observations and return N-1 observations
  mrs_natural o,t;
  mrs_real re1,im1,re2,im2,re,im,abs_out;

  for (o=0; o < (inObservations_ - 1); o++)
  {
    mrs_real *channelOut = scratch_.getData();
    mrs_real *channel1 = scratch1_.getData();
    mrs_real *channel2 = scratch2_.getData();

    for (t=0; t < inSamples_; t++) {
      scratch_(t) = 0;
      scratch1_(t) = in(o,t);
      scratch2_(t) = in(o+1,t);
    }

    mode_ = getctrl("mrs_string/mode")->to<mrs_string>();

    myfft_->rfft(channel1, inSamples_/2, FFT_FORWARD);
    myfft_->rfft(channel2, inSamples_/2, FFT_FORWARD);

    if (mode_ == "general")
    {

      //Compress the magnitude spectrum and zero
      // the imaginary part.
      for (t=1; t < inSamples_/2; t++)
      {
        re1 = channel1[2*t];
        im1 = channel1[2*t+1];

        re2 = channel2[2*t];
        im2 = channel2[2*t+1];

        //(re1 + j im1)*(re2 - j im2) = (re1*re2 + im1*im2) + j(im1re2 - im2re1)

        re = re1*re2 + im1*im2;
        im = re2*im1 - re1*im2;

        channelOut[2*t] = re;
        channelOut[2*t+1] = im;
      }
    }
    else if (mode_ == "phat")
    {

      //Compress the magnitude spectrum and zero
      // the imaginary part.
      for (t=1; t < inSamples_/2; t++)
      {
        re1 = channel1[2*t];
        im1 = channel1[2*t+1];

        re2 = channel2[2*t];
        im2 = channel2[2*t+1];

        //(re1 + j im1)*(re2 - j im2) = (re1*re2 + im1*im2) + j(im1re2 - im2re1)

        re = re1*re2 + im1*im2;
        im = re2*im1 - re1*im2;

        // divide by absolute value (PHAT)
        abs_out = sqrt(re*re + im*im);

        re = re/abs_out;
        im = im/abs_out;

        channelOut[2*t] = re;
        channelOut[2*t+1] = im;

      }

    }
    else if (mode_ == "ml")
    {
      //	Maximum Likelihood - robust solution for time difference calculation
      //
      //	Reference:
      //   MAXIMUM LIKELIHOOD TIME DELAY ESTIMATION WITH PHASE DOMAIN ANALYSIS
      //		IN THE GENERALIZED CROSS CORRELATION FRAMEWORK

      mrs_real *sub_channel1 = sub_scratch1_.getData();
      mrs_real *sub_channel2 = sub_scratch2_.getData();

      mrs_real sub_re1, sub_im1, sub_re2, sub_im2;
      mrs_natural sub_window_size, sub_hop, sub_count, sub_start, sub_end;

      mrs_real re_q1_1,re_q2_2,re_q1_2,im_q1_2;

      mrs_natural window_size  = inSamples_;

      mrs_realvec q1_1(window_size);
      mrs_realvec q2_2(window_size);
      mrs_realvec q1_2(window_size);
      mrs_realvec mu1_2(window_size);
      mrs_realvec v1_2(window_size);


      //-------------------------------------------------------------------------------
      // SUB LOOP -
      // Used to calculate expected correlation mean, phase mean and phase variation
      // based on subsets of the data

      sub_window_size = window_size/4;
      sub_hop = window_size/8;
      sub_start = 0;
      sub_end = sub_start + sub_window_size;
      sub_count = 1;

      // Set to zero
      for (t=0; t < inSamples_; t++)
      {
        q1_1(t) = 0;
        q2_2(t) = 0;
        q1_2(t) = 0;
        mu1_2(t) = 0;
        v1_2(t) = 0;
      }

      while (sub_end < window_size)
      {
        for (t=0; t < sub_window_size; t++)
        {
          sub_scratch1_(t) = 0;
          sub_scratch2_(t) = 0;

          sub_scratch1_(t) = in(o,t+sub_start);
          sub_scratch2_(t) = in(o+1,t+sub_start);
        }

        // zeropad
        for (t=sub_window_size; t < window_size; t++)
        {
          sub_scratch1_(t) = 0;
          sub_scratch2_(t) = 0;
        }

        myfft_->rfft(sub_channel1, inSamples_/2, FFT_FORWARD);
        myfft_->rfft(sub_channel2, inSamples_/2, FFT_FORWARD);

        for (t=0; t < inSamples_/2; t++)
        {
          sub_re1 = sub_channel1[2*t];
          sub_im1 = sub_channel1[2*t+1];

          sub_re2 = sub_channel2[2*t];
          sub_im2 = sub_channel2[2*t+1];

          //(re1 + j im1)*(re2 - j im2) = (re1*re2 + im1*im2) + j(im1re2 - im2re1)

          // autocorrelations
          q1_1(2*t) = q1_1(2*t) + (sub_re1*sub_re1 + sub_im1*sub_im1);
          q1_1(2*t+1) =0;

          q2_2(2*t) = q2_2(2*t) + (sub_re2*sub_re2 + sub_im2*sub_im2);
          q2_2(2*t+1) =0;

          // cross correlation
          q1_2(2*t) = q1_2(2*t) + (sub_re1*sub_re2 + sub_im1*sub_im2);
          q1_2(2*t+1) = q1_2(2*t+1) + (sub_re2*sub_im1 - sub_re1*sub_im2);
        }

        sub_start = sub_start + sub_hop;
        sub_end = sub_end + sub_hop;
        sub_count = sub_count + 1;
      }


      for (t=0; t < inSamples_/2; t++)
      {
        // q = q/sub_count; (for averaging)
        re_q1_1 = q1_1(2*t)/sub_count;
        re_q2_2 = q2_2(2*t)/sub_count;

        re_q1_2 = q1_2(2*t)/sub_count;
        im_q1_2 = q1_2(2*t+1)/sub_count;

        //	mu1_2 = abs(q1_2)/sqrt(q2_2*q1_1)
        mu1_2(2*t) = sqrt(re_q1_2*re_q1_2 + im_q1_2*im_q1_2)/sqrt(re_q1_1*re_q2_2);
        mu1_2(2*t+1) = 0;

        v1_2(2*t) = (1-(mu1_2(2*t)*mu1_2(2*t)))/(mu1_2(2*t)*mu1_2(2*t));

      }

      //cout << "q1_1: " << q1_1(280) << endl;
      //cout << "q2_2: " << q2_2(280) << endl;
      //cout << "q1_2: " << q1_2(280) << endl;
      //cout << "mu1_2: " << mu1_2(280) << endl;
      //cout << "v1_2: " << v1_2(280) << endl;

      //-------------------------------------------------------------------------------
      // MAIN LOOP
      // Multiplied weighting calculated above with generalized cross-correlation

      for (t=1; t < inSamples_/2; t++)
      {
        re1 = channel1[2*t];
        im1 = channel1[2*t+1];

        re2 = channel2[2*t];
        im2 = channel2[2*t+1];

        //(re1 + j im1)*(re2 - j im2) = (re1*re2 + im1*im2) + j(im1re2 - im2re1)
        re = re1*re2 + im1*im2;
        im = re2*im1 - re1*im2;

        // absolute value
        abs_out = sqrt(re*re + im*im);

        //output = A0_1/(abs(A0_1)*sqrt(v0_1))
        channelOut[2*t] = re/(abs_out*sqrt(v1_2(2*t)));
        channelOut[2*t+1] = im/(abs_out*sqrt(v1_2(2*t)));

        //channelOut[2*t] = re/abs_out;
        //channelOut[2*t+1] = im/abs_out;

      }

      //cout << "channel1: " << "re " << channel1[280] << "im  " << channel1[281] << endl;
      //cout << "channel2: " << "re " << channel2[280] << "im  " << channel2[281] << endl;
      //cout << "channelOut: " << channelOut[280] << endl;

    }

    else
    {
      cout << "Invalid Mode" << endl;
    }

    //Take the inverse Fourier Transform
    myfft_->rfft(channelOut, inSamples_/2, FFT_INVERSE);
    //cout << channelOut[24] << endl;

    // Return Cross Correlation Output (with FFT shift - two loops)
    for (t=0; t < inSamples_/2; t++)
      out(o,t) = abs(scratch_(t + inSamples_/2));
    //out(o,2*t+1) = 0;

    for (t=inSamples_/2; t < inSamples_; t++)
      out(o,t) = abs(scratch_(t - inSamples_/2));
    //out(o,2*t+1) = 0;



  }
}
