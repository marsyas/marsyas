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

#ifndef MARSYAS_RADIODRUMINPUT_H
#define MARSYAS_RADIODRUMINPUT_H

#include <marsyas/system/MarSystem.h>

class RtMidiIn;


namespace Marsyas
{
/**
\ingroup MIDI
\brief Special MIDIInput to address the radio drum control structure

Controls:
- \b mrs_natural/port [rw] : input midi port
- \b mrs_bool/virtualPort [rw] : is this a virtual MIDI port?
- \b mrs_bool/initmidi [w] : enable midi (should be \em true )
*/

class RadioDrumInput: public MarSystem
{
private:
  void myUpdate(MarControlPtr sender);
  void addControls();
  static void mycallback(double deltatime, std::vector< unsigned char> * message, void *userData);



  RtMidiIn* rdin;

  int byte3, byte2, byte1;
  int rightstickx,rightsticky,rightstickz,leftstickx,leftsticky,leftstickz;
  bool initMidi;
  MarControlPtr rx_;
  MarControlPtr ry_;
  MarControlPtr rz_;
  MarControlPtr lx_;
  MarControlPtr ly_;
  MarControlPtr lz_;

public:
  RadioDrumInput(std::string name);
  RadioDrumInput(const RadioDrumInput& a);
  ~RadioDrumInput();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif
