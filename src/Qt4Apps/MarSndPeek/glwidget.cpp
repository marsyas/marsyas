#include <QtGui>
#include <QtOpenGL>
#include <QTimer>

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

// The colors for the faces of the cube
GLfloat colors[][3] = {
   { 0.0,  0.0,  0.0}, 
   { 0.0,  0.0,  1.0}, 
   { 0.0,  1.0,  0.0}, 
   { 0.0,  1.0,  1.0}, 
   { 1.0,  0.0,  0.0}, 
   { 1.0,  0.0,  1.0}};

GLWidget::GLWidget(string inAudioFileName, QWidget *parent)
  : QGLWidget(parent)
{
  // Initialize member variables
  object = 0;
  xRot = 30;
  yRot = 0;
  zRot = 0;

  start_xRot = 0;
  start_yRot = 0;
  start_zRot = 0;

  end_xRot = 0;
  end_yRot = 0;
  end_zRot = 0;

  // Setup a timer
  timer = new QTimer(this);
  timerCount = 0;

  test_x = 0;
  test_y = 0;
  test_z = 0;

//   // Connect the timeout signal of the timer to a slot to do the timer rotation
//   connect(timer, SIGNAL(timeout()), this, SLOT(doTimerRotate()));

  // Defaults
  rotation_speed = 10;
  y_scale = 350;


  // Create space for the vertices we will display
  spectrum_ring_buffer = new double*[MAX_SPECTRUM_LINES];
  for (int i = 0; i < MAX_SPECTRUM_LINES; i++) {
	spectrum_ring_buffer[i] = new double[SPECTRUM_SIZE];
	for (int j = 0; j < SPECTRUM_SIZE; j++) {
	  spectrum_ring_buffer[i][j] = 0.0;
	}
  }
  ring_buffer_pos = 0;
  
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

  net->updctrl("SoundFileSource/src/mrs_string/filename",inAudioFileName);
  net->updctrl("SoundFileSource/src/mrs_real/repetitions",-1.0);

//   net->updctrl("mrs_real/israte", 44100.0);
  net->updctrl("AudioSink/dest/mrs_bool/initAudio", true);

  mwr_ = new MarSystemQtWrapper(net);
  mwr_->start();
  mwr_->play();

//   cout << "Creating playback network" << endl;

//   cout << "Creating playback network" << endl;

//   // Create playback network
//   pnet_ = mng.create("Series", "pnet_");
//   pnet_->addMarSystem(mng.create("SoundFileSource", "src"));
// //   pnet_->addMarSystem(mng.create("Gain", "gain"));
//   pnet_->addMarSystem(mng.create("AudioSink", "dest"));
//   pnet_->linkctrl("mrs_bool/notEmpty","SoundFileSource/src/mrs_bool/notEmpty");

//   mwr_ = new MarSystemQtWrapper(pnet_);
//   string infile = "./in_my_life.wav";
//   pnet_->updctrl("SoundFileSource/src/mrs_string/filename",infile);

//   mwr_->updctrl("AudioSink/dest/mrs_bool/initAudio", true);

//   mwr_->start();
//   mwr_->play();
//   cout << "Done creating playback network" << endl;



  // The animation timer
  QTimer *timer = new QTimer( this ); 
  //this creates a Qt timer event
   connect( timer, SIGNAL(timeout()), this, SLOT(animate()) ); 
  //this specifies what is to happen when the timer sends
  //  a signal, ie, when the timer event occurs
  //  (SIGNAL(timeout)) send a signal to
  //  SLOT(animate()) - which is in glbox.cpp 
  timer->start(10);
  //this specifies that the timer event happens every 4
  //   milliseconds and that it is infinite loop
  
}

