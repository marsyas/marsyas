#pragma once

#include "ofxMarsyasNetwork.h"
#include "ofxMSAInteractiveObject.h"

class SoundEngine : public ofxMarsyasNetwork, public ofxMSAInteractiveObject
{
public:
  SoundEngine(string name = "SoundEngine");

  void setup();
  void update();
  void draw();

  // Windowed waveform for display
  Marsyas::mrs_realvec data;

  Marsyas::mrs_realvec min_data;
  Marsyas::mrs_realvec max_data;

private:
  bool muted;
};
