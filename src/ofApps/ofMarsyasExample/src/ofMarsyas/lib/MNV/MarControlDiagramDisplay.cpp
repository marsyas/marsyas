//
//  MarControlDiagramDisplay.cpp
//  allAddonsExample
//
//  Created by Andre Perrotta on 5/17/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "MarControlDiagramDisplay.h"
#include "MarControlDiagramNode.h"
#include "GraphicalEnvironment.h"

using namespace Marsyas;

MarControlDiagramDisplay::MarControlDiagramDisplay(MarSystemWidget* msysw, GraphicalEnvironment* env) {
  setup(msysw, env);
}


MarControlDiagramDisplay::~MarControlDiagramDisplay() {

}


void MarControlDiagramDisplay::setup(MarSystemWidget* msysw, GraphicalEnvironment* env) {
  msysw_ = msysw;

  isVisible_ = true;
  isLoaded_ = false;
  Widget::setup(0, ofGetHeight() - 300, 300, 300, env);
  mBtn_ = new MaximizeButton(x_ + width_ - 18, y_ + 3, 14, 14, env);
  mBtn_->updatePosition(x_ + width_ - 18, y_ + 3, 14, 14);
}


void MarControlDiagramDisplay::update() {
  if(isVisible_) {
    updatePosition(0, ofGetHeight() - 300, 300, 300);
  }
  else {
    updatePosition(0, ofGetHeight() - 20, 300, 300);
  }

  organizeDiagram();
  mBtn_->updatePosition(x_ + width_ - 18, y_ + 3, 14, 14);

}

void MarControlDiagramDisplay::loadMarControl(MarControlPtr ctrl) {
  isLoaded_ = true;
  nodes.clear();
  linkedControls_.clear();
  linkedControls_ = ctrl->getLinks();



  cout<<endl;
  string auxName1, auxName2;
  cout<<endl<<"--------------------------";
  cout<<endl<<"linked controls list:";
  for(int i=0; i<linkedControls_.size(); i++) {
    auxName1 = linkedControls_[i].first->getMarSystem()->getName() + "/" + linkedControls_[i].first->getName();
    auxName2 = linkedControls_[i].second->getMarSystem()->getName() + "/" + linkedControls_[i].second->getName();
    cout<<endl<<auxName1<<" -> "<<auxName2;
  }


  MarControlWidget* aux1;
  MarControlWidget* aux2;
  MarControlDiagramNode* aux3;
  MarControlDiagramNode* aux4;

  aux3 = NULL;

  bool highlight1;
  bool highlight2;

  for(int i=0; i<linkedControls_.size(); i++) {

    highlight1 = false;
    highlight2 = false;

    aux1 = msysw_->getTopMostParent()->ctrlMap_[linkedControls_[i].first];
    aux2 = msysw_->getTopMostParent()->ctrlMap_[linkedControls_[i].second];


    if(ctrl.isEqual(linkedControls_[i].first)) {
      highlight1 = true;
    }

    if(ctrl.isEqual(linkedControls_[i].second)) {
      highlight2 = true;
    }

    for(int j=0; j<nodes.size(); j++) {

      aux3 = nodeExists(nodes[j], aux2);

    }
    if(aux3 == NULL) {

      nodes.push_back(new MarControlDiagramNode(aux2, env_, NULL, highlight2));
      if(aux1 != aux2) {

        nodes[nodes.size() - 1]->addLinker(new MarControlDiagramNode(aux1, env_, nodes[nodes.size() - 1], highlight1));
      }
    }
    else {
      aux3->addLinker(new MarControlDiagramNode(aux1, env_, aux3, highlight1));
    }
  }
  organizeDiagram();
  debugger();
}

MarControlDiagramNode* MarControlDiagramDisplay::nodeExists(MarControlDiagramNode* node, MarControlWidget* cw) {


  if(node->getCtrlWidget() == cw)
  {
    return node;
  }


  std::vector<MarControlDiagramNode*>* aux;
  aux = node->getLinkers();

  for(int i=0; i<aux->size(); i++)
  {
    return nodeExists((*aux)[i], cw);
  }
  return NULL;
}

void MarControlDiagramDisplay::organizeDiagram() {
  for(int i=0; i<nodes.size(); i++) {
    nodes[i]->setX(x_ + 40*i + 20);
    nodes[i]->setY(y_ + height_*0.5);
    nodes[i]->organizeLinkers(100, 100);
  }
}

void MarControlDiagramDisplay::draw() {
  if(isVisible_) {
    ofFill();
    ofSetColor(75, 75, 75, 253);
    ofRect(x_, y_, width_, height_);

    ofSetColor(190, 190, 190, 255);
    ofRect(x_, y_, width_, 20);

    ofNoFill();
    ofSetColor(240, 240, 240, 255);
    ofRect(x_, y_, width_, height_);

    ofSetColor(46, 68, 255, 255);
    env_->fontSmall_.drawString("MarControl Links Diagram", x_ + 2, y_ + 15);

    mBtn_->draw();

    if(isLoaded_) {
      for(int i=0; i<nodes.size(); i++) {
        nodes[i]->draw();
      }
    }

  }
  else {
    ofFill();
    ofSetColor(75, 75, 75, 253);
    ofRect(x_, y_, width_, 20);

    ofSetColor(190, 190, 190, 255);
    ofRect(x_, y_, width_, 20);

    ofNoFill();
    ofSetColor(240, 240, 240, 255);
    ofRect(x_, y_, width_, 20);

    ofSetColor(46, 68, 255, 255);
    env_->fontSmall_.drawString("MarControl Links Diagram", x_ + 2, y_ + 15);

    mBtn_->draw();
  }

}

bool MarControlDiagramDisplay::mouseOver() {
  mBtn_->mouseOver();



  for(int i=0; i<nodes.size(); i++) {
    nodes[i]->mouseOver();
  }

  isMouseOver_ = false;
  if((x_) <= env_->mouse_->x && (x_ + width_) >= env_->mouse_->x)
  {
    if((y_) <= env_->mouse_->y && (y_ + height_) >= env_->mouse_->y)
    {
      isMouseOver_ = true;
      return true;
    }
  }
  return false;

}

bool MarControlDiagramDisplay::mousePressed() {


  if(mouseOver())
  {

    bool aux = mBtn_->mousePressed();
    if(!mBtn_->getState()) {
      isVisible_ = false;
      y_ = ofGetHeight() - 20;
      mBtn_->updatePosition(x_ + width_ - 18, y_ + 3, 14, 14);
    }
    else {
      isVisible_ = true;
      y_ = ofGetHeight() - 300;
      mBtn_->updatePosition(x_ + width_ - 18, y_ + 3, 14, 14);
    }

    for(int i=0; i<nodes.size(); i++) {
      nodes[i]->mousePressed();
    }



    if(env_->mouse_->click == 0)
    {
      toggleState();
      dragLock_ = true;
      return true;
    }
  }
  return false;
}

bool MarControlDiagramDisplay::mouseDragged() {

}

bool MarControlDiagramDisplay::mouseReleased() {

}

void MarControlDiagramDisplay::debugger() {
  cout<<nodes.size()<<" ";
  for(int i=0; i<nodes.size(); i++) {
    nodes[i]->debugger();
  }
}

