#include "glwidget.h"

// Marsyas
#include <marsyas/system/MarSystemManager.h>
#include <config.h>

#include <QtGui>
#include <QtOpenGL>
#include <QTimer>
#include <QTextStream>

#include <math.h>
#include <iostream>
#include <fstream>
#include <sstream>

#ifdef MARSYAS_MACOSX
#  include <OpenGL/glu.h>
#else
#  include <GL/glu.h>
#endif

using namespace std;

GLWidget::GLWidget(const QString & inAudioFileName, QWidget *parent)
  : QGLWidget(parent)
{
  // Initialize member variables
  xRot = 30;
  yRot = 0;
  zRot = 0;
  test_x = 0;
  test_y = 0;
  test_z = 0;

  // sness - Just creating this as 256 elements wide for simplicity,
  // but it should dynamically readjust to be the size of the window
  // from Marsyas.
  waveform_data.create(1,512);

  // Defaults
  rotation_speed = 10;
  y_scale = 350;

  // Allocate space for the ring buffer used to draw the spectrum
  spectrum_ring_buffer = new double*[MAX_SPECTRUM_LINES];
  for (int i = 0; i < MAX_SPECTRUM_LINES; i++) {
    spectrum_ring_buffer[i] = new double[SPECTRUM_SIZE];
    for (int j = 0; j < SPECTRUM_SIZE; j++) {
      spectrum_ring_buffer[i][j] = 0.0;
    }
  }
  ring_buffer_pos = 0;

  //
  // Create the MarSystem to play and analyze the data
  //
  MarSystemManager mng;

  // A series to contain everything
  MarSystem* net = mng.create("Series", "net");

  // A Fanout to let us read audio from either a SoundFileSource or an
  // AudioSource
  MarSystem* inputfanout = mng.create("Fanout", "inputfanout");
  net->addMarSystem(inputfanout);

  inputfanout->addMarSystem(mng.create("AudioSource", "src"));
  inputfanout->addMarSystem(mng.create("SoundFileSource", "src"));

  net->addMarSystem(mng.create("Selector", "inputselector"));

  // Add the AudioSink right after we've selected the input and
  // before we've calculated any features.  Nice trick.
  net->addMarSystem(mng.create("Stereo2Mono", "stereo2mono"));
  net->addMarSystem(mng.create("AudioSink", "dest"));

  net->addMarSystem(mng.create("Windowing", "ham"));
  net->addMarSystem(mng.create("Spectrum", "spk"));
  net->addMarSystem(mng.create("PowerSpectrum", "pspk"));
  net->addMarSystem(mng.create("Gain", "gain"));

  MarSystem* spectrumFeatures = mng.create("Fanout", "spectrumFeatures");
  spectrumFeatures->addMarSystem(mng.create("Centroid", "cntrd"));
  spectrumFeatures->addMarSystem(mng.create("Rolloff", "rlf"));
  spectrumFeatures->addMarSystem(mng.create("Flux", "flux"));
  spectrumFeatures->addMarSystem(mng.create("Rms", "rms"));
  net->addMarSystem(spectrumFeatures);

  // Set the controls of this MarSystem

  net->updControl("Fanout/inputfanout/SoundFileSource/src/mrs_real/repetitions",-1.0);
  net->updControl("mrs_real/israte", 44100.0);
  inputfanout->updControl("AudioSource/src/mrs_bool/initAudio", true);

  if (inAudioFileName.isEmpty())
  {
    cout << "input from AudioSource" << endl;
  }

  m_marsystem = net;
  m_system = new MarsyasQt::System(net);

  m_fileNameControl = m_system->control("Fanout/inputfanout/SoundFileSource/src/mrs_string/filename");
  m_initAudioControl = m_system->control("AudioSink/dest/mrs_bool/initAudio");
  m_spectrumTypeControl = m_system->control("PowerSpectrum/pspk/mrs_string/spectrumType");
  m_inputEnableControl = m_system->control("Selector/inputselector/mrs_natural/enable");
  m_inputDisableControl = m_system->control("Selector/inputselector/mrs_natural/disable");

  m_statsSource = m_system->control("mrs_realvec/processedData");
  m_waveformSource = m_system->control("Windowing/ham/mrs_realvec/processedData");
  m_spectrumSource = m_system->control("PowerSpectrum/pspk/mrs_realvec/processedData");

  // Connect the animation timer that periodically redraws the screen.
  // It is activated in the 'play()' function.
  connect( &m_updateTimer, SIGNAL(timeout()), this, SLOT(animate()) );

  // Run:
  if (inAudioFileName.isEmpty())
    playInput();
  else
    play(inAudioFileName);
}

