//
//  MarSystemThread.cpp
//  OF-Marsyas-helloWorld
//
//  Created by Andre Perrotta on 6/28/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "MarSystemThread.h"

using namespace Marsyas;

MarSystemThread::MarSystemThread(){
    loadMarSystem(NULL);
}

MarSystemThread::MarSystemThread(MarSystem* msys){
    loadMarSystem(msys);
}

MarSystemThread::~MarSystemThread(){
    
}

void MarSystemThread::start(){
    startThread(true, false);
}

void MarSystemThread::stop(){
    stopThread();
}


void MarSystemThread::threadedFunction(){
    while( isThreadRunning() != 0 ){
        if(lock()){
            if(isLoaded()){
                msys_->tick();
                //ofSleepMillis(0);
            }
            unlock();
        }
    }
}

void MarSystemThread::loadMarSystem(MarSystem *msys){
    stop();
    msys_ = msys;
}

bool MarSystemThread::isLoaded(){
    if(msys_ != NULL){
        return true;
    }
    return false;
}

MarSystem* MarSystemThread::getMarSystem(){
    return msys_;
}