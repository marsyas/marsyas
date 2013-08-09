//
//  MarControlLinkWidget.h
//  allAddonsExample
//
//  Created by André Perrotta on 8/18/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef MARSYAS_CONTROL_LINK_WIDGET
#define MARSYAS_CONTROL_LINK_WIDGET


#include "Widget.h"


namespace Marsyas
{

class MarControlWidget;

class MarControlLinkWidget : public Widget
{
private:
  MarControlWidget* src_;
  MarControlWidget* dest_;

public:
  MarControlLinkWidget(MarControlWidget *src, MarControlWidget *dest);
  ~MarControlLinkWidget();

  void setup(MarControlWidget *src, MarControlWidget *dest);


  MarControlWidget *getSrc();
  MarControlWidget *getDst();

  void update();
  void draw();

  void printLinks();
};

}

#endif