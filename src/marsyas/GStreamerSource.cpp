/*
** Copyright (C) 2012 Nate Bogdanowicz <natezb@gmail.com>
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

// TODO Set up a proper GstClock so the whole file isn't instantly queued up

#include "common.h" 
#include "common_header.h"



#ifdef MARSYAS_GSTREAMER
#include "GStreamerSource.h"
#include <glib.h>
#include <gst/gst.h>
#include <gst/app/gstappsink.h>
#include <gst/base/gstbasesrc.h>
#endif MARSYAS_GSTREAMER


#ifdef MARSYAS_GSTREAMER

using namespace Marsyas;


/* This must be a free-standing function (i.e. not a member of a class), since
 * GStreamer is written in plain C. */
void
on_pad_added(GstElement *dec, GstPad *pad, GstElement *element)
{
	(void) dec;
	GstPad *sinkpad;

	sinkpad = gst_element_get_static_pad(element, "sink");
	if(!gst_pad_is_linked(sinkpad)) {
		if(gst_pad_link(pad, sinkpad) != GST_PAD_LINK_OK) {
			MRSERR("Failed to link pads");
		}
	}

	gst_object_unref(sinkpad);
}


GStreamerSource::GStreamerSource(mrs_string name):AbsSoundFileSource("GStreamerSource", name)
{
    size_ = 0;
    pos_ = 0;
    hasData_ = false;
    playing_ = false;
    buffer_size_ = 0;
    buffer_left_ = 0;
    pipe_created_ = false;

    addControls();
}


//TODO: Copy Constructor
GStreamerSource::~GStreamerSource() 
{
	/* Unref the objects we explicitly un-floated */
	gst_object_unref(pipe_);
	gst_object_unref(dec_);
	gst_object_unref(sink_);
}


MarSystem* GStreamerSource::clone() const
{
    return new GStreamerSource(*this);
}


void
GStreamerSource::addControls() 
{
    addctrl("mrs_natural/size", 0);
    addctrl("mrs_natural/pos", 0);
    setctrlState("mrs_natural/pos", true);

    addctrl("mrs_string/filename", "gst-source");
    setctrlState("mrs_string/filename", true);

    addctrl("mrs_bool/hasData", true);
    addctrl("mrs_bool/noteon", false);
    setctrlState("mrs_bool/noteon", true);

    addctrl("mrs_string/filetype", "gst");

    addctrl("mrs_string/currentlyPlaying", "gst-source", ctrl_currentlyPlaying_);
    addctrl("mrs_string/previouslyPlaying", "gst-source", ctrl_previouslyPlaying_);
    addctrl("mrs_real/currentLabel", 0.0, ctrl_currentLabel_);
    addctrl("mrs_real/previousLabel", 0.0, ctrl_previousLabel_);
    addctrl("mrs_natural/nLabels", 0, ctrl_nLabels_);
    addctrl("mrs_string/labelNames", ",", ctrl_labelNames_);
    addctrl("mrs_bool/regression", false, ctrl_regression_);

    addctrl("mrs_bool/lastTickWithData", false);

    addctrl("mrs_real/repetitions", 1.0);
    setctrlState("mrs_real/repetitions", true);

    addctrl("mrs_bool/shuffle", false);
    setctrlState("mrs_bool/shuffle", true);

    addctrl("mrs_real/duration", -1.0);
    setctrlState("mrs_real/duration", true);

    addctrl("mrs_natural/loopPos", (mrs_natural)0);
    setctrlState("mrs_natural/loopPos", true);

    addctrl("mrs_natural/advance", 0);
    setctrlState("mrs_natural/advance", true);

    addctrl("mrs_natural/cindex", 0);
    setctrlState("mrs_natural/cindex", true);

    addctrl("mrs_string/allfilenames", ",");
    addctrl("mrs_natural/numFiles", 1);

    addctrl("mrs_bool/currentHasData", true, ctrl_currentHasData_);
    addctrl("mrs_bool/currentLastTickWithData", false, ctrl_currentLastTickWithData_);
} 


