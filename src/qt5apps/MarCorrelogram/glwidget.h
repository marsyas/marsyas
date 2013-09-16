#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include <QTimer>

// Marsyas

#include <marsyasqt/marsystem_wrapper.h>

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

signals:

  void timerChanged(int value);

private slots:


protected:
  void initializeGL();                    // Initialize the GL window
  void paintGL();                         // Paint the objects in the GL window
  void resizeGL(int width, int height);   // Resize the GL window

private:
  GLuint makeObject();                    // Make the object

  // The current x,y,z rotation angles
  int xRot;
  int yRot;
  int zRot;

  // A timer to make the animation happen
  QTimer m_updateTimer;

  // The current tick of the animation
  int timerCount;

  // The vertices we will draw
  GLfloat **vertices;

  void redrawScene();
  void addDataToRingBuffer();

  // Marsyas
  MarSystem* m_marsystem;
  MarsyasQt::System *m_system;
  MarsyasQt::Control *m_fileNameControl;
  MarsyasQt::Control *m_initAudioControl;

  MarsyasQt::Control *m_statsSource;
  MarsyasQt::Control *m_waveformSource;
  MarsyasQt::Control *m_spectrumSource;

  // Scale
  double y_scale;

  float stats_centroid;
  float stats_rolloff;
  float stats_flux;
  float stats_rms;

  // void setAudioStats();

  MarControlPtr posPtr_;
  MarControlPtr initPtr_;
  MarControlPtr fnamePtr_;

  // Maximum data for drawing when scaling
  Marsyas::mrs_realvec max_data;

};

#endif
