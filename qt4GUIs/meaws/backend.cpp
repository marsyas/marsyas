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

void MarBackend::calculate() {
  MarSystemManager mng;
  MarSystem* pnet = mng.create("Series", "pnet");

  pnet->addMarSystem(mng.create("SoundFileSource", "src"));
  pnet->updctrl("SoundFileSource/src/mrs_string/filename", "test-rec.wav");
  pnet->addMarSystem(mng.create("PitchSACF", "sacf")); 
  pnet->addMarSystem(mng.create("RealvecSink", "rvSink")); 

  mrs_real lowPitch = 36;
  mrs_real highPitch = 79;
  mrs_real lowFreq = pitch2hertz(lowPitch);
  mrs_real highFreq = pitch2hertz(highPitch);

  mrs_natural lowSamples = 
     hertz2samples(highFreq, pnet->getctrl("SoundFileSource/src/mrs_real/osrate")->toReal());
  mrs_natural highSamples = 
     hertz2samples(lowFreq, pnet->getctrl("SoundFileSource/src/mrs_real/osrate")->toReal());
 
  pnet->updctrl("PitchSACF/sacf/mrs_natural/lowSamples", lowSamples);
  pnet->updctrl("PitchSACF/sacf/mrs_natural/highSamples", highSamples);
  pnet->updctrl("mrs_natural/inSamples", 1024);

  while (pnet->getctrl("SoundFileSource/src/mrs_bool/notEmpty")->toBool())
   pnet->tick();

	realvec data = pnet->getctrl("RealvecSink/rvSink/mrs_realvec/data")->toVec();
   for (mrs_natural i=1; i<data.getSize();i+=2)
	   data(i) = samples2hertz((mrs_natural) data(i), pnet->getctrl("SoundFileSource/src/mrs_real/osrate")->toReal());
   
   pnet->updctrl("RealvecSink/rvSink/mrs_bool/done", 1); 

  int samps = 40;
  int totsamps, j;
  totsamps=0;
  while ( true ) {
    if (totsamps >= data.getSize()) break;
    for (j=1; j<samps; j+=2) {
      totsamps+=2; 
      if (totsamps >= data.getSize()) break;
      cout<<data(totsamps+1)<<" ";
    }
    cout<<endl<<"-----"<<endl;
  }
}
/*
void MarBackend::setTempo(float timeBetweenBeats) {
	cout<<"setTempo "<<timeBetweenBeats<<endl;
//  e->set_repeat(Repeat( dtos(timeBetweenBeats)+"s" ));
}
*/
void MarBackend::start() {
	cout<<"play"<<endl;
	mrsWrapper->play();
}

void MarBackend::stop() {
	cout<<"stop"<<endl;
	mrsWrapper->pause();
}
/*
void MarBackend::setIntro(int beats) {
	cout<<beats<<" beats intro"<<endl;
	introBeats = beats;
}

void MarBackend::playBeat() {
	cout<<"BEAT"<<endl;
//	metroNet->updctrl("SoundFileSource/src/mrs_natural/pos",0);
}
*/
