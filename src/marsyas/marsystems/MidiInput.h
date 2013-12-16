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

#ifndef MARSYAS_MIDIINPUT_H
#define MARSYAS_MIDIINPUT_H

#include <marsyas/system/MarSystem.h>
#include <queue>


class RtMidiIn;


namespace Marsyas
{
/**
  \class MidiInput
  \ingroup MIDI
  \brief Update midi inpput using controls. Just through for dataflow.

  Controls:
  - \b mrs_natural/port [rw] : input midi port
  - \b mrs_bool/virtualPort [rw] : is this a virtual MIDI port?
  - \b mrs_bool/initmidi [w] : enable midi (should be \em true )
*/

class MidiInput: public MarSystem
{
private:
  void myUpdate(MarControlPtr sender);
  void addControls();
  static void mycallback(double deltatime, std::vector< unsigned char> * message, void *userData);

  RtMidiIn* midiin;

  bool initMidi;
  bool virtualPort;
  std::queue< std::vector<int> > msgQueue;
  MarControlPtr ctrl_byte1_;
  MarControlPtr ctrl_byte2_;
  MarControlPtr ctrl_byte3_;

public:
  MidiInput(std::string name);
  MidiInput(const MidiInput& a);
  ~MidiInput();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif
