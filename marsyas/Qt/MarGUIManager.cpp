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
\class MarGUIManager
\brief A Qt4 GUI factory class that provides GUI widgets for MarSystems

*/

#include "MarGUIManager.h"

#ifdef MARSYAS_QT

#include "MarSystem.h"

#include "MarSystemControlsGUI.h"
#include "GainControlsGUI.h"
#include "SoundFileSource2ControlsGUI.h"

#include <string>

using namespace std;
using namespace Marsyas;

MarGUIManager::MarGUIManager()
{

}

MarGUIManager::~MarGUIManager()
{

}


MarSystemControlsGUI* 
MarGUIManager::getControlsGUI(MarSystem* msys, QWidget* parent , Qt::WFlags f) 
{
	string type = msys->getType();
	
	if(type == "MarSystem")
		return new MarSystemControlsGUI(msys, parent, f);
	
	if(type == "Gain")
		return new GainControlsGUI(msys, parent, f);

	if(type == "SoundFileSource2")
		return new SoundFileSource2ControlsGUI(msys, parent, f);

	return new MarSystemControlsGUI(msys, parent, f);//default controls GUI
}

#endif //MARSYAS_QT