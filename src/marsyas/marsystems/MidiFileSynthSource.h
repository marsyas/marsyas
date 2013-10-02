/*
** Copyright (C) 1998-2010 George Tzanetakis <gtzan@cs.uvic.ca>
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

#ifndef MARSYAS_MIDIFILESYNTHSOURCE_H
#define MARSYAS_MIDIFILESYNTHSOURCE_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
   \class MidiFileSynthSource
   \ingroup IO
   \brief Reads a MIDI file and synthesizes audio (needs MATLAB and
   the MIDI Toolbox)

   MidiFileSynthSource makes use of the MIDI Toolbox for MATLAB
   (see http://www.jyu.fi/hum/laitokset/musiikki/en/research/coe/materials/miditoolbox/ )
   using the MATLABengine. It takes a midi file and synthesizes the
   corresponding audio. Both the mixed audio and each MIDI
   channel audio is output (the mixed audio is the first observation, while
   the following observations have the audio of each MIDI track, individually
   (this may be usefull for ground truth purposes). For each output audio frame
   is also returned the number of playing MIDI notes (computed from the midi
   data).


   Controls:
   - \b mrs_string/filename [w] : name of the MIDI file to read and synthesize
   - \b mrs_natural/numActiveNotes [r] : number of playing notes at the current
   audio frame sent to the output.
   - \b mrs_natural/nChannels [r] : number of MIDI tracks in the MIDI file
   (also corresponds to the number of individual audio tracks output togheter
   with the mixed audio)
   - \b mrs_natural/pos [r]: current playing position (in samples)
   - \b mrs_bool/hasData [r] : when false, signals the end of the MIDI file
   - \b mrs_real/start [w] : sets the start of the segment of the MIDI file
   to be synthesized (in seconds).
   - \b mrs_real/end [w] : sets the end of the segment of the MIDI file
   to be synthesized (in seconds).
   - \b mrs_natural/winSize [w]: when using overlapping analysis windows (which usually
   make use of a ShiftInput MarSystem somewhere further down in the processing network)
   this control allows to specify the final analysis window size (since inSamples will
   correspond to the hop size). This is important to get a correct estimation of the
   number of active notes in each analysis window (although only outputting the corresponding
   hopsize number of audio samples).
   - \b mrs_bool/sigNewTextWin [w]: sets mode of operation: if true, this MarSystem
   will use MIDI note onsets to signal texture windows, setting the newTextWin control
   to true. If set to false, it will accept signals for new texture windows from an outside
   source (e.g. an onset detector, etc).
   - \b mrs_bool/newTextWin [rw]: depending on the specified mode (see above), this control
   will signal a new texture window.
*/


class MidiFileSynthSource: public MarSystem
{
private:
  std::string filename_;
  mrs_natural nChannels_;
  realvec activeNotes_;
  mrs_natural size_;

  MarControlPtr ctrl_filename_;
  MarControlPtr ctrl_numActiveNotes_;
  MarControlPtr ctrl_nChannels_;
  MarControlPtr ctrl_start_;
  MarControlPtr ctrl_end_;
  MarControlPtr ctrl_pos_;
  MarControlPtr ctrl_hasData_;
  MarControlPtr ctrl_winSize_;
  MarControlPtr ctrl_sigNewTextWin_;
  MarControlPtr ctrl_newTextWin_;
  MarControlPtr ctrl_size_;

  void addControls();
  void myUpdate(MarControlPtr sender);

public:
  MidiFileSynthSource(std::string name);
  MidiFileSynthSource(const MidiFileSynthSource& a);

  ~MidiFileSynthSource();
  MarSystem* clone() const;

  virtual void myProcess(realvec& in,realvec& out);
};

}//namespace Marsyas

#endif /* !MARSYAS_MIDIFILESYNTHSOURCE_H */


