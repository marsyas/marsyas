//
//  MarSystemThread.cpp
//  OF-Marsyas-helloWorld
//
//  Created by Andre Perrotta on 6/28/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "MarSystemThread.h"

using namespace Marsyas;

MarSystemThread::MarSystemThread() {
  loadMarSystem(NULL);
}

MarSystemThread::MarSystemThread(MarSystem* msys) {
  loadMarSystem(msys);
}

MarSystemThread::MarSystemThread(GraphicalEnvironment* env) {
  env_ = env;
  loadMarSystem(NULL);
  tickStatus_ = 0;
  setTickLock_ = true;
}

MarSystemThread::~MarSystemThread() {

}

void MarSystemThread::start() {
  startThread(false, false);
}

void MarSystemThread::restartThread() {
  startThread(true, false);
}


void MarSystemThread::stop() {
  stopThread();
}


void MarSystemThread::threadedFunction() {


  while( isThreadRunning() != 0 ) {
    if(!setTickLock_) {
      tickStatus_ = tick_;
      setTickLock_ = true;
    }
    if(tickStatus_ > 0) {
      if(lock()) {
        if(isLoaded()) {
          msys_->updControl("mrs_bool/active", true);
          msys_->tick();
          env_->probe_->writeToBuffer();
          msys_->updControl("mrs_bool/active", false);
        }
        unlock();
      }
    }
    if(tickStatus_ == 2) {
      tickStatus_ = 0;
      msys_->updControl("mrs_bool/active", false);
    }

  }

}

void MarSystemThread::loadMarSystem(MarSystem *msys) {
  stop();
  msys_ = msys;
}

bool MarSystemThread::isLoaded() {
  if(msys_ != NULL) {
    return true;
  }
  return false;
}

MarSystem* MarSystemThread::getMarSystem() {
  return msys_;
}

void MarSystemThread::setTickStatus(int tick) {
  tick_ = tick;
  setTickLock_ = false;
  //cout<<endl<<"tickstatus = "<<tickStatus_;
}

int MarSystemThread::getTickStatus() {
  return tickStatus_;
}
