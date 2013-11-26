#include "glwidget.h"

#include <marsyas/system/MarSystemManager.h>

#include <QtGui>
#include <QtOpenGL>
#include <QTimer>
#include <QTextStream>

#include <math.h>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

GLWidget::GLWidget(QWidget *parent):
  QGLWidget(parent),
  m_song_len(0),
  m_song_sr(0)
{
  // Initialize member variables
  xRot = 30;
  yRot = 0;
  zRot = 0;

  init = 0;

  xTrans = 0;
  // For 200
  //   zTrans = -127;
  //   yTrans = -5.5;

  // For 50
  //   zTrans = -52;
  z_start = 70;
  //   zTrans = -1 * z_start;
  zTrans = -70;
  yTrans = -6.7;

  test_x = 0;
  test_y = 0;
  test_z = 0;

  insamples = 512;
  spectrum_bins = insamples / 2.0;
  stereo_spectrum_bins = insamples / 2.0;

  dot_size_multiplier = 1.0;

  // The number of vertices used to make the disk
  num_vertices = 10;

  // Defaults
  y_scale = 350;

  // Create space for the vertices we will display
  powerspectrum_ring_buffer = new double*[MAX_Z];
  panning_spectrum_ring_buffer = new double*[MAX_Z];

  for (int i = 0; i < MAX_Z; i++) {
    powerspectrum_ring_buffer[i] = new double[MAX_SPECTRUM_BINS];
    panning_spectrum_ring_buffer[i] = new double[MAX_STEREO_SPECTRUM_BINS];
  }
  clearRingBuffers();
  ring_buffer_pos = 0;

  //
  // Create the MarSystem
  //
  MarSystemManager mng;

  MarSystem *sys;
  sys = mng.create("Series", "net");
  sys->addMarSystem(mng.create("SoundFileSource", "src"));
  sys->addMarSystem(mng.create("AudioSink", "dest"));
  //   net_->addMarSystem(mng.create("Gain", "gain"));

  MarSystem* fanout = mng.create("Fanout", "fanout");

  MarSystem* powerspectrum_series = mng.create("Series", "powerspectrum_series");
  powerspectrum_series->addMarSystem(mng.create("Stereo2Mono", "stereo2mono"));
  powerspectrum_series->addMarSystem(mng.create("Windowing", "ham"));
  powerspectrum_series->addMarSystem(mng.create("Spectrum", "spk"));
  powerspectrum_series->addMarSystem(mng.create("PowerSpectrum", "pspk"));
  powerspectrum_series->addMarSystem(mng.create("Gain", "gain"));

  MarSystem* stereobranches_series = mng.create("Series", "stereobranches_series");
  MarSystem* stereobranches_parallel = mng.create("Parallel", "stereobranches_parallel");
  MarSystem* left = mng.create("Series", "left");
  MarSystem* right = mng.create("Series", "right");

  left->addMarSystem(mng.create("Windowing", "hamleft"));
  left->addMarSystem(mng.create("Spectrum", "spkleft"));

  right->addMarSystem(mng.create("Windowing", "hamright"));
  right->addMarSystem(mng.create("Spectrum", "spkright"));

  stereobranches_parallel->addMarSystem(left);
  stereobranches_parallel->addMarSystem(right);
  stereobranches_series->addMarSystem(stereobranches_parallel);
  stereobranches_series->addMarSystem(mng.create("StereoSpectrum", "sspk"));

  stereobranches_series->addMarSystem(mng.create("Gain", "gain"));

  fanout->addMarSystem(powerspectrum_series);
  fanout->addMarSystem(stereobranches_series);

  sys->addMarSystem(fanout);
  sys->addMarSystem(mng.create("Gain", "gain"));

  sys->setControl("SoundFileSource/src/mrs_real/israte", 44100.0);
  sys->setControl("SoundFileSource/src/mrs_real/osrate", 44100.0);
  sys->setControl("SoundFileSource/src/mrs_real/repetitions",-1.0);
  sys->setControl("AudioSink/dest/mrs_real/israte", 44100.0);
  sys->setControl("SoundFileSource/src/mrs_natural/inSamples",insamples);
  sys->setControl("mrs_natural/inSamples",insamples);
  sys->setControl("mrs_real/israte", 44100.0);
  sys->update();

  m_marsystem = sys;
  m_system = new MarsyasQt::System(sys);

  // Create controls for the system.
  m_fileNameControl = m_system->control("SoundFileSource/src/mrs_string/filename");
  m_sampleRateControl = m_system->control("SoundFileSource/src/mrs_real/osrate");
  m_blockSizeControl = m_system->control("mrs_natural/inSamples");
  m_sizeControl = m_system->control("SoundFileSource/src/mrs_natural/size");
  m_posControl =  m_system->control("SoundFileSource/src/mrs_natural/pos");
  m_initAudioControl = m_system->control("AudioSink/dest/mrs_bool/initAudio");

  m_spectrumControl =
    m_system->control("Fanout/fanout/Series/powerspectrum_series/PowerSpectrum/pspk/mrs_realvec/processedData");
  m_panningControl =
    m_system->control("Fanout/fanout/Series/stereobranches_series/StereoSpectrum/sspk/mrs_realvec/processedData");

  // Connect the animation timer that periodically redraws the screen.
  // It is activated in the 'play()' function.
  connect( &m_updateTimer, SIGNAL(timeout()), this, SLOT(animate()) );
}

