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

#include "ResampleBezier.h"

using namespace std;
using namespace Marsyas;

/** Marsystem for resampling the audio Samplingrate
*
*
*
*
*
*
*/

ResampleBezier::ResampleBezier(mrs_string name):MarSystem("ResampleBezier", name)
{
  //Add any specific controls needed by ResampleBezier
  //(default controls all MarSystems should have
  //were already added by MarSystem::addControl(),
  //called by :MarSystem(name) constructor).
  //If no specific controls are needed by a MarSystem
  //there is no need to implement and call this addControl()
  //method (see for e.g. Rms.cpp)
  addControls();
}

ResampleBezier::ResampleBezier(const ResampleBezier& a) : MarSystem(a)
{
  // For any MarControlPtr in a MarSystem
  // it is necessary to perform this getctrl
  // in the copy constructor in order for cloning to work


  //mrs_bool ctrl_tangentMode	-	false:	(default)tangent at interpolation point is derived from the previous and immediately following sample;
  //								true:	tangents at each interpolation point are parallel to the axis along the indices of the samples
  //										which alows for smooth transition at frame endings,
  //										maximum amplitude will be limited to the maximum prior to interpolation
  //mrs_bool ctrl_samplingRateAdjustmentMode - adjust new resulting SamplingRate for following Marsystems
  //mrs_real stretch - desired stretch ratio (number of output samples = input number of samples*stretch)
  //mrs_real offStart - (default:0) offset from the start (towards the end) of the Samples (if only a part of the samples should be used to interpolate)
  //mrs_real offEnd - (default:0) offset from the end (towards the start) of the Samples (if only a part of the samples should be used to interpolate)


  ctrl_offStart_ = getctrl("mrs_real/offStart");
  ctrl_offEnd_ = getctrl("mrs_real/offEnd");
  ctrl_tangentMode_ = getctrl("mrs_bool/tangentMode");
  ctrl_samplingRateAdjustmentMode_ = getctrl("mrs_bool/samplingRateAdjustmentMode");
  ctrl_stretch_ = getctrl("mrs_real/stretch");

}

ResampleBezier::~ResampleBezier()
{
}

MarSystem*
ResampleBezier::clone() const
{
  return new ResampleBezier(*this);
}

void
ResampleBezier::addControls()
{
  //Add specific controls needed by this MarSystem.
  addctrl("mrs_real/offStart", 0.0, ctrl_offStart_);
  addctrl("mrs_real/offEnd", 0.0, ctrl_offEnd_);
  addctrl("mrs_bool/samplingRateAdjustmentMode", (mrs_bool)true , ctrl_samplingRateAdjustmentMode_);
  addctrl("mrs_bool/tangentMode", (mrs_bool)false , ctrl_tangentMode_);
  addctrl("mrs_real/stretch", 1.0 , ctrl_stretch_);
  setctrlState("mrs_real/stretch", true);
  setctrlState("mrs_bool/samplingRateAdjustmentMode",(mrs_bool)true);

}

void
ResampleBezier::myUpdate(MarControlPtr sender)
{
  MarSystem::myUpdate(sender);


  mrs_real alpha = ctrl_stretch_->to<mrs_real>();

  ctrl_onSamples_->setValue((mrs_natural) (alpha * ctrl_inSamples_->to<mrs_natural>()), NOUPDATE);
  ctrl_onObservations_->setValue(ctrl_inObservations_->to<mrs_natural>());
  if (!(ctrl_samplingRateAdjustmentMode_->to<mrs_bool>()))
  {
    alpha=1.0;
  }

  ctrl_osrate_->setValue(ctrl_israte_->to<mrs_real>()*alpha);



  mrs_string inObsNames = ctrl_inObsNames_->to<mrs_string>();
  // Add prefix to the observation names.
  ctrl_onObsNames_->setValue(obsNamesAddPrefix(inObsNames, "ResampleBezier_"), NOUPDATE);

}

mrs_real
ResampleBezier::interpolBezier(mrs_realvec px,mrs_real t)
{
  mrs_natural n = px.getSize();
  mrs_realvec q;
  q.create(n,n);

  for (mrs_natural i=0; i<n; ++i)
  {
    q(i,0) = px(i);
  }

  //j - number of interpolation step
  //i - number of points
  for (mrs_natural j=1; j<n; j++)
  {
    for (mrs_natural i=0; i<n-j; ++i)
    {
      q(i,j) = (1 - t) * q(i,j - 1) + t * q(i + 1,j - 1);
    }
  }
  return q(0,n-1);
}


