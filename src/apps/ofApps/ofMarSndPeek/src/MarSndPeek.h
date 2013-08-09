#pragma once

#include "ofxMarsyasNetwork.h"
#include "ofxMSAInteractiveObject.h"

struct AudioStats {
  float centroid;
  float rolloff;
  float flux;
  float rms;
};

class MarSndPeek : public ofxMarsyasNetwork, public ofxMSAInteractiveObject
{
public:
  MarSndPeek();
  ~MarSndPeek();

  void allocate();
  void setup();
  void destroy();

  void update();
  void draw();

  string inAudioFileName;

  // Windowed waveform for display
  Marsyas::mrs_realvec waveformData;
  AudioStats stats;

  int fftBins, fftBinsPow2;
  int spectrumLines;
  int powerSpectrumMode;

  double **spectrumRingBuffer;
  int ringBufferPos; // The current head position in the ring buffer

protected:
  void addDataToRingBuffer();

  Marsyas::MarSystem* spectrumFeatures;
  Marsyas::MarSystem* inputfanout;

  float sampleRate;
  int	bufferSize;

private:
  int fftBinsPow2Prev;
  int spectrumLinesPrev;
  int powerSpectrumModePrev;
};
