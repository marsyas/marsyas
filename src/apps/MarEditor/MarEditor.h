#ifndef MAREDITOR_H
#define MAREDITOR_H

#include <QtCore>
#include <QtGui>

#include "ui_MarEditor.h"

//#include "SoundDevice.h"
//#include "AudioPlot.h"
//#include "audioproperties.h"
#include "dockwindow.h"

#include <marsyas/system/MarSystem.h>
#include "MarSystemThread.h"


//***********************************************************
// MarEditor class
//***********************************************************
class MarEditor : public QMainWindow, private Ui::MarEditorClass
{
  Q_OBJECT

private:
  enum GUIState {
    ERROR_AUDIO_DEVICE,
    READY_TO_LOAD,
    PLAYING,
    PAUSED,
    STOPPED
  } guiState_;

  //DockWindow* audioDock_;
  //AudioPlot* audioPlot_;

  Marsyas::MarSystemThread* msysThread_;
  Marsyas::MarSystem* msysNet_;

  DockWindow* mainNetworkDockWin_;

  void setGUIstate(GUIState guiState);
  GUIState GUIstate() const {return guiState_;};

  void createActions();

public:
  MarEditor(QWidget *parent = 0, Qt::WFlags flags = 0);
  ~MarEditor();

private slots:
  void openMarSystem();
  void saveMarSystem();

  void run();
  void pause();
  void stop();
  void rewind();

  void showDockWin(QMainWindow* win);

  //void audioDeviceConfigDialog();
};

#endif // MAREDITOR_H
