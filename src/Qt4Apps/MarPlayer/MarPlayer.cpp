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

#include <QtGui>
#include "MarPlayer.h"

using namespace Marsyas;

MarPlayer::MarPlayer()
{
  ui.setupUi(this);
  init();
}

MarPlayer::~MarPlayer()
{
  delete mapper_;
}

void
MarPlayer::init()
{
  setWindowIcon(QIcon("MarPlayer.icns"));
  createActions();
  createMenus();
  mapper_ = new Mapper();		// mapper communicates with Marsyas 
  createConnections();
}

void 
MarPlayer::createMenus()
{
  fileMenu = menuBar()->addMenu(tr("&File"));
  fileMenu->addAction(openAct);
  menuBar()->addSeparator();
  helpMenu = menuBar()->addMenu(tr("&Help"));
  helpMenu->addAction(aboutAct);
}

void MarPlayer::createActions()
{
  openAct = new QAction(QIcon(":/images/open.png"), tr("&Open..."), this);
  openAct->setShortcut(tr("Ctrl+O"));
  openAct->setStatusTip(tr("Open an existing file"));
  connect(openAct, SIGNAL(triggered()), this, SLOT(open()));
  aboutAct = new QAction(tr("&About"), this);
  aboutAct->setStatusTip(tr("Show the application's About box"));
  connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));
}

void 
MarPlayer::createConnections()
{
  connect(ui.playButton, SIGNAL(clicked()), 
	  mapper_, SLOT(play()));
  
  connect(ui.pauseButton, SIGNAL(clicked()), 
	  mapper_, SLOT(pause()));
  
  connect(ui.positionSlider, SIGNAL(sliderReleased()), 
	  this, SLOT(seekPos()));

  connect(mapper_, SIGNAL(posChanged(int)), 
	  this, SLOT(positionSlider(int)));

  connect(mapper_, SIGNAL(durationChanged(Marsyas::mrs_real)), 
	  this, SLOT(setDuration(Marsyas::mrs_real)));

  connect(mapper_, SIGNAL(timeChanged(Marsyas::mrs_real)), 
	  this, SLOT(setTime(Marsyas::mrs_real)));

  connect(ui.gainSlider, SIGNAL(valueChanged(int)), 
	  mapper_,SLOT(setGain(int)));

  connect(ui.fileComboBox, SIGNAL(activated(const QString&)), 
	  this, SLOT(open(const QString&)));
  
}

void 
MarPlayer::open()
{
  QString fileName = QFileDialog::getOpenFileName(this);
  ui.fileComboBox->insertItem(0,fileName);
  ui.fileComboBox->setCurrentIndex(0);
  open(fileName);
}

void 
MarPlayer::open(const QString& text)
{
  mapper_->open(text, ui.positionSlider->sliderPosition());  
}

void 
MarPlayer::positionSlider(int val)
{
  if (ui.positionSlider->isSliderDown() == false)
    ui.positionSlider->setValue(val);
}

// set duration of song on corresponding slider
void 
MarPlayer::setDuration(Marsyas::mrs_real val) 
{
  QTime dur;
  dur = dur.addSecs((int)val);
  ui.durTimeEdit->setTime(dur);
   
   
  
}

// set current time of soundfile on corresponding slider 
void 
MarPlayer::setTime(Marsyas::mrs_real val) 
{
  
  QTime dur;
  dur = dur.addSecs((int)val);
  ui.posTimeEdit->setTime(dur);
}

void 
MarPlayer::seekPos()
{
  mapper_->setPos(ui.positionSlider->sliderPosition());
}

void MarPlayer::about()
{
  QMessageBox::about(this, tr("About Marsyas SoundFile Player"),
		     tr("The example demonstrates how to write a GUI that combines Qt 4 and Marsyas"));
  
}
