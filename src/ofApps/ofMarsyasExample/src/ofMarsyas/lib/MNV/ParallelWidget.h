//
//  ParallelWidget.h
//  allAddonsExample
//
//  Created by André Perrotta on 7/6/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef MARSYAS_PARALLEL_WIDGET_H
#define MARSYAS_PARALLEL_WIDGET_H


#include "MarSystemWidget.h"


namespace Marsyas
{

class ParallelWidget : public MarSystemWidget
{
private:

  void addControls();
  void updateControls();
  void updateSize();
  void updateChildrenWidgets();


public:
  ParallelWidget(MarSystem *msys, GraphicalEnvironment* env);
  ~ParallelWidget();


  void drawConnections();
};
}

#endif