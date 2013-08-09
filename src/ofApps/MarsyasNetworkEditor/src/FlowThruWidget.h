//
//  FlowThruWidget.h
//  allAddonsExample
//
//  Created by André Perrotta on 7/12/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef MARSYAS_FLOWTHRU_WIDGET_H
#define MARSYAS_FLOWTHRU_WIDGET_H


#include "MarSystemWidget.h"


namespace Marsyas
{

class FlowThruWidget : public MarSystemWidget
{
private:
  void addControls();
  void updateControls();


public:
  FlowThruWidget(MarSystem *msys, GraphicalEnvironment* env);
  ~FlowThruWidget();


  void drawConnections();
};
}

#endif