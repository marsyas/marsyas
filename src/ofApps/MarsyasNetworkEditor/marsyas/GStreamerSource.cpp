/*
** Copyright (C) 2008-2010 George Tzanetakis <gtzan@cs.uvic.ca>
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

/* 
   Author Soren Harward <stharward@gmail.com> 2008
*/

#include "common.h" 
#include "GStreamerSource.h"


#ifdef MARSYAS_GSTREAMER
#include "gst-decode.h"
#endif //MARSYAS_GSTREAMER

using std::ostringstream;
using namespace Marsyas;

GStreamerSource::GStreamerSource(mrs_string name):AbsSoundFileSource("GStreamerSource", name)
{
    data_ = NULL;
    phaseOffset_ = 0.0;
    hasData_ = false;
    addControls();
}

//TODO: Copy Constructor

GStreamerSource::~GStreamerSource() 
{
    delete data_; // this memory got allocated by gst_decode_file
}

MarSystem* GStreamerSource::clone() const
{
    return new GStreamerSource(*this);
}

void
GStreamerSource::addControls() 
{
    addctrl("mrs_natural/nChannels",2);
    addctrl("mrs_real/frequency",44100.0);
    setctrlState("mrs_real/frequency",true);
    addctrl("mrs_natural/size", 0);
    addctrl("mrs_natural/pos", 0);
    setctrlState("mrs_natural/pos", true);
    addctrl("mrs_string/filename", "gst-source");
    setctrlState("mrs_string/filename", true);
    addctrl("mrs_bool/hasData", true);
    addctrl("mrs_bool/noteon", false);
    setctrlState("mrs_bool/noteon", true);
    addctrl("mrs_string/filetype", "raw");
}    
void GStreamerSource::getHeader(mrs_string fileName)
{
#ifdef MARSYAS_GSTREAMER
    audioVector result = gst_decode_file((gchar*)fileName.c_str());

    data_ = (mrs_real*)result.data;
    fileSize_ = (mrs_natural)result.size;
    if (fileSize_ > 0) {
        hasData_ = false;
    }
    sampleCount_ = fileSize_ / sizeof(mrs_natural);
#endif 
}

void GStreamerSource::myUpdate(MarControlPtr sender) 
{
    (void) sender;
 
    //nChannels_ = getctrl("mrs_natural/nChannels")->to<mrs_natural>();  
    inSamples_ = getctrl("mrs_natural/inSamples")->to<mrs_natural>();
    inObservations_ = getctrl("mrs_natural/inObservations")->to<mrs_natural>();
    israte_ = getctrl("mrs_real/israte")->to<mrs_real>();
  
    setctrl("mrs_natural/onSamples", inSamples_);
    setctrl("mrs_natural/onObservations", inObservations_);
    setctrl("mrs_real/osrate", israte_);
  
    filename_ = getctrl("mrs_string/filename")->to<mrs_string>();    
    pos_ = getctrl("mrs_natural/pos")->to<mrs_natural>();

    rate_ = fileSize_ * getctrl("mrs_real/frequency")->to<mrs_real>() / israte_;
}    


void GStreamerSource::myProcess(realvec& in,realvec &out)
{
    MRSMSG("Retrieving data from GStreamerSource\n");
    (void) in;

    mrs_natural i;
    mrs_natural index = pos_;
  
    if (!getctrl("mrs_bool/noteon")->isTrue()) {
        return;
    }

    if (!hasData_) {
        return;
    }

    for (i = 0; i < inSamples_; ++i ) {
        // TODO: looping?
        if (index >= sampleCount_) {
            out(0,i) = 0.0;
        } else {
            out(0,i) = data_[index++];
        }
    }
}
