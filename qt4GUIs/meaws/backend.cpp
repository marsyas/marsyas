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
//using namespace Marsyas;
#include <iostream>
using namespace std;

MarBackend::MarBackend(int testingMethod) {
	method=testingMethod;
	if (method==1) {
		cout<<"Setting up Intonation's String stuff"<<endl;
		startIntonation();
	}
	if (method==2) {
		cout<<"Setting up Control's Wind stuff"<<endl;
		startControl();
	}
}

MarBackend::~MarBackend() {
	cout<<"deleting"<<endl;
	if (method==1) {
		cout<<"doing Intonation stuff"<<endl;
//		mrsWrapper->pause();
//		cout<<"mrsWrapper paused"<<endl;

// something bad happens here.  :(
		delete mrsWrapper;
		cout<<"mrsWrapper deleted"<<endl;
		delete metroNet;
		cout<<"metroNet deleted"<<endl;
	}
}

void MarBackend::makeRecNet() {
  MarSystemManager mng;

  recNet = mng.create("Series", "recNet");
  recNet->addMarSystem(mng.create("AudioSource", "srcRec"));
	recNet->addMarSystem(mng.create("SoundFileSink","destRec"));

	recNet->updctrl("AudioSource/src/mrs_real/israte", 44100.0);
  recNet->updctrl("AudioSource/src/mrs_bool/initAudio", true);
	recNet->updctrl("SoundFileSink/dest/mrs_string/filename","test-rec.wav");
}

void MarBackend::startIntonation() {
	makeRecNet();

 // MarSystemManager mng;

 // allNet = mng.create("Series", "all");

//	MarSystem *parallel = mng.create("Parallel", "para");
//	parallel->addMarSystem(recNet);
//	parallel->addMarSystem(metroNet);
//	allNet->addMarSystem(parallel);

	mrsWrapper = new MarSystemQtWrapper(recNet);
	mrsWrapper->start();

	mrsWrapper->pause();
}

void MarBackend::startControl() {

}

void MarBackend::setTempo(float timeBetweenBeats) {
	cout<<"setTempo "<<timeBetweenBeats<<endl;
//  e->set_repeat(Repeat( dtos(timeBetweenBeats)+"s" ));
}
/*
void MarBackend::startMarBackend() {
	cout<<"play"<<endl;
	mrsWrapper->play();
}

void MarBackend::stopMarBackend() {
	cout<<"stop"<<endl;
	mrsWrapper->pause();
}

void MarBackend::setIntro(int beats) {
	cout<<beats<<" beats intro"<<endl;
	introBeats = beats;
}

void MarBackend::playBeat() {
	cout<<"BEAT"<<endl;
//	metroNet->updctrl("SoundFileSource/src/mrs_natural/pos",0);
}
*/
