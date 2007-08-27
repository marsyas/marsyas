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


#include "BeatHistoFeatures.h"
#include <cfloat> 

using namespace std;
using namespace Marsyas;

BeatHistoFeatures::BeatHistoFeatures(string name):MarSystem("BeatHistoFeatures", name)
{
  //type_ = "BeatHistoFeatures";
  //name_ = name;
  
	mxr_ = NULL;

	//addControls();
}

BeatHistoFeatures::BeatHistoFeatures(const BeatHistoFeatures& a):MarSystem(a)
{
// 	type_ = a.type_;
// 	name_ = a.name_;
// 	ncontrols_ = a.ncontrols_; 		
// 
// 	inSamples_ = a.inSamples_;
// 	inObservations_ = a.inObservations_;
// 	onSamples_ = a.onSamples_;
// 	onObservations_ = a.onObservations_;
// 	dbg_ = a.dbg_;
// 	mute_ = a.mute_;
	mxr_ = NULL;
}

BeatHistoFeatures::~BeatHistoFeatures()
{
  delete mxr_;
}


MarSystem* 
BeatHistoFeatures::clone() const 
{
  return new BeatHistoFeatures(*this);
}

// void 
// BeatHistoFeatures::addControls()
// {
//   //delete mxr_;//[?]
//   //mxr_ = new MaxArgMax("mxr");//[?]
//}

void
BeatHistoFeatures::myUpdate(MarControlPtr sender)
{
  MRSDIAG("BeatHistoFeatures.cpp - BeatHistoFeatures:myUpdate");
  
	delete mxr_;//[!]
	mxr_ = new MaxArgMax("mxr");//[!]

	setctrl("mrs_natural/onSamples", (mrs_natural)1);
  setctrl("mrs_natural/onObservations", (mrs_natural)8);
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));

  flag_.create(getctrl("mrs_natural/inSamples")->to<mrs_natural>());
  mxr_->updctrl("mrs_natural/inSamples", getctrl("mrs_natural/inSamples"));
  mxr_->updctrl("mrs_natural/inObservations", getctrl("mrs_natural/inObservations"));
  mxr_->updctrl("mrs_real/israte", getctrl("mrs_real/israte"));
  mxr_->updctrl("mrs_natural/nMaximums", 3);
  
  mxres_.create(mxr_->getctrl("mrs_natural/onObservations")->to<mrs_natural>(),
	mxr_->getctrl("mrs_natural/onSamples")->to<mrs_natural>());
    
  // setctrl("mrs_string/onObsNames", "BH_Sum, BH_MaxAmp1divSum, BH_MaxAmp2divSum, BH_MaxAmp3divSum, BH_MaxPos1, BH_MAXPos2, BH_MAXPos3, BH_MaAmp2divMaxAmp2");
  
  setctrl("mrs_string/onObsNames", "BH_LowPeakAmp, BH_LowPeakBPM, BH_HighPeakAmp, BH_HighPeakBPM, BH_HighLowRatio, BHSUM1, BHSUM2, BHSUM3");
}

void 
BeatHistoFeatures::harm_prob(mrs_real& pmax, mrs_real factor, 
			     mrs_real& s1, mrs_natural& t1, 
			     mrs_real& s2, mrs_natural& t2, 
			     mrs_natural tmx,
			     mrs_natural size, 
			     realvec& in)
{
  
  mrs_natural index = (mrs_natural) floor(factor * tmx + 0.5);
  
  
  
  mrs_real prob;
  mrs_real c;
  
  if (index > 100.0)
    c = 1.0;
  else 
    c = 0.75;
  
  mrs_real a;
  
  if ((tmx > 50)&&(tmx < 100))
    a = 1.5;
  else 
    a = 0.75;
  

  
  
  if (index < size) 
    {
      prob = a * in(0,tmx) + c *(in(0, index));


      if ((index-1 > 0) && (index-1 < size))
	prob += (c * in(0, index-1));
      if ((index+1 > 0) && (index+1 < size))
	prob += (c * in(0, index+1));
      
       
      if ((index-2 > 0) && (index-2 < size))
	prob += (c * in(0, index-2));
      if ((index+2 > 0) && (index+2 < size))
	prob += (c * in(0, index+2));

      if ((index-3 > 0) && (index-3 < size))
	prob += (c * in(0, index-3));
      if ((index+3 > 0) && (index+3 < size))
	prob += (c * in(0, index+3));
      
      if (index > 150.0)
	{
	  
	  if ((index-4 > 0) && (index-4 < size))
	    prob += (c * in(0, index-4));
	  if ((index+4 > 0) && (index+4 < size))
	    prob += (c * in(0, index+4));
	  
	  if ((index-5 > 0) && (index-5 < size))
	    prob += (c * in(0, index-5));
	  if ((index+5 > 0) && (index+5 < size))
	    prob += (c * in(0, index+5));


	  if ((index-6 > 0) && (index-6 < size))
	    prob += (c * in(0, index-6));
	  if ((index+6 > 0) && (index+6 < size))
	    prob += (c * in(0, index+6));
	}


      /* if (index > 150.0)
	{
	  
	  if ((index-7 > 0) && (index-7 < size))
	    prob += (c * in(0, index-7));
	  if ((index+7 > 0) && (index+7 < size))
	    prob += (c * in(0, index+7));
	  
	  if ((index-8 > 0) && (index-8 < size))
	    prob += (c * in(0, index-8));
	  if ((index+8 > 0) && (index+8 < size))
	    prob += (c * in(0, index+8));


	  if ((index-9 > 0) && (index-9 < size))
	    prob += (c * in(0, index-9));
	  if ((index+9 > 0) && (index+9 < size))
	    prob += (c * in(0, index+9));
	}
      */ 

      
    }
  else 
    prob = 0;  

  
  
  if (prob > pmax) 
    {
      if (tmx < index) 
	{
	  s1 = in(0,tmx);
	  s2 = in(0,index);
	  if ((index-1 > 0) && (index-1 < size))
	    s2 += in(0,index-1);
	  if ((index+1 > 0) && (index+1 < size))
	    s2 += in(0,index+1);
	  
	  if ((index-2 > 0) && (index-2 < size))
	    s2 += in(0,index-2);
	  if ((index+2 > 0) && (index+2 < size))
	    s2 += in(0,index+2);	  

	  if ((index-3 > 0) && (index-3 < size))
	    s2 += in(0,index-3);
	  if ((index+3 > 0) && (index+3 < size))
	    s2 += in(0,index+3);	  

	  t1 = tmx+1;
	  t2 = (int)(factor * t1);
	  pmax = prob;
	}
      else 
	{
	  s1 = in(0,index);
	  if ((index-1 > 0) && (index-1 < size))
	    s1 += in(0,index-1);
	  if ((index+1 > 0) && (index+1 < size))
	    s1 += in(0,index+1);	  

	  if ((index-2 > 0) && (index-2 < size))
	    s1 += in(0,index-2);
	  if ((index+2 > 0) && (index+2 < size))
	    s1 += in(0,index+2);	  

	  if ((index-3 > 0) && (index-3 < size))
	    s1 += in(0,index-3);
	  if ((index+3 > 0) && (index+3 < size))
	    s1 += in(0,index+3);	  



	  s2 = in(0,tmx);
	  t1 = index+1;
	  t2 = (int)(factor * t1);
	  pmax = prob;
	}
    }  

  
  
  
}



