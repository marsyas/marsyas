/************************************************************************/
/*! \class RtAudio
    \brief Realtime audio i/o C++ class.


*/
/************************************************************************/


#include "RtAudio.h"

#include <stdio.h>


#include <iostream>
#include <vector>
using namespace std;



// *************************************************** //
//
// Public common (OS-independent) methods.
//
// *************************************************** //

RtAudio :: RtAudio()
{
}


RtAudio :: ~RtAudio()
{
}


int RtAudio :: getDeviceCount(void)
{
}



char * const RtAudio :: getStreamBuffer(int streamId)
{
}



void RtAudio :: cancelStreamCallback(int streamId)
{
}










void RtAudio :: closeStream(int streamId)
{
}

void RtAudio :: startStream(int streamId)
{
}

void RtAudio :: stopStream(int streamId)
{
}

void RtAudio :: abortStream(int streamId)
{
}

int RtAudio :: streamWillBlock(int streamId)
{
}

void RtAudio :: tickStream(int streamId)
{
}

extern "C" void *callbackHandler(void *ptr)
{

  return 0;
}










