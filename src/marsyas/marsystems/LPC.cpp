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

#include "LPC.h"
#include "../common_source.h"

// #define _MATLAB_LPC_

using std::ostringstream;
using namespace Marsyas;

LPC::LPC(mrs_string name):MarSystem("LPC",name)
{
  addControls();
}

LPC::LPC(const LPC& a):MarSystem(a)
{
  ctrl_coeffs_ = getctrl("mrs_realvec/coeffs");
  ctrl_pitch_ = getctrl("mrs_real/pitch");
  ctrl_power_ = getctrl("mrs_real/power");
}

LPC::~LPC()
{
}

MarSystem*
LPC::clone() const
{
  return new LPC(*this);
}

void
LPC::addControls()
{
  addctrl("mrs_natural/order", (mrs_natural)10);
  addctrl("mrs_realvec/coeffs", realvec(), ctrl_coeffs_);
  addctrl("mrs_real/pitch", 0.0, ctrl_pitch_);
  addctrl("mrs_real/power", 0.0, ctrl_power_);
  setctrlState("mrs_natural/order", true);
  addctrl("mrs_real/lambda", (mrs_real)0.0);
  addctrl("mrs_real/gamma", (mrs_real)1.0);
}

void
LPC::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)
  MRSDIAG("LPC.cpp - LPC:myUpdate");

  order_ = getctrl("mrs_natural/order")->to<mrs_natural>();

  setctrl("mrs_natural/onObservations", (mrs_natural)(order_+2)); // <order_> coefs + pitch value + power value
  setctrl("mrs_natural/onSamples", (mrs_natural)1);
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));

  //LPC features names
  ostringstream oss;
  for (mrs_natural i = 0; i < order_; ++i)
    oss << "LPC_" << i+1 << ",";
  oss << "LPC_Pitch," << "LPC_Gain,";
  setctrl("mrs_string/onObsNames", oss.str());

  temp_.create(order_ ,order_);
  Zs_.create(order_);

  {
    MarControlAccessor acc(ctrl_coeffs_, NOUPDATE);
    realvec& coeffs = acc.to<mrs_realvec>();
    coeffs.stretch(order_+1);
  }

  //MATLAB engine stuff - for testing and validation purposes only!
#ifdef _MATLAB_LPC_
  MATLAB_EVAL("LPC_pitch = [];");
#endif

}

