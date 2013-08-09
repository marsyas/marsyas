#include "MarSndPeek.h"

#define	DEFAULT_SPECTRUM_LINES		50

//--------------------------------------------------------------
MarSndPeek::MarSndPeek()
{
  inAudioFileName = ofToDataPath("bar.wav");
  targetRate = rate = sampleRate / bufferSize;
  spectrumRingBuffer = NULL;

  disableAllEvents();
  ofAddListener(ofEvents.setup, (ofxMSAInteractiveObject*)this, &ofxMSAInteractiveObject::_setup);
  ofAddListener(ofEvents.draw, (ofxMSAInteractiveObject*)this, &ofxMSAInteractiveObject::_draw);
}

//--------------------------------------------------------------
MarSndPeek::~MarSndPeek()
{
  destroy();
}

//--------------------------------------------------------------
void
MarSndPeek::allocate()
{
  waveformData.create(1,fftBins);

  if (spectrumRingBuffer != NULL)
  {
    delete[] spectrumRingBuffer;
    delete spectrumRingBuffer;
  }

  // Allocate space for the ring buffer used to draw the spectrum
  spectrumRingBuffer = new double*[spectrumLines];
  for (int i = 0; i < spectrumLines; i++)
  {
    spectrumRingBuffer[i] = new double[fftBins];
    for (int j = 0; j < fftBins; j++)
      spectrumRingBuffer[i][j] = 0.0;
  }
  ringBufferPos = 0;

}

//--------------------------------------------------------------
void
MarSndPeek::setup()
{
  powerSpectrumMode = powerSpectrumModePrev = 0;
  fftBinsPow2 = fftBinsPow2Prev = 7;
  fftBins = 2 << fftBinsPow2;

  targetPriority = 4;
  spectrumLines = spectrumLinesPrev = DEFAULT_SPECTRUM_LINES;

  //
  // Setup the MarSystem to play and analyze the data
  //

  // A Fanout to let us read audio from either a SoundFileSource or an
  // AudioSource

  inputfanout = mng.create("Fanout", "inputfanout");

  addMarSystem(inputfanout);

  inputfanout->addMarSystem(mng.create("SoundFileSource", "src"));
  inputfanout->addMarSystem(mng.create("AudioSource", "src"));

  addMarSystem(mng.create("Selector", "inputselector"));

  if (inAudioFileName == "")
  {
    updctrl("Selector/inputselector/mrs_natural/enable", 0);
    updctrl("Selector/inputselector/mrs_natural/enable", 1);
    cout << "input from AudioSource" << endl;
  } else {
    updctrl("Selector/inputselector/mrs_natural/enable", 1);
    updctrl("Selector/inputselector/mrs_natural/enable", 0);
    cout << "input from SoundFileSource" << endl;
  }

  // Add the AudioSink right after we've selected the input and
  // before we've calculated any features.  Nice trick.
  addMarSystem(mng.create("Stereo2Mono", "stereo2mono"));
  addMarSystem(mng.create("AudioSink", "dest"));

  addMarSystem(mng.create("Windowing", "ham"));
  addMarSystem(mng.create("Spectrum", "spk"));
  addMarSystem(mng.create("PowerSpectrum", "pspk"));
  addMarSystem(mng.create("Gain", "gain"));

  spectrumFeatures = mng.create("Fanout", "spectrumFeatures");
  spectrumFeatures->addMarSystem(mng.create("Centroid", "cntrd"));
  spectrumFeatures->addMarSystem(mng.create("Rolloff", "rlf"));
  spectrumFeatures->addMarSystem(mng.create("Flux", "flux"));
  spectrumFeatures->addMarSystem(mng.create("Rms", "rms"));
  addMarSystem(spectrumFeatures);

  // Set the controls of this MarSystem

  inputfanout->updctrl("SoundFileSource/src/mrs_real/repetitions",1.0);
  if (inAudioFileName == "")
  {
    updctrl("mrs_real/israte", 44100.0);
    inputfanout->updctrl("AudioSource/src/mrs_bool/initAudio", true);
  }
  else {
    inputfanout->updctrl("SoundFileSource/src/mrs_string/filename", inAudioFileName);
    updctrl("AudioSink/dest/mrs_bool/initAudio", true);
  }

  allocate();
  run();
}

//--------------------------------------------------------------
void
MarSndPeek::destroy()
{}

//--------------------------------------------------------------
void
MarSndPeek::update()
{
//	cout << "tick" << endl;
  using Marsyas::mrs_realvec;

  if (fftBinsPow2 != fftBinsPow2Prev)
  {
    cout << "Num. FFT bins changed." << endl;
    fftBins = 2 << fftBinsPow2;
    allocate();

    fftBinsPow2Prev = fftBinsPow2;
  }

  if (spectrumLines != spectrumLinesPrev)
  {
    cout << "Num. spectrum lines changed." << endl;
    allocate();
    spectrumLinesPrev = spectrumLines;
  }

  addDataToRingBuffer();

  // Set waveform data
  waveformData = getctrl("Windowing/ham/mrs_realvec/processedData")->to<mrs_realvec>();

  // Set audio stats
  mrs_realvec data = getctrl("mrs_realvec/processedData")->to<mrs_realvec>();

  stats.centroid	= data(0,0);
  stats.rolloff	= data(1,0);
  stats.flux		= data(2,0);
  stats.rms		= data(3,0);
}

//--------------------------------------------------------------
void
MarSndPeek::addDataToRingBuffer()
{
  using Marsyas::mrs_realvec;

  mrs_realvec data = getctrl("PowerSpectrum/pspk/mrs_realvec/processedData")->to<mrs_realvec>();

  for (int i = 0; i < fftBins; i++)
    spectrumRingBuffer[ringBufferPos][i] = data(i,0);

  ringBufferPos += 1;
  if (ringBufferPos >= spectrumLines)
    ringBufferPos = 0;
}

//--------------------------------------------------------------
void MarSndPeek::draw()
{}
