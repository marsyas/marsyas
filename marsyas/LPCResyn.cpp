
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
   \class LPCResyn:
   \brief Linear Prediction Coefficients Resynthesis (LPCResyn)

   Resynthesis of signal using Linear Prediction Coefficients (LPCResyn).

*/

#include "LPCResyn.h"

using namespace std;
using namespace Marsyas;

LPCResyn::LPCResyn(string name):MarSystem("LPCResyn",name)
{
  //type_ = "LPCResyn";
  //name_ = name;

	inSize_ = 10;//How do we add definitions? DEFAULTINSIZE
  hopSize_= 512;
  outSize_ = 512;
  order_ = 10;

	addControls();
}

LPCResyn::~LPCResyn()
{
  
}

MarSystem* 
LPCResyn::clone() const 
{
  return new LPCResyn(*this);
}

void 
LPCResyn::addControls()
{
  addctrl("mrs_natural/order", order_);
  setctrlState("mrs_natural/order", true);
}


void
LPCResyn::localUpdate()
{
  MRSDIAG("LPCResyn.cpp - LPCResyn:localUpdate");
  
  //setctrl("mrs_natural/onSamples", 512);//default may need to override when using this marsystem element.
  setctrl("mrs_natural/onObservations", getctrl("mrs_natural/inObservations"));
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));
  
  order_ =  getctrl("mrs_natural/order").toNatural();
  inSize_ = order_+1;
  outSize_ = getctrl("mrs_natural/onSamples").toNatural()-order_;
  
  Zs_.create((mrs_natural)order_);
}


void 
LPCResyn::process(realvec& in, realvec& out)
{
  //cout<<"LPCResyn Process--------------------"<< endl;
  checkFlow(in,out); 
  //cout<<"resyn in vector:"<<in<<endl;
  mrs_real input;
  mrs_real pitch;
  mrs_real output;
  mrs_natural i,j;
  //cout<<" Resyn input= "<<in(inSize_-1)<<" resyn pitch= "<<in(inSize_-2)<< endl;
  //cout<<"Resyn input: "<<in<<endl;
  input = in(inSize_-1);
  pitch = in(inSize_-2);
  ticker_  = pitch;
  //cout<<"Before for loop"<< endl;
  for (i=0; i < outSize_; i++)
    {
      // cout<<"main for loop i= "<<i<< endl;
      output = 0.0;
      if (pitch == 0.0) {
	//cout<<"input: "<< input<<"RAND_MAX"<<RAND_MAX<<" in noise output: "<<output<<endl;
	//output = input * 20.0 * (mrs_real)(1.0- rand() * (RAND_MAX + 1.0));
      output =input * 20.0* (mrs_real)(rand() / (RAND_MAX + 1.0) -0.5);
      }
      else
	{
	  ticker_--;
          //cout<<"ticker: "<<ticker_<<endl;
	  if (ticker_ <= 0)
	    {
              
	      output = input * pitch * 3.0;
	      //cout<<"making impulse output=  "<<output<<endl;
	      ticker_ = pitch;
	    }
	}
      
      //cout<<"Before 2nd for loop"<< endl;
      //cout<<"Zs Size: "<<Zs_.getSize()<<"in size: "<<in.getSize()<<" order: "<<order_<< endl;
      for (j=0; j < order_-1; j++)//***Manj changed this to order-1 instead of order.

	{ 
              	 
	  output += Zs_(j) * in(j);

	  //cout<<"j= "<<j<<" Zs_j= "<<Zs_(j)<<" in j= "<<in(j)<<" output from filtering: "<<output<< endl;
	}
      //cout<<" Resyn input= "<<in(inSize_-1)<<" resyn pitch= "<<in(inSize_-2)<< endl;
      
      for (j= order_-1; j>0; j--)
	Zs_(j) = Zs_(j-1);
      //cout<<"about to assign output"<< endl;
      Zs_(0) = output;

      if(input>0.0001){//Added by Manj *****, input=power
	//printf("output = %f\n", output);
      out(i) = output;
       }
       else out(i)=0;
    }

  
  
 


    
  
  

}


