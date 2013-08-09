#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup() {
  //needed to display properly
  ofSetFrameRate(30);
  ofEnableSetupScreen();
  ofBackground(0, 0, 30);
  ofEnableSmoothing();
  ofEnableAlphaBlending();
  glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
  ofSetVerticalSync(true);

  //create or load a Marsyas network
  marsyas = new ofMarsyas();

  /*
  //uncoment this part if you want to use this method
  //********************************************************
  //create the marsyas network from a .mpl file

  if(!marsyas->createFromFile("peakClustering.mpl")){
      cout<<endl<<"couldn«t load .mpl file !! App will crash !";
  }
  //********************************************************
  */

  //uncoment this part if you want to use this method
  //********************************************************
  //creating manually and loading ofMarsyas with the Marsystem pointer:
  //this is an exampe of a simple sine wave generator network:
  Marsyas::MarSystemManager mng;
  Marsyas::MarSystem *network;
  network = mng.create("Series", "network");
  network->addMarSystem(mng.create("SineSource", "src"));
  network->addMarSystem(mng.create("Gain", "vol"));
  network->addMarSystem(mng.create("AudioSink", "dest"));
  network->updControl("mrs_natural/inSamples", 1024);
  network->updControl("SineSource/src/mrs_real/frequency", 220.0);
  network->updControl("mrs_real/israte", 44100.0);
  network->updControl("AudioSink/dest/mrs_bool/initAudio", true);
  network->updControl("Gain/vol/mrs_real/gain", 0.3);

  if(!marsyas->createFromPointer(network)) {
    cout<<endl<<"Network creation error !! App will crash !";
  }
  //********************************************************
  //starts the marsyas network in another thread and tick() the network
  marsyas->saveToFile("myNetwork.mpl");


  //start the network
  marsyas->start();

}



//--------------------------------------------------------------
void testApp::update() {
  ofSetWindowTitle("Press h for help.   Framerate = " + ofToString((int)ofGetFrameRate()));
  //updating marsyas only affects visualization
  marsyas->update();

  buffer = marsyas->getDataBuffer();

}

//--------------------------------------------------------------
void testApp::draw() {
  marsyas->draw();

}

//--------------------------------------------------------------
void testApp::keyPressed(int key) {
  marsyas->keyPressed(key);
  switch(key) {
  case 's':
    marsyas->saveToFile("sine-wave.mpl");
    break;
  }
}

//--------------------------------------------------------------
void testApp::keyReleased(int key) {
  marsyas->keyReleased(key);
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y) {
  marsyas->mouseMoved(x, y);
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button) {
  marsyas->mouseDragged(x, y, button);
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button) {
  marsyas->mousePressed(x, y, button);
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button) {
  marsyas->mouseReleased(x, y, button);
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h) {
  marsyas->windowResized(w, h);
}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo) {

}