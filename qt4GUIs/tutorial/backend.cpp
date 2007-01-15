/*
** Copyright (C) 2007 Graham Percival <gperciva@uvic.ca>
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

#include "backend.h"
using namespace Marsyas;

MarBackend::MarBackend() {
// make a typical Marsyas network:
  MarSystemManager mng;
  playbacknet = mng.create("Series", "playbacknet");
  playbacknet->addMarSystem(mng.create("SoundFileSource", "src"));
  playbacknet->addMarSystem(mng.create("Gain", "gain"));
  playbacknet->addMarSystem(mng.create("AudioSink", "dest"));
  playbacknet->updctrl("AudioSink/dest/mrs_bool/initAudio", true);

// wrap it up to make it pretend to be a Qt object:
	mrsWrapper = new MarSystemWrapper(playbacknet);
	mrsWrapper->start();

// make these pointers so that we can interface with the network
// in a thread-safe manner:
  filenamePtr = mrsWrapper->getctrl("SoundFileSource/src/mrs_string/filename");
  gainPtr = mrsWrapper->getctrl("Gain/gain/mrs_real/gain");
  positionPtr = mrsWrapper->getctrl("SoundFileSource/src/mrs_natural/pos");

// demonstrates information flow:  Marsyas->Qt.
  QTimer *timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(getBackendPosition()));
  timer->start(1000);
}

MarBackend::~MarBackend() {
	mrsWrapper->wait();
	delete mrsWrapper;
	delete playbacknet;
}

void MarBackend::openBackendSoundfile(string fileName) {
	mrsWrapper->updctrl(filenamePtr,fileName);
}

void MarBackend::setBackendVolume(int vol) {
	float newGain = vol/100.0f;
	mrsWrapper->updctrl(gainPtr, newGain);
}

void MarBackend::getBackendPosition() {
	int newPos = (int) positionPtr->to<mrs_natural>();
	emit changedBackendPosition(newPos);
}

