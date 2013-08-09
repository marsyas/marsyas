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
  // playLabel = new QLabel("Hello");
  trainLabel = new QLabel("Train File: \t ./margrid_train.mf");
  predictLabel = new QLabel("Predict File: \t ./margrid_test.mf");
  gridHeightLabel = new QLabel("Grid Height: ");
  gridWidthLabel = new QLabel("Grid Width: ");
  gridWidth = new QLineEdit(this);
  gridWidth->setMinimumWidth(30);
  gridWidth->setMaximumWidth(30);
  gridWidth->setInputMask("99");
  gridWidth->setText("12");
  gridHeight = new QLineEdit(this);
  gridHeight->setInputMask("99");
  gridHeight->setMinimumWidth(30);
  gridHeight->setMaximumWidth(30);
  gridHeight->setText("12");

  QWidget *margrid = new MarGrid();
  connect(this, SIGNAL(trainFile(QString)), margrid, SLOT(setupTrain(QString)));
  connect(this, SIGNAL(predictFile(QString)), margrid, SLOT(setupPredict(QString)));
  connect(this, SIGNAL(playbackMode(bool)), margrid, SLOT(setPlaybackMode(bool)));
  connect(this, SIGNAL(blackwhiteMode(bool)), margrid, SLOT(setBlackWhiteMode(bool)));
  connect(this, SIGNAL(openPredictGridFile(QString)), margrid, SLOT(openPredictionGrid(QString)));
  connect(this, SIGNAL(savePredictGridFile(QString)), margrid, SLOT(savePredictionGrid(QString)));

  QWidget *gridWidthWidget = new QWidget();
  QHBoxLayout *gridWidthLayout = new  QHBoxLayout;
  gridWidthLayout->addWidget(gridWidthLabel);
  gridWidthLayout->addWidget(gridWidth);
  gridWidthWidget->setLayout(gridWidthLayout);

  QWidget *gridHeightWidget = new QWidget();
  QHBoxLayout *gridHeightLayout = new  QHBoxLayout;
  gridHeightLayout->addWidget(gridHeightLabel);
  gridHeightLayout->addWidget(gridHeight);
  gridHeightWidget->setLayout(gridHeightLayout);


  QGridLayout *gridLayout = new QGridLayout;
  gridLayout->addWidget(trainLabel, 0, 0);
  gridLayout->addWidget(gridHeightWidget, 0, 1, 1, 2);
  gridLayout->addWidget(predictLabel, 1, 0);
  gridLayout->addWidget(gridWidthWidget, 1, 1, 1,2 );
  gridLayout->addWidget(extract, 2, 0);
  gridLayout->addWidget(train, 2, 1);
  gridLayout->addWidget(predict, 2, 2);


  // gridLayout->addWidget(gridHeightWidget,3,0, 1, 3);
  // gridLayout->addWidget(gridWidthWidget,4,0);
  // gridLayout->addWidget(playLabel, 5, 0, 1, 3);
  gridLayout->addWidget(margrid, 6, 0, 1, 3);

  connect(extract, SIGNAL(clicked()), margrid, SLOT(extract()));
  connect(train, SIGNAL(clicked()), margrid, SLOT(train()));
  connect(predict, SIGNAL(clicked()), margrid, SLOT(predict()));
  connect(gridWidth, SIGNAL(textChanged(QString)), margrid, SLOT(setXGridSize(QString)));
  connect(gridHeight, SIGNAL(textChanged(QString)), margrid, SLOT(setYGridSize(QString)));
  connect(margrid, SIGNAL(playingFile(QString)), this, SLOT(playingFile(QString)));
//   connect(margrid, SIGNAL(newGridSize(int, int)), this, SLOT(resizeGrid(int, int)));

  w->setLayout(gridLayout);

}


void
MarGridWindow::playingFile(QString s)
{
  // playLabel->setText(s);
}

