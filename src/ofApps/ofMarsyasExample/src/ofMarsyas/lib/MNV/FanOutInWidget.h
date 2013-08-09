//
//  FanOutInWidget.h
//  allAddonsExample
//
//  Created by André Perrotta on 7/8/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef MARSYAS_FANOUTIN_WIDGET_H
#define MARSYAS_FANOUTIN_WIDGET_H


#include "MarSystemWidget.h"


namespace Marsyas
{

class FanOutInWidget : public MarSystemWidget
{
private:

  void addControls();
  void updateControls();
  void updateSize();
  void updateChildrenWidgets();


public:
  FanOutInWidget(MarSystem *msys, GraphicalEnvironment* env);
  ~FanOutInWidget();


  void drawConnections();
};
}

#endif