GLWidget::~GLWidget()
{
  makeCurrent();
}

void GLWidget::open()
{
  QString fileName = QFileDialog::getOpenFileName(this);
  play(fileName);
}

void GLWidget::playInput()
{
  m_inputDisableControl->setValue(1);
  m_inputEnableControl->setValue(0);

  m_system->start();
  m_updateTimer.start(20);
}

void GLWidget::play( const QString & fileName )
{
  if (fileName.isEmpty())
    return;

  static const bool NO_UPDATE = false;
  m_fileNameControl->setValue(fileName, NO_UPDATE);
  m_initAudioControl->setValue(true, NO_UPDATE);
  m_inputDisableControl->setValue(0, NO_UPDATE);
  m_inputEnableControl->setValue(1, NO_UPDATE);
  m_system->update();

  m_system->start();
  m_updateTimer.start(20);
}

void GLWidget::playPause()
{
  if (m_system->isRunning())
  {
    m_system->stop();
    m_updateTimer.stop();
  }
  else
  {
    m_system->start();
    m_updateTimer.start(20);
  }
}

// The minimum size of the widget
QSize GLWidget::minimumSizeHint() const
{
  return QSize(400, 400);
}

// The maximum size of the widget
QSize GLWidget::sizeHint() const
{
  return QSize(600, 600);
}

// Initialize the GL widget
void GLWidget::initializeGL()
{
  // Set the background color to white
  qglClearColor(Qt::black);

  // Set the shading model
  glShadeModel(GL_SMOOTH);

  // Enable depth testing
  glEnable(GL_DEPTH_TEST);

  // Enable fog for depth cueing
  GLfloat fogColor[4]= {0.0f, 0.0f, 0.0f, 1.0f};
  glClearColor(0.0f,0.0f,0.0f,1.0f);  // Fog colour of black (0,0,0)
  glFogfv(GL_FOG_COLOR, fogColor);    // Set fog color
  glFogi(GL_FOG_MODE, GL_EXP2);       // Set the fog mode
  glFogf(GL_FOG_DENSITY, 0.02f);      // How dense will the fog be
  glHint(GL_FOG_HINT, GL_NICEST);     // Fog hint value : GL_DONT_CARE, GL_NICEST
  glFogf(GL_FOG_START, 10.0f);          // Fog Start Depth
  glFogf(GL_FOG_END, 50.0f);            // Fog End Depth
  glEnable(GL_FOG);                   // Enable fog

  // Antialias lines
  glEnable(GL_LINE_SMOOTH);
  glHint (GL_LINE_SMOOTH_HINT,GL_NICEST);
}

// Paint the GL widget
void GLWidget::paintGL()
{
  // Clear the color and depth buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Load the identity matrix
  glLoadIdentity();

  // Translate the model to 0,0,-10
  glTranslated(0.0, 0.0, -29.0);

  // Rotate the object around the x,y,z axis
  glRotated(xRot / 16.0, 1.0, 0.0, 0.0);
  glRotated(yRot / 16.0, 0.0, 1.0, 0.0);
  glRotated(zRot / 16.0, 0.0, 0.0, 1.0);

  glTranslated(0.0, -0.75, -29.0);

  // Draw the object
  redrawScene();

  //    cout << "redrawing" << endl;
  //   mwr_->play();
  //   cout << mwr_->getctrl("mrs_realvec/processedData")->to<mrs_realvec>() << endl;


}

void GLWidget::animate()
{
  addDataToRingBuffer();
  setWaveformData();
  setAudioStats();
  updateGL();
}

void GLWidget::setAudioStats()
{
  mrs_realvec data = m_statsSource->value().value<realvec>();

  stats_centroid = data(0,0);
  stats_rolloff = data(1,0);
  stats_flux = data(2,0);
  stats_rms = data(3,0);
}

// Read in the waveform data from the waveformnet MarSystem
void GLWidget::setWaveformData()
{
  waveform_data = m_waveformSource->value().value<realvec>();
}

