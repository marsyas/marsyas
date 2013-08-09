//
//  FaninWidget.h
//  allAddonsExample
//
//  Created by André Perrotta on 7/6/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef MARSYAS_FANIN_WIDGET_H
#define MARSYAS_FANIN_WIDGET_H


#include "MarSystemWidget.h"


namespace Marsyas
{

class FaninWidget : public MarSystemWidget
{
private:

  void addControls();
  void updateControls();
  void updateSize();
  void updateChildrenWidgets();


public:
  FaninWidget(MarSystem *msys, GraphicalEnvironment* env);
  ~FaninWidget();


  void drawConnections();
};
}

#endif