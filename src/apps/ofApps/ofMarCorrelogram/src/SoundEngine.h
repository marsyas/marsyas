#pragma once

#include "ofxMarsyasNetwork.h"
#include "ofxMSAInteractiveObject.h"

#define POWERSPECTRUM_BUFFER_SIZE 257
#define MEMORY_SIZE 300

class SoundEngine : public ofxMarsyasNetwork, public ofxMSAInteractiveObject
{
public:
  SoundEngine(string name = "SoundEngine");

  void setup();
  void update();
  void draw();

  // Windowed waveform for display
  Marsyas::mrs_realvec data;

  // Marsyas::mrs_realvec autoCorrelationRingBuffer;

  // int autoCorrelationRingBufferIndex;


  // Marsyas::mrs_realvec min_data;
  Marsyas::mrs_realvec max_data;

  int counter;

private:
  bool muted;
};
