//
//  Environment.h
//  allAddonsExample
//
//  Created by Andre Perrotta on 5/22/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef GraphicalEnvironment_h
#define GraphicalEnvironment_h

#include "ofMain.h"
#include "utils.h"
#include "MarControlDiagramDisplay.h"
#include "MarSystemWidget.h"
#include "ProbingManager.h"

namespace Marsyas {

class Widget;
class MiniViewWidget;
class MarSystemThread;

class GraphicalEnvironment {
protected:

  double targetX_;
  double targetY_;
  bool move_;

  bool showChildrenConnections_;
  bool showLinkedControlsConnections_;

  MarSystemThread* msysThread_;

public://FIXME what shoud be private or public ?

  MarSystemWidget* msysw_;
  bool isLoaded();

  double xOffset_;
  double yOffset_;
  double zoomX_;
  double zoomY_;
  double scale_;
  double miniViewXscale_;
  double miniViewYscale_;
  double networkWidth_;
  double networkHeight_;

  ofTrueTypeFont fontSmall_;
  ofTrueTypeFont fontMedium_;
  ofTrueTypeFont fontBig_;

  //visualizer
  int updateMiniView_;
  int visualizationMode_;

  Marsyas::MiniViewWidget* mVf_;
  Marsyas::globalMouse* mouse_;

  //Control Diagram Display
  Marsyas::MarControlDiagramDisplay* ctrlDisplay_;


  //Probing
  ProbingManager* probe_;
  std::vector<std::vector<double> >* getDataBuffer();

  GraphicalEnvironment();
  ~GraphicalEnvironment();


  void setupForMarSystemWidget(MarSystemWidget* msysw);
  void setupForMarSystemWidgetByName(MarSystemWidget* msysw, string name);
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
  void debugger();

  void setFocusFromMarControlDiagram(int x0, int y0);
  void moveOffsets();

  void calculateTransformations();

  MarSystemWidget* getMarSystemWidget();

  void setMarSystemThread(MarSystemThread* msyst);
  MarSystemThread* getMarSystemThread();

};


}


#endif
