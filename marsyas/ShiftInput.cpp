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
    \class ShiftInput
    \brief Shift next D samples  

    Shift next D samples from the input source to the output source 
into the right-hand end of the outputSlice. 
with gain and put them in the output vector. 
*/

#include "ShiftInput.h"
using namespace std;


ShiftInput::ShiftInput(string name)
{
  type_ = "ShiftInput";
  name_ = name;
  addControls();
}


ShiftInput::~ShiftInput()
{
}

MarSystem* 
ShiftInput::clone() const 
{
  return new ShiftInput(*this);
}


void
ShiftInput::addControls()
{
  addDefaultControls();
  addctrl("natural/Decimation", (natural)MRS_DEFAULT_SLICE_NSAMPLES/2);
  addctrl("natural/WindowSize", (natural)MRS_DEFAULT_SLICE_NSAMPLES);
  setctrlState("natural/WindowSize", true);

  addctrl("bool/reset", true);
  setctrlState("bool/reset", true);

}



void
ShiftInput::update()
{
  
  reset_ = getctrl("bool/reset").toBool();  

  setctrl("natural/onSamples", getctrl("natural/WindowSize"));
  setctrl("natural/onObservations", (natural)1);
  setctrl("real/osrate", getctrl("real/israte"));  
  setctrl("natural/Decimation", getctrl("natural/inSamples"));
  
  W_ = getctrl("natural/WindowSize").toNatural();

  
  if (PW_ != W_) 
    pout_.stretch(W_);
  
  PW_ = W_;  
  N_ = getctrl("natural/onSamples").toNatural();
  D_ = getctrl("natural/inSamples").toNatural();

  defaultUpdate();
}




void 
ShiftInput::process(realvec& in, realvec& out)
{
  checkFlow(in,out);

  if (reset_) 
    {
      pout_.setval(0.0);
      reset_ = false;
      setctrl("bool/reset", (MarControlValue)false);
    }
  
  
  for (t = 0; t < N_-D_; t++)
    out(t) = pout_(t+D_);
  
  for (t=N_-D_; t < N_; t++)
    {
      out(t) = in(t-(N_-D_));
    }
  for (t = 0; t < N_; t++) 
    pout_(t) = out(t);
}






	

	
