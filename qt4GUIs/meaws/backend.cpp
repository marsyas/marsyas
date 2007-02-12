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
		cout<<"Setting up Graham's String stuff"<<endl;
		startGraham();
	}
	if (method==2) {
		cout<<"Setting up Mathieu's Wind stuff"<<endl;
		startMathieu();
	}
}

MarBackend::~MarBackend() {
	cout<<"deleting"<<endl;
	if (method==1) {
		cout<<"doing Graham stuff"<<endl;
// something bad happens here.  :(
		mrsWrapper->pause();
		cout<<"mrsWrapper paused"<<endl;
		delete mrsWrapper;
		cout<<"mrsWrapper deleted"<<endl;
		delete metroNet;
		cout<<"metroNet deleted"<<endl;
	}
}

void MarBackend::startGraham() {
  MarSystemManager mng;

  metroNet = mng.create("Series", "metroNet");
  metroNet->addMarSystem(mng.create("SoundFileSource", "src"));
  metroNet->addMarSystem(mng.create("AudioSink", "dest"));
  metroNet->updctrl("AudioSink/dest/mrs_bool/initAudio", true);

	mrsWrapper = new MarSystemQtWrapper(metroNet);
	mrsWrapper->start();

  filenamePtr = mrsWrapper->getctrl("SoundFileSource/src/mrs_string/filename");


  e = new EvExpr(metroNet,
//    Ex("","'time =' + Timer.time(Timer.cur) >> Stream.opn"),
      Ex( "SoundFileSource/src/mrs_string/filename >> @file", // init
         "file<<'', file<<'sd.wav'" ),  // do this stuff
      Rp("true"));    // every time
  e->set_repeat(Repeat("0.5s"));
  metroNet->updctrl(TmTime("TmSampleCount/Virtual","0s"), e);

	mrsWrapper->pause();
}

void MarBackend::startMathieu() {

}

void MarBackend::setTempo(int tempo) {
  float timeBetweenBeats = 60.0f/tempo;
	cout<<"setTempo "<<timeBetweenBeats<<endl;
  e->set_repeat(Repeat( dtos(timeBetweenBeats)+"s" ));
}

void MarBackend::startMetro() {
	cout<<"play"<<endl;
	mrsWrapper->play();
}

void MarBackend::stopMetro() {
	cout<<"stop"<<endl;
	mrsWrapper->pause();
}


