#include "backend.h"

void MarBackend::startPitchNet(string sfName) {
	mrs_natural lowPitch=36;
	mrs_natural highPitch=128;
	bool plopt=false;

// start cut and paste
  MarSystemManager mng;
  
  // Build the pitch extractor network 
  pitchExtractor = mng.create("Series", "pitchExtractor");
  // pitchExtractor->addMarSystem(mng.create("AudioSource", "src"));

  pitchExtractor->addMarSystem(mng.create("SoundFileSource", "src"));
  pitchExtractor->updctrl("SoundFileSource/src/mrs_string/filename", sfName);
  


  pitchExtractor->addMarSystem(mng.create("AutoCorrelation", "acr"));
  pitchExtractor->updctrl("AutoCorrelation/acr/mrs_real/magcompress", 0.67);
  pitchExtractor->addMarSystem(mng.create("HalfWaveRectifier", "hwr"));
  
  MarSystem* fanout = mng.create("Fanout", "fanout");
  fanout->addMarSystem(mng.create("Gain", "id1"));
  fanout->addMarSystem(mng.create("TimeStretch", "tsc"));
  
  pitchExtractor->addMarSystem(fanout);
  
  MarSystem* fanin = mng.create("Fanin", "fanin");
  fanin->addMarSystem(mng.create("Gain", "id2"));
  fanin->addMarSystem(mng.create("Negative", "nid"));
  
  pitchExtractor->addMarSystem(fanin);
  pitchExtractor->addMarSystem(mng.create("HalfWaveRectifier", "hwr"));
  pitchExtractor->addMarSystem(mng.create("Peaker", "pkr"));
  pitchExtractor->addMarSystem(mng.create("MaxArgMax", "mxr"));

  // update controls 
  pitchExtractor->updctrl("mrs_natural/inSamples", 512);
  pitchExtractor->updctrl("Fanout/fanout/TimeStretch/tsc/mrs_real/factor", 0.5);  
   // Convert pitch bounds to samples 
  //cout << "lowPitch = " << lowPitch << endl;
  //cout << "highPitch = " << highPitch << endl;
  
   mrs_real lowFreq = pitch2hertz(lowPitch);
   mrs_real highFreq = pitch2hertz(highPitch);
   mrs_natural lowSamples = 
     // hertz2samples(highFreq, pitchExtractor->getctrl("AudioSource/src/mrs_real/osrate")->toReal());
     hertz2samples(highFreq, pitchExtractor->getctrl("SoundFileSource/src/mrs_real/osrate")->toReal());
   mrs_natural highSamples = 
     //     hertz2samples(lowFreq, pitchExtractor->getctrl("AudioSource/src/mrs_real/osrate")->toReal());
     hertz2samples(lowFreq, pitchExtractor->getctrl("SoundFileSource/src/mrs_real/osrate")->toReal());
   pitchExtractor->updctrl("Peaker/pkr/mrs_real/peakSpacing", 0.00);
   pitchExtractor->updctrl("Peaker/pkr/mrs_real/peakStrength", 0.4);
   pitchExtractor->updctrl("Peaker/pkr/mrs_natural/peakStart", lowSamples);
   pitchExtractor->updctrl("Peaker/pkr/mrs_natural/peakEnd", highSamples);
   pitchExtractor->updctrl("MaxArgMax/mxr/mrs_natural/nMaximums", 1);
   
   
   
   //cout << (*pitchExtractor) << endl;
   
   realvec pitchres(pitchExtractor->getctrl("mrs_natural/onObservations")->toNatural(), pitchExtractor->getctrl("mrs_natural/onSamples")->toNatural());
   
  
  realvec win(pitchExtractor->getctrl("mrs_natural/inObservations")->toNatural(), 
	      pitchExtractor->getctrl("mrs_natural/inSamples")->toNatural());
  
  
  
  mrs_real pitch;
  //mrs_natural t;
  mrs_natural counter = 0;      
  //mrs_natural pos = 0;
  
  /// playback network 
  MarSystem* playback = mng.create("Series", "playback");
  playback->addMarSystem(mng.create("SineSource", "ss"));
  playback->addMarSystem(mng.create("AudioSink", "dest"));
  playback->updctrl("mrs_natural/inSamples", 512);
  playback->updctrl("mrs_bool/initAudio", true);
  
  
  
  counter = 0;
  
  // Main processing loop 
  // while (1)

  while (pitchExtractor->getctrl("SoundFileSource/src/mrs_bool/notEmpty")->to<mrs_bool>())
  {
    if (plopt) 
      playback->tick();
    
    pitchExtractor->process(win, pitchres);
      
      // pitch = samples2hertz((mrs_natural)pitchres(1), pitchExtractor->getctrl("AudioSource/src/mrs_real/osrate")->toReal());

      pitch = samples2hertz((mrs_natural)pitchres(1), pitchExtractor->getctrl("SoundFileSource/src/mrs_real/osrate")->toReal());
      
//      cout <<  pitchres(0) << endl;

//      cout << "pitch" << "---" << pitchres(0) << endl;
       cout << "midi" << "---" << hertz2pitch(pitch) << endl ;
      if (pitchres(0) > 0.05) 
	playback->updctrl("SineSource/ss/mrs_real/frequency", pitch);
      
      
      
      
      
      counter++;
    }
  
  
  
  
}

void MarBackend::stopPitchNet() {
	delete pitchExtractor;
}

