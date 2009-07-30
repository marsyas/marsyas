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

  xTrans = 0;
  // For 200
  //   zTrans = -127;
  //   yTrans = -5.5;

  // For 50
  zTrans = -52;
  yTrans = -6.7;

  test_x = 0;
  test_y = 0;
  test_z = 0;
  
  //   // sness - Just creating this as 256 elements wide for simplicity,
  //   // but it should dynamically readjust to be the size of the window
  //   // from Marsyas.
  //   waveform_data.create(1,512);
  
  insamples = 512;
  //    insamples = 32;
  spectrum_bins = insamples / 2.0;
  stereo_spectrum_bins = insamples / 4.0;

  // The number of vertices used to make the disk
  num_vertices = 10;
  
  // Defaults
  //   rotation_speed = 10;
  y_scale = 350;

  // Create space for the vertices we will display
  left_spectrum_ring_buffer = new double*[MAX_Z];
  right_spectrum_ring_buffer = new double*[MAX_Z];
  panning_spectrum_ring_buffer = new double*[MAX_Z];

  for (int i = 0; i < MAX_Z; i++) {
	left_spectrum_ring_buffer[i] = new double[MAX_SPECTRUM_BINS];
	right_spectrum_ring_buffer[i] = new double[MAX_SPECTRUM_BINS];
	panning_spectrum_ring_buffer[i] = new double[MAX_STEREO_SPECTRUM_BINS];
  }
  clearRingBuffers();
  ring_buffer_pos = 0;

  //
  // Create the MarSystem
  // 
  MarSystemManager mng;

  net_ = mng.create("Series", "net");
  net_->addMarSystem(mng.create("SoundFileSource", "src"));
  net_->addMarSystem(mng.create("AudioSink", "dest"));
  //   net_->addMarSystem(mng.create("Gain", "gain"));

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

  net_->addMarSystem(stereobranches);

  net_->addMarSystem(mng.create("StereoSpectrum", "sspk"));

  //   net_->addMarSystem(mng.create("Gain", "gain2"));
  //   net_->addMarSystem(mng.create("Gain", "gain3"));

  net_->updctrl("mrs_real/israte", 44100.0);
  net_->updctrl("SoundFileSource/src/mrs_real/israte", 44100.0);
  net_->updctrl("SoundFileSource/src/mrs_real/osrate", 44100.0);
  net_->updctrl("AudioSink/dest/mrs_real/israte", 44100.0);
  net_->updctrl("SoundFileSource/src/mrs_natural/inSamples",insamples);
  net_->updctrl("mrs_natural/inSamples",insamples);

  //   string inAudioFileName = "./pink_l_to_r.wav";
  if (inAudioFileName != "") {
	net_->updctrl("SoundFileSource/src/mrs_string/filename",inAudioFileName);
  }
  net_->updctrl("SoundFileSource/src/mrs_real/repetitions",-1.0);

  //   net_->updctrl("mrs_natural/inSamples",2048);
  net_->updctrl("mrs_natural/inSamples",insamples);

  net_->updctrl("mrs_real/israte", 44100.0);
//   if (inAudioFileName != "") {
//   cout << *net_ << endl;
	net_->updctrl("AudioSink/dest/mrs_bool/initAudio", true);
//   }

  // sness - Set as the biggest possible value so that it doesn't die later.

  mwr_ = new MarSystemQtWrapper(net_);
  if (inAudioFileName != "") {
	mwr_->start();
	mwr_->play();
	play_state = true;
  }

  // Create some handy pointers to access the MarSystem
  posPtr_ = mwr_->getctrl("SoundFileSource/src/mrs_natural/pos");
  sizePtr_ = mwr_->getctrl("SoundFileSource/src/mrs_natural/size");
  osratePtr_ = mwr_->getctrl("SoundFileSource/src/mrs_real/osrate");
  initPtr_ = mwr_->getctrl("AudioSink/dest/mrs_bool/initAudio");
  fnamePtr_ = mwr_->getctrl("SoundFileSource/src/mrs_string/filename");

  //   cout << "inSamples=" << mwr_->getctrl("mrs_natural/inSamples")->to<mrs_real>() << endl;

  //     cout << *net_ << endl;

  //
  // Create the animation timer that periodically redraws the screen
  //
  QTimer *timer = new QTimer( this ); 
  connect( timer, SIGNAL(timeout()), this, SLOT(animate()) ); 
  timer->start(20); // Redraw the screen every 10ms

  setPos(0);
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
//   qglClearColor(Qt::black);
  qglClearColor(Qt::white);

  // Set the shading model
  glShadeModel(GL_SMOOTH);
  
  // Enable depth testing
  glEnable(GL_DEPTH_TEST);

  // Enable fog for depth cueing
   GLfloat fogColor[4]= {0.0f, 0.0f, 0.0f, 1.0f};
