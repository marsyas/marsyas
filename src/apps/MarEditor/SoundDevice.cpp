#include "SoundDevice.h"
#include <QMessageBox>

SoundDevice::SoundDevice(QWidget* parent)
{
  parent_ = parent;
  init();
  defaultConfig();
}

SoundDevice::~SoundDevice()
{
  destroySoundDevice();
}

void
SoundDevice::init()
{
  device_ = NULL;
  api_ = RtAudio::UNSPECIFIED;
  deviceName_ = "default device";
  deviceID_ = 0;
  APIchanged_ = false;

  inChannels_ = 0;
  outChannels_ = 0;
  audioFormat_ = 0;
  sampleRate_ = 0;
  bufferSize_ = 0;
  nrBuffers_ = 0;
  callback_ = NULL;
  userData_ = NULL;

  state_ = NOT_READY;
}

bool
SoundDevice::recoverError()
{
  //if an error occurs, try to set the default configuration!

  // if this call comes from a previous unrecoverable error (i.e. a fatal error)
  // do not try to recover once again => avoid infinite cycle!
  if(state_ == FATAL_ERROR) return false;

  //try to put sound device to its default configuration.
  if(!defaultConfig())
  {
    //it's not possible to setup audio device using default settings,
    //throw a FATAL ERROR message and do not try to recover anymore!
    QMessageBox::warning(parent_,"FATAL Audio Device ERROR!", "It is not possible to recover from error...", QMessageBox::Ok,
                         QMessageBox::NoButton, QMessageBox::NoButton);
    state_ = FATAL_ERROR;
    return false;
  }

  return true;
}

bool
SoundDevice::defaultConfig()
{
  //default configuration
  api_ = RtAudio::WINDOWS_DS;
  deviceID_ = 0;
  deviceName_ = "AUTOMATIC RTAUDIO";
  bufferSize_ = 8192;
  nrBuffers_ = 3;
  outChannels_ = 2;
  inChannels_ = 0;
  sampleRate_ = 44100;
  audioFormat_ = RTAUDIO_SINT16;

  //try to create a new sound device with the default configuration
  return(createSoundDevice());
}

void
SoundDevice::destroySoundDevice()
{
  if(device_) //i.e. if(state_ != NOT_READY || state_ == FATAL_ERROR)
  {
    delete device_;
    device_ = NULL;
  }
  state_ = NOT_READY;
}

bool
SoundDevice::createSoundDevice()
{
  //do nothing if in a FATAL_ERROR or PLAYING state
  if(state_ == PLAYING || state_ == FATAL_ERROR) return false;

  //if a sound device already exists and the API was not changed, just update the current sound device
  if(state_ == READY && !APIchanged_) return(updateSoundDevice());

  int prevBufferSize = bufferSize_;
  int prevNrBuffers = nrBuffers_;

  //if a sound device exists, destroy it first...
  destroySoundDevice();

  //... create new sound device
  try {
    device_ = new RtAudio(deviceID_, outChannels_, deviceID_, inChannels_, audioFormat_,
                          sampleRate_, &bufferSize_, &nrBuffers_, api_);
  }
  catch (RtError &error) {
    QMessageBox::warning(parent_,"Error Creating Audio Device!", QString::fromStdString(error.getMessage()), QMessageBox::Ok,
                         QMessageBox::NoButton,QMessageBox::NoButton);
    device_ = NULL; //if RtAudio device creation fails, this pointer becomes invalid and should be set to NULL!
    recoverError();
    return false;
  }

  state_ = READY;
  APIchanged_ = false;

  //reattach any streamCallback already defined for the replaced audio device
  if(!setStreamCallback(callback_, userData_)) return false;

  //check if pretended buffer sizes and number were possible, and warn user if not...
  checkBufferConf(prevBufferSize, bufferSize_, prevNrBuffers, nrBuffers_);

  return true;
}

