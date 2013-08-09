//
//  CascadeWidget.cpp
//  allAddonsExample
//
//  Created by André Perrotta on 7/6/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//
#include "ofMain.h"
#include "CascadeWidget.h"
#include "GraphicalEnvironment.h"
#include "MarSystemNodeSymbol.h"
#include "GraphicalParameters.h"


using namespace Marsyas;

CascadeWidget::CascadeWidget(MarSystem *msys, GraphicalEnvironment* env) : MarSystemWidget(msys, env)
{

  OutputNodeSymbol_ = new MarSystemNodeSymbol();
}

CascadeWidget::~CascadeWidget()
{

}


void CascadeWidget::updateSize()
{
  if(drawChildren_)
  {
    width_ = 0;
    height_ = 0;
    childrenTotalWidth_ = 0;
    childrenTotalHeight_ = 0;
    for(int i=0; i<children_.size(); i++)
    {
      childrenTotalWidth_ += children_[i]->getWidth() + X_SEPARATION;
      if(childrenTotalHeight_ < children_[i]->getY() - y_ + children_[i]->getHeight())
      {
        childrenTotalHeight_ = children_[i]->getY() - y_ + children_[i]->getHeight();
      }
    }
    if(childrenTotalWidth_ > 0)
    {
      childrenTotalWidth_ += X_SEPARATION + X_MARGIN;
    }
    if(childrenTotalHeight_ > 0)
    {
      childrenTotalHeight_ += 40;
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

void CascadeWidget::updateChildrenWidgets()
{
  int startX = x_;

  if(children_.size() > 0)
  {
    children_[0]->setX(startX + X_SEPARATION);
    startX += children_[0]->getWidth() + X_SEPARATION;
    children_[0]->setY(y_ + HEADER_SEPARATION);
  }

  for(int i=1; i<children_.size(); i++)
  {
    children_[i]->setX(startX + X_SEPARATION);
    startX += children_[i]->getWidth() + X_SEPARATION;
    children_[i]->setY(children_[i-1]->getY() + children_[i-1]->getHeight()*0.5 + CASCADE_AMMOUNT);
  }

}





void CascadeWidget::drawConnections()
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

  int x2 = x_ + width_ - SYMBOL_SEPARATION;
  int y2 = y_ + height_*0.5;
  for(int i=0; i<children_.size(); i++)
  {
    float x0 = children_[i]->getX() + children_[i]->getWidth();
    float y0 = children_[i]->getY() + children_[i]->getHeight()*0.5;
    ofLine(x0, y0, x2, y0);
    ofLine(x2, y0, x2, y2);
    ofCircle(x0, y0, 3);
  }

  if(children_.size() > 0)
  {
    float x0 = x_;
    float y0 = y_ + height_*0.5;
    float x1 = children_[0]->getX();
    float y1 = children_[0]->getY() + children_[0]->getHeight()*0.5;
    ofLine(x0, y0, x1, y1);
    ofCircle(x0, y0, 3);
    ofCircle(x1, y1, 3);
  }

  ofLine(x_ + width_ - SYMBOL_SEPARATION, y_ + height_*0.5, x_ + width_, y_ + height_*0.5);
  OutputNodeSymbol_->draw(x_ + width_ - SYMBOL_SEPARATION, y_ + height_*0.5);
}