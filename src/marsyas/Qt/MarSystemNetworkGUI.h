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
\class MarSystemNetworkGUI
\brief GUI for viewing composite MarSystems 
*/

#ifndef MARSYSTEMNETWORKGUI_H
#define MARSYSTEMNETWORKGUI_H

#include "common.h"

#ifdef MARSYAS_QT

#include <QMainWindow>

class QWidget;
class QTreeWidget;
class QTreeWidgetItem;
class QAction;

namespace Marsyas
{
	class MarSystem;

	class MarSystemNetworkGUI : public QMainWindow
	{
		Q_OBJECT

	private:
		MarSystem* msys_;
		MarSystem* selectedMsys_;

		QAction* showMarSystemNetworkGUIAct_; 
		QAction* showControlsGUIAct_;
		QAction* showDataGUIAct_;
		QAction* showMATLABeditorGUIAct_;

		void populateNetworkViewer(Marsyas::MarSystem* msys, QTreeWidget* tree, QTreeWidgetItem* parent);

	public:
		MarSystemNetworkGUI(Marsyas::MarSystem* msys, QWidget* parent = 0, Qt::WFlags f = 0);
		~MarSystemNetworkGUI();

	private slots:
		void msysSelected(QTreeWidgetItem *item, int column);
		
		void showMarSystemNetworkGUI();
		void showControlsGUI();
		void showDataGUI();
		void showMATLABeditorGUI();

	signals:
		void showGUI(QMainWindow* gui);

	};

}//namespace Marsyas

#endif //MARSYAS_QT

#endif //MARSYSTEMNETWORKGUI_H
