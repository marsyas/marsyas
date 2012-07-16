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

using namespace Marsyas;

ProbingManager::ProbingManager(){
    pData_ = NULL;
}

ProbingManager::~ProbingManager(){
    
}

void ProbingManager::loadProcessedDataPointer(MarControlPtr pData){
    pData_ = pData;
}

void ProbingManager::update(){
    
}

void ProbingManager::draw(){
    ofSetColor(255, 0, 0);
    
    ofDrawBitmapString("visualization mode", 20, 20);
    
    ofDrawBitmapString("control: " + pData_->getMarSystem()->getName() + "/" + pData_->getName(), 20, 50);
    
    std::vector<double> auxVec;
    
    for (int c = 0; c < pData_->to_realvec().getCols(); ++c) {
        for (int r = 0; r < pData_->to_realvec().getRows() ; ++r) {
            auxVec.push_back((double)pData_->to_realvec()(c, r));
        }
    }
    
    ofSetColor(0, 0, 255);
    for(int i=0; i<auxVec.size() - 1; i++){
        ofLine(i*ofGetWidth()/auxVec.size(), auxVec[i]*100 + ofGetHeight()*0.5, (i+1)*ofGetWidth()/auxVec.size(), auxVec[i]*100 + ofGetHeight()*0.5);
    }
}

