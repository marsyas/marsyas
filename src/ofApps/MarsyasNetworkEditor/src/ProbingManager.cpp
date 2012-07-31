//
//  ProbingManager.cpp
//  MNE
//
//  Created by Andre Perrotta on 7/2/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "ProbingManager.h"
#include "ofMain.h"
#include "MarSystem.h"
#include "realvec.h"
#include "GraphicalEnvironment.h"

using namespace Marsyas;

ProbingManager::ProbingManager(){
    pData_ = NULL;
    env_ = NULL;
    
    auxDrawVec_.resize(2);
    for(int i=0; i<auxDrawVec_.size(); i++){
        auxDrawVec_[i] = 0.0;
    }
}

ProbingManager::ProbingManager(GraphicalEnvironment* env){
    pData_ = NULL;
    env_ = env;
    
    writeLock_ = true;
    readLock_ = true;
    
    auxDrawVec_.resize(2);
    for(int i=0; i<auxDrawVec_.size(); i++){
        auxDrawVec_[i] = 0.0;
    }
    
}


ProbingManager::~ProbingManager(){
    
}

void ProbingManager::loadProcessedDataPointer(MarControlPtr pData){
    pData_ = pData;
    
    pDataBuffer_.clear();
    
    int marsyasRate = env_->getMarSystemWidget()->getMarSystemWidgetFromMap(pData_->getMarSystem())->getOsRate();
    int marsyasSampleSize = env_->getMarSystemWidget()->getMarSystemWidgetFromMap(pData_->getMarSystem())->getOnSamples();
    double marsyasFrameRate = (double)marsyasRate/(double)marsyasSampleSize;
    
    pDataBufferSize_ = ceil(marsyasFrameRate/ofGetFrameRate()) + 1000;
    
    pDataBuffer_.resize(pDataBufferSize_);
    
    writePoint_ = 0;
    readPoint_ = 0;
    
    writeLock_ = false;
    readLock_ = true;
    
}

void ProbingManager::update(){
    
}

void ProbingManager::draw(){
    
    if(!readLock_){
        ofSetColor(255, 0, 0);
        
        ofDrawBitmapString("visualization mode", 20, 20);
        
        ofDrawBitmapString("control: " + pData_->getMarSystem()->getName() + "/" + pData_->getName(), 20, 50);
        
        auxDrawVec_.clear();
        auxDrawVec_.resize(2);
        for(int i=0; i<auxDrawVec_.size(); i++){
            auxDrawVec_[i] = 0.0;
        }
        
        while(readPoint_%pDataBufferSize_ != writePoint_%pDataBufferSize_){
            int i = readPoint_%pDataBufferSize_;
            for (int c = 0; c <pDataBuffer_[i].getCols(); ++c) {
                for (int r = 0; r <pDataBuffer_[i].getRows() ; ++r) {
                    auxDrawVec_.push_back((double)pDataBuffer_[i](c, r));
                }
            }
            readPoint_++;
            
        }
    }
    else{
        auxDrawVec_.clear();
        auxDrawVec_.resize(2);
        for(int i=0; i<auxDrawVec_.size(); i++){
            auxDrawVec_[i] = 0.0;
        }

    }
    
    ofSetColor(0, 0, 255);
    for(int i=0; i<auxDrawVec_.size() - 1; i++){
        ofLine(i*ofGetWidth()/auxDrawVec_.size(), auxDrawVec_[i]*100 + ofGetHeight()*0.5, (i+1)*ofGetWidth()/auxDrawVec_.size(), auxDrawVec_[i]*100 + ofGetHeight()*0.5);
    }
    
    
}

void ProbingManager::writeToBuffer(){
    if(!writeLock_){
        pDataBuffer_[writePoint_%pDataBufferSize_] = pData_->to_realvec();
        writePoint_++;
        readLock_ = false;
    }
    
}