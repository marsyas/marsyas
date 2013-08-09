//
//  SeriesWidget.cpp
//  allAddonsExample
//
//  Created by André Perrotta on 6/22/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "SeriesWidget.h"
#include "ofMain.h"

using namespace Marsyas;


SeriesWidget::SeriesWidget(MarSystem* msys, GraphicalEnvironment* env) : MarSystemWidget(msys, env)
{

}

SeriesWidget::~SeriesWidget()
{

}




void SeriesWidget::drawConnections()
{
  ofFill();
  ofSetLineWidth(2);
  ofSetColor(255, 255, 0);

  if(children_.size() > 0)
  {

    for(int i=0; i<(children_.size() - 1); i++)
    {
      float x0 = children_[i]->getX() + children_[i]->getWidth();
      float y0 = children_[i]->getY() + children_[i]->getHeight()*0.5;
      float x1 = children_[i + 1]->getX();
      float y1 = children_[i + 1]->getY() + children_[i + 1]->getHeight()*0.5;
      ofLine(x0, y0, x1, y1);
      ofCircle(x0, y0, 3);
      ofCircle(x1, y1, 3);
    }

    float x0 = x_;
    float y0 = y_ + height_*0.5;
    float x1 = children_[0]->getX();
    float y1 = children_[0]->getY() + children_[0]->getHeight()*0.5;
    ofLine(x0, y0, x1, y1);
    ofCircle(x0, y0, 3);
    ofCircle(x1, y1, 3);
    x0 = children_[children_.size() - 1]->getX() + children_[children_.size() - 1]->getWidth();
    y0 = children_[children_.size() - 1]->getY() + children_[children_.size() - 1]->getHeight()*0.5;
    x1 = x_ + width_;
    y1 = y_ + height_*0.5;
    ofLine(x0, y0, x1, y1);
    ofCircle(x0, y0, 3);
    ofCircle(x1, y1, 3);
  }
}


