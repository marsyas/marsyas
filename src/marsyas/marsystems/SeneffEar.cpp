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

#include "SeneffEar.h"
#include "../common_source.h"

#include <marsyas/fft.h>

#include <sstream>
#include <algorithm>

using namespace std;
using namespace Marsyas;

SeneffEar::SeneffEar(mrs_string name):MarSystem("SeneffEar",name)
{
  //type_ = "SeneffEar";
  //name_ = name;

  firstUpdate = true;
  fs = 0.0f;
  stage = 4;

  addControls();
}

SeneffEar::~SeneffEar()
{
}

MarSystem*
SeneffEar::clone() const
{
  return new SeneffEar(*this);
}

void
SeneffEar::addControls()
{
  addctrl("mrs_natural/stage", stage);
}

void
SeneffEar::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)
  MRSDIAG("SeneffEar.cpp - SeneffEar:myUpdate");

  ostringstream name;

  //first time update
  if (firstUpdate) {
    //PreemphasisRTheta coefficients initialization
    stringstream matrix;
    matrix << "# MARSYAS realvec" << endl;
    matrix << "# Size = 8" << endl;
    matrix << endl;
    matrix << endl;
    matrix << "# type: matrix" << endl;
    matrix << "# rows: 4" << endl;
    matrix << "# columns: 2" << endl;
    matrix << "0.86 3.1148863";
    matrix << "0.99 0.0";
    matrix << "0.5	0.0";
    matrix << "0.95 3.14159";
    matrix << endl;
    matrix << "# Size = 8" << endl;
    matrix << "# MARSYAS realvec" << endl;
    matrix >> PreemphasisRThetaCoeffs;

    //FilterBankRTheta coefficients initialization
    matrix.clear();
    matrix << "# MARSYAS realvec" << endl;
    matrix << "# Size = 200" << endl;
    matrix << endl;
    matrix << endl;
    matrix << "# type: matrix" << endl;
    matrix << "# rows: 40" << endl;
    matrix << "# columns: 5" << endl;
    //         R-z       Theta-z       Radius       Theta       R-z2
    matrix << "0.0       3.14159       0.740055     2.633909    0.8";
    matrix << "0.86      2.997077      0.753637     2.178169    0.8";
    matrix << "0.86      2.879267      0.775569     1.856744    0.8";
    matrix << "0.86      2.761458      0.798336     1.617919    0.8";
    matrix << "0.86      2.643648      0.819169     1.433496    0.8";
    matrix << "0.86      2.525839      0.837158     1.286795    0.8";
    matrix << "0.8       2.964876      0.852598     1.167321    0.8";
    matrix << "0.86      2.408029      0.865429     1.068141    0.8";
    matrix << "0.86      2.29022       0.876208     0.984489    0.8";
    matrix << "0.86      2.17241       0.885329     0.912985    0.8";
    matrix << "0.86      2.054601      0.893116     0.851162    0.8";
    matrix << "0.86      1.936791      0.899823     0.797179    0.8";
    matrix << "0.8       2.788161      0.906118     0.749633    0.8";
    matrix << "0.86      1.818981      0.911236     0.70744     0.8";
    matrix << "0.86      1.701172      0.915747     0.669742    0.8";
    matrix << "0.86      1.583362      0.919753     0.635858    0.8";
    matrix << "0.86      1.465552      0.923335     0.605237    0.8";
    matrix << "0.86      1.347743      0.926565     0.57743     0.8";
    matrix << "0.8       2.611447      0.929914     0.552065    0.8";
    matrix << "0.86      1.229933      0.932576     0.528834    0.8";
    matrix << "0.86      1.112123      0.944589     0.487783    0.75";
    matrix << "0.86      0.994314      0.957206     0.452645    0.660714";
    matrix << "0.86      0.876504      0.956548     0.42223     0.672143";
    matrix << "0.86      0.758694      0.956653     0.395644    0.682143";
    matrix << "0.8       2.434732      0.956518     0.372208    0.690966";
    matrix << "0.86      0.640885      0.956676     0.351393    0.69881";
    matrix << "0.86      0.523075      0.956741     0.316044    0.712143";
    matrix << "0.8       2.258018      0.956481     0.287157    0.723052";
    matrix << "0.8       2.081304      0.956445     0.263108    0.732143";
    matrix << "0.8       1.904589      0.956481     0.242776    0.739835";
    matrix << "0.86      0.405265      0.958259     0.217558    0.749384";
    matrix << "0.8       1.727875      0.963083     0.197086    0.757143";
    matrix << "0.8       1.55116       0.969757     0.175115    0.769048";
    matrix << "0.8       1.374446      0.97003      0.153697    0.780662";
    matrix << "0.8       1.197732      0.970382     0.134026    0.791337";
    matrix << "0.8       1.021017      0.970721     0.118819    0.799596";
    matrix << "0.8       1.5           0.970985     0.106711    0.8";
    matrix << "0.8       1.2           0.971222     0.096843    0.8";
    matrix << "0.8       1.0           0.97144      0.088645    0.8";
    matrix << "0.8       0.9           0.971645     0.081727    0.8";
    matrix << endl;
    matrix << "# Size = 200" << endl;
    matrix << "# MARSYAS realvec" << endl;
    matrix >> FilterBankRThetaCoeffs;

    realvec a,b,c;

    //SeneffPreemphasis coefficients computation
    a.create(3);
    b.create(3);
    a(0) = PreemphasisRThetaCoeffs(0,0)*PreemphasisRThetaCoeffs(0,0);
    a(1) = -2.0f*PreemphasisRThetaCoeffs(0,0)*cos(PreemphasisRThetaCoeffs(0,1));
    a(2) = 1.0f;
    b(0) = PreemphasisRThetaCoeffs(1,0)*PreemphasisRThetaCoeffs(1,0);
    b(1) = -2.0f*PreemphasisRThetaCoeffs(1,0)*cos(PreemphasisRThetaCoeffs(1,1));
    b(2) = 1.0f;
    polyConv(a, b, a);
    b(0) = PreemphasisRThetaCoeffs(2,0)*PreemphasisRThetaCoeffs(2,0);
    b(1) = -2.0f*PreemphasisRThetaCoeffs(2,0)*cos(PreemphasisRThetaCoeffs(2,1));
    b(2) = 1.0f;
    polyConv(a, b, a);
    b(0) = PreemphasisRThetaCoeffs(3,0)*PreemphasisRThetaCoeffs(3,0);
    b(1) = -2.0f*PreemphasisRThetaCoeffs(3,0)*cos(PreemphasisRThetaCoeffs(3,1));
    b(2) = 1.0f;
    polyConv(a, b, c);
    SeneffPreemphasisCoeffs.create(c.getSize());

    for (mrs_natural i = 0; i < c.getSize(); ++i)
      SeneffPreemphasisCoeffs(i) = c(c.getSize() - i - 1);

    //SeneffFilterBank coefficients computation
    channels = FilterBankRThetaCoeffs.getRows();
    SeneffFilterBankCoeffs.create(channels, 5);
    mrs_real sum;
    for (mrs_natural i = 0; i < channels; ++i) {
      SeneffFilterBankCoeffs(i,0) =	1.0f;
      SeneffFilterBankCoeffs(i,1) = -2.0f*FilterBankRThetaCoeffs(i,0)*cos(FilterBankRThetaCoeffs(i,1));
      SeneffFilterBankCoeffs(i,2) = FilterBankRThetaCoeffs(i,0)*FilterBankRThetaCoeffs(i,0);
      sum = SeneffFilterBankCoeffs(i,0) + SeneffFilterBankCoeffs(i,1) + SeneffFilterBankCoeffs(i,2);
      SeneffFilterBankCoeffs(i,0) /= sum;
      SeneffFilterBankCoeffs(i,1) /= sum;
      SeneffFilterBankCoeffs(i,2) /= sum;
    }

    //SeneffForward & SeneffBackward coefficients computation
    SeneffForwardCoeffs.create(mrs_natural(5), static_cast<mrs_natural>(channels));
    SeneffBackwardCoeffs.create(mrs_natural(5), static_cast<mrs_natural>(channels));
    a.create(3),b.create(3);
    for (mrs_natural j = 0; j < channels; j++) {
      a(0) = FilterBankRThetaCoeffs(j,4)*FilterBankRThetaCoeffs(j,4);
      a(1) = -2.0f*FilterBankRThetaCoeffs(j,4)*cos(FilterBankRThetaCoeffs(j,3)/2);
      a(2) = 1.0f;
      polyConv(a, a, c);
      for (mrs_natural i = 0; i < 5; ++i) SeneffForwardCoeffs(i,j) = c(5 - i - 1);
      b(0) = FilterBankRThetaCoeffs(j,2)*FilterBankRThetaCoeffs(j,2);
      b(1) = -2.0f*FilterBankRThetaCoeffs(j,2)*cos(FilterBankRThetaCoeffs(j,3));
      b(2) = 1.0f;
      polyConv(b, b, c);
      for (mrs_natural i = 0; i < 5; ++i) SeneffBackwardCoeffs(i,j) = c(5 - i - 1);
    }

    //SeneffPreemphasis filter creation and coefficients initialization
    SeneffPreemphasisFilter = new Filter("SeneffPreemphasisFilter");
    a.create(1);
    a(0) = 1.0f;
    SeneffPreemphasisFilter->setctrl("mrs_realvec/ncoeffs", SeneffPreemphasisCoeffs);
    SeneffPreemphasisFilter->setctrl("mrs_realvec/dcoeffs", a);

    //SeneffFilterBank filters cascade creation and coefficients initialization
    SeneffFilterBank = new Cascade("SeneffFilterBank");
    Filter* filter;
    a.create(3);
    b.create(3);
    for (mrs_natural i = 0; i < channels; ++i) {
      name.clear();
      name.str("");
      name << "filter_" << i;
      filter = new Filter(name.str());
      b(0) = SeneffFilterBankCoeffs(i,0);
      b(1) = SeneffFilterBankCoeffs(i,1);
      b(2) = SeneffFilterBankCoeffs(i,2);
      a(0) = 1.0f;
      a(1) = SeneffFilterBankCoeffs(i,3);
      a(2) = SeneffFilterBankCoeffs(i,4);
      filter->setctrl("mrs_realvec/ncoeffs", b);
      filter->setctrl("mrs_realvec/dcoeffs", a);
      SeneffFilterBank->addMarSystem(filter);
    }

    //resonatorFilter creation
    resonatorFilter = new Parallel("resonatorFilter");
    a.create(5);
    b.create(5);
    for (mrs_natural i = 0; i < channels; ++i) {
      name.clear();
      name.str("");
      name << "filter_" << i;
      filter = new Filter(name.str());
      for (mrs_natural j = 0; j < 5; j++) b(j) = SeneffForwardCoeffs(j,i);
      for (mrs_natural j = 0; j < 5; j++) a(j) = SeneffBackwardCoeffs(j,i);
      filter->setctrl("mrs_realvec/ncoeffs", b);
      filter->setctrl("mrs_realvec/dcoeffs", a);
      resonatorFilter->addMarSystem(filter);
    }

    //run an impulse through the preemphasis filter
    realvec impulse;
    impulse.create(mrs_natural(1), mrs_natural(256));
    impulse(0) = 1.0f;
    realvec y0;
    y0.create(impulse.getRows(), impulse.getCols());
    SeneffPreemphasisFilter->setctrl("mrs_natural/inSamples", impulse.getCols());
    SeneffPreemphasisFilter->setctrl("mrs_natural/inObservations", impulse.getRows());
    SeneffPreemphasisFilter->update();
    SeneffPreemphasisFilter->process(impulse, y0);

    //then through the cascade of zeros
    realvec y1;
    y1.create(y0.getRows()*channels, y0.getCols());
    SeneffFilterBank->setctrl("Filter/filter_0/mrs_natural/inSamples", y0.getCols());
    SeneffFilterBank->setctrl("Filter/filter_0/mrs_natural/inObservations", y0.getRows());
    SeneffFilterBank->update();
    SeneffFilterBank->process(y0, y1);

    //Finally through each of the resonator filters
    y.create(y1.getRows(), y1.getCols());
    resonatorFilter->setctrl("Filter/filter_0/mrs_natural/inSamples", y1.getCols());
    for (mrs_natural i = 0; i < channels; ++i) {
      name.clear();
      name.str("");
      name << "Filter/" << "filter_" << i << "/mrs_natural/inObservations";
      resonatorFilter->setctrl(name.str(), y0.getRows());
    }
    resonatorFilter->update();
    resonatorFilter->process(y1, y);

    //reset the state vector of each filter
    realvec state;
    state.create(SeneffPreemphasisFilter->getctrl("mrs_realvec/state")->to<mrs_realvec>().getRows(),SeneffPreemphasisFilter->getctrl("mrs_realvec/state")->to<mrs_realvec>().getCols());
    SeneffPreemphasisFilter->setctrl("mrs_natural/stateUpdate", mrs_natural(1));
    SeneffPreemphasisFilter->updControl("mrs_realvec/state", state);
    SeneffPreemphasisFilter->setctrl("mrs_natural/stateUpdate", mrs_natural(0));

    state.create(SeneffFilterBank->getctrl("Filter/filter_0/mrs_realvec/state")->to<mrs_realvec>().getRows(),SeneffFilterBank->getctrl("Filter/filter_0/mrs_realvec/state")->to<mrs_realvec>().getCols());
    for (mrs_natural i = 0; i < channels; ++i) {
      name.clear();
      name.str("");
      name << "Filter/" << "filter_" << i << "/mrs_natural/stateUpdate";
      SeneffFilterBank->setctrl(name.str(), mrs_natural(1));
      name.clear();
      name.str("");
      name << "Filter/" << "filter_" << i << "/mrs_realvec/state";
      SeneffFilterBank->updControl(name.str(), state);
      name.clear();
      name.str("");
      name << "Filter/" << "filter_" << i << "/mrs_natural/stateUpdate";
      SeneffFilterBank->setctrl(name.str(), mrs_natural(0));
    }

    state.create(resonatorFilter->getctrl("Filter/filter_0/mrs_realvec/state")->to<mrs_realvec>().getRows(),resonatorFilter->getctrl("Filter/filter_0/mrs_realvec/state")->to<mrs_realvec>().getCols());
    for (mrs_natural i = 0; i < channels; ++i) {
      name.clear();
      name.str("");
      name << "Filter/" << "filter_" << i << "/mrs_natural/stateUpdate";
      resonatorFilter->setctrl(name.str(), mrs_natural(1));
      name.clear();
      name.str("");
      name << "Filter/" << "filter_" << i << "/mrs_realvec/state";
      resonatorFilter->updControl(name.str(), state);
      name.clear();
      name.str("");
      name << "Filter/" << "filter_" << i << "/mrs_natural/stateUpdate";
      resonatorFilter->setctrl(name.str(), mrs_natural(0));
    }

    //constants initialization
    hwrA = 10.0f;
    hwrB = 65.0f;
    hwrG = 2.35f;

    Cn.create(channels);

    //lowPassFilter creation and coefficients initialization
    lpAlpha = 0.209611f;
    lowPassFilter = new Filter("lowPassFilter");
    a.create(2);
    a(0) = -lpAlpha;
    a(1) = 1.0f;
    polyConv(a, a, a);
    polyConv(a, a, a);
    polyFlip(a);
    b.create(1);
    b(0) = a.sum();
    lowPassFilter->setctrl("mrs_realvec/ncoeffs", b);
    lowPassFilter->setctrl("mrs_realvec/dcoeffs", a);

    //Seneff's Adaptive Gain Control filter creation and coefficients initialization
    initial_yn = (mrs_real)0.23071276;
    alpha_agc = (mrs_real)0.979382181;
    kagc = (mrs_real)0.002;
    AGCfilter = new Filter("AGCfilter");
    a.create(1);
    a(0) = alpha_agc;
    b.create(2);
    b(1) = 1.0f - alpha_agc;
    AGCfilter->setctrl("mrs_realvec/ncoeffs", b);
    AGCfilter->setctrl("mrs_realvec/dcoeffs", a);
    state.create(channels, mrs_natural(1));
    state.setval(initial_yn);
    AGCfilter->setctrl("mrs_natural/inObservations", channels);
    AGCfilter->setctrl("mrs_natural/stateUpdate", mrs_natural(1));
    AGCfilter->updControl("mrs_realvec/state", state);
    AGCfilter->setctrl("mrs_natural/stateUpdate", mrs_natural(0));

    firstUpdate = false;
  }//end first update

  //--------------------------------------------------------------------------------
  //compute the FFT then find the gain peak then divide each forward polynomial by the maximum gain (to normalize) and then multiply by the desired low frequency roll-off.
  if (fs != getctrl("mrs_real/israte")->to<mrs_real>()) {
    fs = getctrl("mrs_real/israte")->to<mrs_real>();
    fft fft;
    realvec Y(y.getCols());
    mrs_real *Yd = Y.getData();
    mrs_real abs2(1.0f),maxAbs2(0.0f);
    mrs_real gain;
    mrs_real rolloff;
    SeneffForwardCoeffsNormalized.create(SeneffForwardCoeffs.getRows(), SeneffForwardCoeffs.getCols());
    for (mrs_natural i = 0; i < channels; ++i) {
      for (mrs_natural j = 0; j < Y.getCols(); j++) Y(j) = y(i,j);
      fft.rfft(Yd, Y.getCols()/2, FFT_FORWARD);
      maxAbs2 = Y(0)*Y(0);
      for (mrs_natural j = 1; j < y.getCols()/2; j++) if ((abs2 = Y(2*j)*Y(2*j) + Y(2*j+1)*Y(2*j+1)) > maxAbs2) maxAbs2 = abs2;
      gain = 1/(y.getCols()*sqrt(maxAbs2));
      rolloff = min((mrs_real)((FilterBankRThetaCoeffs(i,3)/PI*fs/2)/1600),(mrs_real)1.0);
      for (mrs_natural j = 0; j < SeneffForwardCoeffsNormalized.getRows(); j++) SeneffForwardCoeffsNormalized(j,i) = SeneffForwardCoeffs(j,i)*gain*rolloff;
    }
    //then update the resonatorFilter
    realvec b(5);
    for (mrs_natural i = 0; i < channels; ++i) {
      name.clear();
      name.str("");
      name << "Filter/" << "filter_" << i << "/mrs_realvec/ncoeffs";
      for (mrs_natural j = 0; j < 5; j++) b(j) = SeneffForwardCoeffsNormalized(j,i);
      resonatorFilter->setctrl(name.str(), b);
    }
    resonatorFilter->update();

    Tua = (mrs_real)(58.3333/fs);
    Tub = (mrs_real)(8.3333/fs);
  }

  //--------------------------------------------------------------------------------
  //SeneffPreemphasis update
  SeneffPreemphasisFilter->setctrl("mrs_natural/inSamples", getctrl("mrs_natural/inSamples"));
  SeneffPreemphasisFilter->setctrl("mrs_natural/inObservations", getctrl("mrs_natural/inObservations"));
  SeneffPreemphasisFilter->setctrl("mrs_real/israte", getctrl("mrs_real/israte"));
  SeneffPreemphasisFilter->update();
  if ((int)slice_0.getSize() != SeneffPreemphasisFilter->getctrl("mrs_natural/onObservations")->to<mrs_natural>() * SeneffPreemphasisFilter->getctrl("mrs_natural/onSamples")->to<mrs_natural>()) {
    slice_0.create(SeneffPreemphasisFilter->getctrl("mrs_natural/onObservations")->to<mrs_natural>(), SeneffPreemphasisFilter->getctrl("mrs_natural/onSamples")->to<mrs_natural>());
  }

  //SeneffFilterBank filters cascade update
  SeneffFilterBank->setctrl("Filter/filter_0/mrs_natural/inSamples", SeneffPreemphasisFilter->getctrl("mrs_natural/onSamples"));
  SeneffFilterBank->setctrl("Filter/filter_0/mrs_natural/inObservations", SeneffPreemphasisFilter->getctrl("mrs_natural/onObservations"));
  SeneffFilterBank->setctrl("Filter/filter_0/mrs_real/israte", SeneffPreemphasisFilter->getctrl("mrs_real/osrate"));
  SeneffFilterBank->update();
  if ((int)slice_1.getSize() != SeneffFilterBank->getctrl("mrs_natural/onObservations")->to<mrs_natural>() * SeneffFilterBank->getctrl("mrs_natural/onSamples")->to<mrs_natural>()) {
    slice_1.create(SeneffFilterBank->getctrl("mrs_natural/onObservations")->to<mrs_natural>(), SeneffFilterBank->getctrl("mrs_natural/onSamples")->to<mrs_natural>());
  }

  //resonatorFilter update
  resonatorFilter->setctrl("Filter/filter_0/mrs_natural/inSamples", SeneffFilterBank->getctrl("mrs_natural/onSamples"));
  for (mrs_natural i = 0; i < channels; ++i) {
    name.clear();
    name.str("");
    name << "Filter/" << "filter_" << i << "/mrs_natural/inObservations";
    resonatorFilter->setctrl(name.str(), SeneffFilterBank->getctrl("Filter/filter_0/mrs_natural/onObservations"));
  }
  resonatorFilter->setctrl("Filter/filter_0/mrs_real/israte", SeneffFilterBank->getctrl("mrs_real/osrate"));
  resonatorFilter->update();
  if ((int)slice_2.getSize() != resonatorFilter->getctrl("mrs_natural/onObservations")->to<mrs_natural>() * resonatorFilter->getctrl("mrs_natural/onSamples")->to<mrs_natural>()) {
    slice_2.create(resonatorFilter->getctrl("mrs_natural/onObservations")->to<mrs_natural>(), resonatorFilter->getctrl("mrs_natural/onSamples")->to<mrs_natural>());
  }

  //lowPassFilter update
  lowPassFilter->setctrl("mrs_natural/inSamples", resonatorFilter->getctrl("mrs_natural/onSamples"));
  lowPassFilter->setctrl("mrs_natural/inObservations", resonatorFilter->getctrl("mrs_natural/onObservations"));
  lowPassFilter->setctrl("mrs_real/israte", resonatorFilter->getctrl("mrs_real/osrate"));
  lowPassFilter->update();
  if ((int)slice_3.getSize() != lowPassFilter->getctrl("mrs_natural/onObservations")->to<mrs_natural>() * lowPassFilter->getctrl("mrs_natural/onSamples")->to<mrs_natural>()) {
    slice_3.create(lowPassFilter->getctrl("mrs_natural/onObservations")->to<mrs_natural>(), lowPassFilter->getctrl("mrs_natural/onSamples")->to<mrs_natural>());
  }

  //AGCfilter update
  AGCfilter->setctrl("mrs_natural/inSamples", lowPassFilter->getctrl("mrs_natural/onSamples"));
  AGCfilter->setctrl("mrs_natural/inObservations", lowPassFilter->getctrl("mrs_natural/onObservations"));
  AGCfilter->setctrl("mrs_real/israte", lowPassFilter->getctrl("mrs_real/osrate"));
  AGCfilter->update();

  //this update
  setctrl("mrs_natural/onSamples", AGCfilter->getctrl("mrs_natural/onSamples"));
  setctrl("mrs_natural/onObservations", AGCfilter->getctrl("mrs_natural/onObservations")->to<mrs_natural>());
  setctrl("mrs_real/osrate", AGCfilter->getctrl("mrs_real/osrate"));
}

