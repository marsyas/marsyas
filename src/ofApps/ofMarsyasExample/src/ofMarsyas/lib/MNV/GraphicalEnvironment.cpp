//
//  Environment.cpp
//  allAddonsExample
//
//  Created by Andre Perrotta on 5/22/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "GraphicalEnvironment.h"
#include "GraphicalParameters.h"
#include "MiniViewWidget.h"
#include "MarSystemThread.h"


using namespace Marsyas;

GraphicalEnvironment::GraphicalEnvironment() {

  //*********************************************************
  //visualizer OPENGL
  scale_ = 0.8;
  xOffset_ = 0.0;
  yOffset_ = MINIVIEW_HEIGHT;
  visualizationMode_ = 1;

  move_ = false;

  //mouse globals initialization
  mouse_ = new globalMouse();

  //fonts
  fontSmall_.setGlobalDpi(200);
  fontSmall_.loadFont("font.ttf", 6, true, true, true);
  fontMedium_.setGlobalDpi(200);
  fontMedium_.loadFont("font.ttf", 10, true, true, true);
  fontBig_.setGlobalDpi(200);
  fontBig_.loadFont("font.ttf", 15, true, true, true);

  probe_ = new ProbingManager(this);

  showChildrenConnections_ = true;
  showLinkedControlsConnections_ = true;

}


GraphicalEnvironment::~GraphicalEnvironment() {

}


void GraphicalEnvironment::setupForMarSystemWidget(MarSystemWidget* msysw) {


  //MarSystemWidget network
  msysw_ = msysw;

  //**********************************************************
  //Miniview
  miniViewXscale_ = (ofGetWidth() - 20)/(double)msysw_->getWidth();
  miniViewYscale_ = (MINIVIEW_HEIGHT - 20)/(double)msysw_->getHeight();
  updateMiniView_ = 0;
  networkWidth_ = msysw_->getWidth();
  networkHeight_ = msysw_->getHeight();
  mVf_ = new MiniViewWidget(this);
  updateMiniView_ = 0;

  //**********************************************************
  //Control Diagram Display
  ctrlDisplay_ = new MarControlDiagramDisplay(msysw, this);

  calculateTransformations();
}


void GraphicalEnvironment::setupForMarSystemWidgetByName(MarSystemWidget* msysw, string name) {
  setupForMarSystemWidget(msysw->getMarSystemWidgetFromMapByName(name));
}

void GraphicalEnvironment::setup() {

}


void GraphicalEnvironment::update() {
  if(msysw_ != NULL) {
    msysw_->update();
    ctrlDisplay_->update();
    calculateTransformations();
    mVf_->update();
    probe_->update();
  }



}


void GraphicalEnvironment::calculateTransformations() {

  networkWidth_ = msysw_->getWidth();
  networkHeight_ = msysw_->getHeight();

  miniViewXscale_ = (ofGetWidth() - 20)/(double)msysw_->getWidth();
  miniViewYscale_ = (MINIVIEW_HEIGHT - 20)/(double)msysw_->getHeight();

  double view;

  view = (double)mVf_->getWidth()/(double)(msysw_->getWidth()*miniViewXscale_);
  zoomX_ = (double)ofGetWidth()/(double)(view * msysw_->getWidth());

  view = (double)mVf_->getHeight()/(double)(msysw_->getHeight()*miniViewYscale_);
  zoomY_ = (double)(ofGetHeight() - MINIVIEW_HEIGHT - 10.)/(double)(view * msysw_->getHeight());

  double dist;

  dist = mVf_->getX() - 10;
  dist = (double)dist/(double)(ofGetWidth() - 20);
  xOffset_ = dist*msysw_->getWidth();

  dist = mVf_->getY() - 10;
  dist = (double)dist/(double)(MINIVIEW_HEIGHT - 20.0);
  yOffset_ = dist*msysw_->getHeight();

}

void GraphicalEnvironment::draw() {

  if(visualizationMode_ == 1) {
    if(msysw_ != NULL) {
      glPushMatrix();
      glTranslated(0, MINIVIEW_HEIGHT + 10, 0.0);
      glScaled(zoomX_, zoomY_, 1);
      glTranslated(-xOffset_, -yOffset_, 0.);
      msysw_->draw();
      glPopMatrix();

      ofFill();
      ofSetColor(70, 70, 70);
      ofRect(0, 0, ofGetWidth(), MINIVIEW_HEIGHT);
      ofNoFill();
      ofSetColor(240, 240, 240);
      ofRect(1, 1, ofGetWidth() - 2, MINIVIEW_HEIGHT);


      glPushMatrix();
      glTranslated(10, 10, 0);
      glScaled(miniViewXscale_, miniViewYscale_, 1.);
      msysw_->draw();
      glPopMatrix();

      mVf_->draw();

      ctrlDisplay_->draw();
      probe_->draw();
    }
  }
}


