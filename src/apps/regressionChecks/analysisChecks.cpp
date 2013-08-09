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



// Analysis checks
#include "common-reg.h"

#include "Transcriber.h"

void
analysis_pitch(string infile)
{
  MarSystem* pnet = mng.create("Series", "pnet");
  // sets up SoundFileSource, links hasData, and sets srate
  mrs_real srate = addSource( pnet, infile );
  pnet->addMarSystem(Transcriber::makePitchNet(srate, 100.0));
  while (pnet->getctrl("mrs_bool/hasData")->to<mrs_bool>())
  {
    pnet->tick();
    const realvec& processedData =
      pnet->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();
    cout<<samples2hertz( processedData(1), srate)<<" ";
  }
  cout<<endl;
  delete pnet;
}


void
analysis_transcriber(string infile)
{
  realvec pitchList, ampList, boundaries;
  Transcriber::getAllFromAudio(infile, pitchList, ampList, boundaries);
  Transcriber::toMidi(pitchList);
  Transcriber::pitchSegment(pitchList, boundaries,10);
// TODO: since Transcriber amps is currently broken.  -gp
//    Transcriber::ampSegment(ampList, boundaries);
  realvec notes;
  notes = Transcriber::getNotes(pitchList, ampList, boundaries);
  cout<<notes;
}


