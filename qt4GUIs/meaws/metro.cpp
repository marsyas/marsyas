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

#include "metro.h"
//using namespace Marsyas;
#include <iostream>
using namespace std;

Metro::Metro() {
	introBeats=0;
	tempo=60;

  MarSystemManager mng;

  metroNet = mng.create("Series", "metroNet");
  metroNet->addMarSystem(mng.create("SoundFileSource", "srcMetro"));
  metroNet->addMarSystem(mng.create("AudioSink", "dest"));
	metroNet->updctrl("SoundFileSource/srcMetro/mrs_string/filename", "sd.wav");
//	metroNet->updctrl("AudioSink/src/mrs_real/israte", 44100.0);
  metroNet->updctrl("AudioSink/dest/mrs_bool/initAudio", true);

	mrsWrapper = new MarSystemQtWrapper(metroNet);
	mrsWrapper->start();
	positionPtr = mrsWrapper->getctrl("SoundFileSource/srcMetro/mrs_natural/pos");

	int timeBetweenBeats = 60000/tempo;
	timer = new QTimer();
	connect(timer, SIGNAL(timeout()), this, SLOT(beat()));
	timer->start(timeBetweenBeats);
	timer->stop();
}

Metro::~Metro() {
	delete mrsWrapper;
	delete metroNet;
}

void Metro::setTempo(int getTempo) {
	tempo = getTempo;
	float timeBetweenBeats = 60000.0f/tempo;
	cout<<"setTempo "<<timeBetweenBeats<<endl;
}

void Metro::startMetro() {
	cout<<"play"<<endl;
	timer->start();
//	mrsWrapper->play();
}

void Metro::stopMetro() {
	cout<<"stop"<<endl;
	timer->stop();
//	mrsWrapper->pause();
}

void Metro::setIntro(int beats) {
	cout<<beats<<" beats intro"<<endl;
	introBeats = beats;
}

void Metro::beat() {
	cout<<"BEAT"<<endl;
	mrsWrapper->updctrl(positionPtr, 0);
	mrsWrapper->play();
}

