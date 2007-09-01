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
\class GainControlsGUI
\brief GUI for viewing/editing MarSystem controls 
*/

#ifndef GainControlsGUI_H
#define GainControlsGUI_H

#include "common.h"
#include "MarControl.h"

#ifdef MARSYAS_QT

#include "MarSystemControlsGUI.h"

#include <QVector>

class QProgressBar;
class QSlider;

namespace Marsyas
{
	class MarSystem;

	class GainControlsGUI : public MarSystemControlsGUI
	{
		Q_OBJECT

	private:
		QSlider* gainSlider_;
		QVector<QProgressBar*> inVUmeters_;
		QVector<QProgressBar*> outVUmeters_;

		MarControlPtr ctrl_gain_;

		mrs_natural inChannels_;
		mrs_natural onChannels_;

		QMainWindow* createCustomControlsWindow();
		void updateCustomControlsWindow(MarControl* control);

	public:
		GainControlsGUI(MarSystem* msys, QWidget* parent = 0, Qt::WFlags f = 0);
		virtual ~GainControlsGUI();

	protected slots:
		void gainSliderMoved(int value);

	};

}//namespace Marsyas

#endif //MARSYAS_QT

#endif //GainControlsGUI_H
