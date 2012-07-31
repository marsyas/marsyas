//
//  ProbingManager.h
//  MNE
//
//  Created by Andre Perrotta on 7/2/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef ProbingManager_h
#define ProbingManager_h

#include "ofMain.h"
#include "MarControl.h"
#include "MarSystemManager.h"


namespace Marsyas {
    class GraphicalEnvironment;
    
    class ProbingManager{
        
    protected:
        
        
        GraphicalEnvironment* env_;
        
        MarControlPtr pData_;
        
        std::vector<realvec> pDataBuffer_;
        int readPoint_;
        int writePoint_;
        int pDataBufferSize_;
        bool writeLock_;
        bool readLock_;
        
        std::vector<double> auxDrawVec_;
        
        
        
    public:
        
        ProbingManager();
        ProbingManager(GraphicalEnvironment* env);
        ~ProbingManager();
        
        void loadProcessedDataPointer(MarControlPtr pData);
        
        void update();
        void draw();
        
        void writeToBuffer();
        
        
    };
}





#endif
