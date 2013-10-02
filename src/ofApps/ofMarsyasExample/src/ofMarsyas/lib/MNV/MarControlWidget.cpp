//
//  MarControlWidget.cpp
//  allAddonsExample
//
//  Created by André Perrotta on 8/17/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "MarControlWidget.h"

#include "ofMain.h"
#include <marsyas/system/MarSystemManager.h>
#include "MarSystemWidget.h"
#include "MarControlLinkWidget.h"
#include "GraphicalEnvironment.h"
#include "globalVars.h"


using namespace Marsyas;
using namespace std;


MarControlWidget::MarControlWidget(MarControlPtr ctrl, MarSystemWidget *msysw)
{
  //cache...
  msys_ = msysw->getMarSystem();
  ctrl_ = ctrl;
  msysw_ = msysw;
  Widget::setup(0, 0, 10, 10, msysw_->getEnvironment());

  name_ = ctrl_->getName();
  absName_ = msysw_->getName() + "/" + name_;

  setEnv(msysw_->getEnvironment());

  width_ = 11;

  hasConnections = false;

  //visualization
  isSelected_ = false;

  //FIXME: create link widgets?
  //setupLinks();
  //debugger();
  cout<<endl<<msys_->getName()<<"/"<<name_;
}

MarControlWidget::~MarControlWidget()
{
  //FIXME: dealloc link widgets??
  delete links_;

}

void MarControlWidget::update()
{

}

void MarControlWidget::draw()
{
  glPushMatrix();
  glTranslated(x_, y_, 0);
  ofSetRectMode(OF_RECTMODE_CENTER);
  if(isSelected_) {
    ofFill();
    ofSetColor(255, 255, 0);
    ofRect(0, 0, width_, width_);
    glTranslated(0, 0, 0.1);
    ofNoFill();
    ofCircle(0, 0, width_);

  } else {
    ofFill();
    ofSetHexColor(0x18F07B);
    ofRect(0, 0, width_, width_);
  }





  if(isMouseOver_)
  {
    /*
    ofSetRectMode(OF_RECTMODE_CORNER);
    ofSetColor(0, 0, 0, 200);
    ofRectangle rect = env_->fontMedium.getStringBoundingBox(absName_, 20, 20);
    ofRect(rect.x - 2, rect.y - 2, rect.width + 2, rect.height + 2);
    */
    ofSetColor(255, 125, 255);
    glTranslated(0, 0, 2);
    env_->fontMedium_.drawString(absName_, 20, 20);

  }

  glPopMatrix();

  ofSetRectMode(OF_RECTMODE_CORNER);
  //FIXME:
  if(showLinks_) {
    drawLinks();
  }

}



//getters //////////////////////////////////////////////

MarSystemWidget *MarControlWidget::getMarSystemWidget()
{
  return msysw_;
}





// LINKS //////////////////////////////////////////////////////

void MarControlWidget::setupLinks()
{
  MarControlWidget *aux1;

  string auxName1, auxName2;

  linkedControls_ = ctrl_->getLinks();

  links_ = new vector<MarControlLinkWidget*>;
  links_->clear();

  if(linkedControls_.size() > 0)
  {
    //cout<<endl<<"======================================================";
    //cout<<endl<<"linked controls for ControlWidget: "<<absName_;

    for(int i=0; i<linkedControls_.size(); i++)
    {
      /*
      auxName1 = linkedControls_[i].first->getMarSystem()->getName() + "/" + linkedControls_[i].first->getName();
      auxName2 = linkedControls_[i].second->getMarSystem()->getName() + "/" + linkedControls_[i].second->getName();
      cout<<endl<<auxName1<<" ==> "<<auxName2;
       */

      if(ctrl_.isEqual(linkedControls_[i].first) && (!ctrl_.isEqual(linkedControls_[i].second)))
      {
        aux1 = msysw_->getTopMostParent()->ctrlMap_[linkedControls_[i].second];
        if(aux1 != NULL)
        {
          aux1->hasConnections = true;
          links_->push_back(new MarControlLinkWidget(this, aux1));
        }
      }
    }
    //cout<<endl<<"======================================================";
  }
}


std::string MarControlWidget::getName() {
  return absName_;
}

bool MarControlWidget::hasLinks()
{
  if(links_->size() > 0 || hasConnections)
  {
    return true;
  }
  return false;
}

std::vector<MarControlLinkWidget*> *MarControlWidget::getLinks()
{

  if(links_->size() > 0)
  {
    return links_;
  }
  return NULL;
}


void MarControlWidget::drawLinks()
{
  if(isSelected_) {
    ofSetColor(0, 255, 0);
  }
  else {
    ofSetColor(255, 0, 0);
  }
  for(int i=0; i<links_->size(); i++)
  {
    (*links_)[i]->draw();
  }
}

void MarControlWidget::highlightControlWidget() {
  std::map<MarControlPtr, MarControlWidget*>::iterator it;

  for(it = msysw_->getTopMostParent()->ctrlMap_.begin(); it != msysw_->getTopMostParent()->ctrlMap_.end(); it++) {
    if(it->second != this) {
      it->second->isSelected_ = false;
    }
    else {
      isSelected_ = true;
    }
  }
}


bool MarControlWidget::mouseOver()
{

  isMouseOver_ = false;
  if(xForMouse_ - widthForMouse_*0.5 <= env_->mouse_->x && (xForMouse_ + widthForMouse_*0.5) >= env_->mouse_->x)
  {
    if(yForMouse_ - heightForMouse_*0.5 <= env_->mouse_->y && (yForMouse_ + heightForMouse_*0.5) >= env_->mouse_->y)
    {
      //cout<<endl<<xForMouse_<<" "<<yForMouse_<<" "<<widthForMouse_<<" "<<heightForMouse_<<" "<<env_->mouse_->x<<" "<<env_->mouse_->y;
      isMouseOver_ = true;

      return true;
    }
  }

  return false;

}

bool MarControlWidget::mousePressed() {
  if(mouseOver())
  {
    if(env_->mouse_->click == 0)
    {
      toggleState();
      env_->ctrlDisplay_->loadMarControl(ctrl_);
      highlightControlWidget();
      dragLock_ = true;
      return true;
    }
  }
  return false;
}




// MISC /////////////////////////////////////////////////////

void MarControlWidget::debugger()
{


  string auxName1, auxName2;
  cout<<endl<<"--------------------------";
  cout<<endl<<"control: "<<name_;
  cout<<endl<<"linked controls list:";
  for(int i=0; i<linkedControls_.size(); i++) {
    auxName1 = linkedControls_[i].first->getMarSystem()->getName() + "/" + linkedControls_[i].first->getName();
    auxName2 = linkedControls_[i].second->getMarSystem()->getName() + "/" + linkedControls_[i].second->getName();
    cout<<endl<<auxName1<<" -> "<<auxName2;
  }
  if(links_->size() > 0)
  {
    cout<<endl<<"Number of Links: "<<links_->size();
    for(int i=0; i<links_->size(); i++)
    {
      (*links_)[i]->printLinks();
    }
  }
}















