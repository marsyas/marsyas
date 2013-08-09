//
//  ProcessedDataWidget.cpp
//  MNE
//
//  Created by Andre Perrotta on 7/2/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "ProcessedDataWidget.h"
#include "ofMain.h"
#include "MarSystemWidget.h"
#include "GraphicalEnvironment.h"


using namespace Marsyas;

ProcessedDataWidget::ProcessedDataWidget() {

}

ProcessedDataWidget::~ProcessedDataWidget() {

}


ProcessedDataWidget::ProcessedDataWidget(MarSystemWidget* msysw, GraphicalEnvironment* env) {

  msysw_ = msysw;
  env_ = env;
  pData_ = NULL;

  showData_ = false;

  width_ = 10;
  height_ = 10;

  x_ = msysw_->getX() + msysw_->getWidth() - width_;
  y_ = msysw_->getY() + msysw_->getHeight()*0.5 - height_*0.5;


}

void ProcessedDataWidget::loadProcessedDataPointer(MarControlPtr pData) {
  pData_ = pData;
}

void ProcessedDataWidget::update() {
  x_ = msysw_->getX() + msysw_->getWidth() - width_;
  y_ = msysw_->getY() + msysw_->getHeight()*0.5 - height_*0.5;
}

void ProcessedDataWidget::draw() {
  ofFill();
  ofSetColor(255, 141, 0);
  ofRect(x_, y_, width_, height_);
}

bool ProcessedDataWidget::mousePressed() {
  if(mouseOver())
  {
    if(env_->mouse_->click == 0)
    {
      toggleState();
      env_->probe_->loadProcessedDataPointer(pData_);
      dragLock_ = true;
      return true;
    }
  }
  return false;
}