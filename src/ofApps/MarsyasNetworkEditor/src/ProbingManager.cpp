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
    
    
    pDataBuffer_.clear();
    realvec aux;
    aux.create(0.0, 1, 2048);
    for(int i=0; i<PDATA_BUFFER_SIZE; i++){
        pDataBuffer_.push_back(aux);
    }
    
    
    
    
}


ProbingManager::~ProbingManager(){
    
}

void ProbingManager::loadProcessedDataPointer(MarControlPtr pData){
    pData_ = pData;
    pDataBuffer_.clear();
    
    writeLock_ = false;
    
}

void ProbingManager::update(){
    
}

void ProbingManager::draw(){
    
    if(!readLock_){
        
        
        if(pDataBuffer_.size() > 2){
            auxDrawVec_.clear();
            for(int i=0; i<2; i++){
                for(int c=0; c<pDataBuffer_[i].getCols(); c++){
                    for(int r=0; r<pDataBuffer_[i].getRows(); r++){
                        //cout<<endl<<(double)(pDataBuffer_[i](c, r));
                        auxDrawVec_.push_back((double)(pDataBuffer_[i](c, r)));
                    }
                }
            }
            for(int i=0; i<2; i++){
                pDataBuffer_.erase(pDataBuffer_.begin() + i);
            }
        }
        
        
        
        ofSetColor(0, 0, 255);
        for(int i=0; i<(auxDrawVec_.size() - 1); i++){
            ofLine(i*ofGetWidth()/auxDrawVec_.size(), auxDrawVec_[i]*100 + ofGetHeight()*0.5, (i+1)*ofGetWidth()/auxDrawVec_.size(), auxDrawVec_[i+1]*100 + ofGetHeight()*0.5);
        }
    }
    
    
}

void ProbingManager::writeToBuffer(){
    
    if(!writeLock_){
        if(pDataBuffer_.size() > PDATA_BUFFER_SIZE){
            pDataBuffer_.clear();
        }
        pDataBuffer_.push_back(pData_->to_realvec());
        readLock_ = false;
    }
    
}