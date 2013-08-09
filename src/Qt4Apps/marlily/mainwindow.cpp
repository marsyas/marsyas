// C++ stuff for testing
#include <iostream>
using namespace std;

#include "mainwindow.h"

MainWindow::MainWindow()
{
  createMain();
  createActions();
  createMenus();
  createToolBars();
  createStatusBar();
  readSettings();
}

MainWindow::~MainWindow()
{
}

void MainWindow::closeEvent(QCloseEvent *event)
{
  event->accept();
}

void MainWindow::about()
{
  QMessageBox::about(this, tr("About MarLily"),
                     tr("MarLily is a tool to aid investigation of music"
                        "transcription algorithms"));
}

void MainWindow::readSettings()
{
  QSettings settings("MarLily", "MarLily");
  QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
  QSize size = settings.value("size", QSize(700, 400)).toSize();
  resize(size);
  move(pos);
}

void MainWindow::writeSettings()
{
  QSettings settings("MarLily", "MarLily");
  settings.setValue("pos", pos());
  settings.setValue("size", size());
}

void MainWindow::createStatusBar()
{
  normalStatusMessage = new QLabel;
  permanentStatusMessage = new QLabel;
  statusBar()->addWidget(normalStatusMessage);
  statusBar()->addPermanentWidget(permanentStatusMessage);
  statusBar()->showMessage(tr("Ready"));
}

void MainWindow::createMain()
{
  QFrame* centralFrame = new QFrame;
  setCentralWidget(centralFrame);

  pitchPlot = new QtMarPlot;
  ampPlot = new QtMarPlot;

  mainLayout = new QVBoxLayout;
  mainLayout->addWidget(pitchPlot);
  mainLayout->addWidget(ampPlot);
  centralFrame->setLayout(mainLayout);
}

void MainWindow::createMenus()
{
  fileMenu = menuBar()->addMenu(tr("&File"));
  fileMenu->addAction(openUserAct);
  fileMenu->addAction(exitAct);

  menuBar()->addSeparator();

  audioMenu = menuBar()->addMenu(tr("&Audio"));
  audioMenu->addAction(segmentAct);
  audioMenu->addAction(showMusicAct);
  audioMenu->addAction(nextNoteAct);
  audioMenu->addAction(prevNoteAct);


  helpMenu = menuBar()->addMenu(tr("&Help"));
  helpMenu->addAction(aboutAct);
  helpMenu->addAction(aboutQtAct);
}

void MainWindow::createToolBars()
{
}

void MainWindow::createActions()
{
  openUserAct = new QAction(tr("&Open file..."), this);
  openUserAct->setShortcut(tr("Ctrl+O"));
  openUserAct->setStatusTip(tr("Open an existing session"));
  connect(openUserAct, SIGNAL(triggered()), this, SLOT(open()));

  segmentAct = new QAction(tr("S&egment audio ..."), this);
  segmentAct->setShortcut(tr("Ctrl+E"));
  segmentAct->setStatusTip(tr("Segment"));
  connect(segmentAct, SIGNAL(triggered()), this, SLOT(segment()));

  showMusicAct = new QAction(tr("Show &Music..."), this);
  showMusicAct->setShortcut(tr("Ctrl+M"));
  showMusicAct->setStatusTip(tr("Show music"));
  connect(showMusicAct, SIGNAL(triggered()), this, SLOT(showMusic()));

  nextNoteAct = new QAction(tr("&Next note..."), this);
  nextNoteAct->setShortcut(tr("Ctrl+N"));
  nextNoteAct->setStatusTip(tr("nextNote"));
  connect(nextNoteAct, SIGNAL(triggered()), this, SLOT(nextNote()));

  prevNoteAct = new QAction(tr("&Prev note..."), this);
  prevNoteAct->setShortcut(tr("Ctrl+P"));
  prevNoteAct->setStatusTip(tr("prevNote"));
  connect(prevNoteAct, SIGNAL(triggered()), this, SLOT(prevNote()));


  exitAct = new QAction(tr("E&xit"), this);
  exitAct->setShortcut(tr("Ctrl+Q"));
  exitAct->setStatusTip(tr("Exit the application"));
  connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

  aboutAct = new QAction(tr("&About MarLily"), this);
  connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

  aboutQtAct = new QAction(tr("About &Qt"), this);
  aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
  connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

}