//
// Code written by gtzan to support using the Microsoft Pressure
// Sensitive keyboard to control panning of three different input
// sources.  Not hooked up right now.
//
#if 0
void GLWidget::GLWidget_other_constructor(string inAudioFileName, QWidget *parent)
//   : QGLWidget(parent)
{
  // Initialize member variables
  xRot = 30;
  yRot = 0;
  zRot = 0;

  init = 0;

  xTrans = 0;
  // For 200
  //   zTrans = -127;
  //   yTrans = -5.5;

  // For 50
  //   zTrans = -52;
  z_start = 70;
  //   zTrans = -1 * z_start;
  zTrans = -70;
  yTrans = -6.7;

  test_x = 0;
  test_y = 0;
  test_z = 0;

  insamples = 512;
  spectrum_bins = insamples / 2.0;
  stereo_spectrum_bins = insamples / 2.0;

  dot_size_multiplier = 1.0;

  // The number of vertices used to make the disk
  num_vertices = 10;

  // Defaults
  y_scale = 350;

  // Create space for the vertices we will display
  powerspectrum_ring_buffer = new double*[MAX_Z];
  panning_spectrum_ring_buffer = new double*[MAX_Z];

  for (int i = 0; i < MAX_Z; i++) {
    powerspectrum_ring_buffer[i] = new double[MAX_SPECTRUM_BINS];
    panning_spectrum_ring_buffer[i] = new double[MAX_STEREO_SPECTRUM_BINS];
  }
  clearRingBuffers();
  ring_buffer_pos = 0;

  //
  // Create the MarSystem
  //
  MarSystemManager mng;

  net_ = mng.create("Series", "net");


  // net_->addMarSystem(mng.create("SoundFileSource", "src"));

  MarSystem* mixsrc = mng.create("Fanout/mixsrc");
  MarSystem* branch1 = mng.create("Series/branch1");
  MarSystem* branch2 = mng.create("Series/branch2");
  MarSystem* branch3 = mng.create("Series/branch3");

  branch1->addMarSystem(mng.create("SoundFileSource/src"));
  branch1->addMarSystem(mng.create("Gain/gain"));
  branch1->addMarSystem(mng.create("Panorama/pan"));

  branch2->addMarSystem(mng.create("SoundFileSource/src"));
  branch2->addMarSystem(mng.create("Gain/gain"));
  branch2->addMarSystem(mng.create("Panorama/pan"));


  branch3->addMarSystem(mng.create("SoundFileSource/src"));
  branch3->addMarSystem(mng.create("Gain/gain"));
  branch3->addMarSystem(mng.create("Panorama/pan"));

  branch1->updctrl("Panorama/pan/mrs_real/angle", 0.0);
  branch2->updctrl("Panorama/pan/mrs_real/angle", 0.0);
  branch3->updctrl("Panorama/pan/mrs_real/angle", 0.0);

  mixsrc->addMarSystem(branch1);
  mixsrc->addMarSystem(branch2);
  mixsrc->addMarSystem(branch3);

  net_->addMarSystem(mixsrc);
  net_->addMarSystem(mng.create("Sum/sum"));
  net_->updctrl("Sum/sum/mrs_bool/stereo", true);


  net_->addMarSystem(mng.create("AudioSink", "dest"));
  //   net_->addMarSystem(mng.create("Gain", "gain"));

  MarSystem* fanout = mng.create("Fanout", "fanout");

  MarSystem* powerspectrum_series = mng.create("Series", "powerspectrum_series");
  powerspectrum_series->addMarSystem(mng.create("Stereo2Mono", "stereo2mono"));
  powerspectrum_series->addMarSystem(mng.create("Windowing", "ham"));
  powerspectrum_series->addMarSystem(mng.create("Spectrum", "spk"));
  powerspectrum_series->addMarSystem(mng.create("PowerSpectrum", "pspk"));
  powerspectrum_series->addMarSystem(mng.create("Gain", "gain"));

  MarSystem* stereobranches_series = mng.create("Series", "stereobranches_series");
  MarSystem* stereobranches_parallel = mng.create("Parallel", "stereobranches_parallel");
  MarSystem* left = mng.create("Series", "left");
  MarSystem* right = mng.create("Series", "right");

  left->addMarSystem(mng.create("Windowing", "hamleft"));
  left->addMarSystem(mng.create("Spectrum", "spkleft"));

  right->addMarSystem(mng.create("Windowing", "hamright"));
  right->addMarSystem(mng.create("Spectrum", "spkright"));

  stereobranches_parallel->addMarSystem(left);
  stereobranches_parallel->addMarSystem(right);
  stereobranches_series->addMarSystem(stereobranches_parallel);
  stereobranches_series->addMarSystem(mng.create("StereoSpectrum", "sspk"));

  stereobranches_series->addMarSystem(mng.create("Gain", "gain"));

  fanout->addMarSystem(powerspectrum_series);
  fanout->addMarSystem(stereobranches_series);

  net_->addMarSystem(fanout);
  net_->addMarSystem(mng.create("Gain", "gain"));

  net_->updctrl("mrs_real/israte", 44100.0);
  net_->updctrl("Fanout/mixsrc/Series/branch1/SoundFileSource/src/mrs_real/israte", 44100.0);
  net_->updctrl("Fanout/mixsrc/Series/branch2/SoundFileSource/src/mrs_real/osrate", 44100.0);
  net_->updctrl("AudioSink/dest/mrs_real/israte", 44100.0);
  net_->updctrl("Fanout/mixsrc/Series/branch2/SoundFileSource/src/mrs_natural/inSamples",insamples);
  net_->updctrl("mrs_natural/inSamples",insamples);

  if (inAudioFileName != "") {
    // net_->updctrl("SoundFileSource/src/mrs_string/filename",inAudioFileName);
  }

  net_->updctrl("Fanout/mixsrc/Series/branch1/SoundFileSource/src/mrs_string/filename", "one.wav");
  net_->updctrl("Fanout/mixsrc/Series/branch2/SoundFileSource/src/mrs_string/filename", "two.wav");
  net_->updctrl("Fanout/mixsrc/Series/branch3/SoundFileSource/src/mrs_string/filename", "three.wav");



  net_->updctrl("Fanout/mixsrc/Series/branch1/SoundFileSource/src/mrs_real/repetitions",-1.0);
  net_->updctrl("Fanout/mixsrc/Series/branch2/SoundFileSource/src/mrs_real/repetitions",-1.0);
  net_->updctrl("Fanout/mixsrc/Series/branch3/SoundFileSource/src/mrs_real/repetitions",-1.0);

  net_->updctrl("mrs_natural/inSamples",insamples);

  net_->updctrl("mrs_real/israte", 44100.0);
  net_->updctrl("AudioSink/dest/mrs_bool/initAudio", true);

  cout << *net_ << endl;

  mwr_ = new MarSystemQtWrapper(net_);
  if (inAudioFileName != "") {
    mwr_->start();
    mwr_->play();
    play_state = true;
  }

  // Create some handy pointers to access the MarSystem
  posPtr_ = mwr_->getctrl("Fanout/mixsrc/Series/branch1/SoundFileSource/src/mrs_natural/pos");
  sizePtr_ = mwr_->getctrl("Fanout/mixsrc/Series/branch1/SoundFileSource/src/mrs_natural/size");
  osratePtr_ = mwr_->getctrl("Fanout/mixsrc/Series/branch1/SoundFileSource/src/mrs_real/osrate");
  initPtr_ = mwr_->getctrl("AudioSink/dest/mrs_bool/initAudio");
  fnamePtr_ = mwr_->getctrl("Fanout/mixsrc/Series/branch1/SoundFileSource/src/mrs_string/filename");

  // Create the animation timer that periodically redraws the screen
  QTimer *timer = new QTimer( this );
  connect( timer, SIGNAL(timeout()), this, SLOT(animate()) );
  timer->start(20); // Redraw the screen every 10ms

  setPos(0);
}
#endif
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

  m_updateTimer.stop();
  m_system->stop();

  static const bool NO_UPDATE = false;
  m_fileNameControl->setValue(fileName, NO_UPDATE);
  m_initAudioControl->setValue(true, NO_UPDATE);
  m_system->update();

  m_song_len = m_sizeControl->value().toInt();
  m_song_sr = m_sampleRateControl->value().toDouble();

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
  return QSize(800, 400);
}

