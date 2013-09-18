
#ifndef MARPHASEVOCODERWINDOW_H
#define MARPHASEVOCODERWINDOW_H

#include "../common/marsystem_wrapper.h"

#include <QMainWindow>
#include <QMenu>
#include <QSlider>
#include <QLineEdit>
#include <QTimeEdit>
#include <QAction>
#include <QTimer>

using namespace Marsyas;
using namespace MarsyasQt;

class MarPhasevocoderWindow : public QMainWindow
{
  Q_OBJECT

public:
  MarPhasevocoderWindow(const QString & inAudioFileName);

public slots:
  void about();
  void volumeChanged(int value);
  void freqChanged(int value);
  void sinusoidsChanged(int value);
  void posChanged();
  void open();
  void play(const QString & fileName );

private slots:
  void updateControls();

private:
  void createMenus();
  void createActions();
  void createNetwork();
  void applyTimeStretch(int value);

  Marsyas::MarSystem *m_marsystem;
  MarsyasQt::System* m_system;

  MarsyasQt::Control *m_filenameControl;
  MarsyasQt::Control *m_gainControl;
  MarsyasQt::Control *m_posControl;
  MarsyasQt::Control *m_sizeControl;
  MarsyasQt::Control *m_sampleRateControl;
  MarsyasQt::Control *m_freqControl;
  MarsyasQt::Control *m_interpolationControl1;
  MarsyasQt::Control *m_interpolationControl2;
  MarsyasQt::Control *m_sinesControl;
  MarsyasQt::Control *m_initAudioControl;

  QSlider* m_posSlider;
  QSlider* m_timeSlider;
  QTimeEdit *m_posDisplay;

  QMenu*   fileMenu;
  QMenu*   helpMenu;
  QAction* openAct;
  QAction* aboutAct;

  QTimer m_updateTimer;
  int m_song_length;
  double m_song_sr;
};

#endif