void MainWindow::open()
{
  QString openFilename = QFileDialog::getOpenFileName(0,
                         tr("Open File"));
  if (!openFilename.isEmpty())
  {
    string filename = openFilename.toStdString();
// TODO: error
// since this line has a bus error...
// 	   Transcriber::getAllFromAudio(openFilename.toStdString(),
//pitchList, ampList, boundaries);
//    	Transcriber::toMidi(pitchList);

    string readFile;
    readFile = filename;
    readFile.append(".pitches.txt");
    pitchList = new realvec();
    pitchList->readText(readFile);
    readFile = filename;
    readFile.append(".amps.txt");
    ampList = new realvec();
    ampList->readText(readFile);
    readFile = filename;
    readFile.append(".bounds.txt");
    boundaries = new realvec();
    boundaries->readText(readFile);

    currNote=-1;
    display();

  }
}

void MainWindow::segment()
{
  Transcriber::ampSegment(ampList, boundaries);
//	Trasccriber::pitchSegment(pitchList, boundaries);
  pitchSplit = Transcriber::segmentRealvec(pitchList, boundaries);
  ampSplit = Transcriber::segmentRealvec(ampList, boundaries);

  currNote=0;
  display();
}

void MainWindow::message()
{
  QString message;
  if (currNote>-1)
  {
    message = "Note ";
    message.append(QString::number(currNote));
  }
  else
  {
    message = "Entire piece";
  }
  statusBar()->showMessage(message);
}

void MainWindow::nextNote()
{
  currNote++;
  if (currNote > ampSplit->getRows()-1)
    currNote = -1;
  display();
}

void MainWindow::prevNote()
{
  currNote--;
  if (currNote < -1)
    currNote = ampSplit->getRows()-1;
  display();
}

void MainWindow::showMusic()
{

}

void MainWindow::display()
{
  pitchPlot->setPixelWidth(2);
  pitchPlot->setCenterLine(false);
  ampPlot->setVertical(0,1);
  ampPlot->setPlotName("Amplitudes");
  ampPlot->setPixelWidth(2);
  ampPlot->setCenterLine(false);
  if (currNote<0)
  {
    pitchPlot->setData(pitchList);
    ampPlot->setData(ampList);
    QString pitchMessage = "Pitches: ";
    pitchMessage.append(QString::number( pitchList->mean() ));
    pitchPlot->setPlotName(pitchMessage);
    pitchPlot->setVertical(pitchList->minval(),pitchList->maxval());
  }
  else
  {
//		tempPitch = new realvec;
//		tempAmp = new realvec;
//		pitchSplit->getRow(currNote, (*tempPitch));
//		ampSplit->getRow(currNote, (*tempAmp));
    /*
    		pitchPlot->setData(tempPitch);
    		ampPlot->setData(tempAmp);
    		QString ampMessage = "Amplitudes: ";
    		int numVals=0;
    		while (((*tempPitch)(numVals)>0) && (numVals<tempPitch->getCols()))
    			numVals++;
    		ampMessage.append(QString::number( numVals ));
    		ampPlot->setPlotName(ampMessage);
    		QString pitchMessage = "Pitches: ";
    		pitchMessage.append(QString::number( tempPitch->median() ));
    		pitchPlot->setPlotName(pitchMessage);
    		pitchPlot->setVertical(50,80);
    //		pitchPlot->setVertical(tempPitch.minval(),tempPitch.maxval());
    */
  }
  message();
}


