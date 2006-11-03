
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
    \class LPC
    \brief Compute LPC coefficients, Pitch and Power of window.

*/

#include "LPC.h"

using namespace std;
using namespace Marsyas;

LPC::LPC(string name):MarSystem("LPC",name)
{
  //type_ = "LPC";
	//name_ = name;

  inSize_ = 512;//How do we add definitions? DEFAULTINSIZE
  hopSize_= 256;
  outSize_ = 10-1;
  order_ = 10;
  minPitchRes_ = 0.01;
  autocorr_ = NULL;
  highFreq_ = 5000;
  lowFreq_ = 2000;
  firstTime_=1;
  networkCreated_ = false;

	addControls();
}

LPC::~LPC()
{
  delete autocorr_;
}

MarSystem* 
LPC::clone() const 
{
  return new LPC(*this);
}

void 
LPC::addControls()
{
  addctrl("mrs_natural/order", order_);
  setctrlState("mrs_natural/order", true);
  addctrl("mrs_real/minPitchRes",minPitchRes_);
  setctrlState("mrs_real/minPitchRes", true);
  addctrl("mrs_natural/hopSize",hopSize_);
  setctrlState("mrs_natural/hopSize", true);
  addctrl("mrs_real/lowFreq", lowFreq_);
  addctrl("mrs_real/highFreq", highFreq_);
  setctrlState("mrs_real/lowFreq", true);
  setctrlState("mrs_real/highFreq", true);
}

void
LPC::myUpdate()
{ 
  MRSDIAG("LPC.cpp - LPC:myUpdate");
  //cout <<" LPC UPDATE**********" << endl;
  inSize_ = getctrl("mrs_natural/inSamples")->toNatural();
  hopSize_ = getctrl("mrs_natural/hopSize")->toNatural();
  order_ =  getctrl("mrs_natural/order")->toNatural();
  minPitchRes_= getctrl("mrs_real/minPitchRes")->toReal();
  outSize_ = order_+1;  
  highFreq_=getctrl("mrs_real/highFreq")->toReal();
  lowFreq_=getctrl("mrs_real/lowFreq")->toReal();
  //cout <<"hopSize_:"<<hopSize_<<endl;
  setctrl("mrs_natural/onSamples", outSize_);
  setctrl("mrs_natural/onObservations", getctrl("mrs_natural/inObservations"));
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));

	// Build the pitch extractor network 

  if (networkCreated_ == false) 
    {
      pitchExtractor_ = new Series("pitchExtractor");
      pitchExtractor_->addMarSystem(new AutoCorrelation("acr"));
      pitchExtractor_->updctrl("AutoCorrelation/acr/mrs_real/magcompress", 0.67);
      pitchExtractor_->addMarSystem(new HalfWaveRectifier("hwr"));
      
      fanout_ = new Fanout("fanout");
      fanout_->addMarSystem(new Gain("id1"));
      fanout_->addMarSystem(new TimeStretch("tsc"));
      
      pitchExtractor_->addMarSystem(fanout_);
      
      fanin_ = new Fanin("fanin");
      fanin_->addMarSystem(new Gain("id2"));
      fanin_->addMarSystem(new Negative("nid"));
      
      pitchExtractor_->addMarSystem(fanin_);
      pitchExtractor_->addMarSystem(new HalfWaveRectifier("hwr"));
      pitchExtractor_->addMarSystem(new Peaker("pkr"));
      pitchExtractor_->addMarSystem(new MaxArgMax("mxr"));
      networkCreated_ = true;
    }
 
  /* ---------------------
//Not sure what to do with this.
     unsigned int i;
  // Prepare feature names
  featSize_ = outSize_;
  for (i=0; i < featSize_; i++)
   featNames_.push_back("LPC");
  */

    // if(firstTime_){
    //firstTime_=0;
    //}
  


  // update controls for pitchextract network------------------------------

  pitchExtractor_->updctrl("mrs_natural/inSamples", getctrl("mrs_natural/inSamples"));
  pitchExtractor_->updctrl("mrs_natural/inObservations", getctrl("mrs_natural/inObservations"));
  pitchExtractor_->updctrl("mrs_real/israte", getctrl("mrs_real/israte")->toReal());

  pitchExtractor_->updctrl("Fanout/fanout/TimeStretch/tsc/mrs_real/factor", 0.5); 
    
   
   lowSamples_ = hertz2samples(highFreq_,getctrl("mrs_real/israte")->toReal());
   highSamples_ = hertz2samples(lowFreq_,getctrl("mrs_real/israte")->toReal());

   //cout << "LPC update lowSamples: "<<lowSamples_ <<"highSamples: "<<highSamples_<< endl;
   pitchExtractor_->updctrl("Peaker/pkr/mrs_real/peakSpacing", 0.00);
   pitchExtractor_->updctrl("Peaker/pkr/mrs_real/peakStrength", 0.4);
   pitchExtractor_->updctrl("Peaker/pkr/mrs_natural/peakStart", lowSamples_);
   pitchExtractor_->updctrl("Peaker/pkr/mrs_natural/peakEnd", highSamples_);
   pitchExtractor_->updctrl("MaxArgMax/mxr/mrs_natural/nMaximums", 1);
   //cout <<" before pitch res" << endl;
   pitchres_.create(getctrl("mrs_natural/onObservations")->toNatural(),pitchExtractor_->getctrl("mrs_natural/onSamples")->toNatural());
  
   // cout << (*pitchExtractor_) << endl;
  //-----------------------------------------------------------------------

  //update controls for LPC Autocorrelation 
  autocorr_ = new AutoCorrelation("autoCorr");
  autocorr_->updctrl("mrs_natural/inSamples", getctrl("mrs_natural/inSamples"));
  autocorr_->updctrl("mrs_natural/inObservations", getctrl("mrs_natural/inObservations"));
  autocorr_->updctrl("mrs_real/israte", getctrl("mrs_real/israte"));
  autocorr_->update();
  //cout << *autocorr_ << endl;   

  rmat_.create((mrs_natural)order_ - 1,(mrs_natural)order_ - 1);
  temp_.create((mrs_natural)order_ - 1,(mrs_natural)order_ - 1);
  corr_.create((mrs_natural)inSize_);
  Zs_.create((mrs_natural)order_-1);
  pitchExtractor_->update();
}

