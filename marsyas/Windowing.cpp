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
  \class Windowing
  \brief window the input signal

 */

#include "Windowing.h"

using namespace std;
using namespace Marsyas;

Windowing::Windowing(string name):MarSystem("Windowing",name)
{

    addcontrols();
}

Windowing::~Windowing()
{
}

MarSystem* 
Windowing::clone() const 
{
    return new Windowing(*this);
}

void
Windowing::addcontrols()
{
    addctrl("mrs_string/type", "Hamming");
    addctrl("mrs_natural/zeroPhasing", 0);
    addctrl("mrs_natural/size", 0);
    // used for the gaussian window
    addctrl("mrs_real/variance", 0.4);
    
    setctrlState("mrs_string/type", true);
    setctrlState("mrs_natural/zeroPhasing", true);
    setctrlState("mrs_natural/size", true);
    setctrlState("mrs_real/variance", true);
}

void
Windowing::myUpdate()
{
    setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples"));
    setctrl("mrs_natural/onObservations", getctrl("mrs_natural/inObservations"));
    setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));

    // needed?
    setctrl("mrs_string/onObsNames", getctrl("mrs_string/inObsNames"));  

    mrs_real variance = getctrl("mrs_real/variance")->toReal();

    mrs_natural size = getctrl("mrs_natural/size")->toNatural();
    if(size)
    {
        setctrl("mrs_natural/onSamples", size);
    }

    tmp_.create(inSamples_);
    envelope_.create(inSamples_);

    string type = getctrl("mrs_string/type")->toString();
    // should be boolean [!]
    mrs_natural zeroPhase = getctrl("mrs_natural/zeroPhasing")->toNatural();
    if(zeroPhase == 1)
        delta_ = inSamples_/2+1;
    else
        delta_=0;

    mrs_real temp = 0.0;

    if (type == "Hamming")
    {   
            mrs_real A = (mrs_real)0.54;
            mrs_real B = (mrs_real)0.46;

            for (t=0; t < inSamples_; t++)
            {
                temp = 2*PI*t / (inSamples_-1);
                envelope_(t) = A - B * cos(temp);
            }
    }
    else
    {
        if (type == "Hanning")
        {
            mrs_real A = (mrs_real)0.5;
            mrs_real B = (mrs_real)0.5;

            for (t=0; t < inSamples_; t++)
            {
                temp = 2*PI*t / (inSamples_-1);
                envelope_(t) = A - B * cos(temp);
            }
        }
        
        if (type == "Triangle")
        {
            for (t=0; t < inSamples_;t++)
            {
                temp = abs( t - (inSamples_-1)/2 );
                temp = inSamples_ /2 - temp;
                envelope_(t) = 2/inSamples_ * temp;
            }
        }
       
        // zero padded triangle function
        if (type == "Bartlett")
        {
            for (t=0;t<inSamples_;t++)
            {
                temp = abs(t - (inSamples_-1)/2);
                temp  = (inSamples_ -1 )/2 - temp;
                envelope_(t) = 2 / (inSamples_-1) * temp;
            }
        }

        if (type == "Gaussian")
        {
            for (t=0;t< inSamples_; t++)
            {
                temp = ( t - (inSamples_-1)/2 ) / ( variance*(inSamples_-1)/2 );
                temp = temp * temp;
                envelope_(t) = exp(-0.5*temp);
            }
        }
        

        if (type == "Blackman"){
            
            for (t=0;t<inSamples_;t++){
                temp = (PI * t) / inSamples_ -1 ;
                envelope_(t) = 0.42 - 0.5*cos(2*temp) + 0.08*cos(4*temp);
            }
        }
        
        if (type == "Blackman-Harris"){
            
            for (t=0;t<inSamples_;t++){
                temp = (PI * t) / inSamples_ -1 ;
                envelope_(t) = 0.35875 - 0.48829*cos(2*temp) + 0.14128*cos(4*temp) - 0.01168*cos(6*temp);
            }
        }

    }

    // not currently used
    //norm_ = envelope_.mean();
}

void 
Windowing::myProcess(realvec& in, realvec& out)
{
    //checkFlow(in,out);
    //lmartins: if (mute_) return;
    if(getctrl("mrs_bool/mute")->toBool()) return;

    for (o=0; o < inObservations_; o++)
    {
        for (t = 0; t < inSamples_; t++)
        {
            tmp_(t) =  in(o,t)*envelope_(t); // /(norm_);     
        }
        for (t = 0; t < inSamples_/2; t++)
            out(o,t)=tmp_((t+delta_)%inSamples_);
        for (t = inSamples_/2; t < inSamples_; t++)
            out(o,t+(onSamples_-inSamples_))=tmp_((t+delta_)%inSamples_);
    }
    // MATLAB_PUT(out, "peaks");
    //MATLAB_EVAL("plot(peaks(1,:))");
}
