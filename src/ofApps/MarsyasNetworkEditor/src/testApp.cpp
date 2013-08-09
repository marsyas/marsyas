#include "testApp.h"
#include "MarSystemWidgetManager.h"

using namespace Marsyas;
using namespace std;

//--------------------------------------------------------------
void testApp::setup()
{
  ofSetFrameRate(30);
  ofEnableSetupScreen();
  ofBackground(0, 0, 30);
  ofEnableSmoothing();
  ofEnableAlphaBlending();
  glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
  ofSetVerticalSync(true);


  //**********************************************************
  //GraphicalEnvironment creation
  graphicalEnv_ = new GraphicalEnvironment();

  //Marsyas Thread
  msysThread = new MarSystemThread(graphicalEnv_);
  graphicalEnv_->setMarSystemThread(msysThread);



  //***********************************************************
  //used to create a marsystem network manually for testing

  //we usualy start by creating a MarSystem manager
  //to help us on MarSystem creation
  MarSystemManager mng;

  //create the network, which is a simple Series network with a sine wave
  //oscilator and a audio sink object to send the ausio data for playing
  //in the sound card
  network = mng.create("Series", "network");
  //network->addMarSystem(mng.create("AudioSource", "src"));
  network->addMarSystem(mng.create("SineSource", "src"));
  //network->addMarSystem(mng.create("SoundFileSource", "src"));
  network->addMarSystem(mng.create("Gain", "vol"));
  network->addMarSystem(mng.create("AudioSink", "dest"));
  //network->addMarSystem(mng.create("SoundFileSink", "dest2"));


  //set the window (i.e. audio frame) size (in samples). Let's say, 256 samples.
  //This is done in the outmost MarSystem (i.e. the Series/network) because flow
  //controls (as is the case of inSamples) are propagated through the network.
  //Check the Marsyas documentation for mode details.
  network->updControl("mrs_natural/inSamples", 1024);

  //set oscilator frequency to 440Hz
  network->updControl("SineSource/src/mrs_real/frequency", 220.0);

  // set the sampling to 44100  - a safe choice in most configurations
  network->updControl("mrs_real/israte", 44100.0);
  //network->updControl("SoundFileSource/src/mrs_string/filename", "/test.wav");
  network->updControl("SoundFileSource/src/mrs_natural/nChannels", 1);
  //network->updControl("AudioSource/src/mrs_bool/initAudio", true);
  //network->updControl("AudioSource/src/mrs_natural/nChannels", 1);
  network->updControl("AudioSink/dest/mrs_bool/initAudio", true);
  //network->updControl("SoundFileSink/dest2/mrs_string/filename", "helloworld2.wav");
  network->updControl("Gain/vol/mrs_real/gain", 0.1);

  //Create corresponing Widgets
  MarSystemWidgetManager *wmng;
  wmng = new MarSystemWidgetManager(graphicalEnv_);
  networkWidget = wmng->setupWidgets(network);


  //GraphicalEnvironment setup for networkWidget
  graphicalEnv_->setupForMarSystemWidget(networkWidget);
  //graphicalEnv_->setupForMarSystemWidgetByName(networkWidget, "textWinNet");
  //***********************************************************

  msysThread->loadMarSystem(network);
  msysThread->start();


}

//--------------------------------------------------------------
void testApp::update()
{

  ofSetWindowTitle(ofToString(ofGetFrameRate()));


  graphicalEnv_->update();



}


//--------------------------------------------------------------
void testApp::draw()
{

  if(graphicalEnv_->isLoaded()) {
    graphicalEnv_->draw();
  }
  else {
    ofSetColor(255, 0, 0);
    ofDrawBitmapString("Drag and drop a Marsyas .mpl network file or create a network manually on the app setup routine", 10, 10);
  }



}

//--------------------------------------------------------------
void testApp::keyPressed  (int key)
{

  graphicalEnv_->keyPressed(key);


}

//--------------------------------------------------------------
void testApp::keyReleased(int key)
{

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y )
{


  graphicalEnv_->mouseMoved(x, y);



}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button)
{

  graphicalEnv_->mouseDragged(x, y, button);


}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button)
{

  graphicalEnv_->mousePressed(x, y, button);


}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button)
{

  graphicalEnv_->mouseReleased(x, y, button);





}


//--------------------------------------------------------------
void testApp::windowResized(int w, int h)
{

  graphicalEnv_->windowResized(w, h);


}


//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo info) {

  if( info.files.size() > 0 ) {
    MarSystemManager mng;

    network = mng.loadFromFile(info.files[0]); //FIXME: hardcoded .mpl input

    if (!network)
    {
      cout << "couldn't load .mpl file! Program will crash..." << endl;//FIXME: verify that mpl file loaded correctly
      //return;
    }

    //**********************************************************

    //network->put_html(cout);

    //**********************************************************
    //Create corresponing Widgets
    MarSystemWidgetManager *wmng;
    wmng = new MarSystemWidgetManager(graphicalEnv_);
    networkWidget = wmng->setupWidgets(network);
    //**********************************************************


    //**********************************************************
    //GraphicalEnvironment setup for networkWidget
    graphicalEnv_->setupForMarSystemWidget(networkWidget);
    //graphicalEnv_->setupForMarSystemWidgetByName(networkWidget, "textWinNet");

    msysThread->loadMarSystem(network);
    //msysThread->start();

  }

}

