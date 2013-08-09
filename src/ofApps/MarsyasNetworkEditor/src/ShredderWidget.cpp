//
//  ShredderWidget.cpp
//  allAddonsExample
//
//  Created by André Perrotta on 7/11/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//


#include "ofMain.h"
#include "ShredderWidget.h"
#include "MarSystemNodeSymbol.h"

using namespace Marsyas;

ShredderWidget::ShredderWidget(MarSystem *msys, GraphicalEnvironment* env) : MarSystemWidget(msys, env)
{
  InputNodeSymbol_ = new MarSystemNodeSymbol();
}

ShredderWidget::~ShredderWidget()
{
  delete InputNodeSymbol_;
}




void ShredderWidget::updateChildrenWidgets()
{
  int startX = x_;
  if(children_.size() > 0)
  {
    children_[0]->setX(startX + X_SEPARATION);
    children_[0]->setY(y_ + height_*0.5 - children_[0]->height_*0.5);
  }

}

void ShredderWidget::drawConnections()
{
  ofFill();
  ofSetLineWidth(2);
  ofSetColor(255, 255, 0);



  if(children_.size() > 0)
  {
    int x1 = x_ + SYMBOL_SEPARATION;
    int y1 = children_[0]->getY() + children_[0]->getHeight()*0.5;
    ofLine(x_, y_ + height_*0.5, x1, y1);
    float x2 = children_[0]->getX();
    ofLine(x1, y1, x2, y1);
    ofCircle(x2, y1, 3);
    InputNodeSymbol_->draw(x_ + SYMBOL_SEPARATION, y1);
  }

  if(children_.size() > 0)
  {
    float x0 = children_[0]->getX() + children_[0]->getWidth();
    float y0 = children_[0]->getY() + children_[0]->getHeight()*0.5;
    float x1 = x_ + width_;
    float y1 = y_ + height_*0.5;
    ofLine(x0, y0, x1, y1);
    ofCircle(x0, y0, 3);
    ofCircle(x1, y1, 3);
  }

}


