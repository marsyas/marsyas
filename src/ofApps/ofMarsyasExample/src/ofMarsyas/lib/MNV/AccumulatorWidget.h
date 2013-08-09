//
//  AccumulatorWidget.h
//  allAddonsExample
//
//  Created by André Perrotta on 7/11/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef MARSYAS_ACCUMULATOR_WIDGET_H
#define MARSYAS_ACCUMULATOR_WIDGET_H


#include "MarSystemWidget.h"




namespace Marsyas
{

class AccumulatorWidget : public MarSystemWidget
{
private:
  void addControls();
  void updateControls();

public:
  AccumulatorWidget(MarSystem *msys, GraphicalEnvironment* env);
  ~AccumulatorWidget();

  //void update();

  void drawConnections();
  void updateSize();
  void updateChildrenPos();
  void updateChildrenWidgets();
};
}

#endif