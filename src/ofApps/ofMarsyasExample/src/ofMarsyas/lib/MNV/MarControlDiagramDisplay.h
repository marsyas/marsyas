//
//  MarControlDiagramDisplay.h
//  allAddonsExample
//
//  Created by Andre Perrotta on 5/17/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef MarControlDiagramDisplay_h
#define MarControlDiagramDisplay_h
#include "ofMain.h"
#include "Widget.h"
#include "maximizeButton.h"
#include "MarControl.h"
#include "MarSystemWidget.h"


namespace Marsyas
{
class GraphicalEnvironment;
class MarControlDiagramNode;

class MarControlDiagramDisplay : public Widget {

public:

  MarControlDiagramDisplay(MarSystemWidget* msysw, GraphicalEnvironment* env);
  ~MarControlDiagramDisplay();

  void setup(MarSystemWidget* msysw, GraphicalEnvironment* env);
  void update();
  void draw();



  //mouse
  bool mouseOver();
  bool mousePressed();
  bool mouseDragged();
  bool mouseReleased();

  //MarControl stuff
  void loadMarControl(MarControlPtr ctrl);
  bool isLoaded_;


  //draw stuff
  void organizeDiagram();

  void debugger();


protected:

  MarSystemWidget* msysw_;

  MaximizeButton* mBtn_;

  bool isVisible_;

  MarControlPtr ctrl_;

  std::vector<std::pair<MarControlPtr, MarControlPtr> > linkedControls_;

  std::vector<MarControlDiagramNode*> nodes;

  MarControlDiagramNode* nodeExists(MarControlDiagramNode* node, MarControlWidget* cw);
};

}

#endif