bool
SoundDevice::updateSoundDevice()
{
  //do nothing if in a FATAL_ERROR or PLAYING state
  if(state_ == PLAYING || state_ == FATAL_ERROR) return false;

  //if a sound device does not already exist or the API was changed, try to create one now
  if(state_ == NOT_READY || APIchanged_) return(createSoundDevice());

  int prevBufferSize = bufferSize_;
  int prevNrBuffers = nrBuffers_;

  try {
    device_->closeStream();
    device_->openStream(deviceID_, outChannels_, deviceID_, inChannels_, audioFormat_,
                        sampleRate_, &bufferSize_, &nrBuffers_);
  }
  catch (RtError &error) {
    QMessageBox::warning(parent_,"Error Updating Audio Device!", QString::fromStdString(error.getMessage()), QMessageBox::Ok,
                         QMessageBox::NoButton,QMessageBox::NoButton);
    recoverError();
    return false;
  }

  //reattach any stream callback already defined for the updated audio device
  if(!setStreamCallback(callback_, userData_)) return false;

  //check if pretended buffer sizes and number were possible, and warn user if not...
  checkBufferConf(prevBufferSize, bufferSize_, prevNrBuffers, nrBuffers_);

  return true;
}

void
SoundDevice::checkBufferConf(int prevBufferSize, int newBufferSize, int prevNrBuffers, int newNrBuffers)
{
  //check if pretended buffer sizes and number were possible, and warn user if not...
  if (prevBufferSize != newBufferSize || prevNrBuffers != newNrBuffers)
    QMessageBox::warning(parent_,"WARNING: Buffer Configuration",
                         "Buffer Size: " + QString::number(prevBufferSize) + " => " + QString::number(newBufferSize) +
                         "\n Number of Buffers: " + QString::number(prevNrBuffers) + " => " + QString::number(newNrBuffers),
                         QMessageBox::Ok, QMessageBox::NoButton,QMessageBox::NoButton);
}

void
SoundDevice::setSoundDevice(RtAudio::RtAudioApi api, int deviceID, QString deviceName)
{
  //update internal parameters...
  if (api != api_)
  {
    api_ = api;
    APIchanged_ = true;
  }
  deviceID_ = deviceID;
  deviceName_ = deviceName;

  /*
  // if a different device but using the same API, just update it...
  if (deviceID != deviceID_ && api == api_)
  {
  //update internal parameters...
  api_ = api;
  deviceID_ = deviceID;
  deviceName_ = deviceName;

  //update existing sound device
  if(!updateSoundDevice()) return false;
  }
  //if different API (and consequently different device), replace it...
  if(api != api_)
  {
  //update internal parameters...
  api_ = api;
  deviceID_ = deviceID;
  deviceName_ = deviceName;

  //re-create sound device using the new API parameters
  //if(!destroySoundDevice()) return false; //this is automaticcaly done inside createSoundDevice()
  if(!createSoundDevice()) return false;
  }

  return true;
  */
}

void
SoundDevice::setBuffers(int bufferSize, int nrBuffers)
{
  // update internal parameters
  // (in case no audio device exists, these parameters will be used when the audio device is created)
  bufferSize_ = bufferSize;
  nrBuffers_ = nrBuffers;

  //return updateSoundDevice();
}

void
SoundDevice::setSampleRate(int sampleRate)
{
  // update internal parameters
  //(in case no audio device exists, these parameters will be used when the audio device is created)
  sampleRate_ = sampleRate;

  //return updateSoundDevice();
}

void
SoundDevice::setAudioFormat(RtAudioFormat audioFormat)
{
  //update internal parameters
  //(in case no audio device exists, these parameters will be used when the audio device is created)
  audioFormat_ = audioFormat;

  //return updateSoundDevice();
}

void
SoundDevice::setBits(int bits)
{
  unsigned long audioFormat;

  switch(bits) {
  case 8:
    audioFormat = RTAUDIO_SINT8;
    break;
  case 16:
    audioFormat = RTAUDIO_SINT16;
    break;
    /*case 24:
    audioFormat = RTAUDIO_SINT24;
    break;*/
  default:
    QMessageBox::warning(parent_,"Error configuring Audio Device sample format!", "Unsupported sample size format", QMessageBox::Ok,
                         QMessageBox::NoButton,QMessageBox::NoButton);
    //return false;
  }

  //return setAudioFormat(audioFormat);
}

void
SoundDevice::setInChannels(int channels)
{
  //update internal parameters
  //(in case no audio device exists, these parameters will be used when the audio device is created)
  inChannels_ = channels;

  //return updateSoundDevice();
}

void
SoundDevice::setOutChannels(int channels)
{
  //update internal parameters
  //(in case no audio device exists, these parameters will be used when the audio device is created)
  outChannels_ = channels;

  //return updateSoundDevice();
}

