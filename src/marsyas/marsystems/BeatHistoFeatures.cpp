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


#include "../common_source.h"
#include "BeatHistoFeatures.h"
#include <algorithm>
#include <iterator>
#include <cfloat>

using std::ostringstream;
using std::vector;
using std::abs;

using namespace Marsyas;


//#define MTLB_DBG_LOG


static void NormInPlace (realvec& vector)
{
  mrs_real sum = vector.sum ();
  if (sum > 0)
    vector	/= sum;
  return;
}

static mrs_real PeriodicCentroid (realvec& vector, mrs_bool isLog = false, mrs_natural startIdx = 200)
{
  mrs_natural len = vector.getCols ();
  mrs_real res = 0,
           norm = 0;

  for (mrs_natural i = startIdx; i < len; i++)
  {
    mrs_real theta = (isLog)? log(i*1./startIdx)* TWOPI :(i * TWOPI) / startIdx;
    res		+= vector(i) * (.5*(cos(theta)+1));
    norm	+= vector(i);
  }

  return res/norm;
}


static mrs_real PeriodicSpread (realvec& vector, mrs_real centroid, mrs_bool isLog = false, mrs_natural startIdx = 200)
{
  mrs_natural len = vector.getCols ();
  mrs_real res = 0,
           norm = 0;

  for (mrs_natural i = startIdx; i < len; i++)
  {
    mrs_real theta = (isLog)? log(i*1./startIdx)* TWOPI :(i * TWOPI) / startIdx;
    res		+= vector(i) * abs(.5*(cos(theta)+1)-centroid);
    norm	+= vector(i);
  }

  return res/norm;
}


mrs_real BeatHistoFeatures::NumMax (mrs_realvec& vector)
{

  pkr_->process(vector, pkres_);

  return pkres_.sum();
}

//static void BeatChroma (realvec& beatChroma, const realvec& beatHistogram, mrs_real beatRes = .25)
//{
//
//	//
//	mrs_natural i,j,k,
//		len = beatHistogram.getCols ();
//	const mrs_real startBpm		= 25;
//	mrs_natural startIdx = (mrs_natural)(startBpm / beatRes + .5);
//
//	beatChroma.stretch (startIdx);		// length equals startIdx
//
//	for (j = 0; j < startIdx; j++)
//	{
//		mrs_real mean = 0;
//		for (k = 0; k < 3; k++)
//		{
//			for (i = -k; i <= k; i++)
//				mean	+= beatHistogram((j+startIdx)*k+i);
//
//				beatChroma(j)	+= mean/(k+1);
//		}
//	}
//}

static mrs_real SpectralFlatness (const realvec& beatHistogram, mrs_natural startIdx = 200)
{
  mrs_real    res = 0;
  mrs_natural len = beatHistogram.getCols ();
  //mrs_natural sum = beatHistogram.sum ();

  //beatHistogram	/= sum;
  for (mrs_natural i = startIdx; i < len; i++)
    res		+= log(beatHistogram(i)+1e-6);

  return exp(res/(len-startIdx));
}

static mrs_real Std (const realvec& beatHistogram)
{
  return sqrt (beatHistogram.var ());
}

static mrs_real MaxHps (const realvec& beatHistogram, mrs_natural startIdx = 200)
{
  const mrs_natural order = 4;
  mrs_natural k,len = beatHistogram.getCols ();
  mrs_realvec	res = beatHistogram;	// make this a member

  //res.setval(-1e38);

  for (k =2; k < order; k++)
  {
    for (mrs_natural i = startIdx; i < len; i++)
    {
      if (k*i >= len)
        break;
      res(i)		+= log(beatHistogram(k*i)+1e-6);
    }
  }

  for (k = 0; k < startIdx; k++)
    res(k)	= -1e38;


  return exp (res.maxval ());
}

