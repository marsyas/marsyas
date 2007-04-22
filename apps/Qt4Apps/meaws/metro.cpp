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

Metro::Metro(QAction *getVisualMetroBeat, QWidget *parent, string audioFilename) {
//	cout<<"begun making metro"<<endl;
	visualMetroBeat = getVisualMetroBeat;
  connect(visualMetroBeat, SIGNAL(triggered()), this, SLOT(toggleBigMetro()));

	resize(200, 200);
	setWindowTitle("Visual metronome");
//	setParent(parent);

	normalBeatColor=Qt::cyan;
	activeBeatColor=Qt::red;

	introBeats=0;
	bigDisplay=false;
	audio=true;   // for testing

	if (audio) {
		setupAudio(audioFilename);
	}

	timer = new QTimer();
	connect(timer, SIGNAL(timeout()), this, SLOT(beat()));
	setTempo(60);

	flashSpeed = new QTimer();
	connect(flashSpeed, SIGNAL(timeout()), this, SLOT(beatFinished()));
	flashSpeed->setInterval(100);
}

Metro::~Metro() {
//	cout<<"deleting Metro"<<endl;
	delete mrsWrapper;
	delete metroNet;
	delete flashSpeed;
	delete timer;
}

void Metro::setupAudio(string audioFilename) {
	cout<<audioFilename<<endl;
  MarSystemManager mng;
  metroNet = mng.create("Series", "metroNet");
  metroNet->addMarSystem(mng.create("SoundFileSource", "srcMetro"));
  metroNet->addMarSystem(mng.create("AudioSink", "dest"));
	metroNet->updctrl("SoundFileSource/srcMetro/mrs_string/filename", audioFilename);
  metroNet->updctrl("AudioSink/dest/mrs_bool/initAudio", true);

	mrsWrapper = new MarSystemQtWrapper(metroNet);
	mrsWrapper->start();
	positionPtr = mrsWrapper->getctrl("SoundFileSource/srcMetro/mrs_natural/pos");
}

void Metro::setTempo(int getTempo) {
	tempo = getTempo;
	int timeBetweenBeats = 60000/tempo;
//	cout<<"setTempo "<<timeBetweenBeats<<endl;
	timer->setInterval(timeBetweenBeats);
}

void Metro::startMetro() {
	timer->start();
	beat();
	if (audio) {
		mrsWrapper->updctrl(positionPtr, 0);
		mrsWrapper->play();
	}
}

void Metro::stopMetro() {
	timer->stop();
	flashSpeed->stop();
}

void Metro::setIntro(int beats) {
	introBeats = beats;
}

void Metro::beatFinished() {
	if (bigDisplay) {
		drawBeatColor = normalBeatColor;
		update();
	}
// else {
		visualMetroBeat->setIcon(QIcon(":/icons/circle.png"));
//	}
}

void Metro::beat() {
	if (audio) {
		mrsWrapper->updctrl(positionPtr, 0);
		mrsWrapper->play();
	}
	if (bigDisplay) {
		drawBeatColor = activeBeatColor;
		update();
//		flashSpeed->start();
//	} else {
	}
		visualMetroBeat->setIcon(QIcon(":/icons/circle-beat.png"));
		flashSpeed->start();
//	}
}

void Metro::toggleBigMetro() {
	bigDisplay=!bigDisplay;
	if (bigDisplay) {
// pop up the window, draw big circle, etc
		show();
		drawBeatColor = normalBeatColor;
		update();
	} else {
// kill big window
		hide();
	}
}

void Metro::paintEvent(QPaintEvent *) {
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);

	QRectF area(5, 5, width()-10, height()-10);
	painter.setBrush(drawBeatColor);
	painter.drawEllipse(area);
}

