//
//  BufferUtils.h
//  MNE
//
//  Created by André Perrotta on 8/15/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//


#ifndef MARSYAS_BUFFER_UTILS_H
#define MARSYAS_BUFFER_UTILS_H

#include <marsyas/system/MarSystemManager.h>

namespace Marsyas {

struct realvecBufferObject {

  realvecBufferObject* prox;
  realvec value;
};



}



#endif