GLWidget::~GLWidget()
{
  makeCurrent();
  glDeleteLists(object, 1);
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

// Initialize the GL widget
void GLWidget::initializeGL()
{
  // Set the background color to white
  qglClearColor(Qt::black);

//   // Construct the object
//   object = makeObject();
  
//   // Set the shading model to GL_FLAT
   glShadeModel(GL_SMOOTH);
  
  // Enable depth testing
  glEnable(GL_DEPTH_TEST);

//   // Cull the faces that face backwards
//   glEnable(GL_CULL_FACE);
  
   // 
   // Enable fog based depth cueing
   //
    GLfloat fogColor[4]= {0.0f, 0.0f, 0.0f, 1.0f};

    glClearColor(0.0f,0.0f,0.0f,1.0f);// We'll Clear To The Color Of The Fog ( Modified )

    glFogi(GL_FOG_MODE, GL_EXP2);// Fog Mode (GL_EXP, GL_EXP2, GL_LINEAR)
    glFogfv(GL_FOG_COLOR, fogColor);// Set Fog Color
    glFogf(GL_FOG_DENSITY, 0.02f);// How Dense Will The Fog Be
    //    glHint;// Fog Hint Value
    glHint(GL_FOG_HINT, GL_NICEST);// Fog Hint Value(GL_DONT_CARE, GL_NICEST)
    glFogf(GL_FOG_START, 0.1f);// Fog Start Depth
    glFogf(GL_FOG_END, 0.2f);// Fog End Depth
    glEnable(GL_FOG);// Enables GL_FOG

     //
     // Antialias lines
     //
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
  redrawObject();
  
//    cout << "redrawing" << endl;
//   mwr_->play();
//   cout << mwr_->getctrl("mrs_realvec/processedData")->to<mrs_realvec>() << endl;


}

void GLWidget::animate() {
//    cout << "animate" << endl;
  emit updateGL();
  addDataToRingBuffer();

}

void GLWidget::addDataToRingBuffer() {
  mrs_realvec data = mwr_->getctrl("Parallel/stereobranches/Series/left/PowerSpectrum/leftpspk/mrs_realvec/processedData")->to<mrs_realvec>();

  for (int i = 0; i < SPECTRUM_SIZE; i++) {
	spectrum_ring_buffer[ring_buffer_pos][i] = data(i,0);
  }
  
//   cout << "ring_buffer_pos=" << ring_buffer_pos << endl;

  ring_buffer_pos += 1;
  if (ring_buffer_pos >= MAX_SPECTRUM_LINES) {
	ring_buffer_pos = 0;
  }


}

void GLWidget::redrawObject() {

  glColor3f(1,1,1);
  // sness - These positions were determined empirically by moving
  // text around on the screen.  It would be nice to figure out how to
  // do this more automatically. The corners are: top=2.85 bottom=0
  // left=-1.5 right=1.5
  glRasterPos3f(-1.5,2.85,49);
  string str = "test123";
  for(int i = 0; i < 7; i++) {
 	glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, str.at(i));
  }

  for (int i = 0; i < MAX_SPECTRUM_LINES; i++) {
	glColor3f((i / (float)MAX_SPECTRUM_LINES),1,0);
 	//glColor3fv(colors[i % 6]);

	glBegin(GL_LINE_STRIP);
	for (int j = 0; j < SPECTRUM_SIZE; j++) {
	  // sness - The 70 and 350 below were empirically chosen to fit on the screen
	  float x = (j - SPECTRUM_SIZE / 2.0) / 40.0;
// 	  float y = 350 * spectrum_ring_buffer[(i + ring_buffer_pos) % MAX_SPECTRUM_LINES][j];
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

  // Setup an orthographic viewing system
//   glOrtho(-2.0, +2.0, +2.0, -2.0, 4.0, 15.0);
//   gluPerspective(30,1,0.1,1000);
  gluPerspective(20,1,0.1,1000);

  // Switch back to GL_MODELVIEW mode
  glMatrixMode(GL_MODELVIEW);
}

// Draw a face of the cube
void GLWidget::drawCubeFace(int a, int b, int c, int d) {
  // Start drawing the object
  glBegin(GL_POLYGON);

  // Draw the vertices of the cube
  glVertex3fv(vertices[a]);
  glVertex3fv(vertices[b]);
  glVertex3fv(vertices[c]);
  glVertex3fv(vertices[d]);

  // End drawing the object
  glEnd();
}

// Make the cube object
GLuint GLWidget::makeObject()
{
  // Make a new display list
  GLuint list = glGenLists(1);
  glNewList(list, GL_COMPILE);

  // Draw and color each face of the cube
//   glColor3fv(colors[0]);
//   draw_cube_face(0,3,2,1);
//   glColor3fv(colors[2]);
//   draw_cube_face(2,3,7,6);
//   glColor3fv(colors[3]);
//   draw_cube_face(3,0,4,7);
//   glColor3fv(colors[1]);
//   draw_cube_face(1,2,6,5);
//   glColor3fv(colors[4]);
//   draw_cube_face(4,5,6,7);
//   glColor3fv(colors[5]);
//   draw_cube_face(5,4,0,1);

  glColor3fv(colors[0]);
  glBegin(GL_TRIANGLES);
  for (int i = 0; i < 8; i++) {
 	glVertex3f(vertices[i][0]+0.0,vertices[i][1]+0.0,vertices[i][2]+0.0);
	glVertex3f(vertices[i][0]+0.05,vertices[i][1]+0.05,vertices[i][2]+0.0);
	glVertex3f(vertices[i][0]+0.05,vertices[i][1]-0.05,vertices[i][2]+0.0);
  }
  glEnd();

  

  glEndList();
  return list;
}

// Normalize an angle
void GLWidget::normalizeAngle(int *angle)
{
  while (*angle < 0)
	*angle += 360 * 16;
  while (*angle > 360 * 16)
	*angle -= 360 * 16;
}

// // Set the end x rotation value
// void GLWidget::setEndXRotation(int angle)
// {
//   normalizeAngle(&angle);
//   end_xRot = angle;
// }

// // Set the end y rotation value
// void GLWidget::setEndYRotation(int angle)
// {
//   normalizeAngle(&angle);
//   end_yRot = angle;
// }

// // Set the end z rotation value
// void GLWidget::setEndZRotation(int angle)
// {
//   normalizeAngle(&angle);
//   end_zRot = angle;
// }

// // Start the animation timer
// void GLWidget::startTimerRotate()
// {
//   // If we are already doing an animation, just return
//   if (timer->isActive())
// 	return;

//   // Start the timer, with the timeout of the timer being the rotation
//   // speed
//   timer->start(rotation_speed);

//   // Initialize the counter for how many steps we are through the
//   // animation
//   timerCount = 0;
// }

// // Do one click of the animation timer
// void GLWidget::doTimerRotate() 
// {
//   // Increment the animation steps
//   ++timerCount;

//   // If we've done the required number of steps, stop the animation timer
//   if (timerCount >= TIMER_COUNT_STEPS)
// 	timer->stop();

//   // Emit a signal to say that the number of animation steps has changed
//   emit timerChanged(timerCount);

//   // Linearly interpolate the values for the x,y,z rotation values and
//   // emit signals for them
//   int x = start_xRot + ((timerCount / TIMER_COUNT_STEPS) * end_xRot);
//   int y = start_yRot + ((timerCount / TIMER_COUNT_STEPS) * end_yRot);
//   int z = start_zRot + ((timerCount / TIMER_COUNT_STEPS) * end_zRot);

//   // Set the x,y,z rotation angles
//   setXRotation(x);
//   setYRotation(y);
//   setZRotation(z);

// }

// // Set the rotation speed
// void GLWidget::setRotationSpeed(int speed)
// {
//   rotation_speed = speed;
// }



// void GLWidget::playSong() {
// }

// Set the test z rotation value
void GLWidget::setYScale(int scale)
{
  y_scale = scale;
  cout << "y_scale=" << y_scale << endl;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


// Set the test x rotation value
void GLWidget::setTestX(int val)
{
//   test_x = 5 - (val / 10);
  test_x = 5.0 - (val / 20.0);
  cout << "x :: val=" << val << " x=" << test_x << endl;
}

// Set the test y rotation value
void GLWidget::setTestY(int val)
{
  test_y = 5.0 - (val / 20.0);
  cout << "y :: val=" << val << " y=" << test_y << endl;
}

// Set the test z rotation value
void GLWidget::setTestZ(int val)
{
  test_z = val;
  cout << "z :: val=" << val << " z=" << test_z << endl;
}

