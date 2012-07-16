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
#include "MarSystemManager.h"

namespace Marsyas {
    
    class MarSystemThread : public ofThread{
    public:
        
        MarSystemThread();
        MarSystemThread(MarSystem* msys);
        ~MarSystemThread();
        
        MarSystem *msys_;
        
        MarSystem *getMarSystem();
        void loadMarSystem(MarSystem *msys);
        bool isLoaded();
        
        void start();
        void stop();
		void threadedFunction();
        
    };
    
}


#endif
