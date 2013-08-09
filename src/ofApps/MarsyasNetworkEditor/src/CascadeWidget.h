//
//  CascadeWidget.h
//  allAddonsExample
//
//  Created by André Perrotta on 7/6/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//
#ifndef MARSYAS_CASCADE_WIDGET_H
#define MARSYAS_CASCADE_WIDGET_H


#include "MarSystemWidget.h"


namespace Marsyas
{

class CascadeWidget : public MarSystemWidget
{
private:
  void addControls();
  void updateControls();


public:
  CascadeWidget(MarSystem *msys, GraphicalEnvironment* env);
  ~CascadeWidget();

  //void update();

  void drawConnections();
  void updateSize();
  void updateChildrenPos();
  void updateChildrenWidgets();
};
}

#endif