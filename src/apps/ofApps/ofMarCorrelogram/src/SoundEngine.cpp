#include "SoundEngine.h"

//--------------------------------------------------------------
SoundEngine::SoundEngine(string name)
  : ofxMarsyasNetwork(name)
{
  priority	= 2;

  disableAllEvents();
  ofAddListener(ofEvents.setup, (ofxMSAInteractiveObject*)this, &ofxMSAInteractiveObject::_setup);
  ofAddListener(ofEvents.draw, (ofxMSAInteractiveObject*)this, &ofxMSAInteractiveObject::_draw);

  ofSetFrameRate(10); // if vertical sync is off, we can go a bit fast... this caps the framerate at 60fps.

  max_data.create(POWERSPECTRUM_BUFFER_SIZE);

  for (int i = 0; i < POWERSPECTRUM_BUFFER_SIZE; i++) {
    max_data(i) = -999.9;
  }
  counter = 0;
}

//--------------------------------------------------------------
void
SoundEngine::setup() {
  Marsyas::MarSystem *source, *accum, *accum_series;

  accum = mng.create("Accumulator", "accum");
  addMarSystem(accum);

  accum_series = mng.create("Series", "accum_series");
  accum->addMarSystem(accum_series);

  accum_series->addMarSystem(mng.create("SoundFileSource/src"));
  accum_series->addMarSystem(mng.create("Stereo2Mono", "stereo2mono"));
  accum_series->addMarSystem(mng.create("AudioSink", "dest"));
  accum_series->addMarSystem(mng.create("Windowing", "ham"));
  accum_series->addMarSystem(mng.create("Spectrum", "spk"));
  accum_series->addMarSystem(mng.create("PowerSpectrum", "pspk"));

  addMarSystem(mng.create("ShiftInput", "si"));
  addMarSystem(mng.create("AutoCorrelation", "auto"));

  updctrl("Accumulator/accum/mrs_natural/nTimes", 10);

  updctrl("Accumulator/accum/Series/accum_series/SoundFileSource/src/mrs_string/filename", "rutten.wav");
  updctrl("ShiftInput/si/mrs_natural/winSize", MEMORY_SIZE);

  updctrl("mrs_real/israte", 44100.0);
  updctrl("Accumulator/accum/Series/accum_series/AudioSink/dest/mrs_bool/initAudio", true);

  run();
}

//--------------------------------------------------------------
void
SoundEngine::update() {
}

//--------------------------------------------------------------
void
SoundEngine::draw() {

  // Get the correlogram data from the network
  data = getctrl("mrs_realvec/processedData")->to<Marsyas::mrs_realvec>();

  // Every 100 ticks, calculate the max value for each bin
  if (counter % 100 == 0) {
    for (int x = 0; x < MEMORY_SIZE; x++) {
      for (int y = 0; y < POWERSPECTRUM_BUFFER_SIZE; y++) {
        if (data(y,x) > max_data(y)) {
          max_data(y) = data(y,x);
        }
      }
    }
  }

  ofFill();		// draw "filled shapes"

  // Draw a rectangle for each element in the array
  for (int x = 0; x < MEMORY_SIZE; x++) {
    for (int y = 0; y < POWERSPECTRUM_BUFFER_SIZE; y++) {
      int color = 255 - ((data(y,x) * (1.0 / max_data(y))) * 256);
      ofSetColor(color,color,color);
      ofRect(x*4,y*4,4,4);
    }
  }

}