// The maximum size of the widget
QSize GLWidget::sizeHint() const
{
  return QSize(800, 400);
}

// Initialize the GL widget
void GLWidget::initializeGL()
{
  // Set the background color to white
  //   qglClearColor(Qt::black);
  qglClearColor(Qt::black);

  // Set the shading model
  glShadeModel(GL_SMOOTH);

  // Enable depth testing
  glEnable(GL_DEPTH_TEST);

  // Enable fog for depth cueing
  GLfloat fogColor[4]= {0.0f, 0.0f, 0.0f, 1.0f};
  //   GLfloat fogColor[4]= {1.0f, 1.0f, 1.0f, 1.0f};

  glClearColor(0.0f,0.0f,0.0f,1.0f);  // Fog colour of black (0,0,0)

  glFogfv(GL_FOG_COLOR, fogColor);    // Set fog color
  glFogi(GL_FOG_MODE, GL_LINEAR);       // Set the fog mode
  glFogf(GL_FOG_DENSITY, 0.5f);      // How dense will the fog be
  glHint(GL_FOG_HINT, GL_NICEST);     // Fog hint value : GL_DONT_CARE, GL_NICEST
  glEnable(GL_FOG);                   // Enable fog

  setFogStart(-94);
  setFogEnd(-117);

  // Antialias lines
  glEnable(GL_LINE_SMOOTH);
  glHint (GL_LINE_SMOOTH_HINT,GL_NICEST);

  // Disks
  buildDiskLists();

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

  //    setFogStart(-94);
  //    setFogEnd(-117);

  glFogf(GL_FOG_START, 94);          // Fog Start Depth
  glFogf(GL_FOG_END, 117);          // Fog End Depth
}

