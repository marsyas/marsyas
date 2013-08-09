//
//  ShredderWidget.h
//  allAddonsExample
//
//  Created by André Perrotta on 7/11/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef MARSYAS_SHREDDER_WIDGET_H
#define MARSYAS_SHREDDER_WIDGET_H

#include "MarSystemWidget.h"

namespace Marsyas
{


class ShredderWidget : public MarSystemWidget
{
private:
  void addControls();
  void updateControls();


public:
  ShredderWidget(MarSystem *msys, GraphicalEnvironment* env);
  ~ShredderWidget();

  //void update();

  void drawConnections();
  void updateChildrenPos();
  void updateChildrenWidgets();
};
}

#endif