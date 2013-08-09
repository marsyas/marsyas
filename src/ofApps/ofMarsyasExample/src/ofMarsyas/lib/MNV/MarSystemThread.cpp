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
  tick_ = 0;
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

    if(tick_ == 0) {

    }
    else if(tick_ == 1) {
      if(lock()) {
        if(isLoaded()) {
          msys_->updControl(ctrlAux_, true);
          msys_->tick();
          tick_ = 2;
          env_->probe_->writeToBuffer();

        }
        unlock();
      }

    }
    else if(tick_ == 2) {
      if(lock()) {
        if(isLoaded()) {
          msys_->tick();
          env_->probe_->writeToBuffer();
        }
        unlock();
      }

    }
    else if(tick_ == 3) {
      if(lock()) {
        if(isLoaded()) {
          msys_->tick();
          env_->probe_->writeToBuffer();
          msys_->updControl(ctrlAux_, false);
          tick_ = 0;
        }
        unlock();
      }
    }
    else if(tick_ == 4) {
      if(lock()) {
        if(isLoaded()) {
          msys_->updControl(ctrlAux_, true);
          msys_->tick();
          env_->probe_->writeToBuffer();
          msys_->updControl(ctrlAux_, false);
          tick_ = 0;
        }
        unlock();
      }
    }
  }
}

void MarSystemThread::loadMarSystem(MarSystem *msys) {
  stop();
  msys_ = msys;
  if(msys_) {
    ctrlAux_ = msys_->getControlLocal("mrs_bool/active");
    cout<<endl<<endl<<"getControLocal = "<<ctrlAux_->getName();
  }

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
}

int MarSystemThread::getTickStatus() {
  return tick_;
}
