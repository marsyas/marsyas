#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include <QTimer> 

// Marsyas
#include "MarSystemManager.h"
#include "MarSystemQtWrapper.h"
using namespace MarsyasQt;
using namespace Marsyas;

// sness
#define MAX_Z 50
#define MAX_SPECTRUM_BINS 65536
#define MAX_STEREO_SPECTRUM_BINS 65536

#define TIMER_COUNT_STEPS 100.0

class GLWidget : public QGLWidget
{
Q_OBJECT

public:
  GLWidget(string inAudioFileName,QWidget *parent = 0);
  void GLWidget_other_constructor(string inAudioFileName,QWidget *parent = 0);
  ~GLWidget();


  MarSystemQtWrapper* getMarSystemQtWrapper(){return mwr_;};

  QSize minimumSizeHint() const;
  QSize sizeHint() const;

public slots:
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
  void setPos(); 

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
  void playPause(); // Play or pause the playback of the song
  void open(); // Open a new audio file

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
  QTimer *timer;

  // The current tick of the animation
  int timerCount;

//   // The vertices we will draw
//   GLfloat **vertices;

//   void drawCubeFace(int,int,int,int);

  void redrawScene();
  void addDataToRingBuffer();
  
  // Marsyas
  MarSystemManager mng;  
  MarSystemQtWrapper*  mwr_;
  MarSystem* net_;

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

  bool play_state;

  float stats_centroid;
  float stats_rolloff;
  float stats_flux;
  float stats_rms;

//   void setAudioStats();

  MarControlPtr posPtr_;
  MarControlPtr sizePtr_;
  MarControlPtr osratePtr_;
  MarControlPtr initPtr_;
  MarControlPtr fnamePtr_;
  
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
