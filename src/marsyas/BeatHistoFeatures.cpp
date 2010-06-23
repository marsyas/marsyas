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


#include "BeatHistoFeatures.h"
#include <cfloat> 

using namespace std;
using namespace Marsyas;

BeatHistoFeatures::BeatHistoFeatures(string name):MarSystem("BeatHistoFeatures", name)
{
	mxr_ = NULL;
	pkr_ = NULL;
	
	addControls();
}

BeatHistoFeatures::BeatHistoFeatures(const BeatHistoFeatures& a):MarSystem(a)
{
	mxr_ = NULL;
	pkr_ = NULL;
	ctrl_mode_ = getctrl("mrs_string/mode");
}

BeatHistoFeatures::~BeatHistoFeatures()
{
  delete mxr_;
  delete pkr_;
}


MarSystem* 
BeatHistoFeatures::clone() const 
{
  return new BeatHistoFeatures(*this);
}

void 
BeatHistoFeatures::addControls()
{
	addctrl("mrs_string/mode", "method1", ctrl_mode_);
}

void
BeatHistoFeatures::myUpdate(MarControlPtr sender)
{
	(void) sender;
	MRSDIAG("BeatHistoFeatures.cpp - BeatHistoFeatures:myUpdate");
  
	delete mxr_;//[!]
	delete pkr_;
	mxr_ = new MaxArgMax("mxr");//[!]
	pkr_ = new Peaker("pkr");

	setctrl("mrs_natural/onSamples", (mrs_natural)1);	
	setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));
	
	mrs_string mode = ctrl_mode_->to<mrs_string>();
	 
	if (mode == "method1")
	{
		setctrl("mrs_natural/onObservations", (mrs_natural)8);
		setctrl("mrs_string/onObsNames", "BH_SUM,BH_AMP1,BH_AMP2,BH_AMP3,BH_PER1,BH_PER2,BH_PER3,BH_RATIO");
	}
	else if (mode == "method2")
	{
		setctrl("mrs_natural/onObservations", (mrs_natural)8);     // alex 
		setctrl("mrs_string/onObsNames", "b1,b2,b3,b4,b5,b6,b7,b8");
	}
	else 
		cout << "Unsupported mode" << endl;
	
	flag_.create(getctrl("mrs_natural/inSamples")->to<mrs_natural>());
	mxr_->updControl("mrs_natural/inSamples", getctrl("mrs_natural/inSamples"));
	mxr_->updControl("mrs_natural/inObservations", getctrl("mrs_natural/inObservations"));
	mxr_->updControl("mrs_real/israte", getctrl("mrs_real/israte"));
	mxr_->updControl("mrs_natural/nMaximums", 3);


	pkr_->updControl("mrs_natural/inSamples", getctrl("mrs_natural/inSamples"));
	pkr_->updControl("mrs_natural/inObservations", getctrl("mrs_natural/inObservations"));
	pkr_->updControl("mrs_real/israte", getctrl("mrs_real/israte"));


	pkr_->updControl("mrs_natural/peakNeighbors", 40);
	pkr_->updControl("mrs_real/peakSpacing", 0.2);
	pkr_->updControl("mrs_natural/peakStart", 200);
	pkr_->updControl("mrs_natural/peakEnd", 640);


	
	mxres_.create(mxr_->getctrl("mrs_natural/onObservations")->to<mrs_natural>(),
				  mxr_->getctrl("mrs_natural/onSamples")->to<mrs_natural>());
	pkres_.create(pkr_->getctrl("mrs_natural/onObservations")->to<mrs_natural>(),
				  pkr_->getctrl("mrs_natural/onSamples")->to<mrs_natural>());
    
}

mrs_real 
BeatHistoFeatures::sum_nearby(mrs_natural index, mrs_natural radius, mrs_natural size, const realvec& in)
{
    mrs_real sum = 0.0;
    mrs_natural ix;
    for (mrs_natural i = 1; i <= radius; ++i)
    {
        ix = index - i;
        if (0 < ix && ix < size) // Make sure we have a valid index.
            sum += in(0, ix);

        ix = index + i;
        if (0 < ix && ix < size)
            sum += in(0, ix);
    }        

    return sum;
}

