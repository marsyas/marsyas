//
//  MarControlDiagramNode.h
//  allAddonsExample
//
//  Created by Andre Perrotta on 5/17/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef MarControlDiagramNode_h
#define MarControlDiagramNode_h

#include "MarControlWidget.h"
#include "Widget.h"
#include "ofMain.h"




namespace Marsyas {
class MarControlDiagramNode : public Widget {

protected:

  MarControlWidget* ctrlWidget_;
  std::vector<MarControlDiagramNode*>* linkers_;

  MarControlDiagramNode* parent_;

  bool highlighted_;

public:
  MarControlDiagramNode(MarControlWidget* ctrlWidget, GraphicalEnvironment* env, MarControlDiagramNode* parent, bool highlighted);
  ~MarControlDiagramNode();

  void setup(MarControlWidget* ctrlWidget, GraphicalEnvironment* env, MarControlDiagramNode* parent, bool highlighted);
  void update();
  void draw();

  void addLinker(MarControlDiagramNode* ctrlWidget);

  std::vector<MarControlDiagramNode*>* getLinkers();

  MarControlWidget* getCtrlWidget();

  void organizeLinkers(int x, int y);



  void debugger();


  //mouse
  bool mouseOver();
  bool mousePressed();
  bool mouseDragged();
  bool mouseReleased();

  void highlightSelectedControl();


};

}




#endif