// Read in a line of data from the spectrumnet MarSystem
void GLWidget::addDataToRingBuffer()
{
  mrs_realvec data = m_spectrumSource->value().value<realvec>();

  for (int i = 0; i < SPECTRUM_SIZE; i++) {
    spectrum_ring_buffer[ring_buffer_pos][i] = data(i,0);
  }

  //   cout << "ring_buffer_pos=" << ring_buffer_pos << endl;

  ring_buffer_pos += 1;
  if (ring_buffer_pos >= MAX_SPECTRUM_LINES) {
    ring_buffer_pos = 0;
  }


}

void GLWidget::redrawScene() {


  //
  // Draw the text for the various audio statistics
  //
  glColor3f(1,1,1); // White text

  QString centroid_string;
  QTextStream(&centroid_string) << "centroid = " << stats_centroid;
  renderText(20,140,centroid_string);

  QString rolloff_string;
  QTextStream(&rolloff_string) << "rolloff = " << stats_rolloff;
  renderText(20,160,rolloff_string);

  QString flux_string;
  QTextStream(&flux_string) << "flux = " << stats_flux;
  renderText(20,180,flux_string);

  QString rms_string;
  QTextStream(&rms_string) << "rms = " << stats_rms;
  renderText(20,200,rms_string);

  //
  // Draw the waveform
  //
  glColor3f(1,1,1);
  glBegin(GL_LINE_STRIP);
  for (int i = 0; i < 512; i++) {
    float x = ((i - 256 / 2.0) / 200.0) - 0.7;
    float y = (waveform_data(0,i)) + 2.5;
    float z = 49;
    glVertex3f(x,y,z);
  }
  glEnd();

  //
  // Draw the waterfall spectrum
  //
  for (int i = 0; i < MAX_SPECTRUM_LINES; i++) {
    glColor3f((i / (float)MAX_SPECTRUM_LINES),1,0); // A color ramp from yellow to green
    glBegin(GL_LINE_STRIP);
    for (int j = 0; j < SPECTRUM_SIZE; j++) {
      float x = (j - SPECTRUM_SIZE / 2.0) / 40.0;
      float y = y_scale * spectrum_ring_buffer[(i + ring_buffer_pos) % MAX_SPECTRUM_LINES][j];
      float z = i;
      glVertex3f(x,y,z);
    }
    glEnd();
  }

}

// Resize the window
void GLWidget::resizeGL(int width, int height)
{
  // The smallest side of the window
  int side = qMin(width, height);

  // Setup the glViewport
  glViewport((width - side) / 2, (height - side) / 2, side, side);

  // Switch to GL_PROJECTION matrix mode
  glMatrixMode(GL_PROJECTION);

  // Load the identity matrix
  glLoadIdentity();

  // Setup a perspective viewing system
  gluPerspective(20,1,0.1,1000);

  // Switch back to GL_MODELVIEW mode
  glMatrixMode(GL_MODELVIEW);
}

// Normalize an angle
void GLWidget::normalizeAngle(int *angle)
{
  while (*angle < 0)
    *angle += 360 * 16;
  while (*angle > 360 * 16)
    *angle -= 360 * 16;
}

// Set the test z rotation value
void GLWidget::setYScale(int scale)
{
  y_scale = scale;
}

// Set the x rotation angle
void GLWidget::setXRotation(int angle)
{
  normalizeAngle(&angle);
  if (angle != xRot) {
    xRot = angle;
    emit xRotationChanged(angle);
    updateGL();
  }
}

// Set the y rotation angle
void GLWidget::setYRotation(int angle)
{
  normalizeAngle(&angle);
  if (angle != yRot) {
    yRot = angle;
    emit yRotationChanged(angle);
    updateGL();
  }
}

// Set the z rotation angle
void GLWidget::setZRotation(int angle)
{
  normalizeAngle(&angle);
  if (angle != zRot) {
    zRot = angle;
    emit zRotationChanged(angle);
    updateGL();
  }
}

void GLWidget::powerSpectrumModeChanged(int val)
{
  string sval;
  if (val == 0) {
    sval = "power";
  } else if (val == 1) {
    sval = "magnitude";
  } else if (val == 2) {
    sval = "decibels";
  } else if (val == 3) {
    sval = "powerdensity";
  }

  m_spectrumTypeControl->setValue( QString::fromStdString(sval) );
}

void GLWidget::setWaterfallVisible(bool val)
{
  cout << "GLWidget::setWaterfallVisible val=" << val << endl;
}