void
ResampleBezier::myProcess(realvec& in, realvec& out)
{
  //cout << "-------------------------WORLD ENDING!!!" << endl;
  mrs_natural numb=inSamples_*3;	//cubic bezier curve:2 control points for each interpolation point
  mrs_realvec sx;
  mrs_realvec sy;
  mrs_realvec bx;
  mrs_realvec by;
  sx.create(inSamples_);
  sy.create(inSamples_);
  bx.create(numb);
  by.create(numb);

  mrs_realvec px;
  px.create(inSamples_);
  for(mrs_natural i=0; i<inSamples_; ++i)
  {
    px(i)=i;
  }

  //#############chosen length parameterisation############
  mrs_realvec u2;
  u2.create(inSamples_);

  for (mrs_natural o=0; o<inObservations_; o++)
  {
    for (mrs_natural i=0; i<inSamples_; ++i)
    {

      if (i!=inSamples_-1)
      {

        if (i!=0)
        {
          u2(i)=u2(i-1)+sqrt((px(i)-px(i+1))*(px(i)-px(i+1))+(in(o,i)-in(o,i+1))*(in(o,i)-in(o,i+1)));
        }
        else
        {
          u2(0)=0;
        }
      }
      else
      {
        u2(i)=u2(i-1)+sqrt((px(i)-px(0))*(px(i)-px(0))+(in(o,i)-in(o,0))*(in(o,i)-in(o,0)));
      }
    }

    /////////////////END of parameterization

    mrs_natural foo=(mrs_natural)0;
    mrs_natural bar=(mrs_natural)0;

    mrs_real deltauk=(mrs_real)0.0;
    mrs_real deltaukm=(mrs_real)0.0;

    mrs_bool tangentMode=ctrl_tangentMode_->to<mrs_bool>();
    mrs_natural samplesout=getctrl("mrs_natural/onSamples")->to<mrs_natural>();
    for(mrs_natural k=0; k<inSamples_; k++)
    {
      //indices for previous and next element with border management
      mrs_natural bla=k-1;
      if (bla<0)
      {
        bla=0;
      }
      mrs_natural blu=k+1;
      if (blu>inSamples_-1)
      {
        blu=inSamples_-1;
      }

      //indices for previous and next bezier points with border management
      //this are the indizes of the control points that do not actually lie on the interpolation polynomial
      //unlike each point with an index of 3*k
      foo=(3*k-1)%numb;
      if (foo<0)
      {
        foo=foo+numb;
      }
      bar=(3*k+1)%numb;
      if (bar<0)
      {
        bar=bar+numb;
      }

      if (k>=2)
      {
        deltauk=u2(k)-u2(k-1);
        deltaukm=u2(k-1)-u2(k-2);
      }
      else
      {
        deltauk=0.5;
        deltaukm=0.5;
      }
      if (tangentMode==(mrs_bool)true)
      {

        sx(k)=(px(blu)-px(bla))/(deltauk+deltaukm); //normalization for non equidistant parameterization
        sy(k)=(in(o,blu)-in(o,bla))/(deltauk+deltaukm); //normalization for non equidistant parameterization

        bx(foo)=px(k)-deltaukm*sx(k)/2.0;
        by(foo)=in(o,k)-deltaukm*sy(k)/2.0;

        bx(3*k)=px(k);
        by(3*k)=in(o,k);

        bx(bar)=px(k)+deltauk*sx(k)/2.0;
        by(bar)=in(o,k)+deltauk*sy(k)/2.0;
      }
      else
      {
        sx(k)=(px(blu)-px(bla))/(deltauk+deltaukm);

        sy(k)=(in(o,blu)-in(o,bla))/(deltauk+deltaukm);

        bx(foo)=px(k)-0.5*(px(k)-px(bla));

        by(foo)=in(o,k);

        bx(3*k)=px(k);
        by(3*k)=in(o,k);

        bx(bar)=px(k)+0.5*(px(blu)-px(k));
        by(bar)=in(o,k);
      }
    }
    //endof for
    mrs_real offStart=ctrl_offStart_->to<mrs_real>();
    mrs_real offEnd=ctrl_offEnd_->to<mrs_real>();

    mrs_real ratio=(inSamples_-1-offStart-offEnd)/(mrs_real)(samplesout-1);
    mrs_natural index=0;
    mrs_realvec ix;
    mrs_realvec yp;
    ix.create(4);
    yp.create(4);
    for(mrs_natural i=0; i<samplesout; ++i)
    {
      mrs_real ra=offStart+i*ratio;
      while (index+1<ra)
      {
        index=index+1;
      }
      for(mrs_natural j=0; j<4; j++)
      {
        ix(j)=bx((3*index+j)%((int)inSamples_*3));
        yp(j)=by((3*index+j)%((int)inSamples_*3));
      }

      mrs_real difference=ra-index;
      out(o,i)=interpolBezier(yp, difference);
    }
  }

}
