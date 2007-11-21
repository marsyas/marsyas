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

Metro::Metro(QWidget *parent, string audioFilename)
{
	resize(200, 200);
	setWindowTitle("Visual metronome");
//	setParent(parent);

	normalBeatColor_=Qt::cyan;
	activeBeatColor_=Qt::red;

	bigDisplay_=false;
	audio_=false;   // for testing

	if (audio_)
	{
		setupAudio(audioFilename);
	}

	timer_ = new QTimer();
	connect(timer_, SIGNAL(timeout()), this, SLOT(beat()));
	setTempo(60);

	flashSpeed_ = new QTimer();
	connect(flashSpeed_, SIGNAL(timeout()), this, SLOT(beatFinished()));
	flashSpeed_->setInterval(100);
}

Metro::~Metro()
{
//	cout<<"deleting Metro"<<endl;
	delete mrsWrapper_;
	delete metroNet_;
	delete flashSpeed_;
	delete timer_;
}

void Metro::setupAudio(string audioFilename)
{
//	cout<<audioFilename<<endl;
	MarSystemManager mng;
	metroNet_ = mng.create("Series", "metroNet_");
	metroNet_->addMarSystem(mng.create("SoundFileSource", "srcMetro"));
	metroNet_->addMarSystem(mng.create("AudioSink", "dest"));
	metroNet_->updctrl("SoundFileSource/srcMetro/mrs_string/filename", audioFilename);
	metroNet_->updctrl("AudioSink/dest/mrs_bool/initAudio", true);

	mrsWrapper_ = new MarSystemQtWrapper(metroNet_);
	mrsWrapper_->start();
	positionPtr_ = mrsWrapper_->getctrl("SoundFileSource/srcMetro/mrs_natural/pos");
}

void Metro::setTempo(int getTempo)
{
	tempo_ = getTempo;
	// in QT 4.2, the timer_ interval (in ms) must be an integer
	int timeBetweenBeats = 60000/tempo_;
	timer_->setInterval(timeBetweenBeats);
}

void Metro::setIcon(QAction* getVisualAction)
{
	visualMetroBeatAct_ = getVisualAction;
}

void Metro::start()
{
	timer_->start();
	beat();
	if (audio_)
	{
		mrsWrapper_->updctrl(positionPtr_, 0);
		mrsWrapper_->play();
	}
}

void Metro::stop()
{
	timer_->stop();
	flashSpeed_->stop();
}

void Metro::beatFinished()
{
	if (bigDisplay_)
	{
		drawBeatColor_ = normalBeatColor_;
		update();
	}
	visualMetroBeatAct_->setIcon(QIcon(":/icons/circle.png"));
}

void Metro::beat()
{
	if (audio_)
	{
		mrsWrapper_->updctrl(positionPtr_, 0);
		mrsWrapper_->play();
	}
	if (bigDisplay_)
	{
		drawBeatColor_ = activeBeatColor_;
		update();
	}
	visualMetroBeatAct_->setIcon(QIcon(":/icons/circle-beat.png"));
	flashSpeed_->start();
}

void Metro::toggleBigMetro()
{
	bigDisplay_=!bigDisplay_;
	if (bigDisplay_)
	{
// pop up the window, draw big circle, etc
		show();
		drawBeatColor_ = normalBeatColor_;
		update();
	}
	else
	{
// kill big window
		hide();
	}
}

void Metro::paintEvent(QPaintEvent *)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);

	QRectF area(5, 5, width()-10, height()-10);
	painter.setBrush(drawBeatColor_);
	painter.drawEllipse(area);
}

