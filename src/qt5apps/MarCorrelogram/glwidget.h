#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include <QTimer>

// Marsyas

#include "../common/marsystem_wrapper.h"

using namespace MarsyasQt;
using namespace Marsyas;

// sness
#define MAX_SPECTRUM_LINES 50
#define SPECTRUM_SIZE 128

#define POWERSPECTRUM_BUFFER_SIZE 257
#define MEMORY_SIZE 300


#define TIMER_COUNT_STEPS 100.0

class GLWidget : public QGLWidget
{
  Q_OBJECT

public:
  GLWidget(const QString & inAudioFileName, QWidget *parent = 0);
  ~GLWidget();

  QSize minimumSizeHint() const;
  QSize sizeHint() const;

public slots:
  void open(); // Open an audio file selected from dialog
  void play( const QString & fileName ); // Open a given audio file
  void playPause(); // Play or pause the playback of the song
  void animate();

protected:
  void initializeGL();                    // Initialize the GL window
  void paintGL();                         // Paint the objects in the GL window
  void resizeGL(int width, int height);   // Resize the GL window

private:
  void redrawScene();

  QString m_audioFileName;

  // A timer to make the animation happen
  QTimer m_updateTimer;

  // Marsyas
  MarSystem* m_marsystem;
  MarsyasQt::System *m_system;
  MarsyasQt::Control *m_fileNameControl;
  MarsyasQt::Control *m_initAudioControl;

  MarsyasQt::Control *m_statsSource;
  MarsyasQt::Control *m_waveformSource;
  MarsyasQt::Control *m_spectrumSource;

  // Maximum data for drawing when scaling
  Marsyas::mrs_realvec max_data;

};

#endif
