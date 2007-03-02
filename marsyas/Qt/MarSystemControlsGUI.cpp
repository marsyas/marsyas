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
\class MarSystemControlsGUI
\brief GUI for viewing/editing MarSystem controls 
*/

#include "MarSystemControlsGUI.h"

#ifdef MARSYAS_QT

#include <QtCore>
#include <QtGui>

#include "MarSystem.h"
#include "MarControl.h"
#include "MarControlGUI.h"

using namespace std;
using namespace Marsyas;

MarSystemControlsGUI::MarSystemControlsGUI(MarSystem* msys,
																					 QWidget* parent /* = 0 */, 
																					 Qt::WFlags f /* = 0 */) 
																					 : QMainWindow(parent, f)
{
	msys_ = msys;
	msysPrefix_ = msys_->getPrefix();

	//Create tabs
	tab_ = new QTabWidget;

	//create default and custom control editors
	defaultControlsWindow_ = createDefaultControlsWindow();
	//customControlsWindow_ = createCustomControlsWindow();
	
	//add editors to theirs tabs
	tab_->insertTab(0, defaultControlsWindow_, "Default");
// 	if(customControlsWindow_)
// 	{
// 		tab_->insertTab(1, customControlsWindow_, "Custom Editor");
// 		tab_->setCurrentIndex(1);
// 	}

	//set MainWindow central widget
	setCentralWidget(tab_);
}

MarSystemControlsGUI::~MarSystemControlsGUI()
{
	//all child widgets created are automatically deleted
	//by parent or when the widget window is closed,
	//so no need to explicitly delete them.
}

QMainWindow* 
MarSystemControlsGUI::createDefaultControlsWindow()
{
	registry_.clear();

	QMainWindow* defCtrlWin = new QMainWindow();

	//get a copy of the MarSystem controls
	const map<string, MarControlPtr>& controls = msys_->getControls();
	map<string, MarControlPtr>::const_iterator citer;

	QFrame* ctrlFrame = new QFrame;
	//install a layout manager for placing controls widgets
	QGridLayout* gridLayout = new QGridLayout(ctrlFrame);
	
	int r = 0;
	//Create widgets for each MarControl
	for(citer = controls.begin() ; citer != controls.end(); ++citer)
	{
		//get control name
		//string ctrlname = citer->first;
		string ctrlname = citer->second->getName();

		//get control short-name (i.e. without any path)
		string shortctrlname = ctrlname.substr(msysPrefix_.length(),ctrlname.length() - msysPrefix_.length());

		//create label widget
		QLabel* cLabel = new QLabel(QString::fromStdString(shortctrlname));
		cLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
				
		//create MarControlValue widget
		MarControlGUI* cWidget = new MarControlGUI(citer->second);
		
		//forward MarControlGUI signal
// 		Q_ASSERT(connect(cvalueWidget, SIGNAL(controlChanged(std::string,MarControlValue&)),
// 											this, SIGNAL(controlChanged(std::string,MarControlValue&))));
		
		
		//add widgets to layouts
		gridLayout->addWidget(cLabel, r, 0, 1, 1);
		gridLayout->addWidget(cWidget, r, 1, 1, 1);
		r++;

		//add widgets to internal registry
		registry_[ctrlname] = cWidget; 
	}
	QScrollArea* scrollArea = new QScrollArea;
	//scrollArea->setBackgroundRole(QPalette::Dark);
	scrollArea->setWidget(ctrlFrame);

	defCtrlWin->setCentralWidget(scrollArea);
	return defCtrlWin;
}

QMainWindow* 
MarSystemControlsGUI::createCustomControlsWindow()
{
	//to override by any MarSystem that wishes to present
	//its customized controls editor
	return NULL;
}

void
MarSystemControlsGUI::updControl(MarControl* control) //[!]
{
	//update default controls
	map<string, MarControlGUI*>::iterator cWidget;
	cWidget = registry_.find(control->getName());
	if (cWidget != registry_.end())
		cWidget->second->updControl(MarControlPtr(control));

	//and update any customized controls that may also exist
	if(customControlsWindow_)
		updateCustomControlsWindow(control);
}

void
MarSystemControlsGUI::updateCustomControlsWindow(MarControl* control)
{
	//to override by any MarSystem that wishes to present
	//its customized controls editor. Each custom controls editor
	//should know how to do it...
}

#endif //MARSYAS_QT
