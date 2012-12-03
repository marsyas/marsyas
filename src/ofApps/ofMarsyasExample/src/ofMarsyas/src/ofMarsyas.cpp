//
//  ofMarsyas.cpp
//  ofMarsyasExample
//
//  Created by Andre Perrotta on 11/30/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>

#include "ofMarsyas.h"


ofMarsyas::ofMarsyas(){
    //**********************************************************
    //GraphicalEnvironment creation 
    graphicalEnv_ = new Marsyas::GraphicalEnvironment();
    
    //Marsyas Thread
    msysThread_ = new Marsyas::MarSystemThread(graphicalEnv_);
    graphicalEnv_->setMarSystemThread(msysThread_);
}

ofMarsyas::~ofMarsyas(){
    
}

bool ofMarsyas::createFromFile(std::string mpl){
    ofDirectory dir;
    dir.listDir("../data");
    string path = dir.getAbsolutePath();
    cout<<endl<<path;
    string fileName = path + "/" + mpl;
    cout<<endl<<fileName;
    
    network_ = mng_.loadFromFile(fileName); //FIXME: hardcoded .mpl input
    
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


void ofMarsyas::start(){
    msysThread_->start();
}


void ofMarsyas::update(){
    graphicalEnv_->update();
}

//--------------------------------------------------------------
void ofMarsyas::draw(){
    if(graphicalEnv_->isLoaded()){
        graphicalEnv_->draw();
    }
}




//--------------------------------------------------------------
void ofMarsyas::keyPressed(int key){
    graphicalEnv_->keyPressed(key);
    
}

//--------------------------------------------------------------
void ofMarsyas::keyReleased(int key){
    
}

//--------------------------------------------------------------
void ofMarsyas::mouseMoved(int x, int y){
    graphicalEnv_->mouseMoved(x, y);
}

//--------------------------------------------------------------
void ofMarsyas::mouseDragged(int x, int y, int button){
    graphicalEnv_->mouseDragged(x, y, button);
}

//--------------------------------------------------------------
void ofMarsyas::mousePressed(int x, int y, int button){
    graphicalEnv_->mousePressed(x, y, button);
}

//--------------------------------------------------------------
void ofMarsyas::mouseReleased(int x, int y, int button){
    graphicalEnv_->mouseReleased(x, y, button);
}

//--------------------------------------------------------------
void ofMarsyas::windowResized(int w, int h){
    graphicalEnv_->windowResized(w, h);
}