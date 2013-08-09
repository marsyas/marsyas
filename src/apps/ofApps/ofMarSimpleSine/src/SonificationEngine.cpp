#include "SonificationEngine.h"


//--------------------------------------------------------------
SonificationEngine::SonificationEngine(string name)
  : ofxMarsyasNetwork(name)
{
  priority	= 2;
}

//--------------------------------------------------------------
void
SonificationEngine::setup()
{
  cout << "setup" << endl;

  addMarSystem(mng.create("SineSource", "src"));
  addMarSystem(mng.create("AudioSink", "dest"));

  updctrl("mrs_real/israte", 44100.0);
  updctrl("SineSource/src/mrs_real/frequency", 440.0);
  updctrl("AudioSink/dest/mrs_bool/initAudio", true);

  run();
}

//--------------------------------------------------------------
void
SonificationEngine::update()
{
}

//--------------------------------------------------------------
void
SonificationEngine::draw() {
}
