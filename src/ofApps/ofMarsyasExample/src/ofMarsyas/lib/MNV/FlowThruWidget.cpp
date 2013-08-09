//
//  FlowThruWidget.cpp
//  allAddonsExample
//
//  Created by André Perrotta on 7/12/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//
#include "ofMain.h"
#include "FlowThruWidget.h"
using namespace Marsyas;

FlowThruWidget::FlowThruWidget(MarSystem *msys, GraphicalEnvironment* env) : MarSystemWidget(msys, env)
{

}

FlowThruWidget::~FlowThruWidget()
{

}




void FlowThruWidget::drawConnections()
{
  ofFill();
  ofSetLineWidth(2);
  ofSetColor(255, 255, 0);
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

  if(children_.size() > 0)
  {

    float x0 = x_;
    float y0 = y_ + height_*0.5;
    float x1 = x_ + X_SEPARATION*0.5;
    float y1 = y0;
    ofLine(x0, y0, x1, y1);

    x0 = x1;
    y0 = y1;
    x1 = children_[0]->getX();
    y1 = children_[0]->getY() + children_[0]->getHeight()*0.5;
    ofLine(x0, y0, x1, y1);
    ofCircle(x0, y0, 3);
    ofCircle(x1, y1, 3);


    x1 = x0;
    y1 = y_ + HEADER_SEPARATION - 14;
    ofLine(x0, y0, x1, y1);

    x1 = children_[children_.size() - 1]->getX() + children_[children_.size() - 1]->getWidth() + X_SEPARATION*0.5;
    ofLine(x0, y1, x1, y1);

    x0 = x1;
    y0 = y1;
    x1 = x_ + width_;
    y1 = y_ + height_*0.5;
    ofLine(x0, y0, x1, y1);
    ofCircle(x1, y1, 3);

    x0 = children_[children_.size() - 1]->getX() + children_[children_.size() - 1]->getWidth();
    y0 = children_[children_.size() - 1]->getY() + children_[children_.size() - 1]->getHeight()*0.5;
    x1 = x0 + X_SEPARATION*0.25;
    y1 = y0;
    ofLine(x0, y0, x1, y1);
    ofCircle(x0, y0, 3);

    x0 = x1;
    y1 = y_ + height_;
    ofLine(x0, y0, x1, y1);
    ofCircle(x1, y1, 3);

  }
}













