#include "glwidget.h"

#include <marsyas/system/MarSystemManager.h>
#include <config.h>

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
  max_data.create(POWERSPECTRUM_BUFFER_SIZE);

  for (int i = 0; i < POWERSPECTRUM_BUFFER_SIZE; i++) {
    max_data(i) = -999.9;
  }

  //
  // Create the MarSystem to play and analyze the data
  //
  MarSystemManager mng;

  // A series to contain everything
  MarSystem* net = mng.create("Series", "net");

  MarSystem *accum = mng.create("Accumulator", "accum");
  net->addMarSystem(accum);

  MarSystem *accum_series = mng.create("Series", "accum_series");
  accum->addMarSystem(accum_series);

  accum_series->addMarSystem(mng.create("SoundFileSource/src"));
  accum_series->addMarSystem(mng.create("Stereo2Mono", "stereo2mono"));
  accum_series->addMarSystem(mng.create("AudioSink", "dest"));
  accum_series->addMarSystem(mng.create("Windowing", "ham"));
  accum_series->addMarSystem(mng.create("Spectrum", "spk"));
  accum_series->addMarSystem(mng.create("PowerSpectrum", "pspk"));

  net->addMarSystem(mng.create("ShiftInput", "si"));
  net->addMarSystem(mng.create("AutoCorrelation", "auto"));

  net->updControl("Accumulator/accum/mrs_natural/nTimes", 10);

  net->updControl("ShiftInput/si/mrs_natural/winSize", MEMORY_SIZE);

  net->updControl("mrs_real/israte", 44100.0);

  // Create a Qt wrapper that provides thread-safe control of the MarSystem:
  m_marsystem = net;
  m_system = new MarsyasQt::System(net);

  // Get controls:
  m_fileNameControl = m_system->control("Accumulator/accum/Series/accum_series/SoundFileSource/src/mrs_string/filename");
  m_initAudioControl = m_system->control("Accumulator/accum/Series/accum_series/AudioSink/dest/mrs_bool/initAudio");
  m_spectrumSource = m_system->control("mrs_realvec/processedData");

  // Connect the animation timer that periodically redraws the screen.
  // It is activated in the 'play()' function.
  connect( &m_updateTimer, SIGNAL(timeout()), this, SLOT(animate()) );

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

void GLWidget::play( const QString & fileName )
{
  if (fileName.isEmpty())
    return;

  static const bool NO_UPDATE = false;
  m_fileNameControl->setValue(fileName, NO_UPDATE);
  m_initAudioControl->setValue(true, NO_UPDATE);
  m_system->update();

  m_system->start();
  m_updateTimer.start(20);

  m_audioFileName = fileName;
}

void GLWidget::playPause()
{
  if (m_system->isRunning())
  {
    m_system->stop();
    m_updateTimer.stop();
  }
  else if (!m_audioFileName.isEmpty())
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
  glDisable(GL_DEPTH_TEST);
}

// Paint the GL widget
void GLWidget::paintGL()
{
  // Clear the color and depth buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Load the identity matrix
  glLoadIdentity();

  // // Translate the model
  glTranslated(-0.5, -0.5, -3);

  // Draw the object
  redrawScene();

}

void GLWidget::animate()
{
  updateGL();
}

void GLWidget::redrawScene()
{
  mrs_realvec correlogram_data( m_spectrumSource->value().value<mrs_realvec>() );

  for (int x = 0; x < MEMORY_SIZE; x++) {
    for (int y = 0; y < POWERSPECTRUM_BUFFER_SIZE; y++) {
      if (correlogram_data(y,x) > max_data(y)) {
        max_data(y) = correlogram_data(y,x);
        // cout << "max_data(" << y << ")=" << max_data(y) << endl;
      }
    }
  }

  float x1,x2,y1,y2;

  // Draw a rectangle for each element in the array
  for (int x = 0; x < MEMORY_SIZE; x++) {
    for (int y = 0; y < POWERSPECTRUM_BUFFER_SIZE; y++) {
      float color = (correlogram_data(y,x) * (1.0 / max_data(y)));

      glColor3f(color,color,color);
      // cout << "color=" << color << endl;

      x1 = (float)x/MEMORY_SIZE;
      x2 = x1+0.005;

      y1 = float(y)/POWERSPECTRUM_BUFFER_SIZE;
      y2 = y1+0.005;

      glBegin(GL_QUADS);
      glVertex2f(x1, y1);
      glVertex2f(x2, y1);
      glVertex2f(x2, y2);
      glVertex2f(x1, y2);
      glEnd();

    }
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