//perhaps this method could become an independent MarSystem in the future...
void
LPC::autocorrelationWarped(const realvec& in, realvec& r, mrs_real& pitch, mrs_real lambda)
{
  //*Based on the code from: http://www.musicdsp.org/showone.php?id=137

  //find the order-P autocorrelation array, R, for the sequence x
  //of length L and using a warping factor of lambda

  mrs_real* x = in.getData();
  mrs_natural L = in.getSize();
  mrs_real* R = r.getData();
  mrs_natural P = in.getSize()/2;//order_;

  mrs_real* dl = new mrs_real[L];
  mrs_real* Rt = new mrs_real[L];
  mrs_real r1,r2,r1t;
  R[0]=0;
  Rt[0]=0;
  r1=0;
  r2=0;
  r1t=0;
  for(mrs_natural k=0; k<L; k++)
  {
    Rt[0]+=x[k]*x[k];

    dl[k]=r1-lambda*(x[k]-r2);
    r1 = x[k];
    r2 = dl[k];
  }
  for(mrs_natural i=1; i<=P; ++i)
  {
    Rt[i]=0;
    r1=0;
    r2=0;
    for(mrs_natural k=0; k<L; k++)
    {
      Rt[i]+=dl[k]*x[k];

      r1t = dl[k];
      dl[k]=r1-lambda*(r1t-r2);
      r1 = r1t;
      r2 = dl[k];
    }
  }

  for(long i=0; i<=P; ++i)
    R[i]=Rt[i]/in.getSize(); // [ML] change /

  delete[] dl;
  delete[] Rt;

  //----------------------------------------------------
  // estimate pitch
  //----------------------------------------------------
  mrs_real temp = r(0);
  //set peak searching start point to 2% of total window size [?]
  mrs_real j = (mrs_real)in.getSize() * 0.02;
  //detect first local minimum...
  while (r((mrs_natural)j) < temp && j < in.getSize()/2)
  {
    temp = r((mrs_natural)j);
    j++;
  }
  //... and then from there, detect higher peak => period estimate!
  temp = 0.0;
  for (mrs_natural i=(mrs_natural)j; i < in.getSize() * 0.5; ++i)
  {
    if (r(i) > temp)
    {
      j = i;
      temp = r(i);
    }
  }

  // This code is from the original Marsyas0.1 AutoCorrelation class
  //this seems like some sort of Narrowed Autocorrelation (NAC)... [?][!]
  //however, it does not seem to fit the NAC definition...
  //so, what is this for??
  // 	mrs_real norm = 1.0 / (mrs_real)in.getSize();
  // 	mrs_natural k = in.getSize()/2;
  // 	for (mrs_natural i=0; i < in.getSize()/2; ++i)
  // 	r(i) *= (k-i) * norm;

  //if autocorr peak not very prominent => not a good period estimate
  //so discard it...
  if ((r((mrs_natural)j) / r(0)) < 0.4) j=0;
  //avoid detection of too high fundamental freqs (e.g. harmonics)?
  if (j > in.getSize()/4) j = 0;

  //pitch gets in fact the period (i.e. autocorr lag)
  //measured in time samples... maybe this should be converted
  //to a more convenient representation (e.g. pitch in Hz).
  //Such a conversion would have to depend on the warp factor lambda... [!]
  pitch  = (mrs_real)j;

  //MATLAB engine stuff - for testing and validation purposes only!
#ifdef _MATLAB_LPC_
  MATLAB_PUT(pitch, "pitch");
  MATLAB_EVAL("LPC_pitch = [LPC_pitch pitch];");
#endif
}

//Perhaps this method could become a member of realvec...
void
LPC::LevinsonDurbin(const realvec& r, realvec& a, realvec& kVec, mrs_real& e)
{
  //*Based on the code from: http://www.musicdsp.org/showone.php?id=137

  mrs_natural P = order_;
  mrs_real* R = r.getData();
  mrs_real* A = a.getData();
  mrs_real* K = kVec.getData();
  e = 0.0; //prediction error;

  mrs_real Am1[62];

  if(R[0]==0.0)
  {
    for(mrs_natural i=1; i<=P; ++i)
    {
      K[i]=0.0;
      A[i]=0.0;
    }
  }
  else
  {
    mrs_real km,Em1,Em;
    mrs_natural k,s,m;

    for (k=0; k<=P; k++) {
      A[k]=0;
      Am1[k]=0;
    }

    A[0]=1;
    Am1[0]=1;
    km=0;
    Em1=R[0];

    for (m=1; m<=P; m++)              //m=2:N+1
    {
      mrs_real err=0.0f;                //err = 0;

      for (k=1; k<=m-1; k++)        //for k=2:m-1
        err += Am1[k]*R[m-k];     // err = err + am1(k)*R(m-k+1);

      km = (R[m]-err)/Em1;          //km=(R(m)-err)/Em1;
      K[m-1] = -km;
      A[m]=km;                      //am(m)=km;

      for (k=1; k<=m-1; k++)        //for k=2:m-1
        A[k]=Am1[k]-km*Am1[m-k];  // am(k)=am1(k)-km*am1(m-k+1);

      Em=(1-km*km)*Em1;             //Em=(1-km*km)*Em1;

      for(s=0; s<=P; s++)           //for s=1:N+1
        Am1[s] = A[s];            // am1(s) = am(s)

      Em1 = Em;                     //Em1 = Em;
      //e = Em1; //prediction error
      e = Em1*Em1; //RMS prediction error
    }
    e = sqrt(e/(mrs_real)P); //RMS prediction error
  }
}

