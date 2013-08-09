#pragma once

#include "ofMain.h"

#define INITIAL_OFFSET			0.0, -0.75, 0.0
//0.0, -0.75, -29.0
#define ROTATION_RANGE			-90,90
//0,360

#include "MarSndPeek.h"

class testApp : public ofBaseApp {

public:
  testApp();

  void setup();
  void update();
  void draw();

  void pre_draw_sndpeek();
  void post_draw_sndpeek();

  void keyPressed  (int key);
  void keyReleased(int key);
  void mouseMoved(int x, int y );
  void mouseDragged(int x, int y, int button);
  void mousePressed(int x, int y, int button);
  void mouseReleased(int x, int y, int button);
  void windowResized(int w, int h);

  MarSndPeek net;

  ofTrueTypeFont font;

  ofPoint &rotation, &offset;
  ofPoint manualRotation, manualOffset;

  int yScale;
  int rotationSpeed;
};

// openGLES helpers
void perspective(double fovy, double aspect, double zNear, double zFar);