static void MaxAcf (mrs_real& max, mrs_real& mean, const realvec& beatHistogram, realvec& res,mrs_natural startSearchAt, mrs_natural stopSearchAt)
{
  mrs_natural k,len = beatHistogram.getCols ();

  res.setval(0.);

  // compute ACF
  for (k = startSearchAt; k < stopSearchAt; k++) // this can be optimized
  {
    mrs_real val	= 0;

    for (mrs_natural i = k; i < len; i++)
    {
      val += beatHistogram(i) * beatHistogram(i-k);
    }


    res(k)	= val / (len-k);
  }


  //pkr_->process(in, pkres_);

  max = res.maxval ();

  mean = 1e6*res.mean ();
}




BeatHistoFeatures::BeatHistoFeatures(mrs_string name):MarSystem("BeatHistoFeatures", name)
{
  mxr_ = NULL;
  pkr_ = NULL;
  pkr1_ = NULL;

  addControls();
}

BeatHistoFeatures::BeatHistoFeatures(const BeatHistoFeatures& a):MarSystem(a)
{
  mxr_ = NULL;
  pkr_ = NULL;
  pkr1_ = NULL;
  ctrl_mode_ = getctrl("mrs_string/mode");
}

BeatHistoFeatures::~BeatHistoFeatures()
{
  delete mxr_;
  delete pkr_;
  delete pkr1_;

}


MarSystem*
BeatHistoFeatures::clone() const
{
  return new BeatHistoFeatures(*this);
}

void
BeatHistoFeatures::addControls()
{
  addctrl("mrs_string/mode", "method", ctrl_mode_);
}