// Paint the GL widget
void GLWidget::paintGL()
{
  // Clear the color and depth buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Load the identity matrix
  glLoadIdentity();

  // Translate the model to 0,0,-10
  glTranslated(xTrans, yTrans, zTrans);

  // Rotate the object around the x,y,z axis
  glRotated(xRot / 16.0, 1.0, 0.0, 0.0);
  glRotated(yRot / 16.0, 0.0, 1.0, 0.0);
  glRotated(zRot / 16.0, 0.0, 0.0, 1.0);

  glTranslated(xTrans, yTrans, zTrans);

  // Draw the object
  redrawScene();

}

void GLWidget::animate()
{
  Q_ASSERT(m_system->isRunning());

  updateGL();

  addDataToRingBuffer();

  emitControlChanges();
}

void GLWidget::emitControlChanges()
{
  int pos = m_posControl->value().toInt();
  int pos_percent = (m_song_len > 0 ? (float) pos / m_song_len : 0) * 100.f;
  emit posChanged(pos_percent);
}

// "manual" advancement of position
void GLWidget::setPos(int val)
{
  int position = (int)(val / 100.f * m_song_len);
  m_posControl->setValue(position);
}

void GLWidget::addDataToRingBuffer()
{
  realvec powerspectrum_data = m_spectrumControl->value().value<realvec>();
  realvec panning_data = m_panningControl->value().value<realvec>();

  int powerspectrum_rows = powerspectrum_data.getRows();
  int panning_rows = panning_data.getRows();

  for (int i = 0; i < powerspectrum_rows; i++) {
    powerspectrum_ring_buffer[ring_buffer_pos][i] = powerspectrum_data(i,0);
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

  if (init == 0) {
    setFogStart(-94);
    setFogEnd(-117);
    init = 1;
  }

  float mcolor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mcolor);

  //   float min_x = -24.6;
  //   float max_x = 24.6;
  //   float min_y = 3.7;
  //   float max_y = 28.3;

  float min_x = -24.6f;
  float max_x = 24.6f;
  float min_y = 7.1f;
  float max_y = 31.6f;

  float max_z = z_start;

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

  double x;
  double y;
  double z;
  double size;

  for (int i = 0; i < MAX_Z; i++) {
    for (int j = 0; j < stereo_spectrum_bins; j++) {
      x = (panning_spectrum_ring_buffer[(i + ring_buffer_pos) % MAX_Z][j]) * 30.0;
      y = (log10(((22050.0 / double(spectrum_bins)) * j) + (0.5 * (22050.0 / double(spectrum_bins))))) * 7.0;
      z = i;

      size = (powerspectrum_ring_buffer[(i + ring_buffer_pos) % MAX_Z][j]) * 2000;

      if (size > 0.5) {
        size = 0.5;
      }
      if (size > magnitude_cutoff) {
        glTranslated(x,y,z);
        float mcolor[4];

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

        glTranslated(-1.0 * x,-1.0 * y,-1.0 * z);

      }


    }


  }

}

