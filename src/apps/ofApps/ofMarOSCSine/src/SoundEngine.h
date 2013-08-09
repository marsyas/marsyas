#pragma once

#include "ofxMarsyasNetwork.h"
#include "ofxMSAInteractiveObject.h"
#include "SineChanger.h"

class SoundEngine
  : public ofxMarsyasNetwork
  , public ofxMSAInteractiveObject
{
public:
  SoundEngine(string name = "SoundEngine");

  SineChanger* sineChanger;

  void setup();
  void update();
  void draw();
};
