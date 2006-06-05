
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
    \class ShiftOuput
    \brief Shift next D samples  

    Shift next D samples from the input source to the output source 
into the right-hand end of the outputSlice. 
with gain and put them in the output vector. 
*/

#include "ShiftOutput.h"
using namespace std;

ShiftOutput::ShiftOutput():MarSystem()
{
  type_ = "ShiftOutput";
}


ShiftOutput::ShiftOutput(string name)
{
  type_ = "ShiftOutput";
  name_ = name;
  addControls();
  
}


ShiftOutput::~ShiftOutput()
{
}

MarSystem* 
ShiftOutput::clone() const 
{
  return new ShiftOutput(*this);
}



void
ShiftOutput::addControls()
{
  addDefaultControls();
  addctrl("natural/Interpolation", (natural)MRS_DEFAULT_SLICE_NSAMPLES / 2);
  setctrlState("natural/Interpolation", true);
  addctrl("natural/WindowSize", (natural)MRS_DEFAULT_SLICE_NSAMPLES);
  addctrl("natural/Decimation", (natural)MRS_DEFAULT_SLICE_NSAMPLES/2);
}



void
ShiftOutput::update()
{
  setctrl("natural/onSamples", getctrl("natural/Interpolation"));
  setctrl("natural/onObservations", (natural)1);
  setctrl("real/osrate", getctrl("real/israte"));  
  
  setctrl("natural/WindowSize",getctrl("natural/inSamples"));


  defaultUpdate();
  
  
  tmpSlice_.stretch(inObservations_, inSamples_);
  
  I_ = getctrl("natural/onSamples").toNatural();
  N_ = getctrl("natural/inSamples").toNatural();
  Nw_ = getctrl("natural/WindowSize").toNatural();
  D_ = getctrl("natural/Decimation").toNatural();


}




void 
ShiftOutput::process(realvec& in, realvec& out)
{
  checkFlow(in,out);
  
  
  n_ += 2*D_ + Nw_ / 2;
  
  
  for (o=0; o < inObservations_; o++)
    for (t =0; t < inSamples_; t++)
      {
	tmpSlice_(o,t) = in(o,t);
      }

  if (n_ >= 0.0)
    for (t = 0; t < I_; t++)
      {
	out(t) = in(t);
      }
  
}






	

	
  
