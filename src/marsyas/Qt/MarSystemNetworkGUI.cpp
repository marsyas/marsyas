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

#include "MarSystemNetworkGUI.h"

#ifdef MARSYAS_QT

#include <QtCore>
#include <QtGui>

#include "MarSystem.h"

using namespace std;
using namespace Marsyas;

MarSystemNetworkGUI::MarSystemNetworkGUI(MarSystem* msys, QWidget* parent, Qt::WFlags f) 
											: QMainWindow(parent, f)
{

	if(!msys)
		return;

	msys_ = msys;
	selectedMsys_ = NULL;

	//Create network tree widget
	QTreeWidget* netViewer = new QTreeWidget;
	netViewer->setColumnCount(1);
	netViewer->setItemHidden(netViewer->headerItem(),true);
	
	//iterate over all children to populate tree viewer
	populateNetworkViewer(msys, netViewer, NULL);

	//connect doubleclick to MarControl GUI
	connect(netViewer, SIGNAL(itemDoubleClicked (QTreeWidgetItem *, int)),
		this, SLOT(msysSelected(QTreeWidgetItem *, int)));

	//set tree viewer as central widget
	setCentralWidget(netViewer);

	//create popup menu actions
	showMarSystemNetworkGUIAct_ = new QAction("Show network...", this);
	showControlsGUIAct_ = new QAction("Show controls...", this);
	showDataGUIAct_ = new QAction("Show data...", this);
	showMATLABeditorGUIAct_ = new QAction("Show MATLAB script...", this);
#ifndef MARSYAS_MATLAB
	showMATLABeditorGUIAct_->setEnabled(false);
#endif
	connect(showMarSystemNetworkGUIAct_, SIGNAL(triggered()), this, SLOT(showMarSystemNetworkGUI()));
	connect(showControlsGUIAct_, SIGNAL(triggered()), this, SLOT(showControlsGUI()));
	connect(showDataGUIAct_, SIGNAL(triggered()), this, SLOT(showDataGUI()));
	connect(showMATLABeditorGUIAct_, SIGNAL(triggered()), this, SLOT(showMATLABeditorGUI()));

}

MarSystemNetworkGUI::~MarSystemNetworkGUI()
{

}

void
MarSystemNetworkGUI::populateNetworkViewer(MarSystem* msys, QTreeWidget* tree, QTreeWidgetItem* parent)
{
	QTreeWidgetItem* item;
	if(tree && msys)
	{
		string prefix = msys->getPrefix();
		prefix = prefix.substr(1, prefix.length()-2);//remove leading and trailing "/" [!]
		item = new QTreeWidgetItem(tree, QStringList(QString::fromStdString(prefix)));
	}
	else if(parent && msys)
	{
		string prefix = msys->getPrefix();
		prefix = prefix.substr(1, prefix.length()-2);//remove leading and trailing "/" [!]
		item = new QTreeWidgetItem(parent, QStringList(QString::fromStdString(prefix)));
	}
	else
	{
		return;
	}

	vector<MarSystem*> children = msys->getChildren();
	if(children.size() != 0)
	{
		vector<MarSystem*>::iterator child;
		for(child = children.begin(); child != children.end(); ++child)
			populateNetworkViewer(*child, NULL, item);
		if(tree)
			tree->expandItem(item);
	}
}

void
MarSystemNetworkGUI::msysSelected(QTreeWidgetItem* item, int column)
{
	//create complete path to selected MarSystem
	string selectedPath = "/" + item->text(0).toStdString(); 
	QTreeWidgetItem* parent = item->parent();
	while(parent)
	{
		selectedPath = "/" + parent->text(0).toStdString() + selectedPath; 
		parent = parent->parent();
	}

	//get the selected MarSystem pointer
	//selectedPath must be in the form:
	// "ftype/fname/type/name/"
	selectedMsys_ = msys_->getChildMarSystem(selectedPath);

	//Create and show pop-up menu
	QMenu msysMenu(this);
	msysMenu.addAction(showMarSystemNetworkGUIAct_);
	msysMenu.addAction(showControlsGUIAct_);
	msysMenu.addAction(showDataGUIAct_);
	msysMenu.addAction(showMATLABeditorGUIAct_);
	msysMenu.exec(QCursor::pos());
}

void
MarSystemNetworkGUI::showMarSystemNetworkGUI()
{
	QMainWindow* gui = selectedMsys_->getMarSystemNetworkGUI(parentWidget());
	//If there is a parent, the GUI main window is returned to it so it can take care of it.
	//Otherwise, a NULL pointer is returned since its display is taken care by the MarSystem object itself. 
	if(gui)
	{
		emit showGUI(gui);
	}
}

void
MarSystemNetworkGUI::showControlsGUI()
{
	QMainWindow* gui = selectedMsys_->getControlsGUI(parentWidget());
	//If there is a parent, the GUI main window is returned to it so it can take care of it.
	//Otherwise, a NULL pointer is returned since its display is taken care by the MarSystem object itself. 
	if(gui)
	{
		emit showGUI(gui);
	}
}

void
MarSystemNetworkGUI::showDataGUI()
{
	QMainWindow* gui = selectedMsys_->getDataGUI(parentWidget());
	//If there is a parent, the GUI main window is returned to it so it can take care of it.
	//Otherwise, a NULL pointer is returned since its display is taken care by the MarSystem object itself. 
	if(gui)
	{
		emit showGUI(gui);
	}
}

void
MarSystemNetworkGUI::showMATLABeditorGUI()
{
	QMainWindow* gui = selectedMsys_->getMATLABeditorGUI(parentWidget());
	//If there is a parent, the GUI main window is returned to it so it can take care of it.
	//Otherwise, a NULL pointer is returned since its display is taken care by the MarSystem object itself. 
	if(gui)
	{
		emit showGUI(gui);
	}
}

#endif //MARSYAS_QT