void
MarGridWindow::createMenus()
{
  fileMenu = menuBar()->addMenu(tr("&File"));
  fileMenu->addAction(openPredictAct);
  fileMenu->addAction(openTrainAct);
  fileMenu->addAction(playbackAct);
  fileMenu->addAction(blackwhiteAct);
  fileMenu->addSeparator();
  fileMenu->addAction(openPredictGridAct);
  fileMenu->addAction(savePerdictGridAct);


  menuBar()->addSeparator();
  helpMenu = menuBar()->addMenu(tr("&Help"));
  helpMenu->addAction(aboutAct);


}


void
MarGridWindow::openTrainFile()
{
  QString fileName = QFileDialog::getOpenFileName(this);
  cout << "fileName = " << fileName.toStdString() << endl;
  trainLabel->setText("TrainFile: \t" + fileName);
  emit trainFile(fileName);
}


void
MarGridWindow::openPredictFile()
{
  QString fileName = QFileDialog::getOpenFileName(this);
  predictLabel->setText("PredictFile: \t" + fileName);
  emit predictFile(fileName);
}

void
MarGridWindow::openPredictionGrid()
{
  QString fileName = QFileDialog::getOpenFileName(this);
  cout << "Emit" << endl;
  emit openPredictGridFile(fileName);
}

void
MarGridWindow::savePredictionGrid()
{
  QString fileName = QFileDialog::getSaveFileName(this);
  cout << "Save" << endl;
  emit savePredictGridFile(fileName);
}

void
MarGridWindow::resizeGrid(int height, int width)
{
  gridHeight->setText("" + height);
  gridWidth->setText("" + width);
}





void
MarGridWindow::createActions()
{

  openTrainAct = new QAction(tr("&Open Collection File for Training"), this);
  openTrainAct->setStatusTip(tr("Open Collection File for Training"));
  connect(openTrainAct, SIGNAL(triggered()), this, SLOT(openTrainFile()));

  openPredictAct = new QAction(tr("&Open Collection File for Predicting"), this);
  openPredictAct->setStatusTip(tr("Open Collection File for Prediciting"));
  connect(openPredictAct, SIGNAL(triggered()), this, SLOT(openPredictFile()));

  // connect(openAct, SIGNAL(triggered()), this, SLOT(open()));
  aboutAct = new QAction(tr("&About"), this);
  aboutAct->setStatusTip(tr("Show the application's About box"));
  connect(aboutAct, SIGNAL(triggered(bool)), this, SLOT(about()));

  openPredictGridAct = new QAction(tr("&Open Saved Prediction Grid"), this);
  openPredictGridAct->setStatusTip(tr("Open Saved Prediction Grid"));
  connect(openPredictGridAct, SIGNAL(triggered(bool)), this, SLOT(openPredictionGrid()));

  savePerdictGridAct = new QAction(tr("&Save Prediction Grid"),this);
  savePerdictGridAct->setStatusTip(tr("Save Prediction Grid"));
  connect(savePerdictGridAct, SIGNAL(triggered(bool)), this, SLOT(savePredictionGrid()));

  playbackAct = new QAction(tr("&Continuous Playback mode"), this);
  playbackAct->setStatusTip(tr("Continuous Playback mode"));
  playbackAct->setCheckable(true);
  connect(playbackAct, SIGNAL(toggled(bool)), this, SIGNAL(playbackMode(bool)));

  blackwhiteAct = new QAction(tr("&Display as Black and White only"), this);
  blackwhiteAct->setStatusTip(tr("Display as Black and White only"));
  blackwhiteAct->setCheckable(true);
  connect(blackwhiteAct, SIGNAL(toggled(bool)), this, SIGNAL(blackwhiteMode(bool)));
}


void
MarGridWindow::about()
{
  QMessageBox::about(this, tr("Marsyas MarGrid"),  tr("Marsyas MarGrid: Demonstrates continuous-feedback \n content-based music browsing using Self Organizing Maps"));

}
