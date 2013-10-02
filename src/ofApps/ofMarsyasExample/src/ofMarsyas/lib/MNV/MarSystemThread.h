//
//  MarSystemThread.h
//  OF-Marsyas-helloWorld
//
//  Created by Andre Perrotta on 6/28/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef MarSystemThread_h
#define MarSystemThread_h

#include "ofMain.h"
#include "GraphicalEnvironment.h"
#include <marsyas/system/MarSystemManager.h>

namespace Marsyas {

class MarSystemThread : public ofThread {

protected:

  GraphicalEnvironment* env_;
  MarSystem* msys_;

  int tick_;
  bool setTickLock_;
  void restartThread();

public:

  MarSystemThread();
  MarSystemThread(MarSystem* msys);
  MarSystemThread(GraphicalEnvironment* env);
  ~MarSystemThread();



  MarSystem *getMarSystem();
  void loadMarSystem(MarSystem *msys);
  bool isLoaded();

  void start();
  void stop();
  void threadedFunction();

  void setTickStatus(int tick);
  int getTickStatus();
  MarControlPtr ctrlAux_;


};

}


#endif
