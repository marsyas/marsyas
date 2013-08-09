//
//  MiniViewWidget.h
//  allAddonsExample
//
//  Created by André Perrotta on 2/28/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//
#ifndef MINIVIEW_WIDGET_H
#define MINIVIEW_WIDGET_H

#include "GraphicalEnvironment.h"
#include "Widget.h"



namespace Marsyas
{


class MiniViewWidget : public Widget
{
protected:
  float clickPointOffsetX_;
  float clickPointOffsetY_;
  bool miniDragLockDown_;
  bool miniDragLockUp_;

  int targetX_;
  int targetY_;


public:

  MiniViewWidget(GraphicalEnvironment* env);


  void draw();
  void update();
  void updatePosition(int x0, int y0);

  //void move(double x, double y); FIXME !!!

  void focusFromMarControlDiagram(int x0, int y0);

  bool mouseOver();
  bool mousePressed();
  bool mouseDragged();
  bool mouseReleased();
  void debugger();

  void windowResized();
  void lockZoomScaleXY();

  void moveTo(int tx, int ty);
  bool move_;
};

}

#endif