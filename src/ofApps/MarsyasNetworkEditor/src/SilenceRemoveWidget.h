//
//  SilenceRemoveWidget.h
//  allAddonsExample
//
//  Created by André Perrotta on 7/12/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//
#ifndef MARSYAS_SILENCE_REMOVE_WIDGET_H
#define MARSYAS_SILENCE_REMOVE_WIDGET_H


#include "MarSystemWidget.h"



namespace Marsyas
{

class SilenceRemoveWidget : public MarSystemWidget
{
private:
  void addControls();
  void updateControls();


public:
  SilenceRemoveWidget(MarSystem *msys, GraphicalEnvironment* env);
  ~SilenceRemoveWidget();


  void drawConnections();
};
}

#endif