#pragma once

// #include "settings.h"

#include "ofTypes.h"
#include "ofxThread.h"
#include "ofAppRunner.h"

#include "ofxOsc.h"

// listen on port 12345
#define PORT 12345
#define NUM_MSG_STRINGS 20

class SineChanger
  : public ofxThread
{
  friend class SonificationEngine;
public:
  SineChanger();
  ~SineChanger();

  void setup();
  void destroy();
  void update();
  void draw();

  double getCount();

protected:
  void threadedFunction();

private:

  ofxOscReceiver	receiver;

  int				current_msg_string;
  string		msg_strings[NUM_MSG_STRINGS];
  float			timers[NUM_MSG_STRINGS];

  int				mouseX, mouseY;
  string			mouseButtonState;

  int counter;

public:

};
