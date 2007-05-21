/****************************************************************************
**
** Copyright (C) 2005-2005 Trolltech AS. All rights reserved.
**
** This file is part of the example classes of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.trolltech.com/products/qt/opensource.html
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://www.trolltech.com/products/qt/licensing.html or contact the
** sales department at sales@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include <QtGui>

#include "MarGridWindow.h"




MarGridWindow::MarGridWindow()
{
  QWidget *w = new QWidget;
  setCentralWidget(w);

  createActions();
  createMenus();  

  

  
  QPushButton *extract  = new QPushButton(tr("Extract"));
  QPushButton *train = new QPushButton(tr("Train"));
  QPushButton *predict = new QPushButton(tr("Predict"));
  playLabel = new QLabel("Hello");
  
  QWidget *margrid = new MarGrid();

  connect(this, SIGNAL(trainFile(QString)), margrid, SLOT(setup(QString)));

  QGridLayout *gridLayout = new QGridLayout;
  gridLayout->addWidget(extract, 0, 0);
  gridLayout->addWidget(train, 0, 1);
  gridLayout->addWidget(predict, 0, 2);
  gridLayout->addWidget(playLabel, 1, 0, 1, 3);
  gridLayout->addWidget(margrid, 2, 0, 1, 3);

  connect(extract, SIGNAL(clicked()), margrid, SLOT(extract()));
  connect(train, SIGNAL(clicked()), margrid, SLOT(train()));
  connect(predict, SIGNAL(clicked()), margrid, SLOT(predict()));
  connect(margrid, SIGNAL(playingFile(QString)), this, SLOT(playingFile(QString))); 


  w->setLayout(gridLayout);

}


void
MarGridWindow::playingFile(QString s) 
{
  playLabel->setText(s);
}

void
MarGridWindow::createMenus()
{
  fileMenu = menuBar()->addMenu(tr("&File"));
  fileMenu->addAction(openTrainAct);
  menuBar()->addSeparator();
  helpMenu = menuBar()->addMenu(tr("&Help"));
  helpMenu->addAction(aboutAct);
}


void 
MarGridWindow::openTrainFile()
{
  QString fileName = QFileDialog::getOpenFileName(this);
  cout << "fileName = " << fileName.toStdString() << endl;
  emit trainFile(fileName);
}


void 
MarGridWindow::createActions()
{
  
  openTrainAct = new QAction(tr("&Open Training File"), this);
  openTrainAct->setStatusTip(tr("Open Collection File for Training"));
  connect(openTrainAct, SIGNAL(triggered()), this, SLOT(openTrainFile()));
  

  // connect(openAct, SIGNAL(triggered()), this, SLOT(open()));
  aboutAct = new QAction(tr("&About"), this);
  aboutAct->setStatusTip(tr("Show the application's About box"));
  connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));
}


	


void 
MarGridWindow::about()
{
  QMessageBox::about(this, tr("Marsyas MarGrid"),  tr("Marsyas MarGrid: Demonstrates continuous-feedback \n content-based music browsing using Self Organizing Maps"));
  
}