//   GLfloat fogColor[4]= {1.0f, 1.0f, 1.0f, 1.0f};

   glClearColor(0.0f,0.0f,0.0f,1.0f);  // Fog colour of black (0,0,0)
//   glClearColor(1.0f,1.0f,1.0f,1.0f);  // Fog colour of black (0,0,0)

  glFogfv(GL_FOG_COLOR, fogColor);    // Set fog color
  glFogi(GL_FOG_MODE, GL_LINEAR);       // Set the fog mode
  glFogf(GL_FOG_DENSITY, 0.5f);      // How dense will the fog be
  glHint(GL_FOG_HINT, GL_NICEST);     // Fog hint value : GL_DONT_CARE, GL_NICEST
  //   glFogf(GL_FOG_START, 82.0f);          // Fog Start Depth
  //   glFogf(GL_FOG_END, 138.0f);            // Fog End Depth
      glEnable(GL_FOG);                   // Enable fog

//   setFogStart(-31);
//   setFogEnd(-72);

  setFogStart(-60);
  setFogEnd(-100);

  // Antialias lines
  glEnable(GL_LINE_SMOOTH);
  glHint (GL_LINE_SMOOTH_HINT,GL_NICEST);

  //
  // Disks
  //
  
  buildDiskLists();

  GLfloat mat_ambient[] = { 0.5, 0.5, 0.5, 1.0 };
  GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
  GLfloat mat_shininess[] = { 50.0 };
  GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };
  GLfloat model_ambient[] = { 0.5, 0.5, 0.5, 1.0 };

   glClearColor(0.0, 0.0, 0.0, 0.0);
//   glClearColor(1.0, 1.0, 1.0, 1.0);

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

  setPos();
}

// "automatic" advancement of position
void GLWidget::setPos() 
{
  mrs_natural pos = posPtr_->to<mrs_natural>();
  mrs_natural size = sizePtr_->to<mrs_natural>();
  mrs_real srate = osratePtr_->to<mrs_real>();
  
  mrs_real duration = (pos / srate);
//   emit timeChanged(duration);

 
  float rpos = pos * 1.0 / size;
  int sliderPos = rpos * 100.0;
  emit posChanged(sliderPos);
}


// "manual" advancement of position
void GLWidget::setPos(int val)
{
  float fval = val / 100.0f;
  
  float fsize = 
    sizePtr_->to<mrs_natural>();
  fsize *= fval;
  
  int size = (int) fsize;
  mwr_->updctrl(posPtr_, size);
  
}

void GLWidget::setAudioStats() {
  mrs_realvec data = mwr_->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();

  stats_centroid = data(0,0);
  stats_rolloff = data(1,0);
  stats_flux = data(2,0);
  stats_rms = data(3,0);
}

// // Read in the waveform data from the waveformnet MarSystem
// void GLWidget::setWaveformData() {
//   waveform_data = mwr_->getctrl("Windowing/ham/mrs_realvec/processedData")->to<mrs_realvec>();
// }

void GLWidget::addDataToRingBuffer() {
  mrs_realvec left_data = mwr_->getctrl("Parallel/stereobranches/Series/left/PowerSpectrum/leftpspk/mrs_realvec/processedData")->to<mrs_realvec>();
  mrs_realvec right_data = mwr_->getctrl("Parallel/stereobranches/Series/right/PowerSpectrum/rightpspk/mrs_realvec/processedData")->to<mrs_realvec>();
  mrs_realvec panning_data = mwr_->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();

  int leftright_rows = left_data.getRows();
  int panning_rows = panning_data.getRows();

  for (int i = 0; i < leftright_rows; i++) {
	left_spectrum_ring_buffer[ring_buffer_pos][i] = left_data(i,0);
	right_spectrum_ring_buffer[ring_buffer_pos][i] = right_data(i,0);
  }

  for (int i = 0; i < panning_rows; i++) {
	panning_spectrum_ring_buffer[ring_buffer_pos][i] = panning_data(i,0);
  }

  
  ring_buffer_pos += 1;
  if (ring_buffer_pos >= MAX_Z) {
	ring_buffer_pos = 0;
  }
}


