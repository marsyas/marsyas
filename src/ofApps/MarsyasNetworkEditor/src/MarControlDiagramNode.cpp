//
//  MarControlDiagramNode.cpp
//  allAddonsExample
//
//  Created by Andre Perrotta on 5/17/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "MarControlDiagramNode.h"
#include "GraphicalEnvironment.h"
#include "DrawingUtils.h"
#include "GraphicalParameters.h"
#include "MiniViewWidget.h"
#include "MarControl.h"


using namespace Marsyas;

MarControlDiagramNode::MarControlDiagramNode(MarControlWidget* ctrlWidget, GraphicalEnvironment* env, MarControlDiagramNode* parent, bool highlighted) {
  setup(ctrlWidget, env, parent, highlighted);

}

MarControlDiagramNode::~MarControlDiagramNode() {

}
void MarControlDiagramNode::setup(MarControlWidget *ctrlWidget, GraphicalEnvironment *env, MarControlDiagramNode* parent, bool highlighted) {
  ctrlWidget_ = ctrlWidget;
  parent_ = parent;
  linkers_ = new std::vector<MarControlDiagramNode*>;
  linkers_->clear();
  highlighted_ = highlighted;
  Widget::setup(0, 0, 10, 10, env);
}


void MarControlDiagramNode::addLinker(MarControlDiagramNode* ctrlWidget) {
  linkers_->push_back(ctrlWidget);
}


std::vector<MarControlDiagramNode*>* MarControlDiagramNode::getLinkers() {
  return linkers_;
}

MarControlWidget* MarControlDiagramNode::getCtrlWidget() {
  return ctrlWidget_;
}

void MarControlDiagramNode::update() {

}

void MarControlDiagramNode::draw() {



  ofFill();
  if(!highlighted_) {
    ofSetColor(255, 0, 255);
  }
  else {
    ofSetColor(255, 255, 0);
  }
  ofCircle(x_, y_, width_);

  if(isMouseOver_) {
    ofNoFill();
    ofSetColor(0, 0, 255);
    ofCircle(x_, y_, width_);
  }

  if(parent_) {

    ofSetColor(255, 255, 255);
    ofSetLineWidth(1);
    ofArrow(x_, y_, parent_->getX() + 5, parent_->getY());
  }

  for(int i=0; i<linkers_->size(); i++) {
    (*linkers_)[i]->draw();
  }
}

void MarControlDiagramNode::debugger() {
  cout<<linkers_->size()<<" ";
  for(int i=0; i<linkers_->size(); i++) {
    (*linkers_)[i]->debugger();
  }
}



void MarControlDiagramNode::organizeLinkers(int x, int y) {

  float theta;
  for(int i=0; i<linkers_->size(); i++)
  {
    theta = 180.0*(i+1)/(linkers_->size() + 1) + ANGLE_OFFSET;
    (*linkers_)[i]->x_ = SEPARATION*cos(ofDegToRad(theta)) + x_;
    (*linkers_)[i]->y_ = SEPARATION*sin(ofDegToRad(theta)) + y_;
    (*linkers_)[i]->organizeLinkers(x_, y_);
  }



}


void MarControlDiagramNode::highlightSelectedControl() {
  std::map<MarControlPtr, MarControlWidget*>::iterator it;

  for(it = ctrlWidget_->getMarSystemWidget()->getTopMostParent()->ctrlMap_.begin(); it != ctrlWidget_->getMarSystemWidget()->getTopMostParent()->ctrlMap_.end(); it++) {
    if(it->second != ctrlWidget_) {
      it->second->isSelected_ = false;
    }
    else {
      it->second->isSelected_ = true;
    }
  }


}

bool MarControlDiagramNode::mouseOver() {

  for(int i=0; i<linkers_->size(); i++) {
    (*linkers_)[i]->mouseOver();
  }


  isMouseOver_ = false;
  if((x_ - width_*0.5) <= env_->mouse_->x && (x_ + width_*0.5) >= env_->mouse_->x)
  {
    if((y_ - height_*0.5) <= env_->mouse_->y && (y_ + height_*0.5) >= env_->mouse_->y)
    {
      isMouseOver_ = true;
      return true;
    }
  }

  return false;

}

bool MarControlDiagramNode::mousePressed() {
  for(int i=0; i<linkers_->size(); i++) {
    (*linkers_)[i]->mousePressed();
  }

  if(mouseOver())
  {
    if(env_->mouse_->click == 0)
    {
      ctrlWidget_->getMarSystemWidget()->showFamilyUp();
      highlightSelectedControl();
      env_->ctrlDisplay_->loadMarControl(ctrlWidget_->getMarControlPtr());
      env_->mVf_->moveTo(ctrlWidget_->getX(), ctrlWidget_->getY());
      toggleState();
      dragLock_ = true;
      return true;
    }
  }
  return false;


}

bool MarControlDiagramNode::mouseDragged() {

}

bool MarControlDiagramNode::mouseReleased() {

}