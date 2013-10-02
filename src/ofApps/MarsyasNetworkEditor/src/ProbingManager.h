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
#include <marsyas/system/MarSystemManager.h>
#include "BufferUtils.h"
#include "Widget.h"
#include "maximizeButton.h"

#define PDATA_BUFFER_SIZE 512
#define WRITE_BLOCKS 3
#define MAX_WRITE_BLOCKS 150


namespace Marsyas {
class GraphicalEnvironment;

class ProbingManager : public Widget {

protected:


  MarControlPtr pData_;

  realvecBufferObject* bufferHead_;

  realvecBufferObject* readPoint_;
  realvecBufferObject* writePoint_;

  int pDataBufferSize_;
  bool writeLock_;
  bool readLock_;
  int writeCounter_;
  int stepSize_;

  void calcStepSize();

  void calcBufferSize();
  void deallocateBuffer();
  void allocateBuffer();
  void deleteBufferElement(realvecBufferObject* be);

  std::vector<realvec> primaryBuffer_;
  std::vector<double> secondaryBuffer_;


  MaximizeButton* mBtn_;
  bool isVisible_;

  bool isLoaded_;



  //For debugging
  FILE* recording;

  realvec previousRealvec;


public:

  ProbingManager();
  ProbingManager(GraphicalEnvironment* env);
  ~ProbingManager();

  void loadProcessedDataPointer(MarControlPtr pData);

  void update();
  void draw();

  void writeToBuffer();

  int windowSize_;


  //mouse
  bool mouseOver();
  bool mousePressed();
  bool mouseDragged();
  bool mouseReleased();

  void keyPressed(int key);


};
}





#endif
