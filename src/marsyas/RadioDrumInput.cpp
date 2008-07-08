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
    rx_ = getctrl("mrs_natural/rightstickx");
    ry_ = getctrl("mrs_natural/rightsticky");
    rz_ = getctrl("mrs_natural/rightstickz");
    lx_ = getctrl("mrs_natural/leftstickx");
    ly_ = getctrl("mrs_natural/leftsticky");
    lz_ = getctrl("mrs_natural/leftstickz");
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
    addctrl("mrs_natural/rightstickx", 0, rx_);
    addctrl("mrs_natural/rightsticky", 0, ry_);
    addctrl("mrs_natural/rightstickz", 0, rz_); 
    addctrl("mrs_natural/leftstickx", 0, lx_); 
    addctrl("mrs_natural/leftsticky", 0, ly_); 
    addctrl("mrs_natural/leftstickz", 0, lz_);
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

    // fix this to make more sense with specific radio drum input
    if (nBytes ==  3) 
    {

        if ( message->at(0) == 144){
            if(message->at(1) == 1)
                mythis->rightstickx = message->at(2);
            else  if ( message->at(1)==2)
                mythis->rightsticky = message->at(2);
            else  if ( message->at(1)==3)
                mythis->rightstickz = message->at(2);
            else  if ( message->at(1)==4)
                mythis->leftstickx = message->at(2);
            else  if ( message->at(1)==5)
                mythis->leftsticky = message->at(2);
            else  if ( message->at(1)==6)
                mythis->leftstickz = message->at(2);
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

    rx_->setValue((mrs_natural)rightstickx, NOUPDATE);
    ry_->setValue((mrs_natural)rightsticky, NOUPDATE);
    rz_->setValue((mrs_natural)rightstickz, NOUPDATE);
    lx_->setValue((mrs_natural)leftstickx, NOUPDATE);
    ly_->setValue((mrs_natural)leftsticky, NOUPDATE);
    lz_->setValue((mrs_natural)leftstickz, NOUPDATE);

}
