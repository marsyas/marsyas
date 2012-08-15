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
#include "BufferUtils.h"

#define PDATA_BUFFER_SIZE 50


namespace Marsyas {
	class GraphicalEnvironment;
	
	class ProbingManager{
		
	protected:
		
		
		GraphicalEnvironment* env_;
		
		MarControlPtr pData_;
		
		realvecBufferObject* bufferHead_;
		
		realvecBufferObject* readPoint_;
		realvecBufferObject* writePoint_;
		
		int pDataBufferSize_;
		bool writeLock_;
		bool readLock_;
		
		std::vector<realvec> primaryBuffer_;
		std::vector<double> secondaryBuffer_;
		
		
		
	public:
		
		ProbingManager();
		ProbingManager(GraphicalEnvironment* env);
		~ProbingManager();
		
		void loadProcessedDataPointer(MarControlPtr pData);
		
		void update();
		void draw();
		
		void writeToBuffer();
		
		int viewZoom;
		
		
	};
}





#endif
