/*
** Copyright (C) 2000-2010 George Tzanetakis <gtzan@cs.uvic.ca>
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


#include "common-reg.h"

// really useful global functions
mrs_real
addSource(MarSystem* net, string infile)
{
  if (infile == EMPTYSTRING) {
    cout << "Please specify a sound file." << endl;
    exit(1);
  }

  net->addMarSystem(mng.create("SoundFileSource", "src"));
  net->updctrl("SoundFileSource/src/mrs_string/filename", infile);
  net->linkControl("mrs_bool/hasData",
                   "SoundFileSource/src/mrs_bool/hasData");
  return net->getctrl("SoundFileSource/src/mrs_real/osrate")->to<mrs_real>();
}

void
addDest(MarSystem* net, string outfile)
{
  if (outfile == EMPTYSTRING)
  {
    net->addMarSystem(mng.create("AudioSink", "dest"));
    net->updctrl("AudioSink/dest/mrs_bool/initAudio", true);
  }
  else
  {
    net->addMarSystem(mng.create("SoundFileSink", "dest"));
    net->updctrl("SoundFileSink/dest/mrs_string/filename", outfile);
  }
}