// Resize the window
void GLWidget::resizeGL(int width, int height)
{
  // Setup the glViewport
  glViewport(0, 0, width, height);

  // Switch to GL_PROJECTION matrix mode
  glMatrixMode(GL_PROJECTION);

  // Load the identity matrix
  glLoadIdentity();

  // Setup a perspective viewing system
  gluPerspective(20,2.0,0.1,1000);

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

// Set the x rotation angle
void GLWidget::setXRotation(int angle)
{
  if (angle != xRot) {
    xRot = angle;
    // 	cout << "angle=" << angle << " xRot=" << xRot << endl;
    emit xRotationChanged(angle);
    updateGL();
  }
}

// Set the y rotation angle
void GLWidget::setYRotation(int angle)
{
  if (angle != yRot) {
    yRot = angle;
    emit yRotationChanged(angle);
    updateGL();
  }
}

// Set the z rotation angle
void GLWidget::setZRotation(int angle)
{
  if (angle != zRot) {
    zRot = angle;
    emit zRotationChanged(angle);
    updateGL();
  }
}

// Set the x translation val
void GLWidget::setXTranslation(int v)
{
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
  double val = v * -0.1;
  if (val != yTrans) {
    yTrans = val;
    //  	cout << "val=" << val << " yTrans=" << yTrans << endl;
    emit yTranslationChanged(val);
    updateGL();
  }
}

// Set the z translation val
void GLWidget::setZTranslation(int v)
{
  double val = v * -2;
  if (val != zTrans) {
    zTrans = val;
    emit zTranslationChanged(val);
    updateGL();
  }
}


void GLWidget::setFogStart (int v)
{
  double val = v * -1;
  if (val != fogStart) {
    fogStart = val;
    //    	cout << "v=" << v << " fogStart=" << fogStart << endl;

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
    // 	cout << "v=" << v << " fogEnd=" << fogEnd << endl;
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
  setInSamples(val);
}


void GLWidget::setMagnitudeCutoff(int v) {
  magnitude_cutoff = ((v*v*v) * 0.0001) / 10000;
}

void GLWidget::setNumVertices(int v) {
  num_vertices = v;
  glDeleteLists(startList,5);
  buildDiskLists();
}

void GLWidget::setDotSize(int v) {
  dot_size_multiplier = v / 50.0;

  //   cout << "v=" << v << " dsm=" << dot_size_multiplier << endl;

  glDeleteLists(startList,5);
  buildDiskLists();
}

void GLWidget::setSongPosition(int v) {
  setPos(v);
}

void GLWidget::setDisplaySpeed(int v) {
  display_speed = v / 50.0;
}

void GLWidget::clearRingBuffers() {
  for (int i = 0; i < MAX_Z; i++) {
    for (int j = 0; j < spectrum_bins; j++) {
      powerspectrum_ring_buffer[i][j] = 0.0;
    }
    for (int j = 0; j < stereo_spectrum_bins; j++) {
      panning_spectrum_ring_buffer[i][j] = 0.0;
    }
  }
}

void GLWidget::setInSamples(int v)
{
  bool was_playing = m_system->isRunning();

  if (was_playing)
    playPause();

  insamples = v;
  spectrum_bins = insamples / 2.0;
  stereo_spectrum_bins = insamples / 2.0;

  m_blockSizeControl->setValue(insamples, false);
  m_initAudioControl->setValue(true, false);
  m_system->update();

  clearRingBuffers();

  if (was_playing)
    playPause();
}

void GLWidget::buildDiskLists() {

  // Setup for creating the disk
  int max_disks = 5;
  startList = glGenLists(max_disks);
  qobj = gluNewQuadric();
  gluQuadricCallback(qobj, GLU_ERROR, NULL);

  gluQuadricDrawStyle(qobj, GLU_FILL);
  gluQuadricNormals(qobj, GLU_FLAT);

  glNewList(startList+0, GL_COMPILE);
  gluDisk(qobj, 0, 0.1 * dot_size_multiplier, num_vertices, 1);
  glEndList();

  glNewList(startList+1, GL_COMPILE);
  gluDisk(qobj, 0, 0.2 * dot_size_multiplier, num_vertices, 1);
  glEndList();

  glNewList(startList+2, GL_COMPILE);
  gluDisk(qobj, 0, 0.3 * dot_size_multiplier, num_vertices, 1);
  glEndList();

  glNewList(startList+3, GL_COMPILE);
  gluDisk(qobj, 0, 0.35 * dot_size_multiplier, num_vertices, 1);
  glEndList();

  glNewList(startList+4, GL_COMPILE);
  gluDisk(qobj, 0, 0.4 * dot_size_multiplier, num_vertices, 1);
  glEndList();

}

