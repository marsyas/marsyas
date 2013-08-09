//
//  MarControlLinkWidget.cpp
//  allAddonsExample
//
//  Created by André Perrotta on 8/18/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "MarControlLinkWidget.h"

#include "ofMain.h"
#include "MarControlWidget.h"
#include "DrawingUtils.h" //for ofArrow used in ::draw()

using namespace Marsyas;

MarControlLinkWidget::MarControlLinkWidget(MarControlWidget *src, MarControlWidget *dest)
{
  setup(src, dest);
}


MarControlLinkWidget::~MarControlLinkWidget()
{

}

//FIXME: is this really necessary?
void MarControlLinkWidget::setup(MarControlWidget *src, MarControlWidget *dest)
{
  src_ = src;
  dest_ = dest;
}

void MarControlLinkWidget::update()
{

}

void MarControlLinkWidget::draw()
{
  glPushMatrix();
  glTranslated(0, 0, 1);
  //ofSetColor(255, 0, 0);
  ofSetLineWidth(1);
  ofArrow(src_->getX(), src_->getY(), dest_->getX(), dest_->getY());
  glPopMatrix();

}

MarControlWidget *MarControlLinkWidget::getSrc()
{
  return src_;
}

MarControlWidget *MarControlLinkWidget::getDst()
{
  return dest_;
}

void MarControlLinkWidget::printLinks()
{
  cout<<endl<<"src: "<<src_->getName()<<" "<<"dest: "<<dest_->getName();
}