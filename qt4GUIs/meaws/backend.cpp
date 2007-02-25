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
		mrsWrapper->play();
		cout<<"deleting Intonation stuff"<<endl;
// something bad happens here.  :(
		delete mrsWrapper;
		//cout<<"mrsWrapper deleted"<<endl;
		delete recNet;
		//cout<<"recNet deleted"<<endl;
	}
}

void MarBackend::makeRecNet() {
  MarSystemManager mng;

  recNet = mng.create("Series", "recNet");
  recNet->addMarSystem(mng.create("AudioSource", "srcRec"));
	recNet->addMarSystem(mng.create("SoundFileSink","destRec"));

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

void MarBackend::calculate(string filename) {
	string command;
	command = "python2.4 praat-to-pitch.py ";
	command.append(filename);
	command.append(" 120");  // tempo
	system(command.c_str());

	float notepitch;
	ifstream inFile;
	inFile.open("notepitches.txt");
	while (inFile >> notepitch) {
		if (notepitch>0) {
// do whatever I want with the note data.
			cout<<notepitch<<endl;
		}
	}
	inFile.close();

}

void MarBackend::start() {
	cout<<"play"<<endl;
	mrsWrapper->play();
}

void MarBackend::stop() {
	cout<<"stop"<<endl;
	mrsWrapper->pause();
}