void GLWidget::redrawScene() {

  //
  // Draw guidelines
  //
  // 	  glEnable(GL_LIGHTING);
  float guideline_size = 0.01;

  float mcolor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mcolor);

  float min_x = -7;
  float max_x = 7;
  float min_y = 5;
  float max_y = 22;
   float max_z = MAX_Z;
//   float max_z = MAX_Z * 10;

  glBegin(GL_LINES);
  glVertex3f(min_x,min_y,0);
  glVertex3f(min_x,min_y,max_z);
  glEnd();

  glBegin(GL_LINES);
  glVertex3f(min_x,max_y,0);
  glVertex3f(min_x,max_y,max_z);
  glEnd();

  glBegin(GL_LINES);
  glVertex3f(max_x,min_y,0);
  glVertex3f(max_x,min_y,max_z);
  glEnd();

  glBegin(GL_LINES);
  glVertex3f(max_x,max_y,0);
  glVertex3f(max_x,max_y,max_z);
  glEnd();


  //   float min_x = 1024;
  //   float max_x = -1024;
//     min_y = 1024;
//     max_y = -1024;

   double x;
   double y;
   double z;
   double size;

  for (int i = 0; i < MAX_Z; i++) {
 	for (int j = 0; j < stereo_spectrum_bins; j++) {
	  x = (panning_spectrum_ring_buffer[(i + ring_buffer_pos) % MAX_Z][j]) * 7.0;
	  //   	  float y = j/3.0;
	  // Scale the y-range to between 0 and 42
	  //    	  float y = (j / (float)spectrum_bins) * 21.0;
	  //    	  float y = 50.0-((-1.0 * log10(j / (float)stereo_spectrum_bins)) * 25.0);
	  
	  // Convert to log frequency
	  //float y = log10(((22050.0 / float(stereo_spectrum_bins)) * x) + (0.5 * (22050.0 / float(stereo_spectrum_bins)))) * 5.0;
// 	  float y = log10(((22050.0 / float(stereo_spectrum_bins)) * j) + (0.5 * (22050.0 / float(stereo_spectrum_bins)))) * 5.0;

//  	  float y = log10(((22050.0 / float(spectrum_bins)) * j) + (0.5 * (22050.0 / float(spectrum_bins)))) * 5.0;
 	  y = (log10(((22050.0 / double(spectrum_bins)) * j) + (0.5 * (22050.0 / double(spectrum_bins))))) * 5.0;


	  // 	  if (i == 0 && j == 0) {
	  // 		cout << "x=" << x << " y=" << y << endl;
	  // 	  }
	  // 	  if (x < min_x) {
	  // 		min_x = x;
	  // 	  }
	  // 	  if (x > max_x) {
	  // 		max_x = x;
	  // 	  }
//   	  if (y < min_y) {
//   		min_y = y;
//   	  }
//   	  if (y > max_y) {
//   		max_y = y;
//   	  }
	  // sness - This was to figure out why the spectrum would move down
	  //  	  if (i == 0 && j == 0) {
	  //  		cout << "miny=" << y;
	  //  	  }
	  //  	  if (i == 0 && j == stereo_spectrum_bins - 1) {
	  //  		cout << " maxy=" << y << endl;
	  //  	  }
// 	  float z = i * display_speed + (display_speed * MAX_Z);
 	  z = i;

	  // sness - FIXME - probably something wrong in here
	  size = (
					(left_spectrum_ring_buffer[(i + ring_buffer_pos) % MAX_Z][j*2]) + 
					(right_spectrum_ring_buffer[(i + ring_buffer_pos) % MAX_Z][j*2])
					) / 2.0 * 2000; 
		
 	  if (size > 0.5) {
 		size = 0.5;
 	  }
	  
// 	  if (size < 0) {
// 		cout << "size=" << size << endl;
// 	  }

	  //   	  size = 0.2;

	  //  	  if (i > 197) {
	  // // 		cout << "i==0" << endl;
	  //  		glColor3f(1,1,1);
	  // 		size *= 3;
	  //  	  } else {
	  // 		glColor3f((size*5),1,0);
	  // 		//  		glColor3f(0,0,0);
	  //  	  }

	  // 	  glColor3f((size*5),1,0);
	  


	  //    	  glBegin(GL_TRIANGLES);
	  //    	  glVertex3f(x,y,z);
	  //    	  glVertex3f(x+size,y,z);
	  //    	  glVertex3f(x+size,y+size,z);
	  //    	  glEnd();

	  // A disk
	  //     glEnable(GL_LIGHTING);
	  //     glColor3f(1.0, 1.0, 1.0);
	  if (size > magnitude_cutoff) {

		glTranslated(x,y,z);

		// 		  // sness - FIXME
		// 		if (num_vertices == 1) {

		// 		  glColor3f((size*5),1,0);

		// 		  glBegin(GL_TRIANGLES);
		// 		  glVertex3f(x,y,z);
		// 		  glVertex3f(x+size,y,z);
		// 		  glVertex3f(x+size,y+size,z);
		// 		  glEnd();

		// 		} else {

		float mcolor[3];

		// Red dots if big magnitude
		if (size > 0.4) {
		  mcolor[0] = 1.0f;
		  mcolor[1] = 0.0f;
		  mcolor[2] = 0.0f;
		  mcolor[3] = 1.0f;
		} else {
		  mcolor[0] = (size*5);
		  mcolor[1] = 1.0f;
		  mcolor[2] = 0.0f;
		  mcolor[3] = 1.0f;
// 		  float mcolor[] = { (size*5), 1.0f, 0.0f, 1.0f };
		}

		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mcolor);

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
		  
		// 		}

		glTranslated(-1.0 * x,-1.0 * y,-1.0 * z);

	  }


 	}

	  
  }

