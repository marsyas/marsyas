/*
** Copyright (C) 1998-2010 George Tzanetakis <gtzan@cs.uvic.ca>
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

/** 
    \class Mapper
    \brief Mapper maps the state of the GUI to MarSystemQtWrapper 

    MainWindow connects various signals to slots of Mapper 
	and Mapper converts and maps the corresponding values to 
	the appropriate slots of MarSystemQtWrapper. Also 
	Mapper creates the playback network of MarSystems 
	and also receives updates from the MarSystemQtWrapper 
	which it signals to MainWindow to update the GUI. 
*/

#include "Mapper.h" 

using namespace Marsyas;
using namespace MarsyasQt;

Mapper::Mapper()
{
	// create the MarSystem network for playback 
	MarSystemManager mng;  
  
	pnet_ = mng.create("Series", "pnet_");
	pnet_->addMarSystem(mng.create("SoundFileSource", "src"));
	pnet_->addMarSystem(mng.create("Gain", "gain"));
	pnet_->addMarSystem(mng.create("AudioSink", "dest"));
	pnet_->updctrl("mrs_natural/inSamples", 2048);
  
	pnet_->linkctrl("mrs_bool/hasData", "SoundFileSource/src/mrs_bool/hasData");
  
  
	// make a Qt-like thread object wrapped around the MarSystem
	mwr_ = new MarSystemQtWrapper(pnet_);
	//start the MarSystemQtWrapper thread
	mwr_->tickForever();

	// Create MarControlPtr handles for all the controls 
	filePtr_ = mwr_->getctrl("SoundFileSource/src/mrs_string/filename");
	gainPtr_ = mwr_->getctrl("Gain/gain/mrs_real/gain");
	repPtr_ = mwr_->getctrl("SoundFileSource/src/mrs_real/repetitions");
	posPtr_ = mwr_->getctrl("SoundFileSource/src/mrs_natural/pos");  
	sizePtr_ = mwr_->getctrl("SoundFileSource/src/mrs_natural/size");  
	osratePtr_ = mwr_->getctrl("SoundFileSource/src/mrs_real/osrate");  
	initPtr_ = mwr_->getctrl("AudioSink/dest/mrs_bool/initAudio");
  

}

Mapper::~Mapper()
{
	delete mwr_;
	delete pnet_;
}

void 
Mapper::open(QString fileName, int pos)
{
	mwr_->play();
	// update filename
	mwr_->updctrl(filePtr_, fileName.toStdString());
  

	//  loop forever the piece [!]
	mwr_->updctrl(repPtr_, -1.0); 
  
	mwr_->updctrl(initPtr_, (mrs_bool)true);

	mrs_natural size = sizePtr_->to<mrs_natural>();
  
	mrs_real srate = osratePtr_->to<mrs_real>();


	setPos(pos);
  
	Marsyas::mrs_real duration = (size / srate);
	cout << duration << endl;
	emit durationChanged(duration);

  

  
	// timer is used to update the position slider "automatically" 
	// as the sound is playing 
	QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(setPos()));
	timer->start(250);
}


/* "automatic" advancement of position */ 
void 
Mapper::setPos() 
{
	mrs_natural pos = posPtr_->to<mrs_natural>();
	mrs_natural size = sizePtr_->to<mrs_natural>();
	mrs_real srate = osratePtr_->to<mrs_real>();
  
	mrs_real duration = (pos / srate);
	emit timeChanged(duration);
  
	float rpos = pos * 1.0 / size;
	int sliderPos = rpos * 100.0;
	emit posChanged(sliderPos);
}


/* "manual" advancement of position */ 
void 
Mapper::setPos(int val)
{
	float fval = val / 100.0f;
  
	float fsize = 
		sizePtr_->to<mrs_natural>();
	fsize *= fval;
  
	int size = (int) fsize;
	mwr_->updctrl(posPtr_, size);
  
}

void 
Mapper::setGain(int val)
{
	float fval = val / 100.0f;
	mwr_->updctrl(gainPtr_, fval);
}

void 
Mapper::play()
{
	mwr_->play();
}

void 
Mapper::pause()
{
	mwr_->pause();
}

