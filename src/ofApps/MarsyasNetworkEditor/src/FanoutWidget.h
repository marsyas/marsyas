//
//  FanoutWidget.h
//  allAddonsExample
//
//  Created by André Perrotta on 6/22/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef MARSYAS_FANOUT_WIDGET_H
#define MARSYAS_FANOUT_WIDGET_H


#include "MarSystemWidget.h"

namespace Marsyas
{

class FanoutWidget : public MarSystemWidget
{
private:

  void addControls();
  void updateControls();
  void updateSize();
  void updateChildrenWidgets();


public:
  FanoutWidget(MarSystem *msys, GraphicalEnvironment* env);
  ~FanoutWidget();


  void drawConnections();
};
}

#endif