bool
SoundDevice::setStreamCallback(RtAudioCallback callback, void* userData)
{
  //if no callback is defined, just update internal parameters...
  if(!callback)
  {
    callback_ = NULL;
    userData_ = NULL;
    return true;
  }

  //if a callback was already setup, cancel it first...
  if(state_ == STOPPED)
    //this makes status_ = READY
    if(!cancelStreamCallback()) return false;

  //if an audio device exists but with no configured callback, configure callback
  if(state_ == READY)
  {
    try {
      // Set the stream callback function
      device_->setStreamCallback(callback, userData);
    }
    catch (RtError &error) {
      QMessageBox::warning(parent_,"Error setting up Audio Device callback!", QString::fromStdString(error.getMessage()), QMessageBox::Ok,
                           QMessageBox::NoButton, QMessageBox::NoButton);
      recoverError();
      return false;
    }
    //update internal parameters...
    callback_ = callback;
    userData_ = userData;
    state_ = STOPPED;
    return true;
  }

  //if we get here, then something went wrong... => return false!
  return false;
}

bool
SoundDevice::cancelStreamCallback()
{
  if(state_ == STOPPED)
  {
    try {
      device_->cancelStreamCallback();
    }
    catch (RtError &error) {
      QMessageBox::warning(parent_,"Error canceling Audio Device callback!", QString::fromStdString(error.getMessage()), QMessageBox::Ok,
                           QMessageBox::NoButton, QMessageBox::NoButton);
      //recoverError();
      return false;
    }
    state_ = READY;
    return true;
  }
  else //i.e., if in any other state...
    return false; //... can only cancel callback if in STOPPED state
}

bool
SoundDevice::startStream()
{
  if(state_ == STOPPED)
  {
    try {
      device_->startStream();
    }
    catch (RtError &error) {
      QMessageBox::warning(parent_,"Error Starting Audio Stream!", QString::fromStdString(error.getMessage()), QMessageBox::Ok,
                           QMessageBox::NoButton,QMessageBox::NoButton);
      recoverError();
      return false;
    }
    state_ = PLAYING;
    return true;
  }
  else
  {
    // 		QMessageBox::warning(parent_,"Not possible to start Audio Stream!", "Audio Device does not exist!", QMessageBox::Ok,
    // 			QMessageBox::NoButton,QMessageBox::NoButton);
    return false;
  }
}

bool
SoundDevice::stopStream()
{
  if(state_ == PLAYING)
  {
    try {
      device_->stopStream();
    }
    catch (RtError &error) {
      QMessageBox::warning(parent_,"Error Stoping Audio Stream!", QString::fromStdString(error.getMessage()), QMessageBox::Ok,
                           QMessageBox::NoButton,QMessageBox::NoButton);
      recoverError();
      return false;
    }
    state_ = STOPPED;
    return true;
  }
  else
  {
    // 		QMessageBox::warning(parent_,"Not possible to stop Audio Stream!", "Audio Device does not exist!", QMessageBox::Ok,
    // 			QMessageBox::NoButton,QMessageBox::NoButton);
    return false;
  }
}
bool
SoundDevice::abortStream()
{
  if(state_ == PLAYING)
  {
    try {
      device_->abortStream();
    }
    catch (RtError &error) {
      QMessageBox::warning(parent_,"Error aborting Audio Stream!", QString::fromStdString(error.getMessage()), QMessageBox::Ok,
                           QMessageBox::NoButton,QMessageBox::NoButton);
      recoverError();
      return false;
    }
    state_ = STOPPED;
    return true;
  }
  else
  {
    // 		QMessageBox::warning(parent_,"Not possible to abort Audio Stream!", "Audio Device does not exist!", QMessageBox::Ok,
    // 			QMessageBox::NoButton,QMessageBox::NoButton);
    return false;
  }
}

int
SoundDevice::bits()
{
  switch(audioFormat_) {
  case RTAUDIO_SINT8:
    return 8;
  case RTAUDIO_SINT16:
    return 16;
    /*case RTAUDIO_SINT24:
    return 24;*/
  default:
    QMessageBox::warning(parent_,"Error in Audio Device Configuration!", "Unsupported sample size format configured...", QMessageBox::Ok,
                         QMessageBox::NoButton,QMessageBox::NoButton);
    return 0;
  }
}

