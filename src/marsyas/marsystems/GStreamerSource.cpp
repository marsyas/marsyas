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

#include <marsyas/common_source.h>
#include <marsyas/common_header.h>
#include "GStreamerSource.h"

#include <glib.h>
#include <gst/gst.h>
#include <gst/app/gstappsink.h>


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
  lastTickWithData_ = false;
  playing_ = false;
  buffer_size_ = 0;
  buffer_left_ = 0;
  pipe_created_ = false;
  filename_ = "gst-source";

  addControls();
}

GStreamerSource::GStreamerSource(const GStreamerSource& a): AbsSoundFileSource(a)
{
  size_ = 0;
  pos_ = 0;
  hasData_ = false;
  lastTickWithData_ = false;
  playing_ = false;
  buffer_size_ = 0;
  buffer_left_ = 0;
  pipe_created_ = false;
  filename_ = "gst-source";

  ctrl_pos_ = getctrl("mrs_natural/pos");
  ctrl_currentlyPlaying_ = getctrl("mrs_string/currentlyPlaying");
  ctrl_previouslyPlaying_ = getctrl("mrs_string/previouslyPlaying");
  ctrl_regression_ = getctrl("mrs_bool/regression");
  ctrl_currentLabel_ = getctrl("mrs_real/currentLabel");
  ctrl_previousLabel_ = getctrl("mrs_real/previousLabel");
  ctrl_labelNames_ = getctrl("mrs_string/labelNames");
  ctrl_nLabels_ = getctrl("mrs_natural/nLabels");
  ctrl_currentHasData_ = getctrl("mrs_bool/currentHasData");
  ctrl_currentLastTickWithData_ = getctrl("mrs_bool/currentLastTickWithData");
  ctrl_hasData_ = getctrl("mrs_bool/hasData");
  ctrl_lastTickWithData_ = getctrl("mrs_bool/lastTickWithData");
}

