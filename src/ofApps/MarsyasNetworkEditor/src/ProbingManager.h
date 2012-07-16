//
//  ProbingManager.h
//  MNE
//
//  Created by Andre Perrotta on 7/2/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef ProbingManager_h
#define ProbingManager_h

#include "MarControl.h"


namespace Marsyas {
    class ProbingManager{
        
    protected:
        
        MarControlPtr pData_;
        
        
        
    public:
        
        ProbingManager();
        ~ProbingManager();
        
        void loadProcessedDataPointer(MarControlPtr pData);
        
        void update();
        void draw();
        
        
    };
}





#endif
