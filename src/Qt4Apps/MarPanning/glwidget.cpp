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
//   rotation_speed = 10;
  y_scale = 350;

  // Create space for the vertices we will display
  left_spectrum_ring_buffer = new double*[MAX_SPECTRUM_LINES];
  right_spectrum_ring_buffer = new double*[MAX_SPECTRUM_LINES];
  panning_spectrum_ring_buffer = new double*[MAX_SPECTRUM_LINES];

  for (int i = 0; i < MAX_SPECTRUM_LINES; i++) {
	left_spectrum_ring_buffer[i] = new double[SPECTRUM_BINS];
	right_spectrum_ring_buffer[i] = new double[SPECTRUM_BINS];
	panning_spectrum_ring_buffer[i] = new double[SPECTRUM_BINS];

	for (int j = 0; j < SPECTRUM_BINS; j++) {
	  left_spectrum_ring_buffer[i][j] = 0.0;
	  right_spectrum_ring_buffer[i][j] = 0.0;
	  panning_spectrum_ring_buffer[i][j] = 0.0;
	}
  }
  ring_buffer_pos = 0;


//   // Allocate space for the ring buffer used to draw the spectrum
//   spectrum_ring_buffer = new double*[MAX_SPECTRUM_LINES];
//   for (int i = 0; i < MAX_SPECTRUM_LINES; i++) {
// 	spectrum_ring_buffer[i] = new double[SPECTRUM_SIZE];
// 	for (int j = 0; j < SPECTRUM_SIZE; j++) {
// 	  spectrum_ring_buffer[i][j] = 0.0;
// 	}
//   }
//   ring_buffer_pos = 0;
  
//   //
//   // Create the MarSystem to play and analyze the data
//   // 
//   MarSystemManager mng;

//   // A series to contain everything
//   MarSystem* net = mng.create("Series", "net");

//   // A Fanout to let us read audio from either a SoundFileSource or an
//   // AudioSource
//   MarSystem* inputfanout = mng.create("Fanout", "inputfanout");
//   net->addMarSystem(inputfanout);

//   inputfanout->addMarSystem(mng.create("SoundFileSource", "src"));
//   inputfanout->addMarSystem(mng.create("AudioSource", "src"));

//   net->addMarSystem(mng.create("Selector", "inputselector"));

//   if (inAudioFileName == "") {
//  	net->updctrl("Selector/inputselector/mrs_natural/enable", 0);
//  	net->updctrl("Selector/inputselector/mrs_natural/enable", 1);
//  	cout << "input from AudioSource" << endl;
//   } else {
//  	net->updctrl("Selector/inputselector/mrs_natural/enable", 1);
//  	net->updctrl("Selector/inputselector/mrs_natural/enable", 0);
//  	cout << "input from SoundFileSource" << endl;
//   }

//   // Add the AudioSink right after we've selected the input and
//   // before we've calculated any features.  Nice trick.
//   net->addMarSystem(mng.create("AudioSink", "dest"));
//   net->addMarSystem(mng.create("Stereo2Mono", "stereo2mono"));
	
//   net->addMarSystem(mng.create("Windowing", "ham"));
//   net->addMarSystem(mng.create("Spectrum", "spk"));
//   net->addMarSystem(mng.create("PowerSpectrum", "pspk"));
//   net->addMarSystem(mng.create("Gain", "gain"));

//   MarSystem* spectrumFeatures = mng.create("Fanout", "spectrumFeatures");
//   spectrumFeatures->addMarSystem(mng.create("Centroid", "cntrd"));
//   spectrumFeatures->addMarSystem(mng.create("Rolloff", "rlf"));      
//   spectrumFeatures->addMarSystem(mng.create("Flux", "flux"));
//   spectrumFeatures->addMarSystem(mng.create("Rms", "rms"));
//   net->addMarSystem(spectrumFeatures);

//   // Set the controls of this MarSystem
  
//   net->updctrl("Fanout/inputfanout/SoundFileSource/src/mrs_real/repetitions",-1.0);

//   if (inAudioFileName == "") {
// 	net->updctrl("mrs_real/israte", 44100.0);
// 	inputfanout->updctrl("AudioSource/src/mrs_bool/initAudio", true);
//   } else {
//  	net->updctrl("Fanout/inputfanout/SoundFileSource/src/mrs_string/filename",inAudioFileName);
//  	net->updctrl("AudioSink/dest/mrs_bool/initAudio", true);
//   }

//   // Create and start playing the MarSystemQtWrapper that wraps
//   // Marsyas in a Qt thread
//   mwr_ = new MarSystemQtWrapper(net);
//   mwr_->start();

//   if (inAudioFileName == "") {
// 	mwr_->play();
// 	play_state = true;
//   } else {
// 	play_state = false;
//   }

  //
  // Create the MarSystem
  // 
  MarSystemManager mng;

  MarSystem* net = mng.create("Series", "net");
  net->addMarSystem(mng.create("SoundFileSource", "src"));
  net->addMarSystem(mng.create("AudioSink", "dest"));
	
  MarSystem* stereobranches = mng.create("Parallel", "stereobranches");
  MarSystem* left = mng.create("Series", "left");
  MarSystem* right = mng.create("Series", "right");

  left->addMarSystem(mng.create("Windowing", "hamleft"));
  left->addMarSystem(mng.create("Spectrum", "spkleft"));
  left->addMarSystem(mng.create("PowerSpectrum", "leftpspk"));
  left->addMarSystem(mng.create("Gain", "leftgain"));

  right->addMarSystem(mng.create("Windowing", "hamright"));
  right->addMarSystem(mng.create("Spectrum", "spkright"));
  right->addMarSystem(mng.create("PowerSpectrum", "rightpspk"));
  right->addMarSystem(mng.create("Gain", "rightgain"));

  stereobranches->addMarSystem(left);
  stereobranches->addMarSystem(right);

  net->addMarSystem(stereobranches);

  net->addMarSystem(mng.create("StereoSpectrum", "sspk"));

  //   string inAudioFileName = "./pink_l_to_r.wav";
  net->updctrl("SoundFileSource/src/mrs_string/filename",inAudioFileName);
  net->updctrl("SoundFileSource/src/mrs_real/repetitions",-1.0);

//   net->updctrl("mrs_natural/inSamples",2048);

  //   net->updctrl("mrs_real/israte", 44100.0);
  net->updctrl("AudioSink/dest/mrs_bool/initAudio", true);

  mwr_ = new MarSystemQtWrapper(net);
  mwr_->start();
  mwr_->play();
  play_state = true;

  // Create some handy pointers to access the MarSystem
  posPtr_ = mwr_->getctrl("Fanout/inputfanout/SoundFileSource/src/mrs_natural/pos");
  initPtr_ = mwr_->getctrl("AudioSink/dest/mrs_bool/initAudio");
  fnamePtr_ = mwr_->getctrl("Fanout/inputfanout/SoundFileSource/src/mrs_string/filename");

  //
  // Create the animation timer that periodically redraws the screen
  //
  QTimer *timer = new QTimer( this ); 
  connect( timer, SIGNAL(timeout()), this, SLOT(animate()) ); 
  timer->start(20); // Redraw the screen every 10ms
  
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
  glEnable(GL_DEPTH_TEST);

  // Enable fog for depth cueing
  GLfloat fogColor[4]= {0.0f, 0.0f, 0.0f, 1.0f};
  glClearColor(0.0f,0.0f,0.0f,1.0f);  // Fog colour of black (0,0,0)
  glFogfv(GL_FOG_COLOR, fogColor);    // Set fog color
  glFogi(GL_FOG_MODE, GL_LINEAR);       // Set the fog mode
  glFogf(GL_FOG_DENSITY, 0.5f);      // How dense will the fog be
  glHint(GL_FOG_HINT, GL_NICEST);     // Fog hint value : GL_DONT_CARE, GL_NICEST
  glFogf(GL_FOG_START, 0.0f);          // Fog Start Depth
  glFogf(GL_FOG_END, 50.0f);            // Fog End Depth
        glEnable(GL_FOG);                   // Enable fog

  // Antialias lines
  glEnable(GL_LINE_SMOOTH);
  glHint (GL_LINE_SMOOTH_HINT,GL_NICEST);

  //
  // Disks
  //
  
    // Setup for creating the disk
  int max_disks = 5;
     startList = glGenLists(max_disks);
     qobj = gluNewQuadric();
     gluQuadricCallback(qobj, GLU_ERROR, NULL);

     // Create the disk
     gluQuadricDrawStyle(qobj, GLU_FILL); /* all polygons wireframe */
     gluQuadricNormals(qobj, GLU_FLAT);

	 glNewList(startList+0, GL_COMPILE);
	 gluDisk(qobj, 0, 0.07, 10, 1);
	 glEndList();

	 glNewList(startList+1, GL_COMPILE);
	 gluDisk(qobj, 0, 0.09, 10, 1);
	 glEndList();

	 glNewList(startList+2, GL_COMPILE);
	 gluDisk(qobj, 0, 0.10, 10, 1);
	 glEndList();

	 glNewList(startList+3, GL_COMPILE);
	 gluDisk(qobj, 0, 0.11, 10, 1);
	 glEndList();

	 glNewList(startList+4, GL_COMPILE);
	 gluDisk(qobj, 0, 0.12, 10, 1);
	 glEndList();

   GLfloat mat_ambient[] = { 0.5, 0.5, 0.5, 1.0 };
   GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
   GLfloat mat_shininess[] = { 50.0 };
   GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };
   GLfloat model_ambient[] = { 0.5, 0.5, 0.5, 1.0 };

   glClearColor(0.0, 0.0, 0.0, 0.0);

   glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
   glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
   glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
   glLightfv(GL_LIGHT0, GL_POSITION, light_position);
   glLightModelfv(GL_LIGHT_MODEL_AMBIENT, model_ambient);

   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
   glEnable(GL_DEPTH_TEST);
}

