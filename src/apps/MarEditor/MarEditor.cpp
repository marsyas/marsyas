/*
** Copyright (C) 2000-2010 George Tzanetakis <gtzan@cs.uvic.ca>
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



#include "MarEditor.h"

// #include <QFileDialog>
// #include <QMessageBox>
// #include <QDockWidget>
// #include <QTextEdit>

#include <vector>
#include <fstream>

//Marsyas0.2
#include <marsyas/system/MarSystemManager.h>
#include <marsyas/system/MarSystem.h>

#ifdef MARSYAS_MATLAB
#include "MATLABengine.h"
#endif

using namespace Ui;
using namespace Marsyas;
using namespace std;

//***********************************************************************
//	MarEditor Class
//***********************************************************************
MarEditor::MarEditor(QWidget *parent, Qt::WFlags flags)
  : QMainWindow(parent, flags)
{
  setupUi(this);

  //set central widget
  QTextEdit* textEdit = new QTextEdit;
  setCentralWidget(textEdit);

  createActions();
  //createDockWindows();

  msysNet_ = NULL;
  msysThread_ = NULL;
  mainNetworkDockWin_ = NULL;

  // create a default MarSystem network
  MarSystemManager mng;
  msysNet_ = mng.create("Series", "PlayBackNet");
  msysNet_->addMarSystem(mng.create("SoundFileSource2", "src"));
  //msysNet_->addMarSystem(mng.create("Gain", "gain"));
  MarSystem* gainNet = mng.create("Series", "GainNet");
  gainNet->addMarSystem(mng.create("Gain", "gain1"));
  gainNet->addMarSystem(mng.create("Gain", "gain2"));
  msysNet_->addMarSystem(gainNet);
  msysNet_->addMarSystem(mng.create("AudioSink2", "dest"));
  msysNet_->linkctrl("mrs_string/filename", "SoundFileSource2/src/mrs_string/filename");
  msysNet_->linkctrl("mrs_natural/size", "SoundFileSource2/src/mrs_natural/size");
  msysNet_->linkctrl("mrs_natural/pos", "SoundFileSource2/src/mrs_natural/pos");
  msysNet_->linkctrl("mrs_bool/hasData", "SoundFileSource2/src/mrs_bool/hasData");

  // make a Qt-like thread object wrapped around the MarSystem
  msysThread_ = new MarSystemThread(msysNet_);
  //connect signals and slots of MarSystemThread
  connect(msysThread_, SIGNAL(Empty()), this, SLOT(stop()));
  msysThread_->start(); //start thread (although it will start as sleeping)
  msysThread_->setPriority(QThread::HighestPriority);

  //create a networkViewer
  QMainWindow* netViewer = msysNet_->getMarSystemNetworkGUI(this);
  connect(netViewer, SIGNAL(showGUI(QMainWindow*)), this, SLOT(showDockWin(QMainWindow*)));
  //Create Dock Window wrapping this viewer
  mainNetworkDockWin_ = new DockWindow(netViewer->windowTitle(), this);
  mainNetworkDockWin_->setWidget(netViewer);
  mainNetworkDockWin_->setAttribute(Qt::WA_DeleteOnClose, false);
  mainNetworkDockWin_->setAllowedAreas(Qt::AllDockWidgetAreas);
  addDockWidget(Qt::LeftDockWidgetArea, mainNetworkDockWin_);
  //add this DockWindow to the Windows Menu
  menuWindows->addAction(mainNetworkDockWin_->toggleViewAction());

  setGUIstate(STOPPED);

// 	//----------------------------------------
// 	//Main MarSystem Network Viewer
// 	//----------------------------------------
// 	QMainWindow* netViewer = msysNet_->getMarSystemNetworkGUI(this);
// 	Q_ASSERT(connect(netViewer, SIGNAL(showGUI(QMainWindow*)), this, SLOT(showDockWin(QMainWindow*))));
// 	//Create Dock Window wrapping this viewer
// 	DockWindow* winDock = new DockWindow(netViewer->windowTitle(), this);
// 	winDock->setWidget(netViewer);
// 	winDock->setAttribute(Qt::WA_DeleteOnClose, false);
// 	winDock->setAllowedAreas(Qt::AllDockWidgetAreas);
// 	addDockWidget(Qt::LeftDockWidgetArea, winDock);
// 	//add this DockWindow to the Windows Menu
// 	menuWindows->addAction(winDock->toggleViewAction());


  //audioPlot_ = NULL;
  //audioDock_ = NULL;

  //setGUIstate(READY_TO_LOAD);
}

MarEditor::~MarEditor()
{
  msysThread_->stop();
  msysThread_->wait();
  delete msysThread_;

  delete msysNet_;
}

void
MarEditor::setGUIstate(GUIState guiState)
{
  guiState_ = guiState;

  switch(guiState)
  {
  case ERROR_AUDIO_DEVICE:
    actionAudioSettings->setEnabled(true);
    actionOpen->setEnabled(false);
    actionPlay->setEnabled(false);
    actionPause->setEnabled(false);
    actionStop->setEnabled(false);
    actionRewind->setEnabled(false);
    break;
  case READY_TO_LOAD:
    actionAudioSettings->setEnabled(true);
    actionOpen->setEnabled(true);
    actionPlay->setEnabled(false);
    actionPause->setEnabled(false);
    actionStop->setEnabled(false);
    actionRewind->setEnabled(false);
    break;
  case PAUSED:
    actionAudioSettings->setEnabled(true);
    actionOpen->setEnabled(true);
    actionPlay->setEnabled(true);
    actionPause->setEnabled(false);
    actionStop->setEnabled(false);
    actionRewind->setEnabled(true);
    break;
  case STOPPED:
    actionAudioSettings->setEnabled(true);
    actionOpen->setEnabled(true);
    actionPlay->setEnabled(true);
    actionPause->setEnabled(false);
    actionStop->setEnabled(false);
    actionRewind->setEnabled(false);
    break;
  case PLAYING:
    actionAudioSettings->setEnabled(false);
    actionOpen->setEnabled(false);
    actionPlay->setEnabled(false);
    actionPause->setEnabled(true);
    actionStop->setEnabled(true);
    actionRewind->setEnabled(true);
    break;
  default:
    actionAudioSettings->setEnabled(true);
    actionOpen->setEnabled(true);
    actionPlay->setEnabled(false);
    actionPause->setEnabled(false);
    actionStop->setEnabled(false);
    actionRewind->setEnabled(false);
  }
}

// void
// MarEditor::audioDeviceConfigDialog()
// {
// 	audioDevice_->destroySoundDevice();
//
// 	AudioDeviceConfig adcDialog(this, audioDevice_);
// 	adcDialog.exec();
//
// 	if(!audioDevice_->updateSoundDevice())
// 		setGUIstate(ERROR_AUDIO_DEVICE);
// }

// bool
// MarEditor::configureAudioDevice(Signal* audio)
// {
// 	audioDevice_->setOutChannels(audio->channels());
// 	audioDevice_->setInChannels(0);
// 	audioDevice_->setSampleRate(audio->srate());
// 	audioDevice_->setBits(audio->bits());
//
// 	if(!audioDevice_->updateSoundDevice())
// 	{
// 		setGUIstate(ERROR_AUDIO_DEVICE);
// 		return false;
// 	}
// 	else return true;
// }

void
MarEditor::createActions()
{
  // QActions are already created using Qt Designer and defined in ui_MarEditor.h
  // just connect their signals to MarEditor custom slots
  //connect(actionAudioSettings, SIGNAL(triggered()), this, SLOT(audioDeviceConfigDialog()));
  connect(actionOpen, SIGNAL(triggered()), this, SLOT(openMarSystem()));
  connect(actionSave, SIGNAL(triggered()), this, SLOT(saveMarSystem()));
  connect(actionPlay, SIGNAL(triggered()), this, SLOT(run()));
  connect(actionPause, SIGNAL(triggered()), this, SLOT(pause()));
  connect(actionStop, SIGNAL(triggered()), this, SLOT(stop()));
  connect(actionRewind, SIGNAL(triggered()), this, SLOT(rewind()));
}

void
MarEditor::openMarSystem()
{
  QString filename = QFileDialog::getOpenFileName(this);
  if (filename.isEmpty())
    return;

  // destroy existing MarSystem Thread (if any)
  if(msysThread_)
  {
    if(msysThread_->isRunning())
    {
      msysThread_->stop();
      msysThread_->wait();
    }
    delete msysThread_;
    msysThread_ = NULL;
  }

  // read the plugin from the file
  ifstream pluginStream(filename.toStdString().c_str());
  MarSystemManager mngr;
  delete msysNet_;
  msysNet_ = mngr.getMarSystem(pluginStream);
  if (msysNet_ == NULL)
  {
    QMessageBox::critical(this, "MarEditor",
                          "Unable to load MarSystem plugin.\n"
                          "Please open another file");

    setGUIstate(READY_TO_LOAD);
    return;
  }

  //msysNet_->update();

  //add plugin to the processing thread
  msysThread_ = new MarSystemThread(msysNet_);
  //connect signals and slots of MarSystemThread
  connect(msysThread_, SIGNAL(Empty()), this, SLOT(stop()));
  msysThread_->start(); //start thread (although it will start as sleeping)
  msysThread_->setPriority(QThread::HighestPriority);

  //----------------------------------------
  //create a MarSystem Network Viewer
  //----------------------------------------
  QMainWindow* netViewer = msysNet_->getMarSystemNetworkGUI(this);
  connect(netViewer, SIGNAL(showGUI(QMainWindow*)), this, SLOT(showDockWin(QMainWindow*)));

  //Create Dock Window wrapping this viewer
  delete mainNetworkDockWin_;
  mainNetworkDockWin_ = new DockWindow(netViewer->windowTitle(), this);
  mainNetworkDockWin_->setWidget(netViewer);
  mainNetworkDockWin_->setAttribute(Qt::WA_DeleteOnClose, false);
  mainNetworkDockWin_->setAllowedAreas(Qt::AllDockWidgetAreas);
  addDockWidget(Qt::LeftDockWidgetArea, mainNetworkDockWin_);
  //add this DockWindow to the Windows Menu
  menuWindows->addAction(mainNetworkDockWin_->toggleViewAction());

  setGUIstate(STOPPED);

  //---------------------------------
  //create audio properties window
  //---------------------------------
// 	if(audioPropWindow_)
// 		delete audioPropWindow_; //[!]
// 	audioPropWindow_ = new AudioProperties(this, audioFile_);
// 	DockWindow* audioPropDock = new DockWindow(tr("Audio Properties"), this);
// 	audioPropDock->setAllowedAreas(Qt::AllDockWidgetAreas);
// 	audioPropDock->setWidget(audioPropWindow_);
// 	addDockWidget(Qt::TopDockWidgetArea, audioPropDock);
// 	//add this DockWindow to the Windows Menu
// 	menuWindows->addAction(audioPropDock->toggleViewAction());
//
//
// 	//plot audio file
// 	plotAudioWaveform((Signal*)audioFile_);
//
// 	//setup audio device
// 	if (!configureAudioDevice((Signal*)audioFile_))
// 		return;
}

void
MarEditor::saveMarSystem()
{
  QString filename = QFileDialog::getSaveFileName(this);
  if (filename.isEmpty())
    return;

  ofstream oss(filename.toStdString().c_str());
  oss << (*msysNet_) << endl;
}

// void
// MarEditor::plotAudioWaveform(Signal* audio)
// {
// 	if(audioDock_)
// 		delete audioDock_;
// 	audioDock_ = new DockWindow(tr("Audio Waveform"), this);
// 	audioDock_->setAllowedAreas(Qt::AllDockWidgetAreas);
//
// 	audioPlot_ = new AudioPlot(audioDock_);
// 	audioPlot_->setSrc(audio);
// 	audioPlot_->insertAudioCurve("Audio Waveform", audio->getAudio());
// 	audioPlot_->setAxisScale(QwtPlot::xBottom,0.0,100.0,10.0);
//
// 	audioDock_->setWidget(audioPlot_);
// 	addDockWidget(Qt::TopDockWidgetArea, audioDock_);
//
// 	//add this DockWindow to the Windows Menu
// 	menuWindows->addAction(audioDock_->toggleViewAction());
//
// 	//-----------------------
//
// 	if(audioDock2_)
// 		delete audioDock2_;
// 	audioDock2_ = new DockWindow(tr("Audio Waveform 2"), this);
// 	audioDock2_->setAllowedAreas(Qt::AllDockWidgetAreas);
//
//
// 	audioPlot2_ = new AudioPlot(audioDock2_);
// 	audioPlot2_->setSrc(audio);
// 	audioPlot2_->insertAudioCurve("Audio Waveform 2", audio->getAudio());
// 	audioPlot2_->setAxisScale(QwtPlot::xBottom,0.0,100.0,10.0);
//
// 	audioDock2_->setWidget(audioPlot2_);
// 	addDockWidget(Qt::BottomDockWidgetArea, audioDock2_);
//
// 	//add this DockWindow to the Windows Menu
// 	menuWindows->addAction(audioDock2_->toggleViewAction());
//
//
// 	//-----------------------
// 	if(!QObject::connect(audioPlot_->getSyncPicker(), SIGNAL(MouseCursorMoved(QwtDoublePoint&, QMouseEvent*)),
// 		audioPlot2_->getSyncPicker(),  SLOT(MoveMouseCursor(QwtDoublePoint&, QMouseEvent*))))
// 		QMessageBox::warning(this,"Signal/Slot Error", "Error on connection!",QMessageBox::Ok,
// 		QMessageBox::NoButton,QMessageBox::NoButton);
//
// 	if(!QObject::connect(audioPlot2_->getSyncPicker(), SIGNAL(MouseCursorMoved(QwtDoublePoint&, QMouseEvent*)),
// 		audioPlot_->getSyncPicker(),  SLOT(MoveMouseCursor(QwtDoublePoint&, QMouseEvent*))))
// 		QMessageBox::warning(this,"Signal/Slot Error", "Error on connection!",QMessageBox::Ok,
// 		QMessageBox::NoButton,QMessageBox::NoButton);
// }

void
MarEditor::run()
{
  if(GUIstate() == STOPPED || GUIstate() == PAUSED)
  {
    msysNet_->updctrl("mrs_bool/active", true);

    //if(!msysThread_->isRunning())
    msysThread_->wakeup();//start();

    setGUIstate(PLAYING);
  }
}

void
MarEditor::pause()
{
  if(GUIstate() == PLAYING)
  {
    //if(msysThread_->isRunning())
    msysThread_->sleep();//stop();

    msysNet_->updctrl("mrs_bool/active", false);

    setGUIstate(PAUSED);
  }
}

void
MarEditor::stop()
{
  if(GUIstate() == PLAYING)
  {
    //if(msysThread_->isRunning())
    msysThread_->sleep();//stop();

    msysNet_->updctrl("mrs_bool/active", false);

    rewind();

    setGUIstate(STOPPED);
  }
}

void
MarEditor::rewind()
{
  if(GUIstate() == PLAYING || GUIstate() == STOPPED || GUIstate() == PAUSED)
  {
    msysNet_->updctrl("mrs_natural/pos",0);
    msysNet_->updctrl("mrs_bool/hasData", (bool)msysNet_->getctrl("mrs_natural/size")->to<mrs_natural>());
  }
}

void
MarEditor::showDockWin(QMainWindow* win)
{
  //Create Dock Window wrapping a QMainWindow
  DockWindow* winDock = new DockWindow(win->windowTitle(), this);
  winDock->setAllowedAreas(Qt::AllDockWidgetAreas);
  winDock->setWidget(win);
  winDock->setAttribute(Qt::WA_DeleteOnClose, true);
  //addDockWidget(Qt::LeftDockWidgetArea, winDock);
  winDock->setFloating(true);

  //add this DockWindow to the Windows Menu
  menuWindows->addAction(winDock->toggleViewAction());

  winDock->move(QCursor::pos());
  //winDock->setWindowOpacity(0.5);
  winDock->show();
}