//      cout << "min_x=" << min_x << " max_x=" << max_x << " min_y=" << min_y << " max_y=" << max_y << endl;
//    cout << "min_y=" << min_y << " max_y=" << max_y << " sb=" << spectrum_bins << endl;

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
//   cout << "angle=" << angle << endl;
  //   normalizeAngle(&angle);
  if (angle != xRot) {
	xRot = angle;
	emit xRotationChanged(angle);
	updateGL();
  }
}

// Set the y rotation angle
void GLWidget::setYRotation(int angle)
{
//   cout << "angle=" << angle << endl;
  //   normalizeAngle(&angle);
  if (angle != yRot) {
	yRot = angle;
	emit yRotationChanged(angle);
	updateGL();
  }
}

// Set the z rotation angle
void GLWidget::setZRotation(int angle)
{
  //   normalizeAngle(&angle);
//   cout << "angle=" << angle << endl;
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
// 	cout << "v=" << v << " yTrans=" << yTrans << endl;
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
  double val = v * -1;
  if (val != fogStart) {
	fogStart = val;
//   	cout << "v=" << v << " fogStart=" << fogStart << endl;
	emit fogStartChanged(val);
	glFogf(GL_FOG_START, fogStart);          // Fog Start Depth
	updateGL();
  }
}

void GLWidget::setFogEnd(int v)
{
  double val = v * -1;
  if (val != fogEnd) {
	fogEnd = val;
//   	cout << "v=" << v << " fogEnd=" << fogEnd << endl;
	emit fogEndChanged(val);
	glFogf(GL_FOG_END, fogEnd);          // Fog End Depth
	updateGL();
  }
}

void GLWidget::setFFTBins(int val) {

  if (val == 0) {
	set_fft_size(32);
  } else if (val == 1) {
	set_fft_size(64);
  } else if (val == 2) {
	set_fft_size(128);
  } else if (val == 3) {
	set_fft_size(256);
  } else if (val == 4) {
	set_fft_size(512);
  } else if (val == 5) {
	set_fft_size(1024);
  } else if (val == 6) {
	set_fft_size(2048);
  } else if (val == 7) {
	set_fft_size(4096);
  } else if (val == 8) {
	set_fft_size(8192);
  } else if (val == 9) {
	set_fft_size(16384);
  } else if (val == 10) {
	set_fft_size(32768);
  }
}