mrs_real
LPC::predictionError(const realvec& data, const realvec& coeffs)
{
  mrs_natural i, j;
  mrs_real power = 0.0;
  mrs_real error, tmp;

  //load delay line with input data...
  for (i=0; i< order_; ++i)
  {
    Zs_(i) = data(order_-i-1);
  }
  //apply LPC filter and estimate RMS of the error (=~ LPC Gain)
  mrs_natural count = 0;
  for (i=order_; i<(mrs_natural)data.getSize() ; ++i)
  {
    tmp = 0.0;
    for (j=0; j< order_; j++)
      tmp += Zs_(j) * coeffs(j);
    for (j=order_-1; j>0; j--)
      Zs_(j) = Zs_(j-1);

    Zs_(0) = data(i);
    error = data(i) - tmp;
    power += error * error;
    count ++;
  }
  return sqrt(power/(mrs_real)count);//=~ RMS LPC Gain
}




/*

Computation of the autocorrelation coefficients and the prediction error gain
using the implementation of peter Kabal
http://www-mmsp.ece.mcgill.ca/Documents/Software/index.html

*/

mrs_real
LPC::VRfDotProd (mrs_real * x1, mrs_real * x2, mrs_natural N)
{
  mrs_natural i;
  mrs_real sum;

  sum = 0.0;
  for (i = 0; i < N; ++i)
    sum +=  x1[i] * x2[i];

  return sum;
}

void
LPC::SPautoc (mrs_real * x, mrs_natural Nx, mrs_real * cor, mrs_natural Nt)
{
  mrs_natural i;
  mrs_natural N;

  N = Nt;
  if (Nt > Nx)
    N = Nx;

  for (i = 0; i < N; ++i)
    cor[i] =  VRfDotProd (x, &x[i], Nx - i);

  for (i = N; i < Nt; ++i)
    cor[i] = 0.0;

  return;
}


mrs_real
LPC::SPcorXpc (mrs_real * rxx, mrs_real * pc, mrs_natural Np)
{
  mrs_natural i, j, k;
  mrs_real rc, tp;
  mrs_real perr, t, sum;

  perr = rxx[0];


  for (k = 0; k < Np; ++k) {

    /* Calculate the next reflection coefficient */
    sum = rxx[k+1];
    for (i = 0; i < k; ++i)
      sum = sum - rxx[k-i] * pc[i];

    /* Check for zero prediction error */
    if (perr == 0.0 && sum == 0.0)
      rc = 0.0;
    else
      rc =  (-sum / perr);

    /* Calculate the error (equivalent to perr = perr * (1-rc^2))
    A change in sign of perr means that rc (reflection coefficient for stage k)
    has a magnitude greater than unity (corresponding to an unstable synthesis
    filter)
    */
    t = perr + rc * sum;

    perr = t;

    /* Convert from reflection coefficients to predictor coefficients */
    pc[k] = -rc;
    for (i = 0, j = k - 1; i < j; ++i, --j) {
      tp = pc[i] + rc * pc[j];
      pc[j] = pc[j] + rc * pc[i];
      pc[i] = tp;
    }
    if (i == j)
      pc[i] = pc[i] + rc * pc[i];
  }

  return perr;
}



void
LPC::myProcess(realvec& in, realvec& out)
{
  mrs_natural i;
  mrs_real LevinsonError = 0.0;
  // FIXME This variable is defined but (possibly) never used.
  // mrs_real PredictionError = 0.0;
  mrs_real pitch = 0.0, lag = 0.0;

  //MATLAB engine stuff - for testing and validation purposes only!
#ifdef _MATLAB_LPC_
  MATLAB_PUT(featureMode_, "featureMode");
  MATLAB_PUT(in, "LPC_in");
  MATLAB_PUT(order_, "LPC_order");
  MATLAB_PUT(getctrl("mrs_real/gamma")->to<mrs_real>(), "LPC_gamma");
#endif

  //-------------------------
  // warped autocorrelation
  //-------------------------
  //calculate warped autocorrelation coeffs
  realvec r(in.getSize());

  //--------------------------
  // Levison-Durbin recursion
  //--------------------------
  //Calculate LPC alpha and reflections coeffs
  //using Levison-Durbin recursion
  //output format for LPC coeffs:
  // y(n) = x(n) - a(1)x(n-1) - ... - a(order_-1)x(n-order_-1)
  // a = [1 a(1) a(2) ... a(order_-1)]
  realvec a(order_+1);
  realvec k(order_+1); //reflection coeffs

  // previous implementation from musicDSP without correct gain estimation
  // LevinsonDurbin(r, a, k, LevinsonError);

  // implementation adapted from peter Kabal
  //SPautoc (in.getData(), in.getSize(), k.getData(), k.getSize());
  //LevinsonError = SPcorXpc (k.getData(), a.getData(), a.getSize()-1);
  //cout << LevinsonError << endl;

  //this also estimates the pitch - does it work if lambda != 0 [?] [ML] normalization issue
  autocorrelationWarped(in, r, lag, getctrl("mrs_real/lambda")->to<mrs_real>());
  LevinsonError = SPcorXpc (r.getData(), a.getData(), a.getSize()-1);

  // LevinsonError /= in.getSize(); [ML] add this if SPautoc used
  LevinsonError = sqrt(LevinsonError);
  // pitch in Hz
  pitch = getctrl("mrs_real/israte")->to<mrs_real>()/lag ;

  //--------------------------
  // LPC coeffs
  //--------------------------
  //output LPC coeffs
  // y(n) = x(n) - a(1)x(n-1) - ... - a(order_-1)x(n-order_-1)
  // a = [1 a(1) a(2) ... a(order_-1)]
  // out = [a(1) a(2) ... a(order_-1)]


  for(i=0; i < order_; ++i)
    // out(i) = a(i+1); // musicDsp implementation
    out(i) = -a(i); // musicDsp implementation

  //------------------------------
  //output pitch and power values
  //------------------------------
  out(order_) = pitch; // lag in samples <= from ::autocorrelationWarped(...) - does it work if lambda != 0 [?]
  out(order_+1) = LevinsonError; //prediction error (= gain? [?])

  //--------------------------
  // LPC Pole-shifting
  //--------------------------
  //verify if Z-Plane pole-shifting should be performed...
  mrs_real gamma = getctrl("mrs_real/gamma")->to<mrs_real>();
  if(gamma != 1.0)
  {
    for(mrs_natural j = 0; j < order_; j++)
    {
      out(j) = (out(j) * pow((mrs_real)gamma, (mrs_real)j+1));
    }
  }

  {
    MarControlAccessor acc(ctrl_coeffs_);
    realvec& coeffs = acc.to<mrs_realvec>();
    coeffs(0) = 1.0;
    for(i=1; i < order_+1; ++i)
    {
      // coeffs(i) = -a(i); // musicDsp implementation
      coeffs(i) = out(i-1);
      ctrl_pitch_->setValue(pitch);
      ctrl_power_->setValue(LevinsonError);
    }
  }

  //MATLAB engine stuff - for testing and validation purposes only!
#ifdef _MATLAB_LPC_
  MATLAB_PUT(out, "LPC_out");
  MATLAB_PUT(getctrl("mrs_real/pitch")->to<mrs_real>(), "pitch_MARS");
  MATLAB_PUT(getctrl("mrs_real/power")->to<mrs_real>(), "g_MARS");
  MATLAB_PUT(ctrl_coeffs_->to<mrs_realvec>(), "a_MARS");
  MATLAB_EVAL("LPC_test");
  mrs_real matlabGain;
  MATLAB_GET("LPCgain", matlabGain);
#endif

}
