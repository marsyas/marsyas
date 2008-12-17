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
\class MATLABeditorGUI
\brief GUI for viewing/editing MATLAB engine scripts for MarSystems 
*/

#ifndef MATLABEDITORGUI_H
#define MATLABEDITORGUI_H

#include "common.h"

#ifdef MARSYAS_QT
#ifdef MARSYAS_MATLAB

#include "QMainWindow"

class QWidget;
class QTextEdit;
class QPushButton;

namespace Marsyas
{
	//class MarSystem;

	class MATLABeditorGUI : public QMainWindow
	{
		Q_OBJECT

	private:
		//MarSystem* msys_;

		QTextEdit* scriptEditor_;
		QPushButton* applyBtn_;
		
		QTextEdit* outputDisplay_;
		//char* outputBuffer_;

	public:
		MATLABeditorGUI(std::string script, QWidget* parent = 0, Qt::WFlags f = 0);
		~MATLABeditorGUI();

	private slots:
		void enableApplyBtn();
		void updateScript();
		//void updateOutputDisplay();
		void openMfile();
		void saveMfile();

		public slots:
		void setScript(std::string script);

 	signals:
 		void scriptChanged(std::string script);

	};

}//namespace Marsyas

#endif //MARSYAS_MATLAB
#endif //MARSYAS_QT

#endif //MATLABEDITORGUI_H