void
GStreamerSource::init_pipeline()
{
	GstElement *conv;

	/* Initialize GStreamer */
	gst_init(0, NULL);

	/* Create pipeline and elements */
	pipe_ = gst_pipeline_new("pipeline");
	dec_ = gst_element_factory_make("uridecodebin", NULL);
	conv = gst_element_factory_make("audioconvert", NULL);
	sink_ = gst_element_factory_make("appsink", NULL);

	/* Unfloat floating references we wish to access later */
	gst_object_ref_sink(pipe_);
	gst_object_ref_sink(dec_);
	gst_object_ref_sink(sink_);

	/* Configure pipeline and elements */
	gst_pipeline_use_clock(GST_PIPELINE(pipe_), NULL);

	/* Set up the capabilities filtering */
	GstCaps *caps = gst_caps_new_simple("audio/x-raw-float",
					    "width", G_TYPE_INT, sizeof(mrs_real)*8,
					    NULL);
	gst_app_sink_set_caps(GST_APP_SINK(sink_), caps);
	gst_caps_unref(caps);

	/* Add everything to the pipeline and link up static pads */
	gst_bin_add_many(GST_BIN(pipe_), dec_, conv, sink_, NULL);
	gst_element_link(conv, sink_);

	/* Set up callback to link the decoder's dynamic pad to the converter */
	g_signal_connect(dec_, "pad_added", G_CALLBACK(on_pad_added), conv);

	pipe_created_ = true;
}


void
GStreamerSource::getHeader(mrs_string filename)
{
	if(!pipe_created_) {
		init_pipeline();
	}

	/* (Re)Set the uridecodebin's filename */
	gst_element_set_state(pipe_, GST_STATE_NULL);
	if(gst_uri_is_valid(filename.c_str())) {
		/* "filename" is already a valid uri, just use it as-is */
		g_object_set(G_OBJECT(dec_), "uri", filename.c_str(), NULL);
	} else {
		g_object_set(G_OBJECT(dec_), "uri", gst_filename_to_uri(filename.c_str(), NULL), NULL);
	}

	/* Set to PAUSED so we can get a preroll buffer */
	gst_element_set_state(pipe_, GST_STATE_PAUSED);
	buffer_ = gst_app_sink_pull_preroll(GST_APP_SINK(sink_));

	/* Grab the buffer's caps so we can get some useful info */
	GstCaps *buf_caps = gst_buffer_get_caps(buffer_);
	GstStructure *buf_struct = gst_caps_get_structure(buf_caps, 0);
	
	/* Get sample rate from buffer */
	gint rate;
	if(gst_structure_get_int(buf_struct, "rate", &rate)) {
		osrate_ = (mrs_real)rate;
	}

	/* Get number of channels from buffer */
	gint channels;
	if(gst_structure_get_int(buf_struct, "channels", &channels)) {
		onObservations_ = channels;
	}

	/* Get the rough size of the audio */
	gint64 duration;
	GstFormat format = GST_FORMAT_BYTES;

	if(gst_element_query_duration(GST_ELEMENT(pipe_), &format, &duration) && format==GST_FORMAT_BYTES) {
		size_ = duration / sizeof(mrs_real) / onObservations_;
	} else {
		/* GStreamer can't tell us the size of the stream */
		size_ = -1;
	}

	/* Reset the position in case this isn't our first song */
	pos_ = 0;
	
	/* Clean up */
	gst_caps_unref(buf_caps);

	/* Set Controls */
	setctrl("mrs_real/israte", osrate_);
    	setctrl("mrs_real/osrate", osrate_);
	setctrl("mrs_natural/onObservations", onObservations_);
	setctrl("mrs_natural/inObservations", onObservations_);
	setctrl("mrs_natural/size", size_);
	setctrl("mrs_natural/pos", pos_);
	
	/* Start playing, so queue fills with buffers [Should we do this?] */
	gst_element_set_state(pipe_, GST_STATE_PLAYING);
	playing_ = true; // Should we check if set_state worked before doing this?
}


void
GStreamerSource::copyFromBuffer(GstBuffer    *buf,
				mrs_natural  buf_start,
				realvec&     vec,
				mrs_natural  vec_start,
				mrs_natural  length)
{
	mrs_natural i, ch;
	for(ch = 0; ch < onObservations_; ch++) {
		for(i = 0; i < length; i++) {
			mrs_real sample;
			memcpy(&sample, GST_BUFFER_DATA(buf) + ((buf_start + i)*onObservations_ + ch)*sizeof(mrs_real), sizeof(mrs_real));
			vec(ch, vec_start + i) = sample;
		}
	}

}


