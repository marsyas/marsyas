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
	//cout<<"backend "<<testingMethod<<endl;
	method=testingMethod;
	if (method==TEST_INTONATION) {
		//cout<<"Setting up Intonation stuff"<<endl;
		startIntonation();
	}
	if (method==TEST_CONTROL) {
//		cout<<"Setting up Control stuff"<<endl;
		//startControl();
// currently no difference.
		startIntonation();
	}
}

MarBackend::~MarBackend() {
	if (method==TEST_INTONATION) {
		delete mrsWrapper;
		delete recNet;
	}
	if (method==TEST_CONTROL) {

	}
}

void MarBackend::makeRecNet() {
  MarSystemManager mng;

  recNet = mng.create("Series", "recNet");
  recNet->addMarSystem(mng.create("AudioSource", "srcRec"));
//	recNet->addMarSystem(mng.create("SoundFileSink","destRec")); // temporary

	recNet->updctrl("AudioSource/srcRec/mrs_real/israte", 44100.0);
  recNet->updctrl("AudioSource/srcRec/mrs_bool/initAudio", true);
}

void MarBackend::setFileName(string filename) {
	mrsWrapper->updctrl(filenamePtr, filename);
}

void MarBackend::startIntonation() {
	makeRecNet();

	mrsWrapper = new MarSystemQtWrapper(recNet);
	mrsWrapper->start();
	filenamePtr = mrsWrapper->getctrl("SoundFileSink/destRec/mrs_string/filename");
	mrsWrapper->pause();
}

void MarBackend::startControl() {

}

void MarBackend::playFile() {
//	cout<<"playing file now.   (not really, but it will in a few days)"<<endl;
}

void MarBackend::start() {
//	cout<<"play"<<endl;
	mrsWrapper->play();
}

void MarBackend::stop() {
//	cout<<"stop"<<endl;
	mrsWrapper->pause();
}