void GLWidget::set_fft_size(int val) {
//   cout << "setting fft size to " << val << endl;

  setInSamples(val);
  //   net_->updctrl("SoundFileSource/src/mrs_natural/inSamples",val);
  
  //   // sness - Not sure why if you give inSamples of 512, the
  //   // StereoSpectrum only outputs 128 values.  Shouldn't it output 256
  //   // values, one for each bin?


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

void GLWidget::setMagnitudeCutoff(int v) {
  magnitude_cutoff = ((v*v*v) * 0.0001) / 10000;
//   cout << "v=" << v << " mc=" << magnitude_cutoff << endl;
}

void GLWidget::setNumVertices(int v) {
//   cout << "setNumVertices(" << v << ")" << endl;

  num_vertices = v;

//   if (startList) {
	glDeleteLists(startList,5);
//   }


  buildDiskLists();
}

void GLWidget::setSongPosition(int v) {
   cout << "setSongPosition(" << v << ")" << endl;
  setPos(v);

}



void GLWidget::setDisplaySpeed(int v) {
//   cout << "setDisplaySpeed(" << v << ")" << endl;
  
  display_speed = v / 50.0;
}




void GLWidget::open() 
{
  QString fileName = QFileDialog::getOpenFileName(this);

//   net_->updctrl("mrs_real/israte", 44100.0);

  mwr_->updctrl(fnamePtr_, fileName.toStdString());
  mwr_->updctrl(initPtr_, true);

  mwr_->start();
  mwr_->play();
  play_state = true;

}

void GLWidget::clearRingBuffers() {
  for (int i = 0; i < MAX_Z; i++) {
	for (int j = 0; j < spectrum_bins; j++) {
	  left_spectrum_ring_buffer[i][j] = 0.0;
	  right_spectrum_ring_buffer[i][j] = 0.0;
	}
	for (int j = 0; j < stereo_spectrum_bins; j++) {
	  panning_spectrum_ring_buffer[i][j] = 0.0;
	}

  }

}

void GLWidget::setInSamples(int v) {
  insamples = v;
  spectrum_bins = insamples / 2.0;
  stereo_spectrum_bins = insamples / 4.0;

  mwr_->pause();

  mwr_->updctrl("mrs_natural/inSamples",insamples);
//   net_->updctrl("SoundFileSource/src/mrs_natural/inSamples",insamples);

  //
  // sness - FIXME - Ask George about this
  //
  //
  // sness - This *just* about works, but occassionally coredumps
  //  net_->updctrl("SoundFileSource/src/mrs_natural/inSamples",insamples);
  //
  // sness - This coredumps more often
  //   net_->updctrl("SoundFileSource/src/mrs_natural/inSamples",insamples);
  //   net_->updctrl("mrs_natural/inSamples",insamples);


  //     net_->updctrl("Gain/gain/mrs_natural/inSamples",insamples);
  //    net_->updctrl("AudioSink/dest/mrs_bool/initAudio", true);


  clearRingBuffers();

  mwr_->play();

}


void GLWidget::buildDiskLists() {

  // Setup for creating the disk
  int max_disks = 5;
  startList = glGenLists(max_disks);
  qobj = gluNewQuadric();
  gluQuadricCallback(qobj, GLU_ERROR, NULL);

  // Create the disk
  gluQuadricDrawStyle(qobj, GLU_FILL); /* all polygons wireframe */
  gluQuadricNormals(qobj, GLU_FLAT);

  glNewList(startList+0, GL_COMPILE);
  gluDisk(qobj, 0, 0.07, num_vertices, 1);
  glEndList();

  glNewList(startList+1, GL_COMPILE);
  gluDisk(qobj, 0, 0.09, num_vertices, 1);
  glEndList();

  glNewList(startList+2, GL_COMPILE);
  gluDisk(qobj, 0, 0.10, num_vertices, 1);
  glEndList();

  glNewList(startList+3, GL_COMPILE);
  gluDisk(qobj, 0, 0.11, num_vertices, 1);
  glEndList();

  glNewList(startList+4, GL_COMPILE);
  gluDisk(qobj, 0, 0.12, num_vertices, 1);
  glEndList();

}