mrs_bool
GStreamerSource::pull_buffer() {
	buffer_ = gst_app_sink_pull_buffer(GST_APP_SINK(sink_));

	if(buffer_ == NULL) {
		if(gst_app_sink_is_eos(GST_APP_SINK(sink_))) {
			/* EOS, stop pulling buffers */
			hasData_ = false;
			lastTickWithData_ = true;
			
			ctrl_currentHasData_->setValue(hasData_);
			ctrl_currentLastTickWithData_->setValue(lastTickWithData_);
			playing_ = false;
			return false;
		} else {
			/* Something messed up. Should we try pulling again? */
			MRSWARN("GStreamer Error");
			return false;
		}
	} else {
		buffer_size_ = GST_BUFFER_SIZE(buffer_) / sizeof(mrs_real) / onObservations_;
		buffer_left_ = buffer_size_;
	}

	return true;
}


void
GStreamerSource::myProcess(realvec& in, realvec& out)
{
	(void)in;

	// TODO Should output_size be calculated from output.getCols() or in/onSamples_?
	mrs_natural output_size = inSamples_;
	mrs_natural output_left = output_size;
	//mrs_real *output = out.getData();

	while(output_left > 0) {
		if(buffer_left_ == 0) {
			/* Current buffer empty, so pull a new one */
			if(!pull_buffer()) {
				break;
			}
		}

		if(buffer_left_ >= output_left) {
			/* Copy enough to fill the output */
			/* GSt_BUFFER_DATA() returns guint8*, i.e. a byte-pointer, while output it a double-pointer */
			// memcpy(output + output_size - output_left,
			//        GST_BUFFER_DATA(buffer_) + (buffer_size_ - buffer_left_)*sizeof(mrs_real),
			//        output_left*sizeof(mrs_real));
			copyFromBuffer(buffer_, buffer_size_-buffer_left_, out, output_size-output_left, output_left);
			buffer_left_ -= output_left;
			output_left = 0;
		} else {
			/* Copy what's left in the buffer */
			// memcpy(output + (output_size - output_left),
			//        GST_BUFFER_DATA(buffer_) + (buffer_size_ - buffer_left_)*sizeof(mrs_real),
			//        buffer_left_*sizeof(mrs_real));
			copyFromBuffer(buffer_, buffer_size_-buffer_left_, out, output_size-output_left, buffer_left_);
			output_left -= buffer_left_;
			buffer_left_ = 0;
		}

		if(buffer_left_ == 0 && buffer_ != NULL) {
			/* Buffer empty, unref it */
			gst_buffer_unref(buffer_);
		}
	}
	
	// /* Make sure unfilled output is zeroed (TODO is this necessary?) */
	// mrs_natural i;
	// for(i = output_size-1; i >= output_size-output_left; --i) {
	// 	out(0,i) = 0.0;
	// }

	/* Update current position */
	pos_ += output_size - output_left;
	setctrl("mrs_natural/pos", pos_);
}


mrs_bool
GStreamerSource::seek()
{
	mrs_natural cur_pos = tpos_;

	if(pos_ < cur_pos) {
		/* Can't seek backwards */
		return false;
	}

	while(cur_pos + buffer_left_ < pos_) {
		cur_pos += buffer_left_;

		if(!pull_buffer()) {
			/* Hit EOS or error before reaching desired position */
			return false;
		}
		
	}

	/* The current buffer contains the sample at pos_ */
	buffer_left_ -= (pos_ - cur_pos);

	return true;
}


void
GStreamerSource::myUpdate(MarControlPtr sender) 
{
	(void) sender;
 
	// This stuff is already done in MarSystem::update()
	// inSamples_ = getctrl("mrs_natural/inSamples")->to<mrs_natural>();
	// inObservations_ = getctrl("mrs_natural/inObservations")->to<mrs_natural>();
	// israte_ = getctrl("mrs_real/israte")->to<mrs_real>();
	
	/* This seems to be the convention, to ignore/overwrite the onSamples control */
	setctrl("mrs_natural/onSamples", inSamples_);
	
	tfilename_ = filename_;
	filename_ = getctrl("mrs_string/filename")->to<mrs_string>();

	if(g_strcmp0(tfilename_.c_str(), filename_.c_str()) != 0) {

	}

	/* Only deal with pos_ if there's a playing stream */
	if(playing_) {
		tpos_ = pos_;
		pos_ = getctrl("mrs_natural/pos")->to<mrs_natural>();

		if(pos_ != tpos_) {
			/* Position control was written to, try to seek */
			seek();
		}

		/* Write current (possibly updated) position back to the control */
		setctrl("mrs_natural/pos", pos_);
	}
}    


#endif //MARSYAS_GSTREAMER
