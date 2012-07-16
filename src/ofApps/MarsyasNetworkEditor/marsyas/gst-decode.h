/*
    Copyright (c) 2008 Soren Harward <stharward@gmail.com>

    Pretty much all of this code is pulled right out of Mirage decoder, by
    Dominik Schnitzer <dominik@schnitzer.at>, which itself is almost entirely
    derived from the "decodebin" example in the GStreamer documentation.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "config.h"

#ifdef MARSYAS_GSTREAMER

#ifndef GST_DECODE_H
#define GST_DECODE_H

#include <gst/gst.h>
#include <glib.h>

typedef double mrs_real;

typedef struct {
    GList* frameList;
    GList* frameSize;
    unsigned int frameCount;
    unsigned int totalSize;
} gstreamerFrames;

typedef struct {
    mrs_real* data;
    unsigned int size;
} audioVector;

static void cb_newpad (GstElement*, GstPad*, gboolean, GstElement*);
void handoff (GstElement*, GstBuffer*, gstreamerFrames*);
GstElement* gstreamer_init(const gchar*, gstreamerFrames*);
void gstreamer_cleanup(GstElement*);
audioVector gst_decode_file(const gchar*);

#endif

#endif //MARSYAS_GSTREAMER
