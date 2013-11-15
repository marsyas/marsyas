#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <config.h> // needed for MARSYAS_MACOSX

#include <QGLWidget>
#include <QTimer>

#ifdef MARSYAS_MACOSX
#  include <OpenGL/glu.h>
#else
#  include <GL/glu.h>
#endif

// Marsyas
#include "../common/marsystem_wrapper.h"

using namespace Marsyas;
using namespace MarsyasQt;

// sness
#define MAX_Z 50
#define MAX_SPECTRUM_BINS 65536
#define MAX_STEREO_SPECTRUM_BINS 65536

#define TIMER_COUNT_STEPS 100.0

class GLWidget : public QGLWidget
{
  Q_OBJECT

public:
  GLWidget(QWidget *parent = 0);
#if 0
  void GLWidget_other_constructor(string inAudioFileName,QWidget *parent = 0);
#endif
  ~GLWidget();

  QSize minimumSizeHint() const;
  QSize sizeHint() const;

public slots:
  void open(); // Open a new audio file.
  void play( const QString & fileName ); // Play given audio file.
  void playPause(); // Toggle between playing and paused.

  void setXRotation(int angle);
  void setYRotation(int angle);
  void setZRotation(int angle);

  void setXTranslation(int val);
  void setYTranslation(int val);
  void setZTranslation(int val);

  void setFogStart(int val);
  void setFogEnd(int val);

  void setFFTBins(int val);

  void setMagnitudeCutoff(int v);
  void setNumVertices(int v);
  void setDotSize(int v);
  void setSongPosition(int v);
  void setDisplaySpeed(int v);

  void setPos(int value);

  //   void setYScale(int scale);

  //   void powerSpectrumModeChanged(int val);

  //   void setWaterfallVisible(bool val);

  void animate();


signals:
  void xRotationChanged(int angle);
  void yRotationChanged(int angle);
  void zRotationChanged(int angle);

  void xTranslationChanged(int angle);
  void yTranslationChanged(int angle);
  void zTranslationChanged(int angle);

  void fogStartChanged(int angle);
  void fogEndChanged(int angle);

  void posChanged(int val);

  //   void start_xRotationChanged(int angle);
  //   void start_yRotationChanged(int angle);
  //   void start_zRotationChanged(int angle);

  //   void end_xRotationChanged(int angle);
  //   void end_yRotationChanged(int angle);
  //   void end_zRotationChanged(int angle);

  //   void rotationSpeedChanged(int angle);

  void timerChanged(int value);


private slots:
  //   void startTimerRotate();                // Start the animation timer
  //   void doTimerRotate();                   // Do one time step of the animation timer

protected:
  void initializeGL();                    // Initialize the GL window
  void paintGL();                         // Paint the objects in the GL window
  void resizeGL(int width, int height);   // Resize the GL window

private:
  GLuint makeObject();                    // Make the object
  void normalizeAngle(int *angle);        // Normalize an angle given in degrees

  GLuint object;                          // The object that we are drawing

  // The current x,y,z rotation angles
  int xRot;
  int yRot;
  int zRot;

  double xTrans;
  double yTrans;
  double zTrans;

  double fogStart;
  double fogEnd;


  //   // The start x,y,z rotation angles for the animation
  //   int start_xRot;
  //   int start_yRot;
  //   int start_zRot;

  //   // The end x,y,z rotation angles for the animation
  //   int end_xRot;
  //   int end_yRot;
  //   int end_zRot;

  //   // The rotation speed
  //   int rotation_speed;

  // A timer to make the animation happen
  QTimer m_updateTimer;

  // The current tick of the animation
  int timerCount;

  //   // The vertices we will draw
  //   GLfloat **vertices;

  //   void drawCubeFace(int,int,int,int);

  void redrawScene();
  void addDataToRingBuffer();
  void emitControlChanges();

  // Marsyas
  MarSystem* m_marsystem;
  MarsyasQt::System *m_system;
  MarsyasQt::Control *m_fileNameControl;
  MarsyasQt::Control *m_sampleRateControl;
  MarsyasQt::Control *m_blockSizeControl;
  MarsyasQt::Control *m_sizeControl;
  MarsyasQt::Control *m_posControl;
  MarsyasQt::Control *m_initAudioControl;

  MarsyasQt::Control *m_spectrumControl;
  MarsyasQt::Control *m_panningControl;

  int m_song_len;
  int m_song_sr;

  // A ring buffer that holds our data
  double **powerspectrum_ring_buffer;
  double **panning_spectrum_ring_buffer;
  int ring_buffer_pos; // The current head position in the ring buffer

  // Scale
  double y_scale;

  // Where should the z start? (Used for drawing guidelines)
  double z_start;

  // How big the dots should be
  double dot_size_multiplier;


  // Test variables
  double test_x;
  double test_y;
  double test_z;

  // Windowed waveform for display
  mrs_realvec waveform_data;
  void setWaveformData();

  float stats_centroid;
  float stats_rolloff;
  float stats_flux;
  float stats_rms;

  //   void setAudioStats();


  // The disk object
  GLUquadricObj *qobj;
  GLuint startList;

  void errorCallback(GLenum errorCode);

  int stereo_spectrum_bins;
  int spectrum_bins;

  void set_fft_size(int);

  //   int num_triangles;

  void clearRingBuffers();

  int insamples;

  void setInSamples(int);

  float magnitude_cutoff;

  float num_vertices;

  void buildDiskLists();

  float display_speed;

  int init;

};

#endif
