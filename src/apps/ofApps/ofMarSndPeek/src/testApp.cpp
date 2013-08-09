#include "testApp.h"

//--------------------------------------------------------------
testApp::testApp() :
  rotation(manualRotation),
  offset(manualOffset)
{}

//--------------------------------------------------------------
void
testApp::setup() {
//	ofSoundStreamListDevices();

  ofSetBackgroundAuto(true);
  ofBackground(0,0,0);

  ofSetFrameRate(1000/(10));
  glutSetWindowTitle("MarSndPeek");

//	font.loadFont(ofToDataPath("frabk.ttf"), 14);
  font.loadFont(ofToDataPath("HelveticaBold.ttf"), 14);
//	font.loadFont("frabk.ttf", 14);

  // brute force init
  manualRotation.set(30, 0, 0);
  offset.set(INITIAL_OFFSET);

  // Defaults
  rotationSpeed = 10;
  yScale = 350;
}

//--------------------------------------------------------------
void
testApp::update()
{}

//--------------------------------------------------------------
void testApp::draw()
{
  pre_draw_sndpeek();

  //
  // Draw the text for the various audio statistics
  //
  ofSetColor(0xffffff); // White text

  // Draw the waveform
  //
  ofSetColor(0xffffff); // White text

  for (int i = 0; i < net.waveformData.getSize()-1; i++)
    ofLine(((i+0 - 256 / 2.0) / 200.0) - 0.7, net.waveformData(0,i+0) + 2.5,
           ((i+1 - 256 / 2.0) / 200.0) - 0.7, net.waveformData(0,i+1) + 2.5);

  //
  // Draw the waterfall spectrum
  //
  int offset;

  glMatrixMode(GL_MODELVIEW);
  for (int i = 0; i < net.spectrumLines; i++)
  {
    glColor4f((i / (float)net.spectrumLines),1,0,1); // A color ramp from yellow to green

    glTranslatef(0.0, 0.0, 1.0);
    for (int j = 0; j < net.fftBins; j++)
    {
      offset = (i + net.ringBufferPos) % net.spectrumLines;
      ofLine((j+0 - net.fftBins / 2.0) / 40.0, yScale * net.spectrumRingBuffer[offset][j+0],
             (j+1 - net.fftBins / 2.0) / 40.0, yScale * net.spectrumRingBuffer[offset][j+1]);

    }
  }
  glMatrixMode(GL_PROJECTION);

  post_draw_sndpeek();

  int w = ofGetWidth();
  int h = ofGetHeight();

  ofSetColor(0xeeeeee);
  font.drawString(string("centroid = ")+ofToString(net.stats.centroid),	8,h-20);
  font.drawString(string("rolloff = ")+ofToString(net.stats.rolloff),		8,h-40);
  font.drawString(string("flux = ")+ofToString(net.stats.flux),			8,h-60);
  font.drawString(string("rms = ")+ofToString(net.stats.rms),				8,h-80);

  font.drawString(ofToString(net.rate*512, 3)+string(" Hz"),	w-100,h-20);
}

//--------------------------------------------------------------
void
testApp::pre_draw_sndpeek()
{
  // View messes up openGL state
  // Switch to GL_PROJECTION matrix mode
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  // Setup a perspective viewing system
  perspective(20,1,0.1,1000);

  // Switch back to GL_MODELVIEW mode
  glMatrixMode(GL_MODELVIEW);

  // Set the shading model
  glShadeModel(GL_SMOOTH);

  // Enable depth testing
  glEnable(GL_DEPTH_TEST);

  // Enable fog for depth cueing
  GLfloat fogColor[4]= {0.0f, 0.0f, 0.0f, 1.0f};
  glClearColor(0.0f,0.0f,0.0f,1.0f);  // Fog colour of black (0,0,0)
  glFogfv(GL_FOG_COLOR, fogColor);    // Set fog color
  glFogf(GL_FOG_MODE, GL_EXP2);       // Set the fog mode
  //	glFogi(GL_FOG_MODE, GL_LINEAR);       // Set the fog mode
  glFogf(GL_FOG_DENSITY, 0.02f);      // How dense will the fog be
  glHint(GL_FOG_HINT, GL_NICEST);     // Fog hint value : GL_DONT_CARE, GL_NICEST
  glFogf(GL_FOG_START, 10.0f);          // Fog Start Depth
  glFogf(GL_FOG_END, 50.0f);            // Fog End Depth
  glEnable(GL_FOG);                   // Enable fog

  // Antialias lines
  glEnable(GL_LINE_SMOOTH);
  glHint (GL_LINE_SMOOTH_HINT,GL_NICEST);

  // Clear the color and depth buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Load the identity matrix
  glLoadIdentity();

  // Translate the model to 0,0,-10
  glTranslatef(0.0, 0.0, -29.0);

  // Rotate the object around the x,y,z axis
  // negate x-axis
  // swap z and y axis for remote control
  glRotatef(-rotation.x, 1.0, 0.0, 0.0);
  glRotatef(rotation.z, 0.0, 1.0, 0.0);
  glRotatef(rotation.y, 0.0, 0.0, 1.0);

  //	glTranslated(0.0, 0.0, 29.0);
  //	cout << "Current offset: (" << offset.x << "," << offset.y << "," << offset.z << ")" << endl;
  glTranslatef(10*offset.x,
               10*offset.y,
               10*offset.z);
}

//--------------------------------------------------------------
void
testApp::post_draw_sndpeek()
{
  glDisable(GL_LINE_SMOOTH);
  glDisable(GL_FOG);
  glDisable(GL_DEPTH_TEST);

  ofSetupScreen();
}

//--------------------------------------------------------------
void
testApp::keyPressed(int key)
{}

//--------------------------------------------------------------
void
testApp::keyReleased(int key)
{}

//--------------------------------------------------------------
void
testApp::mouseMoved(int x, int y )
{}

//--------------------------------------------------------------
void
testApp::mouseDragged(int x, int y, int button)
{}

//--------------------------------------------------------------
void
testApp::mousePressed(int x, int y, int button)
{}

//--------------------------------------------------------------
void
testApp::mouseReleased(int x, int y, int button)
{}

//--------------------------------------------------------------
void
testApp::windowResized(int w, int h)
{}


void perspective(double fovy, double aspect, double zNear, double zFar)
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  double xmin, xmax, ymin, ymax;

  ymax = zNear * tan(fovy * M_PI / 360.0);
  ymin = -ymax;
  xmin = ymin * aspect;
  xmax = ymax * aspect;


  glFrustum(xmin, xmax, ymin, ymax, zNear, zFar);

  glMatrixMode(GL_MODELVIEW);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

  glDepthMask(GL_TRUE);
}