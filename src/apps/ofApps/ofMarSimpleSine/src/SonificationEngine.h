#pragma once

#include "ofxMarsyasNetwork.h"
#include "ofxMSAInteractiveObject.h"

class SonificationEngine
  : public ofxMarsyasNetwork
  , public ofxMSAInteractiveObject
{
public:
  SonificationEngine(string name = "SonificationEngine");

  void setup();
  void update();
  void draw();
};
