//
//  MarSystemWidget.cpp
//  allAddonsExample
//
//  Created by André Perrotta on 6/17/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "MarSystemWidget.h"
#include "globalVars.h"
#include "GraphicalEnvironment.h"
#include "maximizeButton.h"
#include "toggleButton.h"
#include "MarControlWidget.h"
#include "ProcessedDataWidget.h"

using namespace Marsyas;


MarSystemWidget::MarSystemWidget()
{
  //printf("\nMarSystemWidget() Construtor");
  setup(NULL);
}

MarSystemWidget::MarSystemWidget(MarSystem *msys, GraphicalEnvironment* env) : Widget(0, 0, 0, 0, env)
{
  //printf("\nMarSystemWidget(MarSystem *msys, GraphicalEnvironment* env) Construtor");
  setup(msys);
}

MarSystemWidget::MarSystemWidget(int x0, int y0, int w, int h, MarSystem *msys, GraphicalEnvironment* env): Widget(x0, y0, w, h, env)
{
  //printf("\nMarSystemWidget(int x0, int y0, int w, int h, MarSystem *msys, GraphicalEnvironment* env) Construtor");
  setup(msys);
}

MarSystemWidget::~MarSystemWidget()
{
  //deallocate widgets for all children
  for(int i=0; i < children_.size(); ++i)
  {
    delete children_[i];
  }
  children_.clear();

  //deallocate local MarControl Widgets
  for(int i=0; i < ctrlWidgets_.size(); ++i)
  {
    delete ctrlWidgets_[i];
  }
  ctrlWidgets_.clear();

  delete mBtn_;

}

void MarSystemWidget::setup(MarSystem *msys)
{
  auxCounter = 0;

  msys_ = msys;
  parent_ = NULL;
  topMostParent_ = this;

  if (msys)
  {
    isComposite_ = msys->isComposite();
    name_ = msys_->getName(); //name;
    type_ = msys_->getType(); //type;
  }
  else
  {
    isComposite_ = false;
    name_ = "foo";
    type_ = "foo";
  }

  prefix_ = "/" + type_ + "/" + name_ + "/";
  absPath_ = prefix_;
  children_.clear();

  mBtn_ = new MaximizeButton(x_ + width_ - 25, y_ + 5, 20, 20, env_);
  drawChildren_ = true;
  maxChildrenWidth_ = 0;

  showChildrenConnections_ = true;

  probe_ = new ProcessedDataWidget(this, env_);

  //get controls and create corresponding widgets (and any existing links)
  //createControlWidgets();

  //Needed to start up pretty...
  update();


}

void MarSystemWidget::createControlWidgets()
{
  std::map<std::string, MarControlPtr> tmp;
  std::map<std::string, MarControlPtr>::iterator it;

  //get a list of all the local controls
  tmp = msys_->getLocalControls();

  //create widgets for all local controls
  for(it=tmp.begin(); it != tmp.end(); it++)
  {
    ctrlWidgets_.push_back(new MarControlWidget(it->second, this));
    if(it->second->getName() == "mrs_realvec/processedData") {
      probe_->loadProcessedDataPointer(it->second);
    }
    else if(it->second->getName() == "mrs_natural/onSamples") {
      onSamples_ = it->second->to_natural();
    }
    else if(it->second->getName() == "mrs_real/osrate") {
      osRate_ = it->second->to_real();
    }
  }

  //
  //add all local controls from msys to ctrlMap_
  //
  //pair<map<MarControlPtr, MarControlWidget*>::iterator,bool> ret; //debugging...

  for(int i=0; i<ctrlWidgets_.size(); i++)
  {
    //ret =
    topMostParent_->ctrlMap_.insert(std::pair<MarControlPtr, MarControlWidget*>(ctrlWidgets_[i]->getMarControlPtr(), ctrlWidgets_[i]));
  }


}

void MarSystemWidget::createControlLinkWidgets()
{
  for(int i=0; i<ctrlWidgets_.size(); i++) {
    ctrlWidgets_[i]->setupLinks();
  }
}

void MarSystemWidget::addMarSystemWidget(MarSystemWidget *msysWidget)
{
  msysWidget->setParent(this);

  msysWidget->update();
  children_.push_back(msysWidget);


  //topMostParent->msysMap_.push_back(msysWidget->getMarSystem(), msysWidget);
  //topMostParent_->insertToMsysMap(msysWidget->getMarSystem(), msysWidget);

  //Needed to look pretty...
  update();
}

