//
//  ofMarsyas.h
//  ofMarsyasExample
//
//  Created by Andre Perrotta on 11/30/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef ofMarsyasExample_ofMarsyas_h
#define ofMarsyasExample_ofMarsyas_h

#include "ofMain.h"
#include "MarSystemThread.h"
#include <marsyas/system/MarSystemManager.h>
#include "utils.h"
#include "MarSystemWidget.h"
#include "MiniViewWidget.h"
#include "MarControlDiagramDisplay.h"
#include "GraphicalEnvironment.h"
#include "GraphicalParameters.h"
#include "MarSystemWidgetManager.h"

class ofMarsyas : public ofBaseApp {
public:

  ofMarsyas();
  ~ofMarsyas();

  bool createFromFile(std::string mpl);
  bool createFromPointer(Marsyas::MarSystem* msys);

  void setup();
  void update();
  void draw();

  void keyPressed(int key);
  void keyReleased(int key);
  void mouseMoved(int x, int y);
  void mouseDragged(int x, int y, int button);
  void mousePressed(int x, int y, int button);
  void mouseReleased(int x, int y, int button);
  void windowResized(int w, int h);

  bool help_;


  void start();
  void checkInitSoundCard(Marsyas::MarSystem* msysTest);


  //getters
  Marsyas::MarSystem* getNetworkRoot();
  Marsyas::MarSystemWidget* getNetworkRootWidget();

  Marsyas::MarSystem* getMarsystemByName(std::string msys);
  Marsyas::MarSystemWidget* getMarsystemWidgetByName(std::string msys);

  std::vector<std::vector<double> >* getDataBuffer();


  //utils
  bool saveToFile(std::string name);


  //network managing


  //controls
  void updControl(std::string msysName, std::string ctrlName, std::string ctrlValue);


protected:

  Marsyas::MarSystem *network_;
  Marsyas::MarSystemWidget *networkWidget_;
  Marsyas::MarSystemThread *msysThread_;
  Marsyas::GraphicalEnvironment* graphicalEnv_;
  Marsyas::MarSystemManager mng_;
  Marsyas::MarSystemWidgetManager *wmng_;

};




#endif
