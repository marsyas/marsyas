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

#include "mainwindow.h"

MarQTwindow::MarQTwindow(string fileName)
{
// typical Qt front-end
	QPushButton *quit = new QPushButton(tr("Quit"));
	connect(quit, SIGNAL(clicked()), qApp, SLOT(quit()));

	QPushButton *updatePos = new QPushButton(tr("Update position"));

	QSlider *volume = new QSlider (Qt::Horizontal);
	volume->setRange(0,100);
	volume->setValue(50);

	lcd_ = new QLCDNumber();
	lcd_->setNumDigits(10);

	QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(volume);
	layout->addWidget(updatePos);
	layout->addWidget(lcd_);
	layout->addWidget(quit);
	setLayout(layout);

// make the Marsyas backend
	marBackend_ = new MarBackend();
	marBackend_->openBackendSoundfile(fileName);

// make connections between the Qt front-end and the Marsyas backend:

//		Qt -> Marsyas
	connect(volume, SIGNAL(valueChanged(int)),
	        marBackend_, SLOT(setBackendVolume(int)));

//		Marsyas -> Qt
	connect(marBackend_, SIGNAL(changedBackendPosition(int)),
	        this, SLOT(setMainPosition(int)));

//		Qt -> Marsyas (getBackendPosition) -> Qt (changedBackendPosition)
	connect(updatePos, SIGNAL(clicked()),
	        marBackend_, SLOT(getBackendPosition()));
}

MarQTwindow::~MarQTwindow()
{
	delete marBackend_;
}

void MarQTwindow::setMainPosition(int newPos)
{
	lcd_->display(newPos);
}