void 
BeatHistoFeatures::myProcess(realvec& in, realvec& out)
{
  // in.write("histo.plot");
  
  //checkFlow(in,out);

  mrs_real mx = DBL_MIN;
  mrs_natural tmx  = 0;
  

  
  mrs_real pmax = DBL_MIN;
  mrs_natural t1;
  mrs_natural t2;
  mrs_real s1;
  mrs_real s2;

  t1 = 0;
  t2 = 0;
  s1 = 0.0;
  s2 = 0.0;
  
  flag_.setval(0.0);
  
  for (c=0; c < 3; c++)
    {
      
      for (o=0; o < inObservations_; o++)
	for (t = 0; t < inSamples_; t++)
	  {
	    if (((in(o,t) > mx)&&(flag_(t) == 0.0))&&(t > 40) && (t < 120))
	      {
		mx = in(o,t);
		tmx = t;
	      }
	  }
      
      
      flag_(tmx) = 1.0;
      mx = DBL_MIN;

      
      if (tmx < 120.0)
	{
	  harm_prob(pmax, 2, s1, t1, s2, t2, tmx, inSamples_, in);
	  harm_prob(pmax, 3.0, s1, t1, s2, t2, tmx, inSamples_, in);
	}
      else 
	{
	  harm_prob(pmax, 0.5, s1, t1, s2, t2, tmx, inSamples_, in);
	  harm_prob(pmax, 0.33333, s1, t1, s2, t2, tmx, inSamples_, in);
	}
      
    }
  
  flag_.setval(0.0);
  
  mrs_real sum1 = 0.0;
  mrs_real sum2 = 0.0;
  mrs_real sum3 = 0.0;
  
  for (t = 40; t < 90; t++)
    sum1 += in(0,t);

  for (t = 90; t < 140; t++)
    sum2 += in(0,t);

  for (t = 40; t < 250; t++)
    sum3 += in(0,t);
  
  

  

  
  
  out(0,0) = s1;
  out(1,0) = t1;
  out(2,0) = s2;
  out(3,0) = t2;

  if( t1 == 0 )
	  out(4,0) = 0.0;
  else
  	out(4,0) = t2 / t1;

  out(5,0) = sum1;
  out(6,0) = sum2;
  out(7,0) = sum3;
  


   
  
  
}



/* 
void 
BeatHistoFeatures::myProcess(realvec& in, realvec& out)
{
  //checkFlow(in,out);

  mrs_real sum = 0;

  mxr_->process(in,mxres_);
  
  
  for (o=0; o < inObservations_; o++)
    for (t = 0; t < inSamples_; t++)
      {
	sum += in(o,t);
      }
  

  out(0,0) = sum;
  out(1,0) = mxres_(0,0) / sum; 	// maximum amp1 normalized by sum 
  out(2,0) = mxres_(0,2) / sum;		// maximum amp2 normalized by sum 
  out(3,0) = mxres_(0,4) / sum;		// maximum amp3 normalized by sum 
  out(4,0) = mxres_(0,1);
  out(5,0) = mxres_(0,3);
  out(6,0) = mxres_(0,5);
  out(7,0) = mxres_(0,2) / mxres_(0,0);

  in.write("histo.plot");
  
}
*/ 






	

	