MarSystemWidget* GraphicalEnvironment::getMarSystemWidget() {
  return msysw_;
}


void GraphicalEnvironment::setMarSystemThread(MarSystemThread* msyst) {
  msysThread_ = msyst;
}

MarSystemThread* GraphicalEnvironment::getMarSystemThread() {
  return msysThread_;
}


std::vector<std::vector<double> >* GraphicalEnvironment::getDataBuffer() {
  return probe_->getDataBuffer();
}

void GraphicalEnvironment::keyPressed  (int key)
{
  probe_->keyPressed(key);

  switch(key)
  {
  case '1':
    visualizationMode_ = 1;
    break;
  case '2':
    //visualizationMode_ = 2;
    break;
  case 'c':
    showChildrenConnections_ = !showChildrenConnections_;
    msysw_->switchChildrenConnections(showChildrenConnections_);
    break;
  case 'v':
    showLinkedControlsConnections_ = !showLinkedControlsConnections_;
    msysw_->switchLinkedControlConnections(showLinkedControlsConnections_);
    break;
  case ' ':
    //cout<<endl<<msysThread_->getTickStatus();
    if(msysThread_->getTickStatus() == 0) {
      msysThread_->setTickStatus(1);
    }
    else if(msysThread_->getTickStatus() == 2) {
      msysThread_->setTickStatus(3);
    }

    break;
  case 't':
    msysThread_->setTickStatus(4);
    break;

  }


}

//--------------------------------------------------------------
void GraphicalEnvironment::keyReleased(int key)
{

}

//--------------------------------------------------------------
void GraphicalEnvironment::mouseMoved(int x, int y )
{

  if(msysw_ != NULL) {
    mouse_->x = x;
    mouse_->y = y;

    if(y < MINIVIEW_HEIGHT) {
      mVf_->mouseOver();
    }
    else {
      if(!ctrlDisplay_->mouseOver()) {
        if(!probe_->mouseOver()) {
          msysw_->mouseOver();
        }
      }
    }


    update();
  }



}

//--------------------------------------------------------------
void GraphicalEnvironment::mouseDragged(int x, int y, int button)
{
  if(msysw_ != NULL) {
    mouse_->x = x;
    mouse_->y = y;
    mouse_->click = button;
    mouse_->drag = button;
    mVf_->mouseDragged();
    update();
  }



}

//--------------------------------------------------------------
void GraphicalEnvironment::mousePressed(int x, int y, int button)
{
  if(msysw_ != NULL) {
    mouse_->x = x;
    mouse_->y = y;
    mouse_->click = button;


    if(y < MINIVIEW_HEIGHT) {
      mVf_->mousePressed();

    }
    else {
      if(!ctrlDisplay_->mousePressed()) {
        if(!probe_->mousePressed()) {
          msysw_->mousePressed();
        }

      }
    }


    update();
  }

}

//--------------------------------------------------------------
void GraphicalEnvironment::mouseReleased(int x, int y, int button)
{
  if(msysw_ != NULL) {
    mouse_->x = x;
    mouse_->y = y;
    mouse_->click = -1;
    mouse_->drag = -1;
    mVf_->mouseReleased();
    msysw_->mouseReleased();
    ctrlDisplay_->mouseReleased();
    probe_->mouseReleased();
    update();
  }


}

void GraphicalEnvironment::windowResized(int w, int h)
{
  update();

}


void GraphicalEnvironment::setFocusFromMarControlDiagram(int x0, int y0) {



  move_ = true;

  targetX_ = ofGetWidth()*0.5 - x0;
  targetY_ = ofGetHeight()*0.5 - y0 + MINIVIEW_HEIGHT;

  /*
   xOffset_ = ofGetWidth()*0.5 - x0;
   yOffset_ = ofGetHeight()*0.5 - y0 + MINIVIEW_HEIGHT;
   */
}

bool GraphicalEnvironment::isLoaded() {
  if(msysw_ != NULL) {
    return true;
  }
  return false;
}

void GraphicalEnvironment::debugger() {
  //cout<<endl<<"x offset = "<<xOffset_;
}
