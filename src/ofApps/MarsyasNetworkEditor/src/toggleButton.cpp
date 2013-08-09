//
//  toggleButton.cpp
//  allAddonsExample
//
//  Created by André Perrotta on 6/15/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "toggleButton.h"
#include "globalVars.h"

using namespace Marsyas;

ToggleButton::ToggleButton(int x0, int y0, int w, int h, GraphicalEnvironment* env) : Widget(x0, y0, w, h, env)
{
  state_ = false;
}

void ToggleButton::update()
{

}

void ToggleButton::draw()
{
  ofSetLineWidth(2);
  ofSetRectMode(OF_RECTMODE_CORNER);
  if(isMouseOver_)
  {
    ofFill();
    ofSetColor(220, 220, 220);
    ofRect(x_, y_, width_, height_);
    ofNoFill();
    ofSetColor(50, 50, 50);
    ofRect(x_, y_, width_, height_);
    if(state_)
    {
      ofSetColor(0, 0, 0);
      ofLine(x_, y_, x_ + width_, y_ + height_);
      ofLine(x_, y_ + height_, x_ + width_, y_);
    }
  }
  else
  {
    ofFill();
    ofSetColor(130, 130, 130);
    ofRect(x_, y_, width_, height_);
    ofNoFill();
    ofSetColor(50, 50, 50);
    ofRect(x_, y_, width_, height_);
    if(state_)
    {
      ofSetColor(0, 0, 0);
      ofLine(x_, y_, x_ + width_, y_ + height_);
      ofLine(x_, y_ + height_, x_ + width_, y_);
    }
  }
}
