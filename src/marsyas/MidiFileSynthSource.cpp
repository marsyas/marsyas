/*
 ** Copyright (C) 1998-2008 George Tzanetakis <gtzan@cs.uvic.ca>
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

#include "MidiFileSynthSource.h"

using namespace std;
using namespace Marsyas;

MidiFileSynthSource::MidiFileSynthSource(string name):MarSystem("MidiFileSynthSource",name)
{
	addControls();
	filename_ = "defaultfile";
	nChannels_ = 0;
	size_ = 0;
	frameCount_ = 0;
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
	frameCount_ = a.frameCount_;
	
	ctrl_filename_ = getctrl("mrs_string/filename");
	ctrl_nActiveNotes_ = getctrl("mrs_natural/nActiveNotes");
	ctrl_nChannels_ = getctrl("mrs_natural/nChannels");
	ctrl_pos_ = getctrl("mrs_natural/pos");
	ctrl_notEmpty_ = getctrl("mrs_bool/notEmpty");
	
	ctrl_start_ = getctrl("mrs_real/start");
	ctrl_end_ = getctrl("mrs_real/end");
}

void
MidiFileSynthSource::addControls()
{
	addctrl("mrs_string/filename", "defaultfile", ctrl_filename_);
	setctrlState("mrs_string/filename", true);
	
	addctrl("mrs_natural/nActiveNotes", 0, ctrl_nActiveNotes_);
	addctrl("mrs_natural/nChannels", 0, ctrl_nChannels_);
	
	addctrl("mrs_natural/pos", 0, ctrl_pos_);
	setctrlState("mrs_natural/pos", true);
	
	addctrl("mrs_bool/notEmpty", true, ctrl_notEmpty_); 
	
	addctrl("mrs_real/start", 0.0, ctrl_start_);
	addctrl("mrs_real/end", 0.0, ctrl_end_);
}

void
MidiFileSynthSource::myUpdate(MarControlPtr sender)
{
	(void) sender;
	MRSDIAG("MidiFileSynthSource::myUpdate");
	
	if(filename_ != ctrl_filename_->to<mrs_string>())
	{
		filename_ = ctrl_filename_->to<mrs_string>();
		MATLAB_PUT(filename_, "filename");
		MATLAB_PUT(ctrl_israte_->to<mrs_real>(), "fs");
		MATLAB_PUT(ctrl_inSamples_->to<mrs_natural>(), "frameSize");
		MATLAB_PUT(ctrl_start_->to<mrs_real>(), "startSeg");
		MATLAB_PUT(ctrl_end_->to<mrs_real>(), "endSeg");
		
		//Synthesize MIDI file to audio
		MATLAB_EVAL("MIDIsynth;");
		
		//get stuff from MATLAB
		MATLAB_GET("nChannels", nChannels_);
		ctrl_nChannels_->setValue(nChannels_, NOUPDATE);
		MATLAB_GET("activeNotes", activeNotes_);
		MATLAB_GET("AUDIOout", audio_);
				
		//clear MATLAB objects (not needed anymore --> save some memory)
		MATLAB_EVAL("clear;");
		
		size_ = audio_.getCols();
		
		ctrl_pos_->setValue(0, NOUPDATE);
		if(size_>0)
			ctrl_notEmpty_->setValue(true, NOUPDATE);
		else
			ctrl_notEmpty_->setValue(false, NOUPDATE);
		
		frameCount_ = 0;
	}
	
	ctrl_onSamples_->setValue(ctrl_inSamples_, NOUPDATE);
	ctrl_onObservations_->setValue(nChannels_+1, NOUPDATE);
	ctrl_osrate_->setValue(ctrl_israte_, NOUPDATE);
	ostringstream oss;
	oss << "AudioMIDImix" << ",";
	for (mrs_natural ch = 0; nChannels_; ch++) 
	{
		oss << "AudioMIDIch" << ch << ",";
	}
	ctrl_onObsNames_->setValue(oss.str(), NOUPDATE);
}


void
MidiFileSynthSource::myProcess(realvec& in, realvec &out)
{
	mrs_natural pos = ctrl_pos_->to<mrs_natural>();
	
	if(frameCount_ >= activeNotes_.getCols() ||
		pos > size_)
	{
		ctrl_notEmpty_->setValue(false);
		return;
	}
			
	for(mrs_natural c=0; c < onObservations_; ++c)
	{
		for(mrs_natural i = 0; i < onSamples_; ++i)
		{
			if((pos + i) < size_)
				out(c,i) = audio_(c, pos+i);
			else
				out(c,i) = 0.0; //no more audio data... fill with silence
				
		}
	}
	
	ctrl_pos_->setValue(pos+onSamples_);
			
	ctrl_nActiveNotes_->setValue(activeNotes_(frameCount_++));
}



