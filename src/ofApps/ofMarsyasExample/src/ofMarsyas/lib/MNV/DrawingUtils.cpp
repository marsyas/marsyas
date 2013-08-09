//
//  DrawingUtils.cpp
//  allAddonsExample
//
//  Created by André Perrotta on 8/30/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "DrawingUtils.h"
#include "ofMain.h"

void ofArrow(float x1, float y1, float x2, float y2)
{

  ofLine(x1, y1, x2, y2);
  float a = y1 - y2;
  float b = x1 - x2;
  float tg = a/b;
  float theta = atan(tg);
  if(a < 0 && b < 0)
  {
    theta += M_PI;
  }
  if(a > 0 && b < 0)
  {
    theta += M_PI;
  }

  float xUp = 30*cos(theta - M_PI/12.) + x2;
  float yUp = 30*sin(theta - M_PI/12.) + y2;

  float xDown = 30*cos(theta + M_PI/12.) + x2;
  float yDown = 30*sin(theta + M_PI/12.) + y2;


  ofFill();
  ofTriangle(x2, y2, xUp, yUp, xDown, yDown);

}