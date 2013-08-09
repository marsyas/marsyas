#include "SoundEngine.h"


//--------------------------------------------------------------
SoundEngine::SoundEngine(string name)
  : ofxMarsyasNetwork(name)
{
  priority	= 2;

  disableAllEvents();
  ofAddListener(ofEvents.setup, (ofxMSAInteractiveObject*)this, &ofxMSAInteractiveObject::_setup);
  ofAddListener(ofEvents.draw, (ofxMSAInteractiveObject*)this, &ofxMSAInteractiveObject::_draw);

}

//--------------------------------------------------------------
void
SoundEngine::setup()
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
SoundEngine::update()
{
  if (sineChanger->lock()) {
    updctrl("SineSource/src/mrs_real/frequency", sineChanger->getCount());
    sineChanger->unlock();
  }
}

//--------------------------------------------------------------
void
SoundEngine::draw() {
}