GStreamerSource::~GStreamerSource()
{
  /* Clean up the pipeline and unref the objects we explicitly un-floated */
  if(pipe_ != NULL) {
    gst_element_set_state(pipe_, GST_STATE_NULL);
    gst_object_unref(pipe_);
  }

  if(dec_ != NULL)
    gst_object_unref(dec_);

  if(sink_ != NULL)
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
  addctrl("mrs_natural/pos", 0, ctrl_pos_);
  setctrlState("mrs_natural/pos", true);

  addctrl("mrs_string/filename", filename_);
  setctrlState("mrs_string/filename", true);

  addctrl("mrs_bool/hasData", true, ctrl_hasData_);
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

  addctrl("mrs_bool/lastTickWithData", false, ctrl_lastTickWithData_);

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
  if(!gst_is_initialized() && !gst_init_check(NULL, NULL, NULL)) {
    MRSERR("GStreamer could not be initialized!");
    return;
  }

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
  GstCaps *caps = gst_caps_new_simple("audio/x-raw",
                                      "format", G_TYPE_STRING, g_strdup_printf("F%luLE", sizeof(mrs_real)*8),
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
    if(!pipe_created_) {
      // Pipe creation failed
      size_ = -1;
      return;
    }
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
  GstStateChangeReturn ret = gst_element_set_state(pipe_, GST_STATE_PAUSED);
  if(ret == GST_STATE_CHANGE_FAILURE) {
    MRSERR("GStreamer pipeline failed to change state. This could be due to an invalid filename");
  }
  GstSample *sample = gst_app_sink_pull_preroll(GST_APP_SINK(sink_));

  /* Grab the sample's caps so we can get some useful info */
  GstCaps *samp_caps = gst_sample_get_caps(sample);
  GstStructure *samp_struct = gst_caps_get_structure(samp_caps, 0);

  /* Get sample rate from buffer */
  gint rate;
  if(gst_structure_get_int(samp_struct, "rate", &rate)) {
    osrate_ = (mrs_real)rate;
  }

  /* Get number of channels from buffer */
  gint channels;
  if(gst_structure_get_int(samp_struct, "channels", &channels)) {
    onObservations_ = channels;
  }

  /* Get the rough size of the audio. Use GST_FORMAT_TIME since it works with most file types */
  gint64 duration;
  GstFormat format = GST_FORMAT_TIME;

  // Force blocking on state change completion
  gst_element_get_state(pipe_, NULL, NULL, GST_SECOND);
  if (gst_element_query_duration(GST_ELEMENT(pipe_), format, &duration) && format==GST_FORMAT_TIME) {
    size_ = (mrs_natural)(duration*1e-9*osrate_ + 0.5);
  } else {
    /* GStreamer can't tell us the size of the stream */
    MRSWARN("Query Duration failed:");
    size_ = -1;
  }

  /* Reset these in case this isn't our first song */
  pos_ = 0;
  hasData_ = true;
  lastTickWithData_ = false;

  /* Clean up */
  gst_caps_unref(samp_caps);

  /* Set Controls */
  //setctrl("mrs_real/israte", osrate_);
  setctrl("mrs_real/osrate", osrate_);
  setctrl("mrs_natural/onObservations", onObservations_);
  //setctrl("mrs_natural/inObservations", onObservations_);
  setctrl("mrs_natural/size", size_);
  setctrl("mrs_natural/pos", pos_);
  ctrl_hasData_->setValue(hasData_);
  ctrl_lastTickWithData_->setValue(lastTickWithData_);
  ctrl_currentHasData_->setValue(hasData_);
  ctrl_currentLastTickWithData_->setValue(lastTickWithData_);


  /* Start playing, so queue fills with buffers [Should we do this?] */
  gst_element_set_state(pipe_, GST_STATE_PLAYING);
  playing_ = true; // Should we check if set_state worked before doing this?
}

void
GStreamerSource::copyFromBuffer(GstBuffer	*buf,
                                mrs_natural  buf_start,
                                realvec&	 vec,
                                mrs_natural  vec_start,
                                mrs_natural  length)
{
  mrs_natural i, ch;
  for(ch = 0; ch < onObservations_; ch++) {
    for(i = 0; i < length; i++) {
      mrs_real sample;
      gst_buffer_extract(buf, ((buf_start + i)*onObservations_ + ch)*sizeof(mrs_real), &sample, sizeof(mrs_real));
      vec(ch, vec_start + i) = sample;
    }
  }

}


mrs_bool
GStreamerSource::pull_buffer() {
  GstSample *sample = gst_app_sink_pull_sample(GST_APP_SINK(sink_));

  if(sample == NULL) {
    if(gst_app_sink_is_eos(GST_APP_SINK(sink_))) {
      /* EOS, stop pulling buffers */
      hasData_ = false;
      lastTickWithData_ = true;

      ctrl_hasData_->setValue(hasData_);
      ctrl_lastTickWithData_->setValue(lastTickWithData_);
      ctrl_currentHasData_->setValue(hasData_);
      ctrl_currentLastTickWithData_->setValue(lastTickWithData_);
      playing_ = false;
      return false;
    } else {
      /* Something messed up. Should we try pulling again? */
      MRSERR("GStreamer Error");
      return false;
    }
  } else {
    buffer_ = gst_sample_get_buffer(sample);
    buffer_size_ = gst_buffer_get_size(buffer_) / sizeof(mrs_real) / onObservations_;
    buffer_left_ = buffer_size_;
  }

  return true;
}


void
GStreamerSource::myProcess(realvec& in, realvec& out)
{
  (void)in; // Suppress warning from unused parameter

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
      //		GST_BUFFER_DATA(buffer_) + (buffer_size_ - buffer_left_)*sizeof(mrs_real),
      //		output_left*sizeof(mrs_real));
      copyFromBuffer(buffer_, buffer_size_-buffer_left_, out, output_size-output_left, output_left);
      buffer_left_ -= output_left;
      output_left = 0;
    } else {
      /* Copy what's left in the buffer */
      // memcpy(output + (output_size - output_left),
      //		GST_BUFFER_DATA(buffer_) + (buffer_size_ - buffer_left_)*sizeof(mrs_real),
      //		buffer_left_*sizeof(mrs_real));
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
  GstSeekFlags flags = (GstSeekFlags)(GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_KEY_UNIT);
  gint64 seek_to = (gint64)(newpos_/osrate_*1e9);
  if(seek_to >= 0) {
    gst_element_seek_simple(GST_ELEMENT(pipe_), GST_FORMAT_TIME, flags, seek_to);
  } else {
    MRSWARN("Tried to seek to negative position");
    return false;
  }

  GstStateChangeReturn res = gst_element_get_state(pipe_, NULL, NULL, GST_SECOND);
  if(res != GST_STATE_CHANGE_SUCCESS) {
    MRSERR("Seek failed!");
    return false;
  }

  gint64 cur;
  GstFormat format = GST_FORMAT_TIME;
  if(!gst_element_query_position(GST_ELEMENT(pipe_), format, &cur)) {
    MRSERR("Position query failed!");
    return false;
  }

  pos_ = cur*1e-9*osrate_;
  buffer_left_ = 0;
  return true;
}

void
GStreamerSource::myUpdate(MarControlPtr sender)
{
  (void) sender; // Suppress warning from unused parameter

  // This stuff is already done in MarSystem::update()
  //inSamples_ = getctrl("mrs_natural/inSamples")->to<mrs_natural>();
  //inObservations_ = getctrl("mrs_natural/inObservations")->to<mrs_natural>();
  //israte_ = getctrl("mrs_real/israte")->to<mrs_real>();

  /* This seems to be the convention, to ignore/overwrite the onSamples control */
  setctrl("mrs_natural/onSamples", inSamples_);

  tfilename_ = filename_;
  filename_ = getctrl("mrs_string/filename")->to<mrs_string>();

  /* Check if filename was changed */
  if(g_strcmp0(tfilename_.c_str(), filename_.c_str()) != 0) {
    getHeader(filename_);
  }

  /* Only deal with pos_ if there's a playing stream */
  if(playing_) {
    newpos_ = getctrl("mrs_natural/pos")->to<mrs_natural>();

    if(newpos_ != pos_) {
      /* Position control was written to, try to seek */
      seek();
    }

    /* Write current (possibly updated) position back to the control */
    setctrl("mrs_natural/pos", pos_);
  }
}

