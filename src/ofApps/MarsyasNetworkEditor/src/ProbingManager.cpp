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
#include "MarSystem.h"
#include "realvec.h"
#include "GraphicalEnvironment.h"
#include "GraphicalParameters.h"

using namespace Marsyas;

ProbingManager::ProbingManager(){
	ProbingManager(NULL);
}

ProbingManager::ProbingManager(GraphicalEnvironment* env){
	
    isVisible_ = true;
    isLoaded_ = false;
    Widget::setup(305, ofGetHeight() - PROBING_PANEL_HEIGHT, ofGetWidth() - 308, PROBING_PANEL_HEIGHT, env);
    mBtn_ = new MaximizeButton(x_ + width_ - 18, y_ + 3, 14, 14, env);
    mBtn_->updatePosition(x_ + width_ - 18, y_ + 3, 14, 14);
    
    
	
	pData_ = NULL;
	
	writeLock_ = true;
	readLock_ = true;
	
	
	realvec aux;
	aux.create(0.0, 1, 512);
	
	bufferHead_ = new realvecBufferObject;
	bufferHead_->value = aux;
	bufferHead_->prox = NULL;
	realvecBufferObject* it;
	it = bufferHead_;
	
	for(int i=0; i<PDATA_BUFFER_SIZE; i++){
		it->prox = new realvecBufferObject;
		it = it->prox;
		it->value = aux;
		it->prox = NULL;
		
		if(i == (PDATA_BUFFER_SIZE - 1)){
			it->prox = bufferHead_;
		}
	}
	
	readPoint_ = bufferHead_;
	writePoint_ = bufferHead_;
	
	viewZoom = 2048;
    
    for(int i=0; i<viewZoom; i++){
        secondaryBuffer_.push_back(0.0);
    }
}


ProbingManager::~ProbingManager(){
	
}

void ProbingManager::loadProcessedDataPointer(MarControlPtr pData){
	pData_ = pData;
	readPoint_ = bufferHead_;
	writePoint_ = bufferHead_;
	writeLock_ = false;
	readLock_ = true;
	primaryBuffer_.clear();
    isLoaded_ = true;
	
}

void ProbingManager::update(){
    
    if(isVisible_){
        updatePosition(305, ofGetHeight() - PROBING_PANEL_HEIGHT, ofGetWidth() - 308, PROBING_PANEL_HEIGHT);
    }
    else{
        updatePosition(305, ofGetHeight() - 20, ofGetWidth() - 308, PROBING_PANEL_HEIGHT);
        
    }
    mBtn_->updatePosition(x_ + width_ - 18, y_ + 3, 14, 14);
    
    
	if(!readLock_){
		primaryBuffer_.clear();
		while(readPoint_ != writePoint_){
			primaryBuffer_.push_back(readPoint_->value);
			readPoint_ = readPoint_->prox;
		}
        
        secondaryBuffer_.clear();
        
		for(int i=0; i<primaryBuffer_.size(); i++){
			for(int r=0; r<primaryBuffer_[i].getRows(); r++){
				for(int c=0; c<primaryBuffer_[i].getCols(); c++){
                    secondaryBuffer_.push_back((double)(primaryBuffer_[i](c, r)));
				}
			}
		}
        //writeLock_ = true;
	}
	
    /*
     if(primaryBuffer_.size() >= viewZoom){
     secondaryBuffer_.clear();
     for(int i=0; i<primaryBuffer_.size(); i++){
     for(int r=0; r<primaryBuffer_[i].getRows(); r++){
     for(int c=0; c<primaryBuffer_[i].getCols(); c++){
     secondaryBuffer_.push_back((double)(primaryBuffer_[i](c, r)));
     }
     }
     }
     
     
     for(int i=0; i<viewZoom; i++){
     primaryBuffer_.erase(primaryBuffer_.begin() + i);
     }
     
     if(primaryBuffer_.size() > 1000){
     primaryBuffer_.clear();
     }
     
     
     if(secondaryBuffer_.size() >= 1024){
     for(int i=0; i<(secondaryBuffer_.size() - 1024)*256; i++){
     secondaryBuffer_.erase(secondaryBuffer_.begin() + i);
     }
     }
     
     }
     */
    
    //cout<<endl<<secondaryBuffer_.size();
	
}



void ProbingManager::draw(){
    
    
    
    
    ofFill();
    ofSetColor(75, 75, 75, 253);
    ofRect(x_, y_, width_, height_);
    
    ofSetColor(190, 190, 190, 255);
    ofRect(x_, y_, width_, 20);
    
    ofNoFill();
    ofSetColor(240, 240, 240, 255);
    ofRect(x_, y_, width_, height_);
    
    ofSetColor(46, 68, 255, 255);
    env_->fontSmall_.drawString("Processed data visualization", x_ + 2, y_ + 15);
    
    mBtn_->draw();
    
    if(isLoaded_ && isVisible_){
        ofSetColor(255, 0, 255);
        
        /*
         for(int i=0; i<(secondaryBuffer_.size() - 1); i++){
         ofLine(i*ofGetWidth()/secondaryBuffer_.size(), secondaryBuffer_[i]*100 + ofGetHeight()*0.5, (i+1)*ofGetWidth()/secondaryBuffer_.size(), secondaryBuffer_[i+1]*100 + ofGetHeight()*0.5);
         }
         */
        
        
        for(int i=0; i<secondaryBuffer_.size(); i++){
            ofCircle((float)(i*(width_ - 2))/secondaryBuffer_.size() + x_ + 2, (secondaryBuffer_[i]*60.0 + height_*0.5 + y_), 1);
        }
    }
    
    
}

void ProbingManager::writeToBuffer(){
	if(!writeLock_){
		readLock_ = true;
		writePoint_->value = pData_->to_realvec();
        writePoint_ = writePoint_->prox;
        readLock_ = false;
    }
}


bool ProbingManager::mouseOver(){
    
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

bool ProbingManager::mousePressed(){
    
    
    if(mouseOver())
	{
        bool aux = mBtn_->mousePressed();
        if(!mBtn_->getState()){
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

bool ProbingManager::mouseDragged(){
    
}

bool ProbingManager::mouseReleased(){
    
}










