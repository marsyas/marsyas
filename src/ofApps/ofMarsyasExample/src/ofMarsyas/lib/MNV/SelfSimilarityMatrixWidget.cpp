//
//  SelfSimilarityMatrixWidget.cpp
//  allAddonsExample
//
//  Created by André Perrotta on 7/12/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "ofMain.h"
#include "SelfSimilarityMatrixWidget.h"
using namespace Marsyas;

SelfSimilarityMatrixWidget::SelfSimilarityMatrixWidget(MarSystem *msys, GraphicalEnvironment* env) : MarSystemWidget(msys, env)
{

}

SelfSimilarityMatrixWidget::~SelfSimilarityMatrixWidget()
{

}



void SelfSimilarityMatrixWidget::drawConnections()
{
  ofFill();
  ofSetLineWidth(2);
  ofSetColor(255, 255, 0);

  if(children_.size() > 0)
  {
    float x0 = x_;
    float y0 = y_ + height_*0.5;
    float x1 = children_[0]->getX();
    float y1 = children_[0]->getY() + children_[0]->getHeight()*0.5;
    ofLine(x0, y0, x1, y1);
    ofCircle(x0, y0, 3);
    ofCircle(x1, y1, 3);
    x0 = children_[0]->getX() + children_[0]->getWidth();
    y0 = children_[0]->getY() + children_[0]->getHeight()*0.5;
    x1 = x_ + width_;
    y1 = y_ + height_*0.5;
    ofLine(x0, y0, x1, y1);
    ofCircle(x0, y0, 3);
    ofCircle(x1, y1, 3);
  }
}