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

#ifndef MARSYSTEMCONTROLSGUI_H
#define MARSYSTEMCONTROLSGUI_H

#include "common.h"

#ifdef MARSYAS_QT

#include <string>
#include <map>

#include <QMainWindow>

class QTabWidget;

namespace Marsyas
{

class MarSystem;
class MarControl;
class MarControlGUI;

class MarSystemControlsGUI : public QMainWindow
{
	Q_OBJECT

protected:
	MarSystem* msys_;
	std::string msysPrefix_;

	std::map<std::string, MarControlGUI*> registry_;

	QTabWidget* tab_;

	QMainWindow* defaultControlsWindow_;
	QMainWindow* customControlsWindow_;

	QMainWindow* createDefaultControlsWindow();
	virtual QMainWindow* createCustomControlsWindow();

	virtual void updateCustomControlsWindow(MarControl* control);

public:
	MarSystemControlsGUI(MarSystem* msys, QWidget* parent = 0, Qt::WFlags f = 0);
	virtual ~MarSystemControlsGUI();

public slots:
	void updControl(MarControl* control);

//signals:
	//void controlChanged(std::string cname, MarControlValue& cvalue);

};

}//namespace Marsyas

#endif //MARSYAS_QT

#endif //MARSYSTEMCONTROLSGUI_H
