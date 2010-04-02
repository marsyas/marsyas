/*
** Copyright (C) 1998-2010 George Tzanetakis <gtzan@cs.uvic.ca>
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


#ifndef MARMONITORS_H
#define MARMONITORS_H 

#include "Marx2DGraph.h"
#include "MarSystemManager.h" 
#include "MarSystemQtWrapper.h"
#include "MarSystem.h"
#include "MarControl.h" 
#include <QPushButton> 
#include <QSpinBox>
#include <QDialog>
#include <QLabel> 
#include <QMenuBar>
#include <QMenu>
#include <QListWidget> 
#include <QMainWindow>
#include <QApplication> 
#include <QFileDialog> 
using namespace MarsyasQt;

class MarMonitors : public QMainWindow
{
	Q_OBJECT

	public:
		MarMonitors(string pluginName, string audioInputName);

	public slots: 
		void tick();
		void graph(int size,string xlabel, string label);
		void setup();
		void dialogDone();
		void open();
		void about();  
		

private: 
  void createMenus();
  void createActions();
  void initNetwork(QString pluginName);

  
  string pluginName_;
  string audioInputName_;
  realvec out_;
  QVector<Marx2DGraph*> graphs;
  Marx2DGraph* graph3;
  Marsyas::MarSystem* pnet_;
  MarSystemQtWrapper* mwr_;
  QGridLayout *gridLayout_;
  QWidget* centralWidget_;
  

  QVector<string> probes_;
  QListWidget *listWidget;
  QSpinBox* graphNum;
  std::map<std::string, MarControlPtr> mycontrols_;

	
  int nTicks_;
  int nGraphs_;
  

  QMenu*   fileMenu;  
  QMenu*   helpMenu;
  QAction* openAct;
  QAction* aboutAct;

  
};

	
#endif 