void 
LPC::predict(realvec& data, realvec& coeffs)
{
  mrs_natural i,j;
  mrs_real power = 0.0;
  mrs_real error, tmp;
  for (i=0; i< order_-1; i++) 
    {
      Zs_(i) = data(order_-1-i-1);
    }
  //cout << "hopeSize_ predict: "<<hopSize_<< endl;
  for (i=order_-1; i<= hopSize_ + order_-1; i++)
    {
      
      tmp = 0.0;
      for (j=0; j< order_-1; j++) tmp += Zs_(j) * coeffs(j);
      // cout << "After 1st for loop in LPC predict"<< endl;
      for (j=order_-1-1; j>0; j--) Zs_(j) = Zs_(j-1);
      // cout << "After 2nd for loop in LPC predict"<< endl;
      Zs_(0) = data(i);
      //cout << "After zs(0)=data(i) in LPC predict"<< endl;
      error = data(i) - tmp;
      //cout << "i="<<i<< endl;
      power += error * error;
    }
  //cout << "end of LPC predict power= "<<power_<< endl;
  power_ = sqrt(power) / hopSize_;
}


void 
LPC::myProcess(realvec& in, realvec& out)
{
  //checkFlow(in,out); 
  mrs_natural i,j; 
  autocorr_->process(in, corr_);
  //cout << "begin process lpc"<< endl;
  //cout << "in size "<<in.getSize()<<" pitchres_ size "<<pitchres_.getSize()<< endl;
  //------------------------------------

  pitchExtractor_->process(in, pitchres_);

  //-----------------------------------
  
  //cout << "after pitchEx process "<< endl;
  // pitch_ = samples2hertz((mrs_natural)pitchres_(1), getctrl("mrs_real/israte")->toReal());

  pitch_ = (mrs_natural)pitchres_(1);
  cout << "pitch_ = " << pitch_ << endl;
  
  for (i=1; i<order_; i++)
    for (j=1; j<order_; j++)
      {
	rmat_(i-1,j-1) = corr_(abs((i-j)));
      }
  rmat_.invert(temp_);
  //cout << "After rmat.invert in LPC process"<< endl;
  for (i=0; i < order_-1; i++)
    {
      out(i) = 0.0;
      for (j=0; j < order_-1; j++)
	{
	  out(i) += (rmat_(i,j) * corr_(1+j));
	}
    }
  predict(in, out);
  //cout << "After predict in LPC process"<< endl;
  if ((mrs_real)pitchres_(0)> minPitchRes_)
    out(order_-1) = pitch_;
  else 
    out(order_-1) = 0.0;
  out(order_) = power_;
 
  
  //cout << "After everyting in LPC process------"<<out<< endl;
 


    
  
  

}


