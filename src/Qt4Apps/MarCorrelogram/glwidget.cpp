#include <QtGui>
#include <QtOpenGL>
#include <QTimer>
#include <QTextStream>

#include <math.h>
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

// Marsyas
#include "MarSystemManager.h"
#include "MarSystemQtWrapper.h"
using namespace MarsyasQt;

#include "glwidget.h"

GLWidget::GLWidget(string inAudioFileName, QWidget *parent)
  : QGLWidget(parent)
{
  // // Initialize member variables
  xRot = 30;
  yRot = 0;
  zRot = 0;

  max_data.create(POWERSPECTRUM_BUFFER_SIZE);

  for (int i = 0; i < POWERSPECTRUM_BUFFER_SIZE; i++) {
  	max_data(i) = -999.9;
  }

  // Defaults
  y_scale = 350;

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

  net->updctrl("Accumulator/accum/mrs_natural/nTimes", 10);

  net->updctrl("Accumulator/accum/Series/accum_series/SoundFileSource/src/mrs_string/filename", inAudioFileName);
  net->updctrl("ShiftInput/si/mrs_natural/winSize", MEMORY_SIZE);

  net->updctrl("mrs_real/israte", 44100.0);
  net->updctrl("Accumulator/accum/Series/accum_series/AudioSink/dest/mrs_bool/initAudio", true);
  // net->updctrl("AudioSink/dest/mrs_bool/initAudio", true);

  // // A Fanout to let us read audio from either a SoundFileSource or an
  // // AudioSource
  // MarSystem* inputfanout = mng.create("Fanout", "inputfanout");
  // net->addMarSystem(inputfanout);

  // inputfanout->addMarSystem(mng.create("SoundFileSource", "src"));
  // inputfanout->addMarSystem(mng.create("AudioSource", "src"));

  // net->addMarSystem(mng.create("Selector", "inputselector"));

  // if (inAudioFileName == "") {
  // 	net->updctrl("Selector/inputselector/mrs_natural/enable", 0);
  // 	net->updctrl("Selector/inputselector/mrs_natural/enable", 1);
  // 	cout << "input from AudioSource" << endl;
  // } else {
  // 	net->updctrl("Selector/inputselector/mrs_natural/enable", 1);
  // 	net->updctrl("Selector/inputselector/mrs_natural/enable", 0);
  // 	cout << "input from SoundFileSource" << endl;
  // }

  // Create and start playing the MarSystemQtWrapper that wraps
  // Marsyas in a Qt thread
  mwr_ = new MarSystemQtWrapper(net);
  mwr_->start();

  if (inAudioFileName == "") {
	mwr_->play();
	play_state = true;
  } else {
	play_state = false;
  }
		    
  // Create some handy pointers to access the MarSystem
  posPtr_ = mwr_->getctrl("Fanout/inputfanout/SoundFileSource/src/mrs_natural/pos");
  initPtr_ = mwr_->getctrl("AudioSink/dest/mrs_bool/initAudio");
  fnamePtr_ = mwr_->getctrl("Fanout/inputfanout/SoundFileSource/src/mrs_string/filename");

  //
  // Create the animation timer that periodically redraws the screen
  //
  QTimer *timer = new QTimer( this ); 
  connect( timer, SIGNAL(timeout()), this, SLOT(animate()) ); 
  timer->start(10); // Redraw the screen every 10ms
  
}

GLWidget::~GLWidget()
{
  makeCurrent();
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

void GLWidget::animate() {
  emit updateGL();
  if (play_state) {
	setData();
  }
}

// Read in the waveform data from the waveformnet MarSystem
void GLWidget::setData() {
}

void GLWidget::redrawScene() {
  correlogram_data = mwr_->getctrl("mrs_realvec/processedData")->to<Marsyas::mrs_realvec>();

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

void GLWidget::playPause() 
{
  play_state = !play_state;

  if (play_state == true) {
	mwr_->play();
  } else {
	mwr_->pause();
  }
}


void GLWidget::open() 
{
  QString fileName = QFileDialog::getOpenFileName(this);

  mwr_->updctrl(fnamePtr_, fileName.toStdString());
  mwr_->updctrl(initPtr_, true);

  mwr_->play();
  play_state = true;

}
