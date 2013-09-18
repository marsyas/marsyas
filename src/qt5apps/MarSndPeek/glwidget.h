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
  void open(); // Open a new audio file
  void play( const QString &fileName );
  void playInput();
  void playPause(); // Play or pause the playback of the song

  void setXRotation(int angle);
  void setYRotation(int angle);
  void setZRotation(int angle);

  void setYScale(int scale);

  void powerSpectrumModeChanged(int val);

  void setWaterfallVisible(bool val);

  void animate();


signals:
  void xRotationChanged(int angle);
  void yRotationChanged(int angle);
  void zRotationChanged(int angle);

  void start_xRotationChanged(int angle);
  void start_yRotationChanged(int angle);
  void start_zRotationChanged(int angle);

  void end_xRotationChanged(int angle);
  void end_yRotationChanged(int angle);
  void end_zRotationChanged(int angle);

  void rotationSpeedChanged(int angle);

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

  // The start x,y,z rotation angles for the animation
  int start_xRot;
  int start_yRot;
  int start_zRot;

  // The end x,y,z rotation angles for the animation
  int end_xRot;
  int end_yRot;
  int end_zRot;

  // The rotation speed
  int rotation_speed;

  // A timer to make the animation happen
  QTimer m_updateTimer;

  // The current tick of the animation
  int timerCount;

  // The vertices we will draw
  GLfloat **vertices;

  void drawCubeFace(int,int,int,int);

  void redrawScene();
  void addDataToRingBuffer();

  // Marsyas
  MarSystem* m_marsystem;
  MarsyasQt::System *m_system;
  MarsyasQt::Control *m_fileNameControl;
  MarsyasQt::Control *m_initAudioControl;
  MarsyasQt::Control *m_spectrumTypeControl;
  MarsyasQt::Control *m_inputEnableControl;
  MarsyasQt::Control *m_inputDisableControl;

  MarsyasQt::Control *m_statsSource;
  MarsyasQt::Control *m_waveformSource;
  MarsyasQt::Control *m_spectrumSource;

  // A ring buffer that holds our data
  double **spectrum_ring_buffer;
  int ring_buffer_pos; // The current head position in the ring buffer

  // Scale
  double y_scale;

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

  void setAudioStats();
};

#endif
