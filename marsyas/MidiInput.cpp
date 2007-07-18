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
  \class MidiInput
  \ingroup MIDI
  \brief Update midi inpput using controls. Just through for dataflow. 

  Controls:
  - \b mrs_natural/port [rw] : input midi port
  - \b mrs_bool/virtualPort [rw] : is this a virtual MIDI port?
  - \b mrs_bool/initmidi [w] : enable midi (should be \em true )
 */
    
#include "MidiInput.h"

using namespace std;
using namespace Marsyas;

MidiInput::MidiInput(string name):MarSystem("MidiInput",name)
{
    //type_ = "MidiInput";
    //name_ = name;
    initMidi = false;
#ifdef MARSYAS_MIDIIO
    midiin = NULL;
#endif 
    addControls();
}

MidiInput::~MidiInput()
{
#ifdef MARSYAS_MIDIIO
    delete midiin;
#endif
}

MarSystem* MidiInput::clone() const 
{
    return new MidiInput(*this);
}

void MidiInput::addControls()
{
    addctrl("mrs_natural/port", 0);
    addctrl("mrs_bool/virtualPort", false);
    addctrl("mrs_bool/initmidi", false);
    setctrlState("mrs_bool/initmidi", true);
}

void MidiInput::myUpdate(MarControlPtr sender)
{
    MRSDIAG("MidiInput.cpp - MidiInput:myUpdate");
    // 	setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples"));
    //   setctrl("mrs_natural/onObservations", getctrl("mrs_natural/inObservations"));
    //   setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));
    //   setctrl("mrs_string/onObsNames", getctrl("mrs_string/inObsNames"));
    MarSystem::myUpdate(sender);

#ifdef MARSYAS_MIDIIO
    midiin = NULL;

    initMidi= getctrl("mrs_bool/initmidi")->toBool();
    initMidi = !initMidi;

    virtualPort = getctrl("mrs_bool/virtualPort")->toBool();

    if (!initMidi){
        try { 
            midiin = new RtMidiIn();
        } 
        catch (RtError &error) { 
            error.printMessage();
            return;
        }
        midiin->setCallback(&MidiInput::mycallback, this);
        midiin->ignoreTypes(false, false, false); 
        setctrl("mrs_bool/initmidi", false);

        if (virtualPort)
        {
            try { 
                midiin->openVirtualPort("MarsyasInput");
            }
            catch (RtError &error) 
            {
                error.printMessage();
                return;
            } 
        }
        else
        {
            try { 
                midiin->openPort(getctrl("mrs_natural/port")->toNatural());
            }
            catch (RtError &error) 
            {
                error.printMessage();
                return;
            } 
        }
    }

#endif
}

void MidiInput::mycallback(double deltatime, std::vector< unsigned char > * message, void *userData) 
{
    int nBytes = 0;
    nBytes = message->size();

    MidiInput* mythis = (MidiInput*) userData;

    if (nBytes > 0) 
    {
        if (nBytes > 2) 
        {
            mythis->byte3 = message->at(2); 
            mythis->byte2 = message->at(1);
            mythis->type = message->at(0);
        }

        if ((mythis->type == 160)&&(mythis->byte2 == 9))
        {
            mythis->rval = mythis->byte3;
        }
    }
}

void MidiInput::myProcess(realvec& in, realvec& out)
{
    //checkFlow(in,out);

    for (o=0; o < inObservations_; o++)
        for (t = 0; t < inSamples_; t++)
        {
            out(o,t) =  in(o,t);
        }
}
