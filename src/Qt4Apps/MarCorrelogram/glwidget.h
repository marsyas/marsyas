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
#define MAX_SPECTRUM_LINES 50
#define SPECTRUM_SIZE 128

#define POWERSPECTRUM_BUFFER_SIZE 257
#define MEMORY_SIZE 300


#define TIMER_COUNT_STEPS 100.0

class GLWidget : public QGLWidget
{
Q_OBJECT

public:
  GLWidget(string inAudioFileName,QWidget *parent = 0);
  ~GLWidget();

  QSize minimumSizeHint() const;
  QSize sizeHint() const;

public slots:

  void animate();

signals:

  void timerChanged(int value);

private slots:
  void playPause(); // Play or pause the playback of the song
  void open(); // Open a new audio file

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
  QTimer *timer;

  // The current tick of the animation
  int timerCount;

  // The vertices we will draw
  GLfloat **vertices;

  void redrawScene();
  void addDataToRingBuffer();
  
  // Marsyas
  MarSystemManager mng;  
  MarSystemQtWrapper*  mwr_;
  MarSystem* pnet_;

  // Scale
  double y_scale;
  
  mrs_realvec correlogram_data;
  void setData();

  bool play_state;

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
