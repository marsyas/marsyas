
#ifndef MARLPCWINDOW_H
#define MARLPCWINDOW_H

#include "realvec_edit.h"
#include "../common/marsystem_wrapper.h"

#include <marsyas/system/MarSystem.h>

#include <QMainWindow>
#include <QSlider>
#include <QTimeEdit>
#include <QTimer>

using namespace Marsyas;
using namespace MarsyasQt;

class MarControlGUI;

class MarLpcWindow : public QMainWindow
{
  Q_OBJECT

public:
  MarLpcWindow();
  MarsyasQt::System *getSystem();
  void play( const QString & fileName ); // Play given audio file.

public slots:
  void open();
  void about();

private slots:
  void posChanged();
  void breathinessChanged(int value);
  void cutOffChanged(int value);
  void updateControls();
  void updateResonanceFilter();
  void updateTiltFilter();

private:
  void createMenus();
  void createActions();
  void createNetwork();



  Marsyas::MarSystem* m_lpc;

  MarsyasQt::System *m_system;
  MarsyasQt::Control *m_fileNameControl;
  MarsyasQt::Control *m_sampleRateControl;
  MarsyasQt::Control *m_sizeControl;
  MarsyasQt::Control *m_posControl;
  MarsyasQt::Control *m_tiltFilterControl;
  MarsyasQt::Control *m_resonanceFilterControl;
  MarsyasQt::Control *m_noiseLevelControl;
  MarsyasQt::Control *m_residualLevelControl;
  MarsyasQt::Control *m_initAudioControl;

  int m_song_len;
  int m_song_sr;

  QSlider * m_posSlider;
  QTimeEdit * m_posDisplay;
  QSlider * m_amplitudePoleSlider;
  QSlider * m_frequencyPoleSlider;
  QSlider * m_breathinessSlider;
  QSlider * m_tiltSlider;
  RealvecView *m_tiltFilterView;
  RealvecView *m_resonanceFilterView;

  QMenu*   fileMenu;
  QMenu*   helpMenu;
  QAction* openAct;
  QAction* aboutAct;

  QTimer m_updateTimer;
};

#endif