void MarSystemWidget::insertToMsysMap(MarSystem* msys, MarSystemWidget* msysw)
{

  auxCounter++;
  MRSASSERT(parent_ == NULL);
  msysMap_.insert(pair<MarSystem*, MarSystemWidget*>(msys, msysw));
}


// update methods **************************************************************

void MarSystemWidget::update()
{
  ///////////////////////////////////////
  //update children widgets
  ///////////////////////////////////////

  //FIXME: shouldn't this only be performed if (drawChildren_ == true)??
  for(int i=0; i<children_.size(); i++)
  {
    children_[i]->update();
  }
  updateChildrenWidgets();
  updateSize();


  //////////////////////////////////////
  // update Mouse stuff
  /////////////////////////////////////
  updateValuesForMouse(env_->zoomX_, env_->zoomY_, env_->xOffset_, env_->yOffset_);

  /////////////////////////////////////
  // update maximize button
  /////////////////////////////////////
  mBtn_->updatePosition(x_ + width_ - 25, y_ + 5, env_->zoomX_, env_->zoomY_, env_->xOffset_, env_->yOffset_);


  /////////////////////////////////////
  // update probe button
  /////////////////////////////////////
  probe_->updatePosition(x_ + width_ - probe_->getWidth(), y_ + height_*0.5 - probe_->getHeight()*0.5, env_->zoomX_, env_->zoomY_, env_->xOffset_, env_->yOffset_);


  /////////////////////////////////////
  // update control Widgets
  /////////////////////////////////////
  updateControlWidgets();




}

