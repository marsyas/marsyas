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

#include "Reassign.h"
#include "../common_source.h"

#include <cfloat>

using namespace std;
using namespace Marsyas;

Reassign::Reassign(mrs_string name):MarSystem("Reassign",name)
{
  //type_ = "Reassign";
  //name_ = name;
}


Reassign::~Reassign()
{
}


MarSystem*
Reassign::clone() const
{
  return new Reassign(*this);
}

void
Reassign::myUpdate(MarControlPtr sender)
{
  MRSDIAG("Reassign.cpp - Reassign:myUpdate");

//   setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples"));
//   setctrl("mrs_natural/onObservations", getctrl("mrs_natural/inObservations"));
//   setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));
  MarSystem::myUpdate(sender);

  flag_.create(getctrl("mrs_natural/inSamples")->to<mrs_natural>());

  setctrl("mrs_string/onObsNames", getctrl("mrs_string/inObsNames"));
}

void
Reassign::myProcess(realvec& in, realvec& out)
{
  mrs_natural o,t,c;
  mrs_real mx = DBL_MIN;
  mrs_natural tmx  = 0;
  flag_.setval(0.0);


  for (c=0; c < 5; ++c)
  {

    for (o=0; o < inObservations_; o++)
      for (t = 0; t < inSamples_/2; t++)
      {
        if ((in(o,t) > mx)&&(flag_(t) == 0.0))
        {
          mx = in(o,t);
          tmx = t;
        }
      }


    flag_(tmx) = 1.0;
    mx = DBL_MIN;
    out(0,tmx) = in(0,tmx);


    if ((tmx-1 > 0) && (tmx-1 < inSamples_))
    {
      out(0, tmx) += in(0,tmx-1);
      out(0,tmx-1) = 0.0;
      flag_(tmx-1) = 1.0;
    }


    if ((tmx+1 > 0) && (tmx+1 < inSamples_))
    {
      out(0, tmx) += in(0, tmx+1);
      out(0,tmx+1) = 0.0;
      flag_(tmx+1) = 1.0;
    }

    if ((tmx-2 > 0) && (tmx-2 < inSamples_))
    {
      out(0, tmx) += in(0, tmx-2);
      out(0,tmx-2) = 0.0;
      flag_(tmx-2) = 1.0;
    }


    if ((tmx+2 > 0) && (tmx+2 < inSamples_))
    {
      out(0, tmx) += in(0, tmx+2);
      out(0,tmx+2) = 0.0;
      flag_(tmx+2) = 1.0;
    }


    if ((tmx-3 > 0) && (tmx-3 < inSamples_))
    {
      out(0, tmx) += in(0, tmx-3);
      out(0,tmx-3) = 0.0;
      flag_(tmx-3) = 1.0;
    }


    if ((tmx+3 > 0) && (tmx+3 < inSamples_))
    {
      out(0, tmx) += in(0, tmx+3);
      out(0,tmx+3) = 0.0;
      flag_(tmx+3) = 1.0;
    }


    if ((tmx-4 > 0) && (tmx-4 < inSamples_))
    {
      out(0, tmx) += in(0, tmx-4);
      out(0,tmx-4) = 0.0;
      flag_(tmx-4) = 1.0;
    }


    if ((tmx+4 > 0) && (tmx+4 < inSamples_))
    {
      out(0, tmx) += in(0, tmx+4);
      out(0,tmx+4) = 0.0;
      flag_(tmx+4) = 1.0;
    }


    if ((tmx-5 > 0) && (tmx-5 < inSamples_))
    {
      out(0, tmx) += in(0, tmx-5);
      out(0,tmx-5) = 0.0;
      flag_(tmx-5) = 1.0;
    }


    if ((tmx+5 > 0) && (tmx+5 < inSamples_))
    {
      out(0, tmx) += in(0, tmx+5);
      out(0,tmx+5) = 0.0;
      flag_(tmx+5) = 1.0;
    }


  }


  flag_.setval(0.0);


  for (c=0; c < 4; ++c)
  {

    for (o=0; o < inObservations_; o++)
      for (t = inSamples_/2; t < inSamples_; t++)
      {
        if ((in(o,t) > mx)&&(flag_(t) == 0.0))
        {
          mx = in(o,t);
          tmx = t;
        }
      }


    flag_(tmx) = 1.0;
    mx = DBL_MIN;
    out(0,tmx) = in(0,tmx);


    if ((tmx-1 > 0) && (tmx-1 < inSamples_))
    {
      out(0, tmx) += in(0,tmx-1);
      out(0,tmx-1) = 0.0;
      flag_(tmx-1) = 1.0;
    }


    if ((tmx+1 > 0) && (tmx+1 < inSamples_))
    {
      out(0, tmx) += in(0, tmx+1);
      out(0,tmx+1) = 0.0;
      flag_(tmx+1) = 1.0;
    }

    if ((tmx-2 > 0) && (tmx-2 < inSamples_))
    {
      out(0, tmx) += in(0, tmx-2);
      out(0,tmx-2) = 0.0;
      flag_(tmx-2) = 1.0;
    }


    if ((tmx+2 > 0) && (tmx+2 < inSamples_))
    {
      out(0, tmx) += in(0, tmx+2);
      out(0,tmx+2) = 0.0;
      flag_(tmx+2) = 1.0;
    }


    if ((tmx-3 > 0) && (tmx-3 < inSamples_))
    {
      out(0, tmx) += in(0, tmx-3);
      out(0,tmx-3) = 0.0;
      flag_(tmx-3) = 1.0;
    }


    if ((tmx+3 > 0) && (tmx+3 < inSamples_))
    {
      out(0, tmx) += in(0, tmx+3);
      out(0,tmx+3) = 0.0;
      flag_(tmx+3) = 1.0;
    }


    if ((tmx-4 > 0) && (tmx-4 < inSamples_))
    {
      out(0, tmx) += in(0, tmx-4);
      out(0,tmx-4) = 0.0;
      flag_(tmx-4) = 1.0;
    }


    if ((tmx+4 > 0) && (tmx+4 < inSamples_))
    {
      out(0, tmx) += in(0, tmx+4);
      out(0,tmx+4) = 0.0;
      flag_(tmx+4) = 1.0;
    }


    if ((tmx-5 > 0) && (tmx-5 < inSamples_))
    {
      out(0, tmx) += in(0, tmx-5);
      out(0,tmx-5) = 0.0;
      flag_(tmx-5) = 1.0;
    }


    if ((tmx+5 > 0) && (tmx+5 < inSamples_))
    {
      out(0, tmx) += in(0, tmx+5);
      out(0,tmx+5) = 0.0;
      flag_(tmx+5) = 1.0;
    }


  }





}










