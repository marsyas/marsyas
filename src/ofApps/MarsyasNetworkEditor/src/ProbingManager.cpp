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

using namespace Marsyas;

ProbingManager::ProbingManager(){
	ProbingManager(NULL);
}

ProbingManager::ProbingManager(GraphicalEnvironment* env){
	
	env_ = env;
	
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
	
}

void ProbingManager::update(){
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
	ofSetColor(255, 0, 255);
    
    /*
     for(int i=0; i<(secondaryBuffer_.size() - 1); i++){
     ofLine(i*ofGetWidth()/secondaryBuffer_.size(), secondaryBuffer_[i]*100 + ofGetHeight()*0.5, (i+1)*ofGetWidth()/secondaryBuffer_.size(), secondaryBuffer_[i+1]*100 + ofGetHeight()*0.5);
     }
     */
     
    
    for(int i=0; i<secondaryBuffer_.size(); i++){
        ofCircle((float)i*ofGetWidth()/secondaryBuffer_.size(), (secondaryBuffer_[i]*100.0 + ofGetHeight()*0.5), 1);
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