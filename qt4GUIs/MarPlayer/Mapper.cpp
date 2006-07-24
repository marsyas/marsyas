/*
** Copyright (C) 1998-2006 George Tzanetakis <gtzan@cs.uvic.ca>
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
    \brief Mapper maps the state of the GUI to MarSystemWrapper 

    MainWindow connects various signals to slots of Mapper 
and Mapper converts and maps the corresponding values to 
the appropriate slots of MarSystemWrapper. Also 
Mapper creates the playback network of MarSystems 
and also receives updates from the MarSystemWrapper 
which it signals to MainWindow to update the GUI. 
*/

#include "Mapper.h" 

using namespace Marsyas;


Mapper::Mapper()
{

  // create the MarSystem network for playback 
  MarSystemManager mng;  
  
  pnet_ = mng.create("Series", "pnet_");
  pnet_->addMarSystem(mng.create("SoundFileSource", "src"));
  pnet_->addMarSystem(mng.create("Gain", "gain"));
  pnet_->addMarSystem(mng.create("AudioSink", "dest"));
  pnet_->linkctrl("mrs_bool/notEmpty", "SoundFileSource/src/mrs_bool/notEmpty");
  
  // make a QT-like object wrapped around the MarSystem
  mwr_ = new MarSystemWrapper(pnet_);
}



void 
Mapper::open(QString fileName, int pos)
{
  mwr_->updctrl("SoundFileSource/src/mrs_string/filename", fileName.toStdString());  
  // loop forever the piece 
  mwr_->updctrl("SoundFileSource/src/mrs_real/repetitions", -1.0); 

  
  mrs_natural size = 
    mwr_->getctrl("SoundFileSource/src/mrs_natural/size").toNatural();
  
  mrs_real srate = 
    mwr_->getctrl("SoundFileSource/src/mrs_real/osrate").toReal();    
  
  mrs_real duration = (size / srate);
  emit durationChanged(duration);
  
  setPos(pos);
  mwr_->start();

  // timer is used to update the position slider "automatically" 
  // as the sound is playing 
  QTimer *timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(setPos()));
  timer->start(250);
}


void 
Mapper::setPos() 
{
  mrs_natural pos = mwr_->getctrl("SoundFileSource/src/mrs_natural/pos").toNatural();
  mrs_natural size = mwr_->getctrl("SoundFileSource/src/mrs_natural/size").toNatural();
  mrs_real srate = 
    mwr_->getctrl("SoundFileSource/src/mrs_real/osrate").toReal();    
  
  mrs_real duration = (pos / srate);
  emit timeChanged(duration);
  
  float rpos = pos * 1.0 / size;
  int sliderPos = rpos * 100.0;
  emit posChanged(sliderPos);
}

void 
Mapper::setPos(int val)
{
  float fval = val / 100.0f;
  
  float fsize = 
    mwr_->getctrl("SoundFileSource/src/mrs_natural/size").toNatural();
  fsize *= fval;
  
  int size = (int) fsize;
  mwr_->updctrl("SoundFileSource/src/mrs_natural/pos", size);
}

void 
Mapper::setGain(int val)
{
  float fval = val / 100.0f;
  mwr_->updctrl("Gain/gain/mrs_real/gain", fval);
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

