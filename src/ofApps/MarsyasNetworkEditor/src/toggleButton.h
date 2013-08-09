//
//  toggleButton.h
//  allAddonsExample
//
//  Created by André Perrotta on 6/15/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//


#ifndef MARSYAS_WIDGET_TOGGLE_BUTTON_H
#define MARSYAS_WIDGET_TOGGLE_BUTTON_H
#include "ofMain.h"
#include "Widget.h"

namespace Marsyas
{


class ToggleButton : public Widget
{
public:

  ToggleButton(int x0, int y0, int w, int h, GraphicalEnvironment* env);

  void update();
  void draw();




};

}

#endif
