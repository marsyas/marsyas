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
  \brief  Plays MarSystem and mediates GUI interaction

  Creates a network of MarSystems, then uses the MarsyasQt::System wrapper
  on the network.
  Maps own slots - to which MainWindow connects - to
  slots of MarsyasQt::Control in order to control the system from GUI.
  Emits changing values of MarsyasQt::Control (updated as the system is running)
  via own signals, to which MainWindow connects to update informatino in GUI.
*/

#include "Mapper.h"
#include <marsyas/system/MarSystemManager.h>

using namespace Marsyas;
using namespace MarsyasQt;

Mapper::Mapper()
{
  // Create the MarSystem network for playback.
  MarSystemManager mng;

  m_system = mng.create("Series", "pnet_");
  m_system->addMarSystem(mng.create("SoundFileSource", "src"));
  m_system->addMarSystem(mng.create("Gain", "gain"));
  m_system->addMarSystem(mng.create("AudioSink", "dest"));
  m_system->updControl("mrs_natural/inSamples", 2048);

  m_system->linkControl("mrs_bool/hasData", "SoundFileSource/src/mrs_bool/hasData");

  // Create a handy Qt wrapper for the MarSystem.
  m_qsystem = new MarsyasQt::System(m_system);

  // Get handles for all the controls.
  m_fileControl = m_qsystem->control("SoundFileSource/src/mrs_string/filename");
  m_gainControl = m_qsystem->control("Gain/gain/mrs_real/gain");
  m_repetitionControl = m_qsystem->control("SoundFileSource/src/mrs_real/repetitions");
  m_positionControl = m_qsystem->control("SoundFileSource/src/mrs_natural/pos");
  m_sizeControl = m_qsystem->control("SoundFileSource/src/mrs_natural/size");
  m_outputSampleRateControl = m_qsystem->control("SoundFileSource/src/mrs_real/osrate");
  m_initAudioControl = m_qsystem->control("AudioSink/dest/mrs_bool/initAudio");

  connect( &m_controlEmitTimer, SIGNAL(timeout()), this, SLOT(emitControlValues()) );
}

Mapper::~Mapper()
{
  m_qsystem->stop();

  delete m_qsystem;
  delete m_system;
}

void
Mapper::play(QString fileName)
{
  const bool do_not_update = false;

  // Change filename.
  m_fileControl->setValue(fileName, do_not_update);

  // Loop file forever.
  m_repetitionControl->setValue(-1.0, do_not_update);

  // Always re-initialize audio due to possible changes in
  // number of channels, etc...
  m_initAudioControl->setValue(true, do_not_update);

  // Apply scheduled control changes.
  m_qsystem->update();

  // Play (if not playing yet).
  play();
}


/* "manual" advancement of position */
void
Mapper::setPos(int position_percents)
{
  int position_samples =
    (int) m_sizeControl->value().toDouble() * (position_percents / 100.0);

  m_positionControl->setValue(position_samples);
}

void
Mapper::setGain(int position_percents)
{
  m_gainControl->setValue( position_percents / 100.0 );
}

void
Mapper::play()
{
  m_qsystem->start();
  m_controlEmitTimer.start(200);
}

void
Mapper::pause()
{
  m_controlEmitTimer.stop();
  m_qsystem->stop();
}

void Mapper::emitControlValues()
{
  Q_ASSERT(m_qsystem->isRunning());

  int pos = m_positionControl->value().toInt();
  int size = m_sizeControl->value().toInt();
  double srate = m_outputSampleRateControl->value().toDouble();

  int duration_secs = (int)(size / srate);
  QTime duration = QTime(0,0).addSecs(duration_secs);
  emit durationChanged(duration);

  int time_secs = (int)(pos / srate);
  QTime time = QTime(0,0).addSecs(time_secs);
  emit timeChanged(time);

  int sliderPos = 100.f * pos / size;
  emit posChanged(sliderPos);
}