void MarSystemWidget::updateSize()
{
  ///////////////////////////////////////
  // update OutBox
  //////////////////////////////////////
  if(drawChildren_)
  {
    width_ = 0;
    height_ = 0;
    childrenTotalWidth_ = 0;
    childrenTotalHeight_ = 0;
    for(int i=0; i<children_.size(); i++)
    {
      childrenTotalWidth_ += children_[i]->width_ + X_SEPARATION;
      if(children_[i]->height_ > childrenTotalHeight_)
      {
        childrenTotalHeight_ = children_[i]->height_;
      }
    }
    if(childrenTotalWidth_ > 0)
    {
      childrenTotalWidth_ += X_SEPARATION;
    }
    if(childrenTotalHeight_ > 0)
    {
      childrenTotalHeight_ += 80;
    }

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


void MarSystemWidget::updateChildrenWidgets()
{
  int startX = x_;
  for(int i=0; i<children_.size(); i++)
  {
    children_[i]->x_ = startX + X_SEPARATION;
    startX += children_[i]->width_ + X_SEPARATION;
    children_[i]->y_ = y_ + height_*0.5 - children_[i]->height_*0.5;
  }
}

void MarSystemWidget::updateControlWidgets()
{

  hiddenControlPosition_ = ofPoint(x_ + width_ - 10, y_ + height_ - CTRL_PANNEL_HEIGHT*0.5);

  MarSystemWidget *aux;
  aux = this;

  int breaker = 0;

  while(aux != NULL && breaker == 0)
  {
    if(!aux->drawChildren_)
    {
      breaker = 1;
      hiddenControlPosition_ = aux->getHiddenControlPosition();
    }
    aux = aux->getParent();
  }

  if(breaker == 0)
  {
    int startX = x_;
    for(int i=0; i<ctrlWidgets_.size(); i++)
    {
      if(ctrlWidgets_[i]->hasLinks())
      {
        ctrlWidgets_[i]->updatePosition(startX + 10, y_ + height_ - CTRL_PANNEL_HEIGHT*0.5, ctrlWidgets_[i]->getWidth(), ctrlWidgets_[i]->getHeight(), env_->zoomX_, env_->zoomY_, env_->xOffset_, env_->yOffset_);
        //controlsWidgets[i]->x = startX + 10;
        //controlsWidgets[i]->y = y + 10;
        startX = ctrlWidgets_[i]->getX() + ctrlWidgets_[i]->getWidth() + 5;
      }
    }
  }
  else
  {
    for(int i=0; i<ctrlWidgets_.size(); i++)
    {
      if(ctrlWidgets_[i]->hasLinks())
      {
        ctrlWidgets_[i]->updatePosition(hiddenControlPosition_.x, hiddenControlPosition_.y, 0., 0., 0., 0.);
      }
    }
  }
}

void MarSystemWidget::updateButtonPosition()
{
  mBtn_->updatePosition(x_ + width_ - 25, y_ + 5, env_->zoomX_, env_->zoomY_, env_->xOffset_, env_->yOffset_);
}

//drawing methods *************************************************************

void MarSystemWidget::draw()
{
  //////////////////////////////////////////
  //draw out box
  //////////////////////////////////////////
  glPushMatrix();
  glLineWidth(1.0);
  glTranslated(x_, y_, 0);
  ofSetRectMode(OF_RECTMODE_CORNER);
  if(isMouseOver_)
  {
    ofSetColor(255, 255, 255);
  }
  else
  {
    if(parent_ == NULL) //use a different color in case we are the top parent
    {
      ofSetColor(0, 255, 0);
    }
    else
    {
      ofSetHexColor(0x62A4FF);
    }
  }
  ofNoFill();
  ofRect(0, 0, width_, height_);
  glPopMatrix();

  glPushMatrix();
  glLineWidth(1.0);
  glTranslated(x_, y_, 0);
  int xSeparation = 10;
  int ySeparation = nameHeight_ + 10;
  ofSetRectMode(OF_RECTMODE_CORNER);
  ofSetColor(255, 255, 255);
  ofFill();
  env_->fontMedium_.drawString(type_ + "/" + name_, xSeparation, ySeparation);
  glPopMatrix();

  //////////////////////////////////////////
  // draw children
  //////////////////////////////////////////
  if(children_.size() > 0)
  {
    mBtn_->draw();
  }
  if(drawChildren_)
  {
    drawChildrenWidgets();
    if(showChildrenConnections_) {
      drawConnections();
    }
  }

  //////////////////////////////////////////
  // draw control widgets
  //////////////////////////////////////////
  drawControlWidgets();

  probe_->draw();
}

void MarSystemWidget::drawChildrenWidgets()
{
  for(int i=0; i<children_.size(); i++)
  {
    children_[i]->draw();
  }
}

void MarSystemWidget::drawConnections()
{
  //nothing to draw here...
  //to be overridded only by composite MarSystemWidgets
  //(e.g. SeriesWidget, ParallelWidgets, etc).
}

void MarSystemWidget::drawControlWidgets()
{

  //TODO: aperrotta: draw outbox for MarControlWidgets
  ofSetLineWidth(1);
  ofSetHexColor(0xAFAEFA);
  glPushMatrix();
  glTranslated(x_, y_ + height_ - CTRL_PANNEL_HEIGHT, 0);
  ofNoFill();
  ofRect(0, 0, width_, CTRL_PANNEL_HEIGHT);
  glPopMatrix();
  ofSetColor(0, 255, 0);
  ofCircle(x_ + width_ - 10, y_ + height_ - CTRL_PANNEL_HEIGHT*0.5, 7);


  for(int i=0; i<ctrlWidgets_.size(); ++i)
  {
    if(	ctrlWidgets_[i]->hasLinks()) {
      ctrlWidgets_[i]->draw();
    }
  }

}


// Getters *********************************************************************

string MarSystemWidget::getName()
{
  return name_;
}

void MarSystemWidget::setParent(MarSystemWidget *msysWidget)
{
  parent_ = msysWidget;
  setTopMostParent();
}

MarSystemWidget *MarSystemWidget::getParent()
{
  return parent_;
}

vector<MarSystemWidget*> MarSystemWidget::getChildren()
{
  return children_;
}

void MarSystemWidget::setMarSystem(MarSystem *msys)
{
  msys_ = msys;
}

MarSystem *MarSystemWidget::getMarSystem()
{
  return msys_;

}

void MarSystemWidget::setTopMostParent() {
  MarSystemWidget* aux;
  aux = parent_;

  if(aux != NULL) {
    while(aux->getParent() != NULL)
    {
      aux = aux->getParent();
    }
    topMostParent_ = aux;
  }
  else {
    topMostParent_ = this;
  }

  for(int i=0; i<children_.size(); i++) {
    children_[i]->setTopMostParent();
  }

}

MarSystemWidget *MarSystemWidget::getTopMostParent() {
  return topMostParent_;
}

std::vector<MarControlWidget*> MarSystemWidget::getControlWidgets() {
  return ctrlWidgets_;
}

MarControlWidget* MarSystemWidget::getMarControlWidgetFromCtrlMap(MarControlPtr ptr) {
  if(ctrlMap_.size() > 0) {
    return ctrlMap_[ptr];
  }
  return NULL;
}

MarSystemWidget* MarSystemWidget::getMarSystemWidgetFromMapByName(string name) {

  std::map<MarSystem*, MarSystemWidget*>::iterator it;

  for(it = msysMap_.begin() ; it != msysMap_.end(); it++) {
    if((*it).second->getName() == name) {
      return (*it).second;
    }
  }
  return NULL;
}
MarSystemWidget* MarSystemWidget::getMarSystemWidgetFromMap(MarSystem* msys) {
  std::map<MarSystem*, MarSystemWidget*>::iterator it;

  for(it = msysMap_.begin() ; it != msysMap_.end(); it++) {
    if((*it).first == msys) {
      return (*it).second;
    }
  }
  return NULL;
}



ofPoint MarSystemWidget::getHiddenControlPosition() {
  return hiddenControlPosition_;
}


int MarSystemWidget::getOsRate() {
  return osRate_;
}

int MarSystemWidget::getOnSamples() {
  return onSamples_;
}

void MarSystemWidget::showFamilyUp() {


  MarSystemWidget* aux;
  aux = parent_;
  while(aux != NULL) {
    aux->drawChildren_ = true;
    aux->mBtn_->setState(true);
    aux->update();
    aux = aux->parent_;
  }

  drawChildren_ = true;
  mBtn_->setState(true);
  update();


}

void MarSystemWidget::switchChildrenConnections(bool state) {
  showChildrenConnections_ = state;
  for(int i=0; i<children_.size(); i++) {
    children_[i]->switchChildrenConnections(state);
  }
}

void MarSystemWidget::switchLinkedControlConnections(bool state) {
  for(int i=0; i<ctrlWidgets_.size(); ++i)
  {
    ctrlWidgets_[i]->showLinks_ = state;
  }
  for(int i=0; i<children_.size(); i++) {
    children_[i]->switchLinkedControlConnections(state);
  }
}

// Mouse methods **************************************************************

bool MarSystemWidget::mouseOver()
{
  bool aux = mBtn_->mouseOver();
  aux = probe_->mouseOver();

  int aux1 = 0;
  bool dummy;
  if(drawChildren_)
  {
    for(int i=0; i<children_.size(); i++)
    {
      dummy = children_[i]->mouseOver();
      if(dummy)
      {
        aux1++;
      }
    }
  }
  isMouseOver_ = false;

  for(int i=0; i<ctrlWidgets_.size(); i++) {
    ctrlWidgets_[i]->mouseOver();
  }

  if(aux1 == 0)
  {
    Widget::mouseOver();
    if(isMouseOver_)
    {
      return true;
    }
  }
  else
  {
    return true;
  }
  return false;
}

bool MarSystemWidget::mousePressed()
{
  update();
  for(int i=0; i<children_.size(); i++)
  {
    children_[i]->mousePressed();
  }


  for(int i=0; i<ctrlWidgets_.size(); i++) {
    ctrlWidgets_[i]->mousePressed();
  }

  bool tester;
  tester = mBtn_->mousePressed();
  tester = probe_->mousePressed();
  drawChildren_ = mBtn_->getState();
  update();
  if(parent_ != NULL)
  {
    parent_->update();
  }

}

bool MarSystemWidget::mouseDragged()
{
  for(int i=0; i<children_.size(); i++)
  {
    children_[i]->mouseDragged();
  }
  mBtn_->mouseDragged();
}

bool MarSystemWidget::mouseReleased()
{
  for(int i=0; i<children_.size(); i++)
  {
    children_[i]->mouseReleased();
  }
  mBtn_->mouseReleased();
}

bool MarSystemWidget::isMaximized()
{
  return drawChildren_;
}

// MISC ***********************************************************************

void MarSystemWidget::debugger()
{



  /*
   cout<<endl<<"============================================";
   cout<<endl<<"printing controls for MarSystemWidget: ";
   cout<<endl<<name_;//<<" width = "<<width_<<"  mBtn: "<<mBtn_->getX()<<", "<<mBtn_->getY();
   for(int i=0; i<ctrlWidgets_.size(); i++){
   ctrlWidgets_[i]->debugger();
   }
   cout<<endl<<"============================================";
   cout<<endl;
   */
  for(int i=0; i<children_.size(); i++) {
    children_[i]->debugger();
  }


}