// Paint the GL widget
void GLWidget::paintGL()
{
  // Clear the color and depth buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  // Load the identity matrix
  glLoadIdentity();

  // Translate the model to 0,0,-10
//   glTranslated(0.0, 0.5, -115.0);
  glTranslated(xTrans, yTrans, zTrans);
//    cout << "xTrans=" << xTrans << "  yTrans=" << yTrans << " zTrans=" << zTrans << " xRot=" << xRot << " yRot=" << yRot << " zRot=" << zRot << endl;

  // Rotate the object around the x,y,z axis
  glRotated(xRot / 16.0, 1.0, 0.0, 0.0);
  glRotated(yRot / 16.0, 0.0, 1.0, 0.0);
  glRotated(zRot / 16.0, 0.0, 0.0, 1.0);

  glTranslated(xTrans, yTrans, zTrans);

//   glTranslated(xTrans, yTrans, zTrans);

//   glTranslated(0.0, 0.5, -115.0);

  // Draw the object
  redrawScene();
  
  //    cout << "redrawing" << endl;
  //   mwr_->play();
  //   cout << mwr_->getctrl("mrs_realvec/processedData")->to<mrs_realvec>() << endl;


}

void GLWidget::animate() {
  //    cout << "animate" << endl;
  emit updateGL();
  if (play_state) {
	addDataToRingBuffer();
// 	setWaveformData();
// 	setAudioStats();
  }
}

void GLWidget::setAudioStats() {
  mrs_realvec data = mwr_->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();

  stats_centroid = data(0,0);
  stats_rolloff = data(1,0);
  stats_flux = data(2,0);
  stats_rms = data(3,0);
}

// Read in the waveform data from the waveformnet MarSystem
void GLWidget::setWaveformData() {
  waveform_data = mwr_->getctrl("Windowing/ham/mrs_realvec/processedData")->to<mrs_realvec>();
}

void GLWidget::addDataToRingBuffer() {
  mrs_realvec left_data = mwr_->getctrl("Parallel/stereobranches/Series/left/PowerSpectrum/leftpspk/mrs_realvec/processedData")->to<mrs_realvec>();
  mrs_realvec right_data = mwr_->getctrl("Parallel/stereobranches/Series/left/PowerSpectrum/leftpspk/mrs_realvec/processedData")->to<mrs_realvec>();
  mrs_realvec panning_data = mwr_->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();

  for (int i = 0; i < SPECTRUM_BINS; i++) {
	left_spectrum_ring_buffer[ring_buffer_pos][i] = left_data(i,0);
	right_spectrum_ring_buffer[ring_buffer_pos][i] = right_data(i,0);
	panning_spectrum_ring_buffer[ring_buffer_pos][i] = panning_data(i,0);
  }
  
  ring_buffer_pos += 1;
  if (ring_buffer_pos >= MAX_SPECTRUM_LINES) {
	ring_buffer_pos = 0;
  }
}