// If the bins of <in> surrounding index <factor>*<tmx> have a sum greater
// than <pmax>, set s1, s2, t1, t2. Otherwise do nothing.
void 
BeatHistoFeatures::harm_prob(mrs_real& pmax, mrs_real factor, 
			     mrs_real& s1, mrs_natural& t1, 
			     mrs_real& s2, mrs_natural& t2, 
			     mrs_natural tmx,
			     mrs_natural size, 
			     const realvec& in)
{

    mrs_natural index = (mrs_natural) floor(factor * tmx + 0.5);    
    mrs_real c = (index > 100.0) ? 1.0 : 0.75;
    mrs_real a = ((50 < tmx) && (tmx < 100)) ? 1.5 : 0.75;
    mrs_real prob = 0.0;

    if (index < size)
    {
        prob = a * in(0,tmx) + c * in(0, index);

        // Decide how far to look based on index; in the past, values as high
        // as 9 have been used if index > 150.
        mrs_natural radius = index > 150 ? 6 : 3;
        prob += c * sum_nearby(index, radius, size, in);
    }

    if (prob > pmax)
    {
        pmax = prob;
        if (tmx < index)
        {
            s1 = in(0,tmx);
            s2 = in(0,index) + sum_nearby(index, 3, size, in);
            t1 = tmx+1;
        }
        else
        {
            s1 = in(0,index) + sum_nearby(index, 3, size, in);
            s2 = in(0,tmx);
            t1 = index+1;
        }

        t2 = (mrs_natural)(factor * t1);
    }  
}



// void 
// BeatHistoFeatures::myProcess(realvec& in, realvec& out)
// {
//   // in.write("histo.plot");
//   //checkFlow(in,out);
//   mrs_natural o,c,t;
//   mrs_real mx = DBL_MIN;
//   mrs_natural tmx  = 0;
//   mrs_real pmax = DBL_MIN;
//   mrs_natural t1 = 0;
//   mrs_natural t2 = 0;
//   mrs_real s1 = 0.0;
//   mrs_real s2 = 0.0;

//   flag_.setval(0.0);
  
//   // c, o, and t are declared in MarSystem.h. 
//   // TODO: Why do we use c < 3 here? (i.e. why 3?)
//   for (c=0; c < 3; ++c)
//   {
//       for (o=0; o < inObservations_; o++)
//       {
//           for (t = 0; t < inSamples_; t++)
//           {
//               if (((in(o,t) > mx) && (flag_(t) == 0.0)) && (40 < t) && (t < 120))
//               {
//                   mx = in(o,t);
//                   tmx = t;
//               }
//           }
//       }

//       flag_(tmx) = 1.0;
//       mx = DBL_MIN; // reset max

//       if (tmx < 120.0)
//       {
//           harm_prob(pmax, 2, s1, t1, s2, t2, tmx, inSamples_, in);
//           harm_prob(pmax, 3.0, s1, t1, s2, t2, tmx, inSamples_, in);
//       }
//       else 
//       {
//           harm_prob(pmax, 0.5, s1, t1, s2, t2, tmx, inSamples_, in);
//           harm_prob(pmax, 0.33333, s1, t1, s2, t2, tmx, inSamples_, in);
//       }
//   }
  
//   flag_.setval(0.0);
  
//   mrs_real sum1 = 0.0;
//   for (t = 40; t < 90; t++)
//     sum1 += in(0,t);

//   mrs_real sum2 = 0.0;
//   for (t = 90; t < 140; t++)
//     sum2 += in(0,t);

//   mrs_real sum3 = 0.0;
//   for (t = 40; t < 250; t++)
//     sum3 += in(0,t);
    
//   out(0,0) = s1;
//   out(1,0) = t1;
//   out(2,0) = s2;
//   out(3,0) = t2;
//   out(4,0) = (0 == t1) ? 0.0 : (t2 / t1);
//   out(5,0) = sum1;
//   out(6,0) = sum2;
//   out(7,0) = sum3;
// }


void 
BeatHistoFeatures::method1(realvec& in, realvec& out) 
{
	mrs_real sum = 0;
  
	for (o=0; o < inObservations_; o++)
		for (t = 0; t < inSamples_; t++)
		{
			sum += in(o,t);
		}
	out(0,0) = sum;
	
	// zero-out below 50BPM 
	for (int i=0; i < 200; i++) 
		in(i) = 0;
	
	pkr_->process(in, pkres_);
	mxr_->process(pkres_,mxres_);
	
	
	
	
	out(1,0) = mxres_(0,0) / sum; 	// maximum amp1 normalized by sum 
	out(2,0) = mxres_(0,2) / sum;		// maximum amp2 normalized by sum 
	out(3,0) = mxres_(0,4) / sum;		// maximum amp3 normalized by sum 
	out(4,0) = mxres_(0,1) /4.0;
	out(5,0) = mxres_(0,3) /4.0;
	out(6,0) = mxres_(0,5) /4.0;
	out(7,0) = mxres_(0,2) / mxres_(0,0);
	
	

}




void 
BeatHistoFeatures::method2(realvec& in, realvec& out)
{

	// alex 
}


void 
BeatHistoFeatures::myProcess(realvec& in, realvec& out)
{
	mrs_string mode = ctrl_mode_->to<mrs_string>();
	
	if (mode == "method1")
		method1(in,out);
	else if (mode == "method2") 
		method2(in,out);
	else 
		cout << "Unsupported mode" << endl;
	
}







	

	
