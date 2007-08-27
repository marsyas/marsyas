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

#include "Biquad.h"

using namespace std;
using namespace Marsyas;

Biquad::Biquad(string name):MarSystem("Biquad",name)
{

    filter = new Filter("filter");

    b.create(3);	
    a.create(3);

    addControls();
}

Biquad::~Biquad()
{
}


MarSystem* Biquad::clone() const 
{
    return new Biquad(*this);
}

void Biquad::addControls()
{

    addctrl("mrs_string/type","lowpass");
    addctrl("mrs_real/resonance", 0.5);
    addctrl("mrs_real/frequency", 500.0);

    setctrlState("mrs_string/type", true);
    setctrlState("mrs_real/resonance", true);
    setctrlState("mrs_real/frequency", true);

}

void Biquad::myUpdate(MarControlPtr sender)
{
    /* 
        Formula utilized is from "Audio-EQ-Cookbook" by Robert Bristow-Johnson
        http://www.musicdsp.org/files/Audio-EQ-Cookbook.txt
        Last retrieved March 20, 2007.
    */

    mrs_string type = getctrl("mrs_string/type")->to<mrs_string>();

    freq_ = getctrl("mrs_real/frequency")->to<mrs_real>();
    q_ = getctrl("mrs_real/resonance")->to<mrs_real>();

    fs_ = getctrl("mrs_real/israte")->to<mrs_real>();
    w0_ = 2 * PI * freq_ / fs_;

    if (type == "lowpass")
    {
        alpha_ = sin(w0_)/(2*q_);

        b(0) = (1 - cos(w0_))/2;
        b(1) =  1 - cos(w0_);
        b(2) = (1 - cos(w0_))/2;

        a(0) = 1 + alpha_;
        a(1) = 2 * cos(w0_);
        a(2) = 1 - alpha_;

        filter->setctrl("mrs_realvec/ncoeffs", b);
        filter->setctrl("mrs_realvec/dcoeffs", a);

    }
    else
    {
        cout << "BIQUAD: wrong type specified" << endl;
    
    }
    filter->setctrl("mrs_real/israte", fs_);
    filter->setctrl("mrs_real/osrate", fs_);
    //filter->setctrl("mrs_real/osrate", getctrl("mrs_real/osrate")->to<mrs_real>());
    filter->setctrl("mrs_natural/inObs", getctrl("mrs_natural/inObs")->to<mrs_natural>());
    filter->setctrl("mrs_natural/onObs", getctrl("mrs_natural/onObs")->to<mrs_natural>());
    filter->setctrl("mrs_natural/inSamples", getctrl("mrs_natural/inSamples")->to<mrs_natural>());
    filter->setctrl("mrs_natural/onSamples", getctrl("mrs_natural/onSamples")->to<mrs_natural>());
}


void Biquad::myProcess(realvec& in, realvec& out)
{

    filter->process(in,out);	

}
