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
\class SoundFileSource2ControlsGUI
\brief GUI for viewing/editing MarSystem controls 
*/

#ifndef SoundFileSource2ControlsGUI_H
#define SoundFileSource2ControlsGUI_H

#include "common.h"

#ifdef MARSYAS_QT

#include "MarControl.h"
#include "MarSystemControlsGUI.h"

class QLabel;
class QSlider;

namespace Marsyas
{
	class MarSystem;

	class SoundFileSource2ControlsGUI : public MarSystemControlsGUI
	{
		Q_OBJECT

	private:
		QLabel* filenameLbl_;
		QSlider* posSlider_;
		QLabel* currentPos_;
		bool posSliderPressed_;

		MarControlPtr ctrl_pos_;

		QMainWindow* createCustomControlsWindow();
		void updateCustomControlsWindow(MarControl* control);

	public:
		SoundFileSource2ControlsGUI(MarSystem* msys, QWidget* parent = 0, Qt::WFlags f = 0);
		virtual ~SoundFileSource2ControlsGUI();

	private slots:
		void openAudioFile();
		void posSliderMoved(int value);
		void posSliderPressed();

	};

}//namespace Marsyas

#endif //MARSYAS_QT

#endif //SoundFileSource2ControlsGUI_H