void GLWidget::redrawScene() {

  for (int i = 0; i < MAX_SPECTRUM_LINES; i++) {
 	for (int j = 0; j < SPECTRUM_BINS; j++) {
	  float x = (panning_spectrum_ring_buffer[(i + ring_buffer_pos) % MAX_SPECTRUM_LINES][j]) * 7.0;
// 	  float y = j/15.0;
  	  float y = j/3.0;
	  float z = i;

	  float size = ((left_spectrum_ring_buffer[(i + ring_buffer_pos) % MAX_SPECTRUM_LINES][j]) + 
					(right_spectrum_ring_buffer[(i + ring_buffer_pos) % MAX_SPECTRUM_LINES][j])) / 2.0 * 2000; 
		
 	  if (size > 0.5) {
 		size = 0.5;
 	  }

//   	  size = 0.2;

//  	  if (i > 197) {
// // 		cout << "i==0" << endl;
//  		glColor3f(1,1,1);
// 		size *= 3;
//  	  } else {
// 		glColor3f((size*5),1,0);
// 		//  		glColor3f(0,0,0);
//  	  }

	  glColor3f((size*5),1,0);

//    	  glBegin(GL_TRIANGLES);
//    	  glVertex3f(x,y,z);
//    	  glVertex3f(x+size,y,z);
//    	  glVertex3f(x+size,y+size,z);
//    	  glEnd();

	  // A disk
//     glEnable(GL_LIGHTING);
//     glColor3f(1.0, 1.0, 1.0);
	  if (size > 0.005) {

 		float mcolor[] = { (size*5), 1.0f, 0.0f, 1.0f };
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mcolor);

		glTranslatef(x,y,z);
		
 		if (size > 0.3) {
		  glCallList(startList+4);
 		} else if (size > 0.2) {
 		  glCallList(startList+3);
 		} else if (size > 0.1) {
 		  glCallList(startList+2);
 		} else if (size > 0.05) {
 		  glCallList(startList+1);
 		} else {
 		  glCallList(startList);
 		}

		glTranslatef(-x,-y,-z);
	  }


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

// Normalize an angle
void GLWidget::normalizeAngle(int *angle)
{
  while (*angle < 0)
	*angle += 360 * 16;
  while (*angle > 360 * 16)
	*angle -= 360 * 16;
}

// // Set the test z rotation value
// void GLWidget::setYScale(int scale)
// {
//   y_scale = scale;
// }

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

// Set the x translation val
void GLWidget::setXTranslation(int v)
{
//   normalizeVal(&val);
  double val = v * -0.1;
  if (val != xTrans) {
	xTrans = val;
	emit xTranslationChanged(val);
	updateGL();
  }
}

// Set the y translation val
void GLWidget::setYTranslation(int v)
{
//   normalizeVal(&val);
  double val = v * -0.1;
  if (val != yTrans) {
	yTrans = val;
	emit yTranslationChanged(val);
	updateGL();
  }
}

// Set the z translation val
void GLWidget::setZTranslation(int v)
{
//   normalizeVal(&val);
  double val = v * -2;
  if (val != zTrans) {
	zTrans = val;
	emit zTranslationChanged(val);
	updateGL();
  }
}


void GLWidget::setFogStart(int v)
{
  double val = v * -2;
  if (val != fogStart) {
	fogStart = val;
	cout << "v=" << v << " fogStart=" << fogStart << endl;
	emit fogStartChanged(val);
	glFogf(GL_FOG_START, fogStart);          // Fog Start Depth
	updateGL();
  }
}

void GLWidget::setFogEnd(int v)
{
  double val = v * -2;
  if (val != fogEnd) {
	fogEnd = val;
	cout << "v=" << v << " fogEnd=" << fogEnd << endl;
	emit fogEndChanged(val);
	glFogf(GL_FOG_END, fogEnd);          // Fog End Depth
	updateGL();
  }
}

// void GLWidget::powerSpectrumModeChanged(int val) 
// {
//   string sval;
//   if (val == 0) {
// 	sval = "power";
//   } else if (val == 1) {
// 	sval = "magnitude";
//   } else if (val == 2) {
// 	sval = "decibels";
//   } else if (val == 3) {
// 	sval = "powerdensity";
//   }
  
//   mwr_->updctrl("PowerSpectrum/pspk/mrs_string/spectrumType",sval);

// }

// void GLWidget::setWaterfallVisible(bool val)
// {
//   cout << "GLWidget::setWaterfallVisible val=" << val << endl;
// }

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

