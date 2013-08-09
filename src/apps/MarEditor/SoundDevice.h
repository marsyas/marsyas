#ifndef SOUNDDEVICE_H
#define SOUNDDEVICE_H

#include <QWidget>
#include <QString>
#include "RtAudio.h"

class SoundDevice
{
private:
  QWidget* parent_;
  RtAudio* device_;
  RtAudio::RtAudioApi api_;
  bool APIchanged_;

  QString deviceName_;
  int deviceID_;
  int inChannels_;
  int outChannels_;
  RtAudioFormat audioFormat_;
  int sampleRate_;
  int bufferSize_; //buffer size in samples
  int nrBuffers_;

  RtAudioCallback callback_;
  void* userData_;

  enum SoundDeviceState
  {
    NOT_READY,
    READY,
    STOPPED,
    PLAYING,
    FATAL_ERROR
  } state_;

  void init();
  bool defaultConfig();

  bool recoverError();

  void checkBufferConf(int prevBufferSize, int newBufferSize, int prevNrBuffers, int newNrBuffers);

public:
  SoundDevice(QWidget* parent = 0);
  ~SoundDevice();

  bool createSoundDevice();
  bool updateSoundDevice();
  void destroySoundDevice();

  void setSoundDevice(RtAudio::RtAudioApi api, int deviceID, QString deviceName);

  void setBuffers(int bufferSize, int nrBuffers);
  void setSampleRate(int sampleRate);
  void setAudioFormat(RtAudioFormat audioFormat);
  void setBits(int bits);
  void setInChannels(int channels);
  void setOutChannels(int channels);

  bool setStreamCallback(RtAudioCallback callback, void* userData);
  bool cancelStreamCallback();

  RtAudio::RtAudioApi API() const {return api_;};
  QString deviceName() const {return deviceName_;};
  int deviceID() const {return deviceID_;};
  int inChannels() const {return inChannels_;};
  int outChannels() const {return outChannels_;};
  RtAudioFormat audioFormat() const {return audioFormat_;};
  int bits();
  int sampleRate() {return sampleRate_;};
  int bufferSize() const {return bufferSize_;};
  int nrBuffers() const {return nrBuffers_;};

  SoundDeviceState state() const {return state_;};

  bool startStream();
  bool stopStream();
  bool abortStream();
};


#endif // SOUNDDEVICE_H
