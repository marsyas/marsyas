#include "SoundEngine.h"

//--------------------------------------------------------------
SoundEngine::SoundEngine(string name)
  : ofxMarsyasNetwork(name)
{
  priority	= 2;

  disableAllEvents();
  ofAddListener(ofEvents.setup, (ofxMSAInteractiveObject*)this, &ofxMSAInteractiveObject::_setup);
  ofAddListener(ofEvents.draw, (ofxMSAInteractiveObject*)this, &ofxMSAInteractiveObject::_draw);

  ofSetFrameRate(60); // if vertical sync is off, we can go a bit fast... this caps the framerate at 60fps.

  min_data.create(256,1);
  max_data.create(256,1);

  for (int i = 0; i < 256; i++) {
    min_data(i,0) = 999.9;
    max_data(i,0) = -999.9;
  }
}

//--------------------------------------------------------------
void
SoundEngine::setup() {
  Marsyas::MarSystem *fanout, *source;

  addMarSystem(mng.create("SoundFileSource/src"));
  addMarSystem(mng.create("Stereo2Mono", "stereo2mono"));
  addMarSystem(mng.create("AudioSink", "dest"));

  addMarSystem(mng.create("Windowing", "ham"));
  addMarSystem(mng.create("Spectrum", "spk"));
  addMarSystem(mng.create("PowerSpectrum", "pspk"));
  addMarSystem(mng.create("Gain", "gain"));

  updctrl("SoundFileSource/src/mrs_string/filename", "rutten.wav");

  updctrl("mrs_real/israte", 44100.0);
  updctrl("AudioSink/dest/mrs_bool/initAudio", true);

  run();
}

//--------------------------------------------------------------
void
SoundEngine::update() {

  data = getctrl("PowerSpectrum/pspk/mrs_realvec/processedData")->to<Marsyas::mrs_realvec>();
  // cout << data << endl;

  for (int i = 0; i < 256; i++) {
    if (data(i,0) > max_data(i,0)) {
      max_data(i,0) = data(i,0);
    }
    if (data(i,0) < min_data(i,0)) {
      min_data(i,0) = data(i,0);
    }
  }

  // cout << "max_data=" << max_data << "\tmin_data=" << min_data << endl;
}

//--------------------------------------------------------------
void
SoundEngine::draw() {


  //--------------------------- circles
  //let's draw a circle:
  ofFill();		// draw "filled shapes"

  for (int i = 0; i < 256; i++) {
    // int color = (data(i,0) * (1.0 / max_data)) * 256;
    int color = (data(i,0) * (1.0 / max_data(i,0))) * 256;
    ofSetColor(color,color,color);
    ofRect(i*4,10,4,100);

  }

}
