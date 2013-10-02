//
//  ProbingManager.cpp
//  MNE
//
//  Created by Andre Perrotta on 7/2/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "utils.h"
#include "ProbingManager.h"
#include "ofMain.h"
#include <marsyas/system/MarSystem.h>
#include <marsyas/realvec.h>
#include "GraphicalEnvironment.h"
#include "GraphicalParameters.h"

using namespace Marsyas;

ProbingManager::ProbingManager() {
  ProbingManager(NULL);
}

ProbingManager::ProbingManager(GraphicalEnvironment* env) {

  isVisible_ = true;
  isLoaded_ = false;
  Widget::setup(305, ofGetHeight() - PROBING_PANEL_HEIGHT, ofGetWidth() - 308, PROBING_PANEL_HEIGHT, env);
  mBtn_ = new MaximizeButton(x_ + width_ - 18, y_ + 3, 14, 14, env);
  mBtn_->updatePosition(x_ + width_ - 18, y_ + 3, 14, 14);



  pData_ = NULL;

  writeLock_ = true;
  readLock_ = true;

  bufferHead_ = NULL;
  windowSize_ = 1;
  stepSize_ = 1;


  //for Debugging
  //recording = fopen("/Applications/Programming/of_0071_osx_release/apps/Marsyas-apps/MNE-SVN/bin/data/sound.txt", "w");

}


ProbingManager::~ProbingManager() {

}

void ProbingManager::calcBufferSize() {
  int maxW = WRITE_BLOCKS;
  int minOr = 10;

  int S = pData_->to_realvec().getRows()*pData_->to_realvec().getCols();
  int Ar = pData_->getMarSystem()->getControl("mrs_real/osrate")->to_real();

  pDataBufferSize_ = (int)((maxW - minOr*S/Ar)*S) + 1;

  cout<<endl<<"bufferSize = "<<pDataBufferSize_;

}

void ProbingManager::deallocateBuffer() {
  realvecBufferObject* it = bufferHead_;
  cout<<endl<<"deallocating buffer";


  while (it->prox != bufferHead_) {
    it = it->prox;
  }
  it->prox = NULL;

  deleteBufferElement(bufferHead_);



}

void ProbingManager::deleteBufferElement(realvecBufferObject* be) {
  if(be) {
    deleteBufferElement(be->prox);
    delete be;
  }
}

void ProbingManager::allocateBuffer() {

  realvec aux;
  aux.create(0.0, pData_->to_realvec().getCols(), pData_->to_realvec().getRows());

  bufferHead_ = new realvecBufferObject;
  bufferHead_->value = aux;
  bufferHead_->prox = NULL;
  realvecBufferObject* it;
  it = bufferHead_;


  for(int i=0; i<(pDataBufferSize_ - 1); i++) {
    it->prox = new realvecBufferObject;
    it = it->prox;
    it->value = aux;
    it->prox = NULL;

    if(i == (pDataBufferSize_ - 2)) {
      it->prox = bufferHead_;
    }
  }

  readPoint_ = bufferHead_;
  writePoint_ = bufferHead_;
  writeCounter_ = 0;

}

void ProbingManager::loadProcessedDataPointer(MarControlPtr pData) {
  writeLock_ = true;

  pData_ = pData;

  calcBufferSize();

  if(bufferHead_) {
    deallocateBuffer();
  }

  allocateBuffer();

  writeLock_ = false;
  readLock_ = true;
  secondaryBuffer_.clear();
  secondaryBuffer_.resize(int(width_));
  isLoaded_ = true;



}

void ProbingManager::calcStepSize() {
  stepSize_ = ceil((double)windowSize_*WRITE_BLOCKS*pData_->to_realvec().getCols()/(double)(width_)) + 1; //FIXME this only works for one channel
  secondaryBuffer_.clear();
  secondaryBuffer_.resize(int(width_));
}

void ProbingManager::update() {

  if(isVisible_) {
    updatePosition(305, ofGetHeight() - PROBING_PANEL_HEIGHT, ofGetWidth() - 308, PROBING_PANEL_HEIGHT);
  }
  else {
    updatePosition(305, ofGetHeight() - 20, ofGetWidth() - 308, PROBING_PANEL_HEIGHT);

  }
  mBtn_->updatePosition(x_ + width_ - 18, y_ + 3, 14, 14);


  if(!readLock_) {
    if(writeCounter_ >= WRITE_BLOCKS) {
      int i = WRITE_BLOCKS;
      writeCounter_ = 0;
      while(i > 0) {
        for(int j=0; j<readPoint_->value.getCols(); j = j + stepSize_) {
          secondaryBuffer_.push_back(readPoint_->value(0, j));
          secondaryBuffer_.erase(secondaryBuffer_.begin() + 0);
        }
        readPoint_ = readPoint_->prox;
        i--;
      }
      //cout<<endl<<secondaryBuffer_.size();
    }

  }







  //cout<<endl<<secondaryBuffer_.size();

}



void ProbingManager::draw() {




  ofFill();
  ofSetColor(75, 75, 75, 253);
  ofRect(x_, y_, width_, height_);

  ofSetColor(190, 190, 190, 255);
  ofRect(x_, y_, width_, 20);

  ofNoFill();
  ofSetColor(240, 240, 240, 255);
  ofRect(x_, y_, width_, height_);

  ofSetColor(46, 68, 255, 255);
  env_->fontSmall_.drawString("Processed data visualization:", x_ + 2, y_ + 15);
  env_->fontSmall_.drawString("window size = " + ofToString(windowSize_) + "* mrs_natural/inSamples", x_ + 302, y_ + 15);

  mBtn_->draw();



  if(isLoaded_ && isVisible_) {



    ofSetColor(255, 0, 255);

    for(int i=0; i<(secondaryBuffer_.size() - 1); i++) {
      ofLine((double)(i*(width_ - 3))/secondaryBuffer_.size() + x_ + 2, (secondaryBuffer_[i]*60.0 + height_*0.5 + y_), (double)((i+1)*(width_ - 3))/secondaryBuffer_.size() + x_ + 2, (secondaryBuffer_[i+1]*60.0 + height_*0.5 + y_));
    }



    /*
     for(int i=0; i<(secondaryBuffer_.size()); i++){
     ofLine(i, secondaryBuffer_[i]*200 + ofGetHeight()*0.5, i+1, secondaryBuffer_[i+1]*200 + ofGetHeight()*0.5);
     }
     */


  }


}

void ProbingManager::writeToBuffer() {
  if(!writeLock_) {
    readLock_ = true;
    writeCounter_++;
    writePoint_->value = pData_->to_realvec();
    writePoint_ = writePoint_->prox;
    readLock_ = false;
  }
}


bool ProbingManager::mouseOver() {

  mBtn_->mouseOver();

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

bool ProbingManager::mousePressed() {


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

    if(env_->mouse_->click == 0)
    {
      toggleState();
      dragLock_ = true;
      return true;
    }
  }
  return false;
}

bool ProbingManager::mouseDragged() {

}

bool ProbingManager::mouseReleased() {

}


void ProbingManager::keyPressed  (int key)
{


  switch(key)
  {
  case OF_KEY_UP:
    windowSize_++;
    if(windowSize_ > MAX_WRITE_BLOCKS) {
      windowSize_ = 1;
    }
    calcStepSize();
    break;
  case OF_KEY_DOWN:
    windowSize_--;
    if(windowSize_ <= 0) {
      windowSize_ = MAX_WRITE_BLOCKS;
    }
    calcStepSize();
    break;
  }
  cout<<endl<<" windowSize_ = "<<windowSize_<<" stepSize_ = "<<stepSize_;


}







