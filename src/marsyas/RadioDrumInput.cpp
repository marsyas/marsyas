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


#include "RadioDrumInput.h"

using namespace std;
using namespace Marsyas;

RadioDrumInput::RadioDrumInput(string name):MarSystem("RadioDrumInput",name)
{
    initMidi = false;
#ifdef MARSYAS_MIDIIO
    rdin = NULL;
#endif 
    addControls();
}

RadioDrumInput::RadioDrumInput(const RadioDrumInput& a): MarSystem(a) 
{
    ctrl_byte1_ = getctrl("mrs_natural/byte1");
    ctrl_byte2_ = getctrl("mrs_natural/byte2");
    ctrl_byte3_ = getctrl("mrs_natural/byte3");
}

RadioDrumInput::~RadioDrumInput()
{
#ifdef MARSYAS_MIDIIO
    delete rdin;
#endif
}

MarSystem* RadioDrumInput::clone() const 
{
    return new RadioDrumInput(*this);
}

void RadioDrumInput::addControls()
{
    addctrl("mrs_natural/port", 0);
    addctrl("mrs_bool/initmidi", false);
    setctrlState("mrs_bool/initmidi", true);
    addctrl("mrs_natural/byte1", 0, ctrl_byte1_);
    addctrl("mrs_natural/byte2", 0, ctrl_byte2_);
    addctrl("mrs_natural/byte3", 0, ctrl_byte3_);
}

void RadioDrumInput::myUpdate(MarControlPtr sender)
{
    MRSDIAG("RadioDrumInput.cpp - RadioDrumInput:myUpdate");
    MarSystem::myUpdate(sender);

#ifdef MARSYAS_MIDIIO
    rdin = NULL;

    initMidi= getctrl("mrs_bool/initmidi")->to<mrs_bool>();

    if (!initMidi){
        try { 
            rdin = new RtMidiIn();
        } 
        catch (RtError3 &error) { 
            error.printMessage();
            return;
        }
        rdin->setCallback(&RadioDrumInput::mycallback, this);
        rdin->ignoreTypes(false, false, false); 
        setctrl("mrs_bool/initmidi", false);

        try { 
            rdin->openPort(getctrl("mrs_natural/port")->to<mrs_natural>());
        }
        catch (RtError3 &error) 
        {
            error.printMessage();
            return;
        } 
        initMidi = !initMidi;
    }

#endif
}

void RadioDrumInput::mycallback(double deltatime, std::vector< unsigned char > * message, void *userData) 
{
    // FIXME Unused parameter
    (void) deltatime;
    int nBytes = 0;
    nBytes = message->size();

    RadioDrumInput* mythis = (RadioDrumInput*) userData;

    if (nBytes > 0) 
    {
        if (nBytes > 2) 
        {
            mythis->byte3 = message->at(2); 
            mythis->byte2 = message->at(1);
            mythis->byte1 = message->at(0);

        }

    }
}

void RadioDrumInput::myProcess(realvec& in, realvec& out)
{
    // just pass data through 
    for (o=0; o < inObservations_; o++)
        for (t = 0; t < inSamples_; t++)
        {
            out(o,t) =  in(o,t);
        }
    
    ctrl_byte1_->setValue((mrs_natural)byte1, NOUPDATE);
    ctrl_byte2_->setValue((mrs_natural)byte2, NOUPDATE);
    ctrl_byte3_->setValue((mrs_natural)byte3, NOUPDATE);

}
