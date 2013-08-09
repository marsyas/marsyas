//
//  MarSystemWidgetManager.h
//  allAddonsExample
//
//  Created by André Perrotta on 7/1/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef MARSYAS_MARSYSTEM_WIDGET_MANAGER_H
#define MARSYAS_MARSYSTEM_WIDGET_MANAGER_H

#include "ofMain.h"
#include "GraphicalEnvironment.h"

#undef PI //FIXME

namespace Marsyas
{

class MarSystem;
class MarSystemWidget;

class MarSystemWidgetManager
{
private:
  GraphicalEnvironment *env_;
  MarSystemWidget* createWidget(MarSystem* msys);
  MarSystemWidget* getWidget(MarSystem* msys);
  void createControlWidgets(MarSystemWidget* msysw);
  void createControlLinkWidgets(MarSystemWidget* msysw);
  void createWidgetsMaps(MarSystemWidget* msysw);

public:
  MarSystemWidgetManager(GraphicalEnvironment* env);
  ~MarSystemWidgetManager();

  MarSystemWidget *setupWidgets(MarSystem* msys);

};
}

#endif