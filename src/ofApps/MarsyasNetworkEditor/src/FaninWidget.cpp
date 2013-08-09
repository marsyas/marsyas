//
//  FaninWidget.cpp
//  allAddonsExample
//
//  Created by André Perrotta on 7/6/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//
#include "ofMain.h"
#include "FaninWidget.h"
#include "GraphicalEnvironment.h"
#include "MarSystemNodeSymbol.h"
using namespace Marsyas;


FaninWidget::FaninWidget(MarSystem *msys, GraphicalEnvironment* env) : MarSystemWidget(msys, env)
{

  InputNodeSymbol_ = new MarSystemNodeSymbol();
  OutputNodeSymbol_ = new MarSystemNodeSymbol();
}

FaninWidget::~FaninWidget()
{

}




void FaninWidget::drawConnections()
{

  ofFill();
  ofSetLineWidth(2);
  ofSetColor(255, 255, 0);

  float x0 = x_ + SYMBOL_SEPARATION;
  float y0 = y_ + height_*0.5;
  float x3 = x_ + width_ - SYMBOL_SEPARATION;
  float y3 = y_ + height_*0.5;
  for(int i=0; i<children_.size(); i++)
  {
    float x1 = children_[i]->getX();
    float y1 = children_[i]->getY() + children_[i]->getHeight()*0.5;
    ofLine(x0, y0, x0, y1);
    ofLine(x0, y1, x1, y1);
    ofCircle(x1, y1, 3);
    float x2 = children_[i]->getX() + children_[i]->getWidth();
    float y2 = children_[i]->getY() + children_[i]->getHeight()*0.5;
    ofLine(x2, y2, x3, y3);
    //ofLine(x2, y2, x3, y2);
    //ofLine(x3, y2, x3, y3);
    ofCircle(x2, y2, 3);
  }
  ofCircle(x0, y0, 3);
  ofCircle(x3, y3, 3);

  ofLine(x_ + SYMBOL_SEPARATION, y_ + height_*0.5, x_, y_ + height_*0.5);
  ofLine(x_ + width_ - SYMBOL_SEPARATION, y_ + height_*0.5, x_ + width_, y_ + height_*0.5);
  InputNodeSymbol_->draw(x_ + SYMBOL_SEPARATION, y_ + height_*0.5);
  OutputNodeSymbol_->draw(x_ + width_ - SYMBOL_SEPARATION, y_ + height_*0.5);
}

void FaninWidget::updateSize()
{
  if(drawChildren_)
  {
    width_ = 0;
    height_ = 0;
    childrenTotalWidth_ = 0;
    childrenTotalHeight_ = 0;
    for(int i=0; i<children_.size(); i++)
    {
      childrenTotalHeight_ += children_[i]->getHeight() + Y_SEPARATION;
      if(children_[i]->getWidth() > childrenTotalWidth_)
      {
        childrenTotalWidth_ = children_[i]->getWidth();
      }
    }

    for(int i=0; i<children_.size(); i++)
    {
      children_[i]->setWidth(childrenTotalWidth_);
      children_[i]->maxChildrenWidth_ = childrenTotalWidth_;
    }

    if(childrenTotalWidth_ > 0)
    {
      childrenTotalWidth_ += 2*X_SEPARATION;
    }
    if(childrenTotalHeight_ > 0)
    {
      childrenTotalHeight_ += 80;
    }
    /////////////////////
    //Size for ShowWindow
    ////////////////////
    nameWidth_ = env_->fontMedium_.stringWidth(type_ + "/" + name_);
    nameHeight_ = env_->fontMedium_.stringHeight(type_ + "/" + name_);

    if(nameWidth_ > childrenTotalWidth_ && nameWidth_ > width_)
    {
      width_ = nameWidth_ + 30;
    }
    else if(width_ < childrenTotalWidth_)
    {
      width_ = childrenTotalWidth_;
    }

    if(width_ < maxChildrenWidth_)
    {
      width_ = maxChildrenWidth_;
    }

    if(nameHeight_ > childrenTotalHeight_)
    {
      height_ = nameHeight_ + 20;
    }
    else
    {
      height_ = childrenTotalHeight_;
    }

    //because of control panel
    height_ += CTRL_PANNEL_HEIGHT;
    //////////////////////////////////

  }
  else
  {
    nameWidth_ = env_->fontMedium_.stringWidth(type_ + "/" + name_);
    width_ = nameWidth_ + 50;

    if(width_ < maxChildrenWidth_)
    {
      width_ = maxChildrenWidth_;
    }

    nameHeight_ = env_->fontMedium_.stringHeight(type_ + "/" + name_);
    height_ = nameHeight_ + 20;

    //because of control panel
    height_ += CTRL_PANNEL_HEIGHT;
  }
}

void FaninWidget::updateChildrenWidgets()
{
  int startX = x_ + X_SEPARATION;
  int startY = y_ + HEADER_SEPARATION;
  for(int i=0; i<children_.size(); i++)
  {
    children_[i]->setX(startX);
    children_[i]->setY(startY);
    startY += children_[i]->getHeight() + Y_SEPARATION;
  }
}
