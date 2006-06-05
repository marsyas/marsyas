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


/** 
    \class ERB
    \brief ERB Filterbank

    ERB (Equivalent rectangular bandwith) filterbank. Based on 
the code from the Auditory Toolbox by Malcolm Slaney. 
*/


#include "ERB.h"
#include "Series.h"
#include "Filter.h"
#include <sstream>
using namespace std;


ERB::ERB(string name)
{
  type_ = "ERB";
  name_ = name;
  
  fs = 0;
  numChannels = 0;
  lowFreq = 0;
  
  addControls();
}


ERB::~ERB()
{
}

MarSystem* 
ERB::clone() const 
{
  return new ERB(*this);
}


void 
ERB::addControls()
{
  addDefaultControls();
  
  addctrl("natural/numChannels", 1);
  addctrl("real/lowFreq", 100.0f);
  
  setctrlState("natural/numChannels", true);
  setctrlState("real/lowFreq", true);
  
}

void 
ERB::update()
{
  MRSDIAG("ERB.cpp - ERB:update");
  
  //FilterBank creation
  if (numChannels != getctrl("natural/numChannels").toNatural()){
    numChannels = getctrl("natural/numChannels").toNatural();
    if (filterBank) delete filterBank;
    filterBank = new Fanout("filterBank");
    stringstream name;
    for(natural i = 0; i < numChannels; i++){
      name << "channel_" << i;
      Series* channel = new Series(name.str());
      name.str("");
      name << "filter_" << i << "_" << 0;
      Filter* filter_0 = new Filter(name.str());
      name.str("");
      name << "filter_" << i << "_" << 1;
      Filter* filter_1 = new Filter(name.str());
      name.str("");
      name << "filter_" << i << "_" << 2;
      Filter* filter_2 = new Filter(name.str());
      name.str("");
      name << "filter_" << i << "_" << 3;
      Filter* filter_3 = new Filter(name.str());
      name.str("");
      channel->addMarSystem(filter_0);
      channel->addMarSystem(filter_1);
      channel->addMarSystem(filter_2);
      channel->addMarSystem(filter_3);
      filterBank->addMarSystem(channel);
    }
    centerFreqs.create(numChannels);
    fcoefs.create(numChannels, 10);
  }
  
  setctrl("natural/onSamples", getctrl("natural/inSamples"));
  setctrl("natural/onObservations", numChannels*getctrl("natural/inObservations").toNatural());
  setctrl("real/osrate", getctrl("real/israte"));
  
  //Coefficients computation
  lowFreq = getctrl("real/lowFreq").toReal();
  fs = getctrl("real/israte").toReal();
  highFreq = fs/2;
  EarQ = 9.26449f;
  minBW = 24.7f;
  order = 1;
  
  for (natural i = 0; i < numChannels; i++){
    centerFreqs(i) = - (EarQ*minBW) + exp((i+1)*(-log(highFreq+EarQ*minBW) + log(lowFreq+EarQ*minBW))/numChannels)*(highFreq + EarQ*minBW);
  }
  
  A0 = 1/fs;
  A2 = 0.0f;
  B0 = 1.0f;
  
  for (natural i = 0; i < numChannels; i++){
    fcoefs(i,0) = A0;
    fcoefs(i,1) = A11(centerFreqs(i), B(E(centerFreqs(i))));
    fcoefs(i,2) = A12(centerFreqs(i), B(E(centerFreqs(i))));
    fcoefs(i,3) = A13(centerFreqs(i), B(E(centerFreqs(i))));
    fcoefs(i,4) = A14(centerFreqs(i), B(E(centerFreqs(i))));
    fcoefs(i,5) = A2;
    fcoefs(i,6) = B0;
    fcoefs(i,7) = B1(centerFreqs(i), B(E(centerFreqs(i))));
    fcoefs(i,8) = B2( B(E(centerFreqs(i))));
    fcoefs(i,9) = gain(centerFreqs(i), B(E(centerFreqs(i))));
  }
  
  //Controls update
  stringstream channel,filter,ctrl;
  realvec b(1,3),a(1,3);
  channel << "Series/channel_0/";
  ctrl << channel.str() << "natural/inSamples";
  filterBank->setctrl(ctrl.str(), getctrl("natural/inSamples"));
  ctrl.str("");
  ctrl << channel.str() << "natural/inObservations";
  filterBank->setctrl(ctrl.str(), getctrl("natural/inObservations"));
  ctrl.str("");
  ctrl << channel.str() << "natural/onObservations";
  filterBank->setctrl(ctrl.str(), getctrl("natural/inObservations"));
  ctrl.str("");
  ctrl << channel.str() << "real/israte";
  filterBank->setctrl(ctrl.str(), getctrl("real/israte"));
  for(natural i = 0; i < numChannels; i++){
    //filter 0
    channel.str("");
    channel << "Series/channel_" << i << "/";
    filter.str("");
    filter << channel.str() << "Filter/filter_" << i << "_" << 0 << "/";
    ctrl.str("");
    ctrl << filter.str() << "natural/inSamples";
    filterBank->setctrl(ctrl.str(), getctrl("natural/inSamples"));
    ctrl.str("");
    ctrl << filter.str() << "natural/inObservations";
    filterBank->setctrl(ctrl.str(), getctrl("natural/inObservations"));
    ctrl.str("");
    ctrl << filter.str() << "natural/onObservations";
    filterBank->setctrl(ctrl.str(), getctrl("natural/inObservations"));
    ctrl.str("");
    ctrl << filter.str() << "real/israte";
    filterBank->setctrl(ctrl.str(), getctrl("real/israte"));
    a(0) = fcoefs(i,6);
    a(1) = fcoefs(i,7);
    a(2) = fcoefs(i,8);
    b(0) = fcoefs(i,0)/fcoefs(i,9);
    b(1) = fcoefs(i,1)/fcoefs(i,9);
    b(2) = fcoefs(i,5)/fcoefs(i,9);
    ctrl.str("");
    ctrl << filter.str() << "realvec/ncoeffs";
    filterBank->setctrl(ctrl.str(), b);
    ctrl.str("");
    ctrl << filter.str() << "realvec/dcoeffs";
    filterBank->setctrl(ctrl.str(), a);
    //filter 1
    filter.str("");
    filter << channel.str() << "Filter/filter_" << i << "_" << 1 << "/";
    a(0) = fcoefs(i,6);
    a(1) = fcoefs(i,7);
    a(2) = fcoefs(i,8);
    b(0) = fcoefs(i,0);
    b(1) = fcoefs(i,2);
    b(2) = fcoefs(i,5);
    ctrl.str("");
    ctrl << filter.str() << "realvec/ncoeffs";
    filterBank->setctrl(ctrl.str(), b);
    ctrl.str("");
    ctrl << filter.str() << "realvec/dcoeffs";
    filterBank->setctrl(ctrl.str(), a);
    //filter 2
    filter.str("");
    filter << channel.str() << "Filter/filter_" << i << "_" << 2 << "/";
    a(0) = fcoefs(i,6);
    a(1) = fcoefs(i,7);
    a(2) = fcoefs(i,8);
    b(0) = fcoefs(i,0);
    b(1) = fcoefs(i,3);
    b(2) = fcoefs(i,5);
    ctrl.str("");
    ctrl << filter.str() << "realvec/ncoeffs";
    filterBank->setctrl(ctrl.str(), b);
    ctrl.str("");
    ctrl << filter.str() << "realvec/dcoeffs";
    filterBank->setctrl(ctrl.str(), a);
    //filter 3
    filter.str("");
    filter << channel.str() << "Filter/filter_" << i << "_" << 3 << "/";
    a(0) = fcoefs(i,6);
    a(1) = fcoefs(i,7);
    a(2) = fcoefs(i,8);
    b(0) = fcoefs(i,0);
    b(1) = fcoefs(i,4);
    b(2) = fcoefs(i,5);
    ctrl.str("");
    ctrl << filter.str() << "realvec/ncoeffs";
    filterBank->setctrl(ctrl.str(), b);
    ctrl.str("");
    ctrl << filter.str() << "realvec/dcoeffs";
    filterBank->setctrl(ctrl.str(), a);
  }
  //update the whole filter bank
  filterBank->update();
  
  defaultUpdate();
}


real 
ERB::E(real x)
{
  return pow(pow(x/EarQ, (real)order) + pow(minBW, (real)order), 1/(real)order);
}

real 
ERB::B(real x)
{
  return 1.019f*2.0f*PI*x;
}

real 
ERB::B1(real x, real y)
{
  return -2.0f*cos(2.0f*x*PI/fs)/exp(y/fs);
}

real 
ERB::B2(real x)
{
  return exp(-2.0f*x/fs);
}

real 
ERB::A11(real x, real y)
{
  return -(2.0f/fs*cos(2.0f*x*PI/fs)/exp(y/fs) + 2.0f*sqrt(3.0f+sqrt(8.0f))/fs*sin(2.0f*x*PI/fs)/exp(y/fs))/2.0f;
}


real 
ERB::A12(real x, real y)
{
  return -(2.0f/fs*cos(2.0f*x*PI/fs)/exp(y/fs) - 2.0f*sqrt(3.0f+sqrt(8.0f))/fs*sin(2.0f*x*PI/fs)/exp(y/fs))/2.0f;
}

real 
ERB::A13(real x, real y)
{
  return -(2.0f/fs*cos(2.0f*x*PI/fs)/exp(y/fs) + 2.0f*sqrt(3.0f-sqrt(8.0f))/fs*sin(2.0f*x*PI/fs)/exp(y/fs))/2.0f;
}

