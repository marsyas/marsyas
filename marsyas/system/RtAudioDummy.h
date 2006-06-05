/************************************************************************/
/*! \class RtAudio
    \brief Realtime audio i/o C++ class.

    Dummy RtAudio class just for compiling when 
    RtAudio is not enabled. 
*/
/************************************************************************/

#if !defined(__RTAUDIO_H)
#define __RTAUDIO_H



#include <map>
#include "common.h"


// RtAudio class declaration.


class RtAudio
{
public:

  RtAudio();
  ~RtAudio();



  void cancelStreamCallback(int streamId);

  int getDeviceCount(void);

  char * const getStreamBuffer(int streamId);

  void tickStream(int streamId);

  void closeStream(int streamId);


  void startStream(int streamId);


  void stopStream(int streamId);

  void abortStream(int streamId);

  int streamWillBlock(int streamId);


protected:

private:

};


#endif
