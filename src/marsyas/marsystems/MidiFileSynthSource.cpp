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

#include "../common_source.h"
#include "MidiFileSynthSource.h"

using std::ostringstream;
using namespace Marsyas;

MidiFileSynthSource::MidiFileSynthSource(mrs_string name):MarSystem("MidiFileSynthSource",name)
{
  addControls();
  nChannels_ = 0;
  size_ = 0;
}

MidiFileSynthSource::~MidiFileSynthSource()
{

}

MarSystem*
MidiFileSynthSource::clone() const
{
  return new MidiFileSynthSource(*this);
}

MidiFileSynthSource::MidiFileSynthSource(const MidiFileSynthSource& a):MarSystem(a)
{
  filename_ = a.filename_;
  nChannels_ = a.nChannels_;
  size_ = a.size_;

  ctrl_filename_ = getctrl("mrs_string/filename");
  ctrl_numActiveNotes_ = getctrl("mrs_natural/numActiveNotes");
  ctrl_nChannels_ = getctrl("mrs_natural/nChannels");
  ctrl_pos_ = getctrl("mrs_natural/pos");
  ctrl_hasData_ = getctrl("mrs_bool/hasData");

  ctrl_start_ = getctrl("mrs_real/start");
  ctrl_end_ = getctrl("mrs_real/end");

  ctrl_winSize_ = getctrl("mrs_natural/winSize");

  ctrl_sigNewTextWin_ = getctrl("mrs_bool/sigNewTextWin");
  ctrl_newTextWin_ = getctrl("mrs_bool/newTextWin");

  ctrl_size_ = getctrl("mrs_natural/size");
}

void
MidiFileSynthSource::addControls()
{
  addctrl("mrs_string/filename", "", ctrl_filename_);
  setctrlState("mrs_string/filename", true);

  addctrl("mrs_natural/numActiveNotes", 0, ctrl_numActiveNotes_);

  addctrl("mrs_natural/nChannels", 0, ctrl_nChannels_);

  addctrl("mrs_natural/pos", 0, ctrl_pos_);

  addctrl("mrs_bool/hasData", true, ctrl_hasData_);

  addctrl("mrs_real/start", 0.0, ctrl_start_);
  addctrl("mrs_real/end", 0.0, ctrl_end_);

  addctrl("mrs_natural/winSize", MRS_DEFAULT_SLICE_NSAMPLES, ctrl_winSize_);
  ctrl_winSize_->setState(true);

  addctrl("mrs_bool/sigNewTextWin", true, ctrl_sigNewTextWin_);
  ctrl_sigNewTextWin_->setState(true);

  addctrl("mrs_bool/newTextWin", false, ctrl_newTextWin_);
  ctrl_newTextWin_->setState(true);

  addctrl("mrs_natural/size", 0, ctrl_size_);
}

void
MidiFileSynthSource::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)
  MRSDIAG("MidiFileSynthSource::myUpdate");

  MATLAB_PUT(ctrl_winSize_->to<mrs_natural>(), "winSize");
  MATLAB_PUT(inSamples_, "hopSize");

  mrs_bool sigNewTextWin = ctrl_sigNewTextWin_->to<mrs_bool>();
  MATLAB_PUT((mrs_natural)sigNewTextWin, "sigNewTextWin");
  //in case texture windows are set from the outside...
  if(!sigNewTextWin)
  {
    if(ctrl_newTextWin_->isTrue())
    {
      //get number of playing notes in texture window
      mrs_natural numActiveNotes;
      MATLAB_GET("numActiveNotes", numActiveNotes);
      ctrl_numActiveNotes_->setValue(numActiveNotes);

      MATLAB_EVAL("textWinStart = textWinEnd+1;textWinEnd = endPos2;");

      //reset MATLAB
      //MATLAB_EVAL("numActiveNotes = 0;");

      //reset control
      ctrl_newTextWin_->setValue(false, NOUPDATE);
    }
  }

  if(filename_ != ctrl_filename_->to<mrs_string>())
  {
    filename_ = ctrl_filename_->to<mrs_string>();
    MATLAB_PUT(filename_, "filename");
    MATLAB_PUT(israte_, "fs");
    MATLAB_PUT(ctrl_start_->to<mrs_real>(), "startSeg");
    MATLAB_PUT(ctrl_end_->to<mrs_real>(), "endSeg");

    //Synthesize MIDI file to audio
    MATLAB_EVAL("synthetizeMIDI;");

    MATLAB_GET("numChannels", nChannels_);
    ctrl_nChannels_->setValue(nChannels_, NOUPDATE);

    MATLAB_GET("audioLength", size_); //in samples
    ctrl_size_->setValue(size_, NOUPDATE);

    ctrl_pos_->setValue(0, NOUPDATE);
    if(size_>0)
      ctrl_hasData_->setValue(true, NOUPDATE);
    else
      ctrl_hasData_->setValue(false, NOUPDATE);
  }

  ctrl_onSamples_->setValue(ctrl_inSamples_, NOUPDATE);
  ctrl_onObservations_->setValue(1, NOUPDATE);
  ctrl_osrate_->setValue(ctrl_israte_, NOUPDATE);
  ctrl_onObsNames_->setValue("MIDIaudio", NOUPDATE);
}

void
MidiFileSynthSource::myProcess(realvec& in, realvec &out)
{
  (void) in;
  if(size_ == 0)
    return;

  mrs_natural pos = ctrl_pos_->to<mrs_natural>();

  //detect end of file
  if(pos > size_)
  {
    out.setval(0.0);//fill with silence

    if(!ctrl_hasData_->isTrue())
      return;

    ctrl_hasData_->setValue(false);

    //flush last texture window
    if(ctrl_sigNewTextWin_->isTrue())
    {
      mrs_natural numActiveNotes;
      MATLAB_GET("numActiveNotes", numActiveNotes);
      ctrl_numActiveNotes_->setValue(numActiveNotes);
      ctrl_newTextWin_->setValue(true, NOUPDATE);
    }

    return;
  }

  MATLAB_PUT(pos+1, "pos");//MATLAB uses indexes in the range [1,...]
  MATLAB_EVAL("computeAudioFrame;");
  MATLAB_GET("audioFrame", out);

  ctrl_pos_->setValue(pos+onSamples_);

  //check if this audio frame is the last one in the current texture window:
  //if it is, then signal a new texture window
  if(ctrl_sigNewTextWin_->isTrue())
  {
    mrs_natural newTextWin = 0;
    MATLAB_GET("newTextWin", newTextWin);
    if(newTextWin!=0)//if true...
    {
      mrs_natural numActiveNotes;
      MATLAB_GET("numActiveNotes", numActiveNotes);
      ctrl_numActiveNotes_->setValue(numActiveNotes);
      //MATLAB_EVAL("numActiveNotes = 0;"); //reset
    }
    ctrl_newTextWin_->setValue(newTextWin!=0, NOUPDATE);
  }
}