void
SeneffEar::polyConv(realvec& a, realvec& b, realvec& c)
{
  mrs_natural la(a.getSize());
  mrs_natural lb(b.getSize());
  mrs_natural n = la + lb - 1;

  realvec ta(a); ta.stretch(n);
  realvec tb(b); tb.stretch(n);
  realvec tc;    tc.create(n);

  for (mrs_natural k = 0; k < n; k++) {
    for (mrs_natural i = 0; i <= k; ++i) {
      tc(k) += ta(i)*tb(k-i);
    }
  }
  if ((mrs_natural)c.getSize() != n) c.create(n);
  c = tc;
}

void
SeneffEar::polyFlip(realvec& a)
{
  mrs_natural la(a.getSize());
  realvec ta(a);

  for (mrs_natural i = 0; i < la; ++i) {
    a(i) = ta(la - i - 1);
  }
}


void
SeneffEar::myProcess(realvec& in, realvec& out)
{
  checkFlow(in, out);
  //lmartins: if (mute_) return;
  if(getctrl("mrs_bool/mute")->to<mrs_bool>()) return;

  mrs_natural s = 0;
  stage = getctrl("mrs_natural/stage")->to<mrs_natural>();

  SeneffPreemphasisFilter->process(in, slice_0);
  SeneffFilterBank->process(slice_0, slice_1);
  if (s++ == stage) {out = slice_1; return;}
  resonatorFilter->process(slice_1, slice_2);
  if (s++ == stage) {out = slice_2; return;}
  //Seneff's detector non-linearity
  for (mrs_natural i = 0; i < slice_2.getRows(); ++i) {
    for (mrs_natural j = 0; j < slice_2.getCols(); j++) {
      slice_2(i,j) = hwrA*atan(hwrB*max((mrs_real)0.0,slice_2(i,j))) + exp(hwrA*hwrB*min((mrs_real)0.0,slice_2(i,j)));
    }
  }
  //Seneff's short-term adaptation (a reservoir hair cell model)
  mrs_real flow;
  for (mrs_natural j = 0; j < slice_2.getCols(); j++) {
    for (mrs_natural i = 0; i < slice_2.getRows(); ++i) {
      flow = max((mrs_real)0.0,Tua*(slice_2(i,j)-Cn(i)));
      Cn(i) = Cn(i) + flow - Tub*Cn(i);
      slice_2(i,j) = flow;
    }
  }
  if (s++ == stage) {out = slice_2; return;}
  //Seneff's Low Pass filter (to model the loss of Synchrony)
  lowPassFilter->process(slice_2, slice_3);
  if (s++ == stage) {out = slice_3; return;}
  //Seneff's Adaptive Gain Control
  AGCfilter->process(slice_3, out);
  for (mrs_natural i = 0; i < out.getRows(); ++i) {
    for (mrs_natural j = 0; j < out.getCols(); j++) {
      out(i,j) = slice_3(i,j)/(1.0f + kagc*out(i,j));
    }
  }
}
