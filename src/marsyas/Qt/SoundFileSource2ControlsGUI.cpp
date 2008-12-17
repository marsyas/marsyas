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
\class ControlsGUI
\brief GUI for viewing/editing SoundFileSource2ControlsGUI controls 
*/

#include "SoundFileSource2ControlsGUI.h"

#ifdef MARSYAS_QT

#include <QtCore>
#include <QtGui>

#include <cmath>
#include <limits>

#include "MarSystem.h"

using namespace std;
using namespace Marsyas;

SoundFileSource2ControlsGUI::SoundFileSource2ControlsGUI(MarSystem* msys,
																 QWidget* parent /* = 0 */, 
																 Qt::WFlags f /* = 0 */) 
																 : MarSystemControlsGUI(msys,parent, f)
{
	customControlsWindow_ = createCustomControlsWindow();
	tab_->insertTab(1, customControlsWindow_, "Custom");
	tab_->setCurrentIndex(1);

	ctrl_pos_ = msys_->getctrl("mrs_natural/pos");
}

SoundFileSource2ControlsGUI::~SoundFileSource2ControlsGUI()
{

}

QMainWindow* 
SoundFileSource2ControlsGUI::createCustomControlsWindow()
{
	QMainWindow* custCtrlWin = new QMainWindow();
	QFrame* frame = new QFrame(); 
	QGridLayout* layout = new QGridLayout(frame);

	//---------------------------------------------------------
	//create Open Audio File Push Button
	//---------------------------------------------------------
	QPushButton* openBtn = new QPushButton("Open Audio File...");
	openBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	connect(openBtn, SIGNAL(clicked(bool)),
										this, SLOT(openAudioFile()));
	layout->addWidget(openBtn, 0, 0, 1, 1);

	//---------------------------------------------------------
	//create opened AudioFileName label
	//---------------------------------------------------------
	filenameLbl_ = new QLabel(QString::fromStdString(msys_->getctrl("mrs_string/filename")->to<mrs_string>()));
	layout->addWidget(filenameLbl_, 0, 1, 1, 1);

	//---------------------------------------------------------
	//create playing position slider
	//---------------------------------------------------------
	posSlider_ = new QSlider(Qt::Horizontal);
	posSlider_->setRange(0, (int)(msys_->getctrl("mrs_natural/size")->to<mrs_natural>()));
	posSlider_->setTracking(false);
	
	connect(posSlider_, SIGNAL(valueChanged(int)),
		this, SLOT(posSliderMoved(int)));
	
	posSliderPressed_ = false;
	connect(posSlider_, SIGNAL(sliderPressed()),
		this, SLOT(posSliderPressed()));

	layout->addWidget(posSlider_, 1, 0, 1, 2);

	//---------------------------------------------------------
	custCtrlWin->setCentralWidget(frame);
	return custCtrlWin;
}

void
SoundFileSource2ControlsGUI::posSliderPressed()
{
	posSliderPressed_ = true;
}

void
SoundFileSource2ControlsGUI::posSliderMoved(int value)
{
	//string cname = msys_->getPrefix() + "mrs_natural/pos";
	//emit controlChanged(cname, (MarControlValue)value);
	ctrl_pos_->setValue((mrs_natural)value, true);
	posSliderPressed_ = false;
}

void
SoundFileSource2ControlsGUI::openAudioFile()
{
	//open audio file
	QString audioFileName = QFileDialog::getOpenFileName(this);
	if (!audioFileName.isEmpty())
	{
		//string cname = msys_->getPrefix() + "mrs_string/filename";
		//emit controlChanged(cname, (MarControlValue)audioFileName->toStdString());
		msys_->getctrl("mrs_string/filename")->setValue(audioFileName.toStdString(), true);

	}
	else return;
}

void
SoundFileSource2ControlsGUI::updateCustomControlsWindow(MarControl* control)
{
	string cname = control->getName();

	//update pos slider
	if(cname == "mrs_natural/pos" && !posSliderPressed_)
	{
		int value = (int)control->to<mrs_natural>();
		if(posSlider_->value() != value )
		{
			posSlider_->setValue(value);
		}
		return;
	}

	//update pos slider range
	if(cname == "mrs_natural/size")
	{
		int value = (int)control->to<mrs_natural>();
		if(posSlider_->maximum() != value)
		{
			posSlider_->setMaximum(value);
		}
		return;
	}
	
	//update filename label
	if(cname == "mrs_string/filename")
	{
		filenameLbl_->setText(QString::fromStdString(control->to<mrs_string>()));
		return;
	}
}

#endif //MARSYAS_QT