real 
ERB::A14(real x, real y)
{
  return -(2.0f/fs*cos(2.0f*x*PI/fs)/exp(y/fs) - 2.0f*sqrt(3.0f-sqrt(8.0f))/fs*sin(2.0f*x*PI/fs)/exp(y/fs))/2.0f;
}

real 
ERB::gain(real x, real y)
{
  real r1(2.0f*x*PI/fs);
  real r2(sqrt(3.0f-sqrt(8.0f)));
  real r3(sqrt(3.0f+sqrt(8.0f)));
  real z1r(cos(2.0f*r1)),z1i(sin(2.0f*r1));
  real z2r(cos(r1)),z2i(sin(r1));
  real z3r(-2.0f*z1r/fs),z3i(-2.0f*z1i/fs);
  real z4r(2.0f*exp(-y/fs)*z2r/fs),z4i(2.0f*exp(-y/fs)*z2i/fs);
  return abs(z3r + z4r*(cos(r1)-r2*sin(r1)),z3i + z4i*(cos(r1)-r2*sin(r1)))*
    abs(z3r + z4r*(cos(r1)+r2*sin(r1)),z3i + z4i*(cos(r1)+r2*sin(r1)))*
    abs(z3r + z4r*(cos(r1)-r3*sin(r1)),z3i + z4i*(cos(r1)-r3*sin(r1)))*
    abs(z3r + z4r*(cos(r1)+r3*sin(r1)),z3i + z4i*(cos(r1)+r3*sin(r1)))/
    pow(abs(-2.0f/exp(2.0f*y/fs) - 2.0f*z1r + 2.0f*(1.0f + z1r)/exp(y/fs),2.0f*z1i*(-1.0f + 1.0f/exp(y/fs))),4);
}

real 
ERB::abs(real r1, real r2)
{
  return sqrt(r1*r1 + r2*r2);
}

void 
ERB::process(realvec& in, realvec& out)
{
  checkFlow(in,out);
  if (mute_) return;
  
  filterBank->process(in, out);
}

