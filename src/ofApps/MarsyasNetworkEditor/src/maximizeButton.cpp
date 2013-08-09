//
//  maximizeButton.cpp
//  allAddonsExample
//
//  Created by André Perrotta on 7/14/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "maximizeButton.h"

#include "toggleButton.h"
#include "globalVars.h"

using namespace Marsyas;

MaximizeButton::MaximizeButton(int x0, int y0, int w, int h, GraphicalEnvironment* env) : Widget(x0, y0, w, h, env)
{
  state_ = true;
}

void MaximizeButton::update()
{

}

void MaximizeButton::draw()
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

  }
  else
  {
    ofFill();
    ofSetColor(130, 130, 130);
    ofRect(x_, y_, width_, height_);
    ofNoFill();
    ofSetColor(50, 50, 50);
    ofRect(x_, y_, width_, height_);

  }
  glPushMatrix();
  glTranslated(0, 0, 0.1);
  if(state_)
  {
    ofSetColor(0, 0, 0);
    ofLine(x_ + 3, y_ + height_*0.5, x_ + width_ - 3, y_ + height_*0.5);
  }
  else
  {
    ofSetColor(0, 0, 0);
    ofLine(x_ + 3, y_ + height_*0.5, x_ + width_ - 3, y_ + height_*0.5);
    ofLine(x_ + width_*0.5, y_ + 3, x_ + width_*0.5, y_ + height_ - 3);
  }
  glPopMatrix();
}