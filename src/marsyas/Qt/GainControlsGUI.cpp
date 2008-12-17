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
\brief GUI for viewing/editing GainControlsGUI controls 
*/

#include "GainControlsGUI.h"

#ifdef MARSYAS_QT

#include <QtCore>
#include <QtGui>

#include <cmath>
#include <limits>

#include "MarSystem.h"

using namespace std;
using namespace Marsyas;

GainControlsGUI::GainControlsGUI(MarSystem* msys,
																 QWidget* parent /* = 0 */, 
																 Qt::WFlags f /* = 0 */) 
																 : MarSystemControlsGUI(msys,parent, f)
{
	customControlsWindow_ = createCustomControlsWindow();
	tab_->insertTab(1, customControlsWindow_, "Custom");
	tab_->setCurrentIndex(1);

	ctrl_gain_ = msys_->getctrl("mrs_real/gain");
}

GainControlsGUI::~GainControlsGUI()
{

}

QMainWindow* 
GainControlsGUI::createCustomControlsWindow()
{
	QMainWindow* custCtrlWin = new QMainWindow;
	QFrame* frame = new QFrame(); 
	QHBoxLayout* layout = new QHBoxLayout(frame);
	QProgressBar* VUmeter;
	
	//---------------------------------------------------------
	//create a VUmeter for input channels
	//---------------------------------------------------------
	inChannels_ = msys_->getctrl("mrs_natural/inObservations")->to<mrs_natural>();
	inVUmeters_.clear();
	for(mrs_natural i = 0; i < inChannels_; i++)
	{
		VUmeter = new QProgressBar();
		VUmeter->setOrientation(Qt::Vertical);
		VUmeter->setRange(-60,0);//dB scale
		VUmeter->setValue(-60);
		layout->addWidget(VUmeter);
		inVUmeters_.push_back(VUmeter);
	}
	
	//---------------------------------------------------------
	//create a slider for gain control
	//---------------------------------------------------------
	gainSlider_ = new QSlider(Qt::Vertical);
	gainSlider_->setRange(0,100);
	gainSlider_->setValue((int)(msys_->getctrl("mrs_real/gain")->to<mrs_real>()*100));
	gainSlider_->setSingleStep(1);
	layout->addWidget(gainSlider_);
	connect(gainSlider_, SIGNAL(valueChanged(int)),
		this, SLOT(gainSliderMoved(int)));

	//---------------------------------------------------------
	//create a VUmeter for output channels
	//---------------------------------------------------------
	onChannels_ = msys_->getctrl("mrs_natural/inObservations")->to<mrs_natural>();
	outVUmeters_.clear();
	for(mrs_natural i = 0; i < onChannels_; i++)
	{
		VUmeter = new QProgressBar();
		VUmeter->setOrientation(Qt::Vertical);
		VUmeter->setRange(-60,0);//dB scale
		VUmeter->setValue(-60);
		layout->addWidget(VUmeter);
		outVUmeters_.push_back(VUmeter);
	}

	custCtrlWin->setCentralWidget(frame);
	return custCtrlWin;
}

void
GainControlsGUI::gainSliderMoved(int value)
{
	mrs_real svalue = (mrs_real)value/100.0;
	//string cname = msys_->getPrefix() + "mrs_real/gain";
	//emit controlChanged(cname, (MarControlValue)svalue);
	ctrl_gain_->setValue(svalue, true);
}

void
GainControlsGUI::updateCustomControlsWindow(MarControl* control)
{
	string cname = control->getName();

	//update Gain Slider value
	if(cname == "mrs_real/gain")
	{
		int svalue = (int)(control->to<mrs_real>()*100);
		if(gainSlider_->value() != svalue)
		{
			disconnect(gainSlider_, SIGNAL(valueChanged(int)),
				this, SLOT(gainSliderMoved(int)));
			gainSlider_->setValue(svalue);
			connect(gainSlider_, SIGNAL(valueChanged(int)),
				this, SLOT(gainSliderMoved(int)));
		}
		return;
	}

	//update input VU meter (using a dB scale)
	if(cname == "mrs_realvec/inRMS")
	{
		realvec inRMS = control->to<mrs_realvec>();
		mrs_real RMSdB;
		for(int i = 0; i < inChannels_; i++)
		{
			if(inRMS(i) == 0.0)
				RMSdB = -60;
			else
				RMSdB = 20*log10(inRMS(i) + numeric_limits<mrs_real>::min());

			inVUmeters_[i]->setValue((int)RMSdB);
		}
		return;
	}

	//update output VU meter (using a dB scale)
	if(cname == "mrs_realvec/outRMS")
	{
		realvec outRMS = control->to<mrs_realvec>();
		mrs_real RMSdB;
		for(int i = 0; i < onChannels_; i++)
		{
			if(outRMS(i) == 0.0)
				RMSdB = -60;
			else
				RMSdB = 20*log10(outRMS(i) + numeric_limits<mrs_real>::min());

			outVUmeters_[i]->setValue((int)RMSdB);
		}
		return;
	}

	//update number of input VUmeters if nr of input channels has changed
	if(cname == "mrs_natural/inObservations" &&
		 (inChannels_ != control->to<mrs_natural>()))
	{
		delete customControlsWindow_; 	
		customControlsWindow_ = createCustomControlsWindow();
		tab_->insertTab(1, customControlsWindow_, "Custom Editor");
		tab_->setCurrentIndex(1);
		return;
	}

	//update number of output VUmeters if nr of output channels has changed
	if(cname == "mrs_natural/onObservations" &&
		onChannels_ != control->to<mrs_natural>())
	{
		delete customControlsWindow_; 	
		customControlsWindow_ = createCustomControlsWindow();
		tab_->insertTab(1, customControlsWindow_, "Custom Editor");
		tab_->setCurrentIndex(1);
		return;
	}
}

#endif //MARSYAS_QT
