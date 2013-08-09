//
//  ofMarsyas.cpp
//  ofMarsyasExample
//
//  Created by Andre Perrotta on 11/30/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>

#include "ofMarsyas.h"


ofMarsyas::ofMarsyas() {
  setup();
}

ofMarsyas::~ofMarsyas() {

}


void ofMarsyas::setup() {
  //**********************************************************
  //GraphicalEnvironment creation
  graphicalEnv_ = new Marsyas::GraphicalEnvironment();

  //Marsyas Thread
  msysThread_ = new Marsyas::MarSystemThread(graphicalEnv_);
  graphicalEnv_->setMarSystemThread(msysThread_);

  help_ = false;
}

bool ofMarsyas::createFromFile(std::string mpl) {
  ofDirectory dir;
  dir.listDir("../data");
  string path = dir.getAbsolutePath();
  //cout<<endl<<path;
  string fileName = path + "/" + mpl;
  //cout<<endl<<fileName;

  network_ = mng_.loadFromFile(fileName);

  if (!network_)
  {
    cout << "couldn't load .mpl file! Program will crash..." << endl;//FIXME: verify that mpl file loaded correctly
    return false;
  }

  wmng_ = new Marsyas::MarSystemWidgetManager(graphicalEnv_);
  networkWidget_ = wmng_->setupWidgets(network_);
  graphicalEnv_->setupForMarSystemWidget(networkWidget_);
  msysThread_->loadMarSystem(network_);

  return true;
}


bool ofMarsyas::createFromPointer(Marsyas::MarSystem* msys) {
  if(msys == NULL) {
    return false;
  }
  network_ = msys;
  wmng_ = new Marsyas::MarSystemWidgetManager(graphicalEnv_);
  networkWidget_ = wmng_->setupWidgets(network_);
  graphicalEnv_->setupForMarSystemWidget(networkWidget_);
  msysThread_->loadMarSystem(network_);

  return true;
}

void ofMarsyas::start() {
  checkInitSoundCard(network_);
  msysThread_->start();

}

void ofMarsyas::checkInitSoundCard(Marsyas::MarSystem* msysTest) {
  if(msysTest->getType() == "AudioSink") {
    if(!msysTest->hasControlState("mrs_bool/initAudio")) {
      msysTest->updControl("mrs_bool/initAudio", true);
    }

  }
  else {
    std::vector <Marsyas::MarSystem*> children;
    children = msysTest->getChildren();
    for(int i=0; i<children.size(); i++) {
      checkInitSoundCard(children[i]);
    }
  }

}


void ofMarsyas::update() {
  graphicalEnv_->update();
}

void ofMarsyas::draw() {
  if(help_) {
    ofSetColor(255, 0, 0);
    ofDrawBitmapString("Marsyas Network Viewer Help", 20, 20);
    ofDrawBitmapString("===========================", 20, 40);
    ofDrawBitmapString("Keymap:", 20, 60);
    ofDrawBitmapString("h -> show/hide help", 20, 80);
    ofDrawBitmapString("space bar -> start/stop audio", 20, 100);
    ofDrawBitmapString("t -> tick the network once", 20, 120);
    ofDrawBitmapString("c -> show/hide connections", 20, 140);
    ofDrawBitmapString("v -> show/hide linked controls", 20, 160);
    ofDrawBitmapString("click on orange squares to see data on Processed Data Display", 20, 180);
    ofDrawBitmapString("click on green squares to see linked controls on Controls Diagram Display", 20, 200);
  }

  else {
    if(graphicalEnv_->isLoaded()) {
      graphicalEnv_->draw();
    }
  }



}


Marsyas::MarSystem* ofMarsyas::getNetworkRoot() {
  return network_;
}

Marsyas::MarSystemWidget* ofMarsyas::getNetworkRootWidget() {
  return networkWidget_;
}

std::vector<std::vector<double> >* ofMarsyas::getDataBuffer() {
  return graphicalEnv_->getDataBuffer();
}




bool ofMarsyas::saveToFile(std::string name) {
  ofDirectory dir;
  dir.listDir("../data");
  string path = dir.getAbsolutePath();
  //cout<<endl<<path;
  string fileName = path + "/" + name;
  cout<<endl<<fileName;
  ofstream oss;
  oss.open(fileName.c_str());
  oss << *network_;
  oss.close();
  if(oss.good()) {
    return true;
  }
  else {
    cout<<endl<<"save operation failed !";
    return false;
  }
}

void ofMarsyas::updControl(std::string msysName, std::string ctrlName, std::string ctrlValue) {
  networkWidget_->getMarSystemWidgetFromMapByName(msysName)->getMarSystem()->updControl(ctrlName, ctrlValue);
}

//--------------------------------------------------------------
void ofMarsyas::keyPressed(int key) {
  graphicalEnv_->keyPressed(key);

  switch(key) {
  case 'h':
    help_ = !help_;
    break;
  }

}

//--------------------------------------------------------------
void ofMarsyas::keyReleased(int key) {

}

//--------------------------------------------------------------
void ofMarsyas::mouseMoved(int x, int y) {
  graphicalEnv_->mouseMoved(x, y);
}

//--------------------------------------------------------------
void ofMarsyas::mouseDragged(int x, int y, int button) {
  graphicalEnv_->mouseDragged(x, y, button);
}

//--------------------------------------------------------------
void ofMarsyas::mousePressed(int x, int y, int button) {
  graphicalEnv_->mousePressed(x, y, button);
}

//--------------------------------------------------------------
void ofMarsyas::mouseReleased(int x, int y, int button) {
  graphicalEnv_->mouseReleased(x, y, button);
}

//--------------------------------------------------------------
void ofMarsyas::windowResized(int w, int h) {
  graphicalEnv_->windowResized(w, h);
}