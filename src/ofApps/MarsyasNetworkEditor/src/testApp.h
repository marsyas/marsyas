#ifndef _TEST_APP
#define _TEST_APP


#include "ofMain.h"

#include "MarSystemThread.h"
#include <marsyas/system/MarSystemManager.h>

#include "utils.h"
#include "MarSystemWidget.h"
#include "MiniViewWidget.h"
#include "MarControlDiagramDisplay.h"

#include "GraphicalEnvironment.h"
#include "GraphicalParameters.h"

#undef PI //FIXME



class Marsyas::MarSystem;
//class Marsyas::MarSystemWidget;
//class Marsyas::globalMouse;


class testApp : public ofBaseApp
{
public:
  void setup();
  void update();
  void draw();

  void keyPressed  (int key);
  void keyReleased(int key);
  void mouseMoved(int x, int y );
  void mouseDragged(int x, int y, int button);
  void mousePressed(int x, int y, int button);
  void mouseReleased(int x, int y, int button);
  void windowResized(int w, int h);

  void dragEvent(ofDragInfo dragInfo);


  //Marsyas stuff
  Marsyas::MarSystem *network;
  Marsyas::MarSystemWidget *networkWidget;
  Marsyas::MarSystemThread *msysThread;



  //GraphicalEnvironment
  Marsyas::GraphicalEnvironment* graphicalEnv_;


};

#endif