void
BeatHistoFeatures::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)
  MRSDIAG("BeatHistoFeatures.cpp - BeatHistoFeatures:myUpdate");

  delete mxr_;//[!]
  delete pkr_;
  delete pkr1_;

  mxr_ = new MaxArgMax("mxr");//[!]
  pkr_ = new Peaker("pkr");
  pkr1_ = new Peaker("pkr1");


  setctrl("mrs_natural/onSamples", (mrs_natural)1);
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));

  mrs_string mode = ctrl_mode_->to<mrs_string>();

  setctrl("mrs_natural/onObservations", (mrs_natural)18);     // alex
  setctrl("mrs_string/onObsNames", "BeatHisto_Sum,BeatHisto_LowPeakAmp,BeatHisto_MidPeakAmp,BeatHisto_HighPeakAmp,BeatHisto_LowBPM,BeatHisto_MidBPM,BeatHistoHighBPM,BeatHisto_LowMidBPMRatio,BeatHisto_MaxAcr,BeatHisto_MeanACR,BeatHisto_MaxHPS,BeatHisto_Flatness,BeatHisto_Std,BeatHisto_PeriodicCentroid1,BeatHisto_PeriodicCentroi2,BeatHisto_PeriodicSpread1,BeatHisto_PeriodicSpread2,BeatHisto_NumMax");

  flag_.create(getctrl("mrs_natural/inSamples")->to<mrs_natural>());
  mxr_->updControl("mrs_natural/inSamples", getctrl("mrs_natural/inSamples"));
  mxr_->updControl("mrs_natural/inObservations", getctrl("mrs_natural/inObservations"));
  mxr_->updControl("mrs_real/israte", getctrl("mrs_real/israte"));
  mxr_->updControl("mrs_natural/nMaximums", 3);


  pkr_->updControl("mrs_natural/inSamples", getctrl("mrs_natural/inSamples"));
  pkr_->updControl("mrs_natural/inObservations", getctrl("mrs_natural/inObservations"));
  pkr_->updControl("mrs_real/israte", getctrl("mrs_real/israte"));


  pkr1_->updControl("mrs_natural/inSamples", getctrl("mrs_natural/inSamples"));
  pkr1_->updControl("mrs_natural/inObservations", getctrl("mrs_natural/inObservations"));
  pkr1_->updControl("mrs_real/israte", getctrl("mrs_real/israte"));


  pkr1_->updControl("mrs_natural/peakNeighbors", 40);
  pkr1_->updControl("mrs_real/peakSpacing", 0.1);
  pkr1_->updControl("mrs_natural/peakStart", 200);
  pkr1_->updControl("mrs_natural/peakEnd", 640);


  pkr_->updControl("mrs_natural/peakNeighbors", 40);
  pkr_->updControl("mrs_real/peakSpacing", 0.1);
  pkr_->updControl("mrs_natural/peakStart", 200);
  pkr_->updControl("mrs_natural/peakEnd", 640);

  pkr_->setctrl("mrs_real/peakStrengthRelMax", 0.5);
  pkr_->setctrl("mrs_real/peakStrengthRelThresh", 2.0);
  pkr_->setctrl("mrs_real/peakStrengthThreshLpParam", 0.95);
  pkr_->setctrl("mrs_natural/peakNeighbors", 4);



  mxres_.create(mxr_->getctrl("mrs_natural/onObservations")->to<mrs_natural>(),
                mxr_->getctrl("mrs_natural/onSamples")->to<mrs_natural>());
  pkres_.create(pkr_->getctrl("mrs_natural/onObservations")->to<mrs_natural>(),
                pkr_->getctrl("mrs_natural/onSamples")->to<mrs_natural>());

  pkres1_.create(pkr1_->getctrl("mrs_natural/onObservations")->to<mrs_natural>(),
                 pkr1_->getctrl("mrs_natural/onSamples")->to<mrs_natural>());


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
BeatHistoFeatures::beatHistoFeatures(realvec& in, realvec& out)
{

  mrs_real sum = 0;

  for (mrs_natural o=0; o < inObservations_; o++)
    for (mrs_natural t = 0; t < inSamples_; t++)
    {
      sum += in(o,t);
    }


  mrs_real result[2];
  mrs_natural i,startIdx = 200;
  // zero-out below 50BPM
  for (i=0; i < startIdx; i++)
    in(i) = 0;

  for (i = startIdx; i < in.getCols (); i++)
    if (in(i) < 0)
      in(i) = 0;





  pkr1_->process(in, pkres1_);
  mxr_->process(pkres1_,mxres_);



  vector<mrs_real> bpms;
  bpms.push_back(mxres_(0,1));
  bpms.push_back(mxres_(0,3));
  bpms.push_back(mxres_(0,5));

  sort(bpms.begin(), bpms.end());

  out(0,0) = sum;
  for (unsigned int i=0; i<bpms.size(); i++)
    for (unsigned int j =0; j < bpms.size(); j++)
    {
      if (bpms[i] == mxres_(0,2*j+1))
        out(i+1,0) = mxres_(0,2*j);
    }



  out(4,0) = bpms[0] /4.0;
  out(5,0) = bpms[1] /4.0;
  out(6,0) = bpms[2] /4.0;
  out(7,0) = out(4,0) / out(5,0);



  NormInPlace (in);



#ifdef MARSYAS_MATLAB
#ifdef MTLB_DBG_LOG
  MATLAB_PUT(in, "beathist");
  MATLAB_EVAL("figure(1);plot((201:800)/4, beathist(201:800)),grid on");
#endif
#endif

  MaxAcf (result[0], result[1],in, flag_, startIdx, 600);
  out(8,0)	= result[0];
  out(9,0)	= result[1];
  out(10,0)	= MaxHps (in, startIdx);
  out(11,0)    = SpectralFlatness (in, startIdx);
  out(12,0)	= Std(in);
  out(13,0)	= PeriodicCentroid(in, false, startIdx);
  out(14,0)	= PeriodicCentroid(in, true, startIdx);
  out(15,0)	= PeriodicSpread(in, out(13,0), false, startIdx);
  out(16,0)	= PeriodicSpread(in, out(14,0), true, startIdx);
  out(17,0)	= NumMax(in);

}


void
BeatHistoFeatures::myProcess(realvec& in, realvec& out)
{
  mrs_string mode = ctrl_mode_->to<mrs_string>();
  beatHistoFeatures(in,out);
}










