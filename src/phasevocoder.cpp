#include <cstdio>

#ifndef WIN32
#include <config.h>
#else 
#include <win_config.h> 
#endif

#ifdef MRSMIDI
#include "RtMidi.h"
#else

#endif
#include "MarSystemManager.h"
#include "Messager.h"
#include "Conversions.h"
#include "CommandLineOptions.h"
#include "AudioSink.h"
#include "SoundFileSink.h"

#include <string>

using namespace std;
using namespace Marsyas;

#define EMPTYSTRING "MARSYAS_EMPTY"
string fileName = EMPTYSTRING;
string pluginName = EMPTYSTRING;

// Global variables for command-line options 
bool helpopt_ = 0;
bool usageopt_ =0;
int fftSize_ = 512;
int winSize_ = 512;
int dopt = 64;
int iopt = 64;
int sopt = 80;
int bopt = 128;
int vopt_ = 1;
mrs_real gopt_ = 1.0;
mrs_natural eopt_ = 0;

mrs_real popt = 1.0;
bool auto_ = false;
mrs_natural midi_ = -1;
bool microphone_ = false;

CommandLineOptions cmd_options;

void 
printUsage(string progName)
{
  MRSDIAG("phasevocoder.cpp - printUsage");
  cerr << "Usage : " << progName << " [file]" << endl;
  cerr << endl;
  cerr << "If no filename is given the default live audio input is used. " << endl;
}

void 
printHelp(string progName)
{
  MRSDIAG("phasevocoder.cpp - printHelp");
  cerr << "phasevocoder, MARSYAS, Copyright George Tzanetakis " << endl;
  cerr << "--------------------------------------------" << endl;
  cerr << "Usage : " << progName << " [file]" << endl;
  cerr << endl;
  cerr << "if no filename is given the default live audio input is used. " << endl;
  cerr << "Options:" << endl;
  cerr << "-n --fftsize         : size of fft " << endl;
  cerr << "-w --winsize         : size of window " << endl;
  cerr << "-d --decimation      : decimation size " << endl;
  cerr << "-i --interpolatation : interpolation size" << endl;
  cerr << "-p --pitchshift      : pitch shift" << endl;
  cerr << "-s --sinusoids       : number of sinusoids" << endl;
  cerr << "-v --voices          : number of voices" << endl;
  cerr << "-m --midiport        : midi input port number" << endl;
  cerr << "-b --buffersize      : audio buffer size" << endl;
  cerr << "-g --gain            : gain (0.0-1.0) " << endl;
  cerr << "-f --filename        : output filename" << endl;
  cerr << "-u --usage           : display short usage info" << endl;
  cerr << "-h --help            : display this information " << endl;
  
  exit(1);
}

// original monophonic phasevocoder 
void 
phasevocSeries(string sfName, mrs_natural N, mrs_natural Nw, 
		    mrs_natural D, mrs_natural I, mrs_real P, 
		    string outsfname)
{
  cout << "phasevocSeries" << endl;
  MarSystemManager mng;
  
  // create the phasevocoder network
  MarSystem* pvseries = mng.create("Series", "pvseries");
  
  if (microphone_) 
    pvseries->addMarSystem(mng.create("AudioSource", "src"));
  else 
    pvseries->addMarSystem(mng.create("SoundFileSource", "src"));
    
  pvseries->addMarSystem(mng.create("ShiftInput", "si"));
  pvseries->addMarSystem(mng.create("PvFold", "fo"));
  pvseries->addMarSystem(mng.create("Spectrum", "spk"));
  pvseries->addMarSystem(mng.create("PvConvert", "conv"));
  pvseries->addMarSystem(mng.create("PvOscBank", "ob"));
  pvseries->addMarSystem(mng.create("ShiftOutput", "so"));
  pvseries->addMarSystem(mng.create("Gain", "gain"));
  
	MarSystem *dest;
	if (outsfname == EMPTYSTRING) 
		dest = new AudioSink("dest");
	else
	{
		dest = new SoundFileSink("dest");
		//dest->updctrl("mrs_string/filename", outsfname);
	}
  pvseries->addMarSystem(dest);

	if (outsfname == EMPTYSTRING) 
		pvseries->updctrl("AudioSink/dest/mrs_natural/bufferSize", bopt);

	// update the controls
	if (microphone_) 
	{
		pvseries->updctrl("AudioSource/src/mrs_natural/inSamples", D);
		pvseries->updctrl("AudioSource/src/mrs_natural/inObservations", 1);
	}
	else
	{
		pvseries->updctrl("SoundFileSource/src/mrs_string/filename", sfName);
		pvseries->updctrl("SoundFileSource/src/mrs_natural/inSamples", D);
		pvseries->updctrl("SoundFileSource/src/mrs_natural/inObservations", 1);

		// if audio output loop to infinity and beyond 
		if (outsfname == EMPTYSTRING) 
			pvseries->updctrl("SoundFileSource/src/mrs_real/repetitions", -1.0);
	}
  
	pvseries->updctrl("ShiftInput/si/mrs_natural/Decimation", D);
  pvseries->updctrl("ShiftInput/si/mrs_natural/WindowSize", Nw);
  pvseries->updctrl("PvFold/fo/mrs_natural/FFTSize", N);
  pvseries->updctrl("PvFold/fo/mrs_natural/WindowSize", Nw);
  pvseries->updctrl("PvFold/fo/mrs_natural/Decimation", D);
  pvseries->updctrl("PvConvert/conv/mrs_natural/Decimation",D);      
  pvseries->updctrl("PvConvert/conv/mrs_natural/Sinusoids", (mrs_natural) sopt);  
  pvseries->updctrl("PvOscBank/ob/mrs_natural/Interpolation", I);
  pvseries->updctrl("PvOscBank/ob/mrs_real/PitchShift", P);
  pvseries->updctrl("ShiftOutput/so/mrs_natural/Interpolation", I);
  pvseries->updctrl("ShiftOutput/so/mrs_natural/WindowSize", Nw);      
  pvseries->updctrl("ShiftOutput/so/mrs_natural/Decimation", D);
  pvseries->updctrl("Gain/gain/mrs_real/gain", gopt_);


  cout << *pvseries << endl;

  if (outsfname == EMPTYSTRING) 
		pvseries->updctrl("AudioSink/dest/mrs_bool/initAudio", true);

	int type;
	int byte2, byte3;
	double stamp;
	mrs_real diff;

#ifdef MRSMIDI
	RtMidiIn *midiin = NULL;

	// open midi if midiPort is specified 
	if (midi_ != -1) 
	{
		try {
			midiin = new RtMidiIn();
		}
		catch (RtError &error) {
			error.printMessage();
			exit(1);
		}
		try {
			midiin->openPort(midi_);
		}
		catch (RtError &error) {
			error.printMessage();
			exit(1);

		}
	}
#endif  

	// midi message 
	std::vector<unsigned char> message;
	int nBytes;

	if (outsfname != EMPTYSTRING)
		dest->updctrl("mrs_string/filename", outsfname);

	while(1)
	{
#ifdef MRSMIDI
		if (midi_ != -1) 
		{
			stamp = midiin->getMessage( &message );
			nBytes = message.size();
			if (nBytes >2)
			{
				byte3 = message[2];
				byte2 = message[1];
				type = message[0];

				if ((byte3 != 0)&&(type == 144))
				{
					diff = byte2 - 60.0;
					pvseries->updctrl("PvOscBank/ob/mrs_real/PitchShift", pow((double)1.06, (double)diff));	  	      
					mrs_real gain = byte3 * 1.0 / 128.0;
					pvseries->updctrl("Gain/gain/mrs_real/gain", gain);
				}
				if (type == 224) 
				{
					mrs_real pitchShift = byte3 * 1.0/ 64.0;
					pvseries->updctrl("PvOscBank/ob/mrs_real/PitchShift", pitchShift);  
				}
				if ((type == 176)&&(byte2 == 100)) 
				{
					pvseries->updctrl("PvConvert/conv/mrs_natural/Sinusoids", byte3);
				}
				if ((type == 176) && (byte2 == 101)&& (byte3 > 4))
				{
					pvseries->updctrl("SoundFileSource/src/mrs_natural/inSamples", byte3);
					pvseries->updctrl("ShiftInput/si/mrs_natural/Decimation", byte3); 
					pvseries->updctrl("PvFold/fo/mrs_natural/Decimation", byte3);
					pvseries->updctrl("PvConvert/conv/mrs_natural/Decimation", byte3 );
					pvseries->updctrl("ShiftOutput/so/mrs_natural/Decimation", byte3 );
				} 
			}
		}
#endif //MRSMIDI

		pvseries->tick();

		if (!microphone_) 
			if (pvseries->getctrl("SoundFileSource/src/mrs_bool/notEmpty")->toBool() == false)
				break;
	}
}

void 
phasevocPoly(string sfName, mrs_natural N, mrs_natural Nw, 
						 mrs_natural D, mrs_natural I, mrs_real P, 
						 string outsfname)
{
	cout << "Phasevocoder::polyphonic" << endl;

	MarSystemManager mng;

	MarSystem* total = mng.create("Series", "total");
	MarSystem* mixer = mng.create("Fanout", "mixer");

	// Use the prototype for the Composite Phasevocoder
	// Check phasevocSeries for the components 
	// or look at the MarSystemManager code 

	// vector of phasevocoders 
	vector<MarSystem*> pvoices;
	for (int i=0; i < vopt_; i++)
	{
		ostringstream oss;
		oss << "pvseries" << i;
		pvoices.push_back(mng.create("PhaseVocoder", oss.str()));

		// one-to-many mapping that under the hood 
		// utilizes linkctrl when the phasevocoder 
		// prototype is added to the MarSystemManager
		pvoices[i]->updctrl("mrs_natural/Decimation", D);
		pvoices[i]->updctrl("mrs_natural/WindowSize", Nw);
		pvoices[i]->updctrl("mrs_natural/FFTSize", N);
		pvoices[i]->updctrl("mrs_natural/Interpolation", I);
		pvoices[i]->updctrl("mrs_real/PitchShift", P);
		pvoices[i]->updctrl("mrs_natural/Sinusoids", sopt);
		pvoices[i]->updctrl("mrs_real/gain", gopt_);
		mixer->addMarSystem(pvoices[i]);
	}

	if (microphone_) 
		total->addMarSystem(mng.create("AudioSource", "src"));
	else
		total->addMarSystem(mng.create("SoundFileSource", "src"));

	total->addMarSystem(mixer);
	total->addMarSystem(mng.create("Sum", "sum"));

	if (outsfname == EMPTYSTRING) 
	{
		total->addMarSystem(mng.create("AudioSink", "dest"));
		total->updctrl("AudioSink/dest/mrs_natural/bufferSize", bopt);
	}
	else
	{
		total->addMarSystem(mng.create("SoundFileSink","dest"));
		total->updctrl("SoundFileSink/dest/mrs_string/filename", outsfname);
	}

	if (microphone_) 
	{
		total->updctrl("AudioSource/src/mrs_natural/inSamples", D);
		total->updctrl("AudioSource/src/mrs_natural/inObservations", 1);
	}
	else
	{
		total->updctrl("SoundFileSource/src/mrs_string/filename", sfName);
		total->updctrl("SoundFileSource/src/mrs_natural/inSamples", D);
		total->updctrl("SoundFileSource/src/mrs_natural/inObservations", 1);

		// if audio output loop to infinity and beyond 
		if (outsfname == EMPTYSTRING) 
			total->updctrl("SoundFileSource/src/mrs_real/repetitions", -1.0);
	}
	
	int type;
	string cname;

#ifdef MRSMIDI
	RtMidiIn *midiin = NULL;
#endif

	int byte2, byte3;
	double stamp;
	mrs_real diff;

	// used to keep track of polyphony
	vector<int> voices;
	for (int i=0; i < vopt_; i++) 
	{
		voices.push_back(60);
	}

#ifdef MRSMIDI  
	if (midi_ != -1) 
	{
		try {
			midiin = new RtMidiIn();
		}
		catch (RtError &error) {
			error.printMessage();
			exit(1);
		}
		try {
			midiin->openPort(midi_);
		}
		catch (RtError &error) {
			error.printMessage();
			exit(1);
		}
	}
#endif

	std::vector<unsigned char> message;
	int nBytes;
	int voiceCount =0;

	while(1)
	{
		if (!auto_) 
		{
			if (midi_ != -1) 
			{
#ifdef MRSMIDI
				stamp = midiin->getMessage( &message );
#endif
				nBytes = message.size();
				if (nBytes >2)
				{
					byte3 = message[2];
					byte2 = message[1];
					type = message[0];

					if (byte3 != 0) 
					{
						voices[voiceCount] = byte2;
						voiceCount = (voiceCount + 1) % vopt_;

						for (int i=0; i < vopt_; i++)
						{
							diff = voices[i] - 60.0;
							if (voices[i] != 0)
								pvoices[i]->updctrl("mrs_real/PitchShift", 
								pow((double)1.06, (double)diff));	  	      
							// cout << pow((double) 1.06, (double) diff) << endl;
						}
					}
				}
			}
		}
		total->tick();
	} 
}

void 
phasevocCrossSynth(string sfName, mrs_natural N, mrs_natural Nw, 
		 mrs_natural D, mrs_natural I, mrs_real P, 
		 string outsfname)
{
	cout << "PHASE-VOCODER CROSS-SYNTHESIS MAG/PHASE" << endl;
	MarSystemManager mng;

	// build cross-synthesis network 
	MarSystem* total = mng.create("Series", "total");
	MarSystem* pvfan = mng.create("Fanout", "pvfan");

	MarSystem* branch1 = mng.create("Series", "branch1");
	branch1->addMarSystem(mng.create("SoundFileSource","src1"));
	branch1->addMarSystem(mng.create("ShiftInput", "si1"));
	branch1->addMarSystem(mng.create("PvFold", "fo1"));
	branch1->addMarSystem(mng.create("Spectrum", "spk1"));
	branch1->addMarSystem(mng.create("Gain", "gain1"));
	branch1->updctrl("Gain/gain/mrs_real/gain1", 1.0);

	MarSystem* branch2 = mng.create("Series", "branch2");
	branch2->addMarSystem(mng.create("SoundFileSource","src2"));
	branch2->addMarSystem(mng.create("ShiftInput", "si2"));
	branch2->addMarSystem(mng.create("PvFold", "fo2"));
	branch2->addMarSystem(mng.create("Spectrum", "spk2"));
	branch1->addMarSystem(mng.create("Gain", "gain2"));
	branch1->updctrl("Gain/gain/mrs_real/gain2", 1.0);

	pvfan->addMarSystem(branch1);
	pvfan->addMarSystem(branch2);

	total->addMarSystem(pvfan);
	total->addMarSystem(mng.create("PvConvolve", "pvconv"));
	total->addMarSystem(mng.create("PvConvert",  "conv"));
	total->addMarSystem(mng.create("PvOscBank",  "ob"));
	total->addMarSystem(mng.create("ShiftOutput", "so"));
	total->addMarSystem(mng.create("SoundFileSink", "sdest"));
	total->addMarSystem(mng.create("Gain", "destgain"));
	total->addMarSystem(mng.create("AudioSink", "dest"));

	// link controls
	total->linkctrl("mrs_string/filename1", 
		"Fanout/pvfan/Series/branch1/SoundFileSource/src1/mrs_string/filename");

	total->linkctrl("mrs_string/filename2", 
		"Fanout/pvfan/Series/branch2/SoundFileSource/src2/mrs_string/filename");

	total->linkctrl("mrs_real/repetitions", 
		"Fanout/pvfan/Series/branch1/SoundFileSource/src1/mrs_real/repetitions");

	total->linkctrl("mrs_real/repetitions", 
		"Fanout/pvfan/Series/branch2/SoundFileSource/src2/mrs_real/repetitions");

	total->linkctrl("mrs_natural/inSamples1", 
		"Fanout/pvfan/Series/branch1/SoundFileSource/src1/mrs_natural/inSamples");

	total->linkctrl("mrs_natural/inSamples2", 
		"Fanout/pvfan/Series/branch2/SoundFileSource/src2/mrs_natural/inSamples");

	total->linkctrl("mrs_natural/Decimation", 
		"Fanout/pvfan/Series/branch1/ShiftInput/si1/mrs_natural/Decimation");
	total->linkctrl("mrs_natural/Decimation", 
		"Fanout/pvfan/Series/branch1/PvFold/fo1/mrs_natural/Decimation");

	total->linkctrl("mrs_natural/Decimation", 
		"Fanout/pvfan/Series/branch2/ShiftInput/si2/mrs_natural/Decimation");
	total->linkctrl("mrs_natural/Decimation", 
		"Fanout/pvfan/Series/branch2/PvFold/fo2/mrs_natural/Decimation");

	total->linkctrl("mrs_natural/Decimation", 
		"PvConvert/conv/mrs_natural/Decimation");
	total->linkctrl("mrs_natural/Decimation", 
		"ShiftOutput/so/mrs_natural/Decimation");
	total->linkctrl("mrs_natural/Sinusoids",
		"PvConvert/conv/mrs_natural/Sinusoids");

	total->linkctrl("mrs_natural/FFTSize", 
		"Fanout/pvfan/Series/branch1/PvFold/fo1/mrs_natural/FFTSize");
	total->linkctrl("mrs_natural/FFTSize", 
		"Fanout/pvfan/Series/branch2/PvFold/fo2/mrs_natural/FFTSize");  

	total->linkctrl("mrs_natural/WindowSize", 
		"Fanout/pvfan/Series/branch1/PvFold/fo1/mrs_natural/WindowSize");
	total->linkctrl("mrs_natural/WindowSize", 
		"Fanout/pvfan/Series/branch2/PvFold/fo2/mrs_natural/WindowSize");  

	total->linkctrl("mrs_natural/WindowSize",
		"ShiftOutput/so/mrs_natural/WindowSize");  

	total->linkctrl("mrs_natural/Interpolation",
		"ShiftOutput/so/mrs_natural/Interpolation");  

	total->linkctrl("mrs_natural/Interpolation",
		"PvOscBank/ob/mrs_natural/Interpolation");  

	total->linkctrl("mrs_real/PitchShift", 
		"PvOscBank/ob/mrs_real/PitchShift");

	// update controls
	total->updctrl("mrs_string/filename1", "/home/gtzan/data/sound/Nov2005Concert/ele1.wav");
	total->updctrl("mrs_string/filename2", "/home/gtzan/data/sound/music_speech/music/gravity.au");
	total->updctrl("mrs_real/repetitions", -1.0);
	total->updctrl("mrs_natural/inSamples1", D);
	total->updctrl("mrs_natural/inSamples2", D);
	total->updctrl("mrs_natural/Decimation", D);
	total->updctrl("mrs_natural/WindowSize", Nw);
	total->updctrl("mrs_natural/FFTSize", N);
	total->updctrl("mrs_natural/Interpolation", I);
	total->updctrl("mrs_real/PitchShift", P);
	total->updctrl("mrs_natural/Sinusoids", sopt);
	total->updctrl("SoundFileSink/sdest/mrs_string/filename", "cross.wav");
	total->updctrl("Gain/destgain/mrs_real/gain", gopt_);

	while(1)
	{
		total->tick();
	}

}

void 
phasevocConvolve(string sfName, mrs_natural N, mrs_natural Nw, 
								 mrs_natural D, mrs_natural I, mrs_real P, 
								 string outsfname)
{
	cout << "PHASE-VOCODER CROSS-SYNTHESIS WITH CONVOLUTION" << endl;
	MarSystemManager mng;

	// build cross-synthesis network 
	MarSystem* total = mng.create("Series", "total");
	MarSystem* pvfan = mng.create("Fanout", "pvfan");

	MarSystem* branch1 = mng.create("Series", "branch1");
	branch1->addMarSystem(mng.create("SoundFileSource","src1"));
	branch1->addMarSystem(mng.create("ShiftInput", "si1"));
	branch1->addMarSystem(mng.create("PvFold", "fo1"));
	branch1->addMarSystem(mng.create("Spectrum", "spk1"));
	branch1->addMarSystem(mng.create("Gain", "gain"));
	branch1->updctrl("Gain/gain/mrs_real/gain", 0.25);


	MarSystem* branch2 = mng.create("Series", "branch2");
	// branch2->addMarSystem(mng.create("SoundFileSource","src2"));
	branch2->addMarSystem(mng.create("AudioSource","src2"));

	branch2->addMarSystem(mng.create("ShiftInput", "si2"));
	branch2->addMarSystem(mng.create("PvFold", "fo2"));
	branch2->addMarSystem(mng.create("Spectrum", "spk2"));

	pvfan->addMarSystem(branch1);
	pvfan->addMarSystem(branch2);

	total->addMarSystem(pvfan);
	total->addMarSystem(mng.create("PvConvolve", "pvconv"));
	total->addMarSystem(mng.create("PvConvert",  "conv"));
	total->addMarSystem(mng.create("PvOscBank",  "ob"));
	total->addMarSystem(mng.create("ShiftOutput", "so"));
	total->addMarSystem(mng.create("SoundFileSink", "sdest"));
	total->addMarSystem(mng.create("Gain", "destgain"));
	total->addMarSystem(mng.create("AudioSink", "dest"));

	// link controls
	total->linkctrl("mrs_string/filename1", 
		"Fanout/pvfan/Series/branch1/SoundFileSource/src1/mrs_string/filename");

	// total->linkctrl("mrs_string/filename2", 
	// "Fanout/pvfan/Series/branch2/SoundFileSource/src2/mrs_string/filename");

	total->linkctrl("mrs_real/repetitions", 
		"Fanout/pvfan/Series/branch1/SoundFileSource/src1/mrs_real/repetitions");

	// total->linkctrl("mrs_real/repetitions", 
	// "Fanout/pvfan/Series/branch2/SoundFileSource/src2/mrs_real/repetitions");

	total->linkctrl("mrs_natural/inSamples1", 
		"Fanout/pvfan/Series/branch1/SoundFileSource/src1/mrs_natural/inSamples");

	// total->linkctrl("mrs_natural/inSamples2", 
	// "Fanout/pvfan/Series/branch2/SoundFileSource/src2/mrs_natural/inSamples");

	total->linkctrl("mrs_natural/inSamples2", 
		"Fanout/pvfan/Series/branch2/AudioSource/src2/mrs_natural/inSamples");

	total->linkctrl("mrs_natural/Decimation", 
		"Fanout/pvfan/Series/branch1/ShiftInput/si1/mrs_natural/Decimation");
	
	total->linkctrl("mrs_natural/Decimation", 
		"Fanout/pvfan/Series/branch1/PvFold/fo1/mrs_natural/Decimation");

	total->linkctrl("mrs_natural/Decimation", 
		"Fanout/pvfan/Series/branch2/ShiftInput/si2/mrs_natural/Decimation");

	total->linkctrl("mrs_natural/Decimation", 
		"Fanout/pvfan/Series/branch2/PvFold/fo2/mrs_natural/Decimation");

	total->linkctrl("mrs_natural/Decimation", 
		"PvConvert/conv/mrs_natural/Decimation");

	total->linkctrl("mrs_natural/Decimation", 
		"ShiftOutput/so/mrs_natural/Decimation");

	total->linkctrl("mrs_natural/Sinusoids",
		"PvConvert/conv/mrs_natural/Sinusoids");

	total->linkctrl("mrs_natural/FFTSize", 
		"Fanout/pvfan/Series/branch1/PvFold/fo1/mrs_natural/FFTSize");

	total->linkctrl("mrs_natural/FFTSize", 
		"Fanout/pvfan/Series/branch2/PvFold/fo2/mrs_natural/FFTSize");  

	total->linkctrl("mrs_natural/WindowSize", 
		"Fanout/pvfan/Series/branch1/PvFold/fo1/mrs_natural/WindowSize");

	total->linkctrl("mrs_natural/WindowSize", 
		"Fanout/pvfan/Series/branch2/PvFold/fo2/mrs_natural/WindowSize");  

	total->linkctrl("mrs_natural/WindowSize",
		"ShiftOutput/so/mrs_natural/WindowSize");  

	total->linkctrl("mrs_natural/Interpolation",
		"ShiftOutput/so/mrs_natural/Interpolation");  

	total->linkctrl("mrs_natural/Interpolation",
		"PvOscBank/ob/mrs_natural/Interpolation");  

	total->linkctrl("mrs_real/PitchShift", 
		"PvOscBank/ob/mrs_real/PitchShift");

	// update controls
	total->updctrl("mrs_string/filename1", "/home/gtzan/data/sound/Nov2005Concert/ele1.wav");
	// total->updctrl("mrs_string/filename2", "/home/gtzan/data/sound/music_speech/music/gravity.au");
	total->updctrl("mrs_real/repetitions", -1.0);
	total->updctrl("mrs_natural/inSamples1", D);
	total->updctrl("mrs_natural/inSamples2", D);
	total->updctrl("mrs_natural/Decimation", D);
	total->updctrl("mrs_natural/WindowSize", Nw);
	total->updctrl("mrs_natural/FFTSize", N);
	total->updctrl("mrs_natural/Interpolation", I);
	total->updctrl("mrs_real/PitchShift", P);
	total->updctrl("mrs_natural/Sinusoids", sopt);
	total->updctrl("SoundFileSink/sdest/mrs_string/filename", "cross.wav");
	total->updctrl("Gain/destgain/mrs_real/gain", gopt_);

	while(1)
	{
		total->tick();
	}
}

// phasevocoder variant for Jan 2006 event 
// at Royal BC Museum with Andy playing the radio drum

void 
phasevocHeterophonicsRadioDrum(string sfName1, string sfName2, mrs_natural N, 
			       mrs_natural Nw, 
			       mrs_natural D, mrs_natural I, mrs_real P, 
			       string outsfname)
{

	cout << "HETEROPHONICS - RADIODRUM " << endl;

	vopt_ = 2;
	cout << "sfName1 = " << sfName1 << endl;
	cout << "sfName2 = " << sfName2 << endl;

	MarSystemManager mng;
	MarSystem* total = mng.create("Series", "total");

	MarSystem* mic;
	mic = mng.create("SoundFileSource", "mic");
	// total->addMarSystem(mic);
	// total->updctrl("SoundFileSource/mic/mrs_string/filename", sfName);

	MarSystem* mixer = mng.create("Fanout", "mixer");

	MarSystem *bpvoc0 = mng.create("PhaseVocoder", "bpvoc0");  
	MarSystem* bpvoc1 = mng.create("PhaseVocoder", "bpvoc1");

	// vector of voices
	vector<MarSystem*> pvoices;
	for (int i=0; i < vopt_; i++)
	{
		if (i == 0) 
		{
			pvoices.push_back(mng.create("Series", "background0"));
			pvoices[i]->addMarSystem(mng.create("SoundFileSource", "mixsrc"));
			pvoices[i]->updctrl("SoundFileSource/mixsrc/mrs_string/filename", sfName1);
			pvoices[i]->updctrl("SoundFileSource/mixsrc/mrs_real/repetitions", -1.0);

			pvoices[i]->addMarSystem(mng.create("Gain", "bgain0"));	  
			pvoices[i]->addMarSystem(bpvoc0);
			pvoices[i]->updctrl("Gain/bgain0/mrs_real/gain", 1.0);
			bpvoc0->updctrl("mrs_natural/Decimation", D);
			bpvoc0->updctrl("mrs_natural/WindowSize", Nw);
			bpvoc0->updctrl("mrs_natural/FFTSize", N);
			bpvoc0->updctrl("mrs_natural/Interpolation", I);
			bpvoc0->updctrl("mrs_real/PitchShift", 1.0);
			bpvoc0->updctrl("mrs_natural/Sinusoids", sopt);
			mixer->addMarSystem(pvoices[i]);
		}
		else if (i==1)
		{
			pvoices.push_back(mng.create("Series", "background1"));
			pvoices[i]->addMarSystem(mng.create("SoundFileSource", "mixsrc"));
			pvoices[i]->updctrl("SoundFileSource/mixsrc/mrs_string/filename", sfName2);

			pvoices[i]->updctrl("SoundFileSource/mixsrc/mrs_real/repetitions", -1.0);

			pvoices[i]->addMarSystem(mng.create("Gain", "bgain1"));	  
			pvoices[i]->addMarSystem(bpvoc1);
			pvoices[i]->updctrl("Gain/bgain1/mrs_real/gain", 1.0);
			bpvoc1->updctrl("mrs_natural/Decimation", D);
			bpvoc1->updctrl("mrs_natural/WindowSize", Nw);
			bpvoc1->updctrl("mrs_natural/FFTSize", N);
			bpvoc1->updctrl("mrs_natural/Interpolation", I);
			bpvoc1->updctrl("mrs_real/PitchShift", 1.0);
			bpvoc1->updctrl("mrs_natural/Sinusoids", sopt);
			mixer->addMarSystem(pvoices[i]);
		}
	}

  total->addMarSystem(mixer);
  total->addMarSystem(mng.create("Sum", "sum"));

	if (outsfname == EMPTYSTRING) 
	{
		total->addMarSystem(mng.create("AudioSink", "dest"));
		total->updctrl("AudioSink/dest/mrs_natural/bufferSize", bopt);
	}
	else
	{

		total->addMarSystem(mng.create("SoundFileSink","dest"));
		total->updctrl("SoundFileSink/dest/mrs_string/filename", outsfname);
	}
  
  int type;
  string cname;

  mrs_real diff;
	mrs_natural fc = 0;
  mrs_real time = 0.0;
  mrs_real epsilon = 0.0029024;
  
  diff = 0.0;
  
  bool trigger = true;
  
  total->updctrl("mrs_natural/inSamples", D);
  
  cout << "Ready to start processing " << endl;

#ifdef MRSMIDI
	RtMidiIn *midiin = NULL;
	// open midi if midiPort is specified 
	if (midi_ != -1) 
	{
		try 
		{
			midiin = new RtMidiIn();
		}
		catch (RtError &error) 
		{
			error.printMessage();
			exit(1);
		}
		try 
		{
			midiin->openPort(midi_);
		}
		catch (RtError &error) 
		{
			error.printMessage();
			exit(1);
		}
	}
#endif 

  // midi message 
  std::vector<unsigned char> message;
  int nBytes;
  int byte2, byte3;
  double s1x;
  double s1y;
  double s1z;
  
  double s2x;
  double s2y;
  double s2z;
  
  double stamp;
  
	while(1)
	{
		if (midi_ != -1) 
		{
#ifdef MRSMIDI
			stamp = midiin->getMessage( &message );
#endif 
			nBytes = message.size();
			if (nBytes >2)
			{
				byte3 = message[2];
				byte2 = message[1];
				type = message[0];

				if ((type == 160)&&(byte2 == 1))
				{
					pvoices[0]->updctrl("SoundFileSource/mixsrc/mrs_bool/advance", true);

					cout << "s1 whack" << endl;

				}
				if ((type == 160)&&(byte2 == 8))
				{
					s1x = byte3 / 128.0;

					bpvoc0->updctrl("mrs_natural/Sinusoids", (mrs_natural) (s1x * sopt)+1);
				}
				if  ((type == 160)&&(byte2 == 9))
				{
					s1y = byte3 / 128.0;
					bpvoc0->updctrl("mrs_real/PitchShift", (mrs_real) 0.5  + s1y * 1.5);
				}
				if  ((type == 160)&&(byte2 == 10))
				{
					s1z = (byte3-14.0) / 128.0;
					pvoices[0]->updctrl("Gain/bgain0/mrs_real/gain", s1z);   
				}
				if ((type == 160)&&(byte2 == 2))
				{
					cout << "s2 whack" << endl;
					pvoices[1]->updctrl("SoundFileSource/mixsrc/mrs_bool/advance", true);
				}
				if ((type == 160)&&(byte2 == 11))
				{
					s2x = byte3 / 128.0;
					bpvoc1->updctrl("mrs_natural/Sinusoids", (mrs_natural) (s2x * sopt)+1);
				}
				if  ((type == 160)&&(byte2 == 12))
				{
					s2y = byte3 / 128.0;
					bpvoc1->updctrl("mrs_real/PitchShift", (mrs_real) 0.5  + s2y * 1.5);
				}
				if  ((type == 160)&&(byte2 == 13))
				{
					s2z = (byte3 -14) / 128.0;
					pvoices[1]->updctrl("Gain/bgain1/mrs_real/gain", s2z);  
				}
			} 
		}
		// play the sound  
		total->tick();
	}
}

// phasevocoder variant for November 2005 concert 
// at UVic 

void 
phasevocHeterophonics(string sfName, mrs_natural N, mrs_natural Nw, 
		  mrs_natural D, mrs_natural I, mrs_real P, 
		  string outsfname)
{

  cout << "HETEROPHONICS " << endl;
  
  // Heterophonics contains piece-specific code
  // for a piece performed in November 2005 at UVic 
  // It combines a polyphonic phasevocoder 
  // with real-time pitch tracking 
  
  // hardwire 3 voices 
  // the first two a pitch shifted version of the input 
  // the third is pitch shifted background material 
  // load from various soundfiles 
  vopt_ = 4;
    
  MarSystemManager mng;
  MarSystem* total = mng.create("Series", "total");
  MarSystem* mixer = mng.create("Fanout", "mixer");
  MarSystem* bpvoc = mng.create("PhaseVocoder", "bpvoc");
  
  // vector of voices
	vector<MarSystem*> pvoices;
	for (int i=0; i < vopt_; i++)
	{
		if (i < 2) 
		{
			ostringstream oss;
			oss << "pvseries" << i;
			pvoices.push_back(mng.create("PhaseVocoder", oss.str()));
			pvoices[i]->updctrl("mrs_natural/Decimation", D);
			pvoices[i]->updctrl("mrs_natural/WindowSize", Nw);
			pvoices[i]->updctrl("mrs_natural/FFTSize", N);
			pvoices[i]->updctrl("mrs_natural/Interpolation", I);
			pvoices[i]->updctrl("mrs_real/PitchShift", P);
			pvoices[i]->updctrl("mrs_natural/Sinusoids", sopt);
			pvoices[i]->updctrl("mrs_real/gain", gopt_);
			mixer->addMarSystem(pvoices[i]);
		}
		else if (i==2)
		{
			pvoices.push_back(mng.create("Series", "background"));
			pvoices[i]->addMarSystem(mng.create("SoundFileSource", "mixsrc"));
			pvoices[i]->updctrl("SoundFileSource/mixsrc/mrs_string/filename", "/home/gtzan/data/sound/Nov2005Concert/silence.wav");
			pvoices[i]->updctrl("SoundFileSource/mixsrc/mrs_real/repetitions", -1.0);
			bpvoc = mng.create("PhaseVocoder", "bpvoc");
			pvoices[i]->addMarSystem(mng.create("Gain", "bgain"));	  
			pvoices[i]->addMarSystem(bpvoc);
			pvoices[i]->updctrl("Gain/bgain/mrs_real/gain", 1.0);
			bpvoc->updctrl("mrs_natural/Decimation", D);
			bpvoc->updctrl("mrs_natural/WindowSize", Nw);
			bpvoc->updctrl("mrs_natural/FFTSize", N);
			bpvoc->updctrl("mrs_natural/Interpolation", I);
			bpvoc->updctrl("mrs_real/PitchShift", 1.0);
			bpvoc->updctrl("mrs_natural/Sinusoids", sopt);
			mixer->addMarSystem(pvoices[i]);
		}
		else if (i==3) 
		{
			pvoices.push_back(mng.create("SoundFileSource", "osrc"));
			pvoices[i]->updctrl("mrs_string/filename", "/home/gtzan/data/sound/Nov2005Concert/silence.wav");
			mixer->addMarSystem(pvoices[i]);
		}
	}

  MarSystem* mic;
  mic = mng.create("AudioSource", "mic");
  total->addMarSystem(mixer);
  total->addMarSystem(mng.create("Sum", "sum"));

	if (outsfname == EMPTYSTRING) 
	{
		total->addMarSystem(mng.create("AudioSink", "dest"));
		total->updctrl("AudioSink/dest/mrs_natural/bufferSize", bopt);
	}
	else
	{

		total->addMarSystem(mng.create("SoundFileSink","dest"));
		total->updctrl("SoundFileSink/dest/mrs_string/filename", outsfname);
	}
  
  // vectors used for sharing between phasevocoder 
  // network and pitch extraction network 
  realvec in, min, out, pin;
   
  mic->updctrl("mrs_natural/inSamples", D);
  mic->updctrl("mrs_natural/inObservations", 1);
  total->updctrl("mrs_natural/inSamples", D);
  total->updctrl("mrs_natural/inObservations", 1);
  
  in.create( (long)1, (long)D);
  min.create((long)1, (long)D);
  out.create(total->getctrl("mrs_natural/onObservations")->toNatural(), 
	     total->getctrl("mrs_natural/inSamples")->toNatural());
  
  pin.create((long)1, (long)512);

  string cname;
  mrs_real diff;
  mrs_natural fc = 0;
  
  // Build the pitch extractor network 
  MarSystem* pitchExtractor = mng.create("Series", "pitchExtractor");
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
  mrs_natural lowPitch = 32;
  mrs_natural highPitch = 100;
  
  mrs_real lowFreq = pitch2hertz(lowPitch);
  mrs_real highFreq = pitch2hertz(highPitch);
  mrs_natural lowSamples = 
    hertz2samples(highFreq, 22050.0);
  mrs_natural highSamples = 
    hertz2samples(lowFreq, 22050.0);
  pitchExtractor->updctrl("Peaker/pkr/mrs_real/peakSpacing", 0.1);
  pitchExtractor->updctrl("Peaker/pkr/mrs_real/peakStrength", 0.5);
  pitchExtractor->updctrl("Peaker/pkr/mrs_natural/peakStart", lowSamples);
  pitchExtractor->updctrl("Peaker/pkr/mrs_natural/peakEnd", highSamples);
  pitchExtractor->updctrl("MaxArgMax/mxr/mrs_natural/nMaximums", 1);
   
  MarSystem* gainExtractor = mng.create("MaxArgMax", "mgain");
  gainExtractor->updctrl("mrs_natural/inSamples", 512);
  
  realvec pitchres(pitchExtractor->getctrl("mrs_natural/onObservations")->toNatural(), pitchExtractor->getctrl("mrs_natural/onSamples")->toNatural());
  
  realvec mgres(gainExtractor->getctrl("mrs_natural/onObservations")->toNatural(), 
		gainExtractor->getctrl("mrs_natural/onSamples")->toNatural());
   
  mrs_real pitch = 0.0;
  mrs_real prev_pitch = 0.0;
  
  mrs_real glide0;
  mrs_real glide1;
   
  mrs_real time = 0.0;
  mrs_real epsilon = 0.0029024;
  
  mrs_natural epoch =0;
  diff = 0.0;
   
  bool trigger = true;
  
  pvoices[0]->updctrl("mrs_real/gain", 0.0);
  pvoices[1]->updctrl("mrs_real/gain", 0.0);
  
  vector<string> tablas;
  tablas.push_back("/home/gtzan/data/sound/Nov2005Concert/tabla/Ge2.wav");
  tablas.push_back("/home/gtzan/data/sound/Nov2005Concert/tabla/Ge3_s1.wav");
  tablas.push_back("/home/gtzan/data/sound/Nov2005Concert/tabla/ke1_s1.wav");
  tablas.push_back("/home/gtzan/data/sound/Nov2005Concert/tabla/na10_s1.wav");
  tablas.push_back("/home/gtzan/data/sound/Nov2005Concert/tabla/Dhi1_s1.wav");
  tablas.push_back("/home/gtzan/data/sound/Nov2005Concert/tabla/na10_s1.wav");
  tablas.push_back("/home/gtzan/data/sound/Nov2005Concert/tabla/tun1_s1.wav");
  tablas.push_back("/home/gtzan/data/sound/Nov2005Concert/tabla/na7.wav");
  tablas.push_back("/home/gtzan/data/sound/Nov2005Concert/tabla/te10.wav");
  tablas.push_back("/home/gtzan/data/sound/Nov2005Concert/tabla/Dha12_s1.wav");
  tablas.push_back("/home/gtzan/data/sound/Nov2005Concert/tabla/tun1_s1.wav");
  tablas.push_back("/home/gtzan/data/sound/Nov2005Concert/tabla/te12_s1.wav");

  mrs_natural note, prev_note = 0;

  mrs_natural noteCount = 0;
  mrs_natural sealCount = 0;
   
	while(1)
	{
		time += 0.0029024;

		if ((pitch >= 1000) && (pitch<= 1050) &&
			(pitchres(0) > 0.5) && 
			(epoch == 0))
		{
			cout << "Epoch " << epoch << ": Whole-tone descending - Middle C"  << endl;
			epoch++;
		}

		if ((pitch >= 460.0) &&
			(pitch <= 480.0) &&
			(pitchres(0) > 0.5) && 
			(epoch == 1))
		{
			cout << "Epoch " << epoch << ": Harmonizer - Ab" << endl;
			epoch ++;	  
			pvoices[0]->updctrl("mrs_real/gain", 0.30);
			pvoices[1]->updctrl("mrs_real/gain", 0.30);
			pvoices[0]->updctrl("mrs_real/PitchShift", 0.25);
			pvoices[1]->updctrl("mrs_real/PitchShift", pow((double)1.06, (double)-17.0));
			pvoices[2]->updctrl("SoundFileSource/mixsrc/mrs_string/filename", "/home/gtzan/data/sound/Nov2005Concert/silence.wav");
			pvoices[3]->updctrl("mrs_string/filename", "/home/gtzan/data/sound/Nov2005Concert/silence.wav");
		}

		if ((pitch >= 750.0) &&
			(pitch <= 770.0) &&
			(pitchres(0) > 0.5) && 
			(epoch == 2))
		{
			cout << "Epoch " << epoch << ": Speech manipulation high D" << endl;
			epoch ++;

			glide0 = 0.25;
			glide1 = 1.0;
			pvoices[0]->updctrl("mrs_real/gain", 0.0);
			pvoices[1]->updctrl("mrs_real/gain", 0.0);
			pvoices[2]->updctrl("SoundFileSource/mixsrc/mrs_string/filename", "/home/gtzan/data/sound/Nov2005Concert/rainSS.wav");
		}

		if ((pitch >= 1000) && (pitch<= 1050) &&
			(pitchres(0) > 0.5) && 
			(epoch == 3))
		{
			cout << "Epoch " << epoch << ": Greek Folk solo middle B" << endl;
			pvoices[0]->updctrl("mrs_real/gain", 0.0);
			pvoices[1]->updctrl("mrs_real/gain", 0.0);
			epoch ++;
			pvoices[2]->updctrl("SoundFileSource/mixsrc/mrs_string/filename", "/home/gtzan/data/sound/Nov2005Concert/silence.wav");
		}

		if ((pitch >= 435) && (pitch<= 455) &&
			(pitchres(0) > 0.5) && 
			(epoch == 4))
		{
			cout << "Epoch " << epoch << ": Greek Folk solo + tabla : Middle Bb" << endl;
			pvoices[0]->updctrl("mrs_real/gain", 0.0);
			pvoices[1]->updctrl("mrs_real/gain", 0.0);
			epoch++;
			pvoices[2]->updctrl("SoundFileSource/mixsrc/mrs_string/filename", "/home/gtzan/data/sound/Nov2005Concert/silence.wav");	  
		}

		if ((pitch >= 415) && (pitch<= 430) &&
			(pitchres(0) > 0.5) && 
			(epoch == 5))
		{
			cout << "Epoch " << epoch << ": Seashell calls - Low F#" << endl;	  
			pvoices[0]->updctrl("mrs_real/gain", 0.0);
			pvoices[1]->updctrl("mrs_real/gain", 0.0);

			epoch ++;
			pvoices[2]->updctrl("SoundFileSource/mixsrc/mrs_string/filename", "/home/gtzan/data/sound/Nov2005Concert/silence.wav");
		}

		if ((pitch >= 320) && (pitch<= 330) &&
			(pitchres(0) > 0.5) && 
			(epoch == 6))

		{
			pvoices[0]->updctrl("mrs_real/gain", 0.0);
			pvoices[1]->updctrl("mrs_real/gain", 0.0);
			cout << "Epoch " << epoch << ": Orchestra manipulation : High D" << endl;
			epoch ++;
			pvoices[2]->updctrl("SoundFileSource/mixsrc/mrs_string/filename", "/home/gtzan/data/sound/Nov2005Concert/storm1.wav");
			bpvoc->updctrl("mrs_real/PitchShift", 1.0);
		}

		if ((pitch >= 1000) && (pitch<= 1050) &&
			(pitchres(0) > 0.5) && 
			(epoch == 7))
		{
			cout << "Epoch " << epoch << ": Glissanti : Low Eb" << endl;
			epoch ++;
			pvoices[0]->updctrl("mrs_real/gain", 0.20);
			pvoices[1]->updctrl("mrs_real/gain", 0.20);
			glide1 = 1.0;
			pvoices[2]->updctrl("SoundFileSource/mixsrc/mrs_string/filename", "/home/gtzan/data/sound/Nov2005Concert/silence.wav");
		}

		if ((pitch >= 270) && (pitch<= 280) &&
			(pitchres(0) > 0.5) && 
			(epoch == 8))
		{
			cout << "Epoch " << epoch << ": Seals High D" << endl;
			epoch ++;
			pvoices[0]->updctrl("mrs_real/gain", 0.75);
			pvoices[1]->updctrl("mrs_real/gain", 0.75);
			pvoices[0]->updctrl("mrs_real/PitchShift", 0.558395);
			pvoices[1]->updctrl("mrs_real/PitchShift", 0.747259);
			pvoices[2]->updctrl("SoundFileSource/mixsrc/mrs_string/filename", "/home/gtzan/data/sound/silence.wav");
			pvoices[3]->updctrl("mrs_string/filename", "/home/gtzan/data/sound/Nov2005Concert/seal.wav");	  
			sealCount = 0;
		}

		if ((pitch >= 1000) && (pitch<= 1050) &&
			(pitchres(0) > 0.5) && 
			(sealCount > 1000) &&
			(epoch == 9))
		{
			cout << "Epoch " << epoch << ": Storm : Low Eb" << endl;
			epoch ++;
			pvoices[0]->updctrl("mrs_real/gain", 0.75);
			pvoices[1]->updctrl("mrs_real/gain", 0.75);
			pvoices[0]->updctrl("mrs_real/PitchShift", 0.558395);
			pvoices[1]->updctrl("mrs_real/PitchShift", 0.747259);
			pvoices[2]->updctrl("SoundFileSource/mixsrc/mrs_string/filename", "/home/gtzan/data/sound/Nov2005Concert/silence.wav");
			bpvoc->updctrl("mrs_real/PitchShift", 1.0);
			pvoices[2]->updctrl("Gain/bgain/mrs_real/gain", 1.0);
			pvoices[3]->updctrl("mrs_string/filename", "/home/gtzan/data/sound/Nov2005Concert/storm2.wav");	  
			pvoices[3]->updctrl("mrs_real/repetitions", -1.0);
		}

		if ((pitch >= 270) && (pitch<= 280) &&
			(pitchres(0) > 0.5) && 
			(epoch == 10))
		{
			cout << "Epoch " << epoch << ": Drips : Solo sax F# Low" << endl;
			epoch ++;
			pvoices[0]->updctrl("mrs_real/gain", 0.5);
			pvoices[1]->updctrl("mrs_real/gain", 0.5);
			pvoices[0]->updctrl("mrs_real/PitchShift", pow((double)1.06, (double)7.04));
			pvoices[1]->updctrl("mrs_real/PitchShift", pow((double)1.06, (double)10.0));
			pvoices[2]->updctrl("SoundFileSource/mixsrc/mrs_string/filename", "/home/gtzan/data/sound/Nov2005Concert/drip03.wav");
			bpvoc->updctrl("mrs_real/PitchShift", 1.0);
			pvoices[2]->updctrl("Gain/bgain/mrs_real/gain", 1.0);
			pvoices[3]->updctrl("mrs_string/filename", "/home/gtzan/data/sound/Nov2005Concert/howl2.wav");	  
		}

		if ((pitch >= 320) && (pitch<= 330) &&
			(pitchres(0) > 0.5) && 
			(epoch == 11))
		{
			cout << "Epoch " << epoch << ": Finale" << endl;
			epoch ++;
			pvoices[0]->updctrl("mrs_real/gain", 0.0);
			pvoices[1]->updctrl("mrs_real/gain", 0.0);
			pvoices[0]->updctrl("mrs_real/PitchShift", 1.0);
			pvoices[1]->updctrl("mrs_real/PitchShift", 1.0);
			pvoices[2]->updctrl("SoundFileSource/mixsrc/mrs_string/filename", "/home/gtzan/data/sound/Nov2005Concert/silence.wav");
			bpvoc->updctrl("mrs_real/PitchShift", 1.0);
			pvoices[2]->updctrl("Gain/bgain/mrs_real/gain", 0.0);
			pvoices[3]->updctrl("mrs_string/filename", "/home/gtzan/data/sound/Nov2005Concert/silence.wav");	  
		}

		// read input for microphone 64 samples
		mic->process(in, min);

		// accumulte in buffers of 512 for pitch extraction
		for (int i=0; i < D; i++)
			pin(0, fc * D + i) = min(0,i);

		// play the sound  
		total->process(min, out);      

		if (fc == 7) 
		{
			// extract pitch 
			pitchExtractor->process(pin, pitchres);
			// extract gain
			gainExtractor->process(pin, mgres);

			pitch = samples2hertz((mrs_natural)pitchres(1), 22050.0);
			note = (mrs_natural)hertz2pitch(pitch);

			if (epoch == 3) 
			{
				diff = hertz2pitch(pitch) - 79.0;
				bpvoc->updctrl("mrs_real/PitchShift", pow((double)1.06, (double)diff));
				pvoices[2]->updctrl("Gain/bgain/mrs_real/gain", 2.5 * mgres(0));
			}

			if (epoch == 5) 
			{
				if ((note != prev_note)&&(pitchres(0) > 0.5)&&(noteCount > 4))
				{
					mrs_natural tablaIndex = note % 12;
					pvoices[3]->updctrl("mrs_string/filename", tablas[tablaIndex]);
					noteCount = 0;
				}

			}

			if (epoch == 7)
			{
				if (pitchres(0) > 0.5)
				{
					diff = hertz2pitch(pitch) - 64.0;
					bpvoc->updctrl("mrs_real/PitchShift", pow((double)1.06, (double)diff));
				}
				pvoices[2]->updctrl("Gain/bgain/mrs_real/gain", 2.0 * mgres(0));
			}

			if (epoch == 8)
			{
				if ((note != prev_note)&&(pitchres(0) > 0.5))
				{
					glide1 = 1.0;
				}

				diff = hertz2pitch(pitch) - 79.0;
				pvoices[0]->updctrl("mrs_real/PitchShift", 0.25);
				pvoices[1]->updctrl("mrs_real/gain", 1.0);
				pvoices[1]->updctrl("mrs_real/PitchShift", glide1);
				glide1 -= 0.01;
				if (glide1 < 0.125) 
					glide1 = 1.0;
			}

			if (epoch == 9) 
			{
				sealCount++;
				if (sealCount == 2000) 
					cout << "Ready for high D" << endl;

			}

			if (epoch == 11)
			{

				if ((note != prev_note)&&(pitchres(0) > 0.5))
				{
					pvoices[2]->updctrl("SoundFileSource/mixsrc/mrs_string/filename", "/home/gtzan/data/sound/Nov2005Concert/drip03.wav");

				}
				pvoices[2]->updctrl("Gain/bgain/mrs_real/gain", mgres(0));
			}

			prev_pitch = pitch;
			prev_note = note;
			noteCount++;
		}
		fc = (fc + 1) % 8;      
	}
}

void 
initOptions()
{
	cmd_options.addBoolOption("help", "h", false);
	cmd_options.addBoolOption("usage", "u", false);
	cmd_options.addNaturalOption("voices", "v", 1);
	cmd_options.addStringOption("filename", "f", EMPTYSTRING);
	cmd_options.addStringOption("plugin", "p", EMPTYSTRING);
	cmd_options.addNaturalOption("winsize", "w", winSize_);
	cmd_options.addNaturalOption("fftsize", "n", fftSize_);
	cmd_options.addNaturalOption("decimation", "d", dopt);
	cmd_options.addNaturalOption("interpolation", "i", iopt);
	cmd_options.addNaturalOption("sinusoids", "s", sopt);
	cmd_options.addNaturalOption("bufferSize", "b", bopt);
	cmd_options.addRealOption("gain", "g", 1.0);
	cmd_options.addRealOption("pitchshift", "p", 1.0);
	cmd_options.addBoolOption("auto", "a", auto_);
	cmd_options.addNaturalOption("midi", "m", midi_);
	cmd_options.addNaturalOption("epochHeterophonics", "e", eopt_);
}

void 
loadOptions()
{
	helpopt_ = cmd_options.getBoolOption("help");
	usageopt_ = cmd_options.getBoolOption("usage");
	pluginName = cmd_options.getStringOption("plugin");
	fileName   = cmd_options.getStringOption("filename");
	winSize_ = cmd_options.getNaturalOption("winsize");
	fftSize_ = cmd_options.getNaturalOption("fftsize");
	dopt = cmd_options.getNaturalOption("decimation");
	iopt = cmd_options.getNaturalOption("interpolation");
	sopt = cmd_options.getNaturalOption("sinusoids");
	bopt = cmd_options.getNaturalOption("bufferSize");
	popt = cmd_options.getRealOption("pitchshift");
	auto_ = cmd_options.getBoolOption("auto");
	midi_ = cmd_options.getNaturalOption("midi");
	vopt_ = cmd_options.getNaturalOption("voices");
	gopt_ = cmd_options.getRealOption("gain");
	eopt_ = cmd_options.getNaturalOption("epochHeterophonics");
}

int
main(int argc, const char **argv)
{
	MRSDIAG("sftransform.cpp - main");

	initOptions();
	cmd_options.readOptions(argc, argv);
	loadOptions();  

	vector<string> soundfiles = cmd_options.getRemaining();
	vector<string>::iterator sfi;

	string progName = argv[0];  

	if (helpopt_) 
		printHelp(progName);

	if (usageopt_)
		printUsage(progName);

	cerr << "Phasevocoder configuration (-h show the options): " << endl;
	cerr << "fft size (-n)      = " << fftSize_ << endl;
	cerr << "win size (-w)      = " << winSize_ << endl;
	cerr << "decimation (-d)    = " << dopt << endl;
	cerr << "interpolation (-i) = " << iopt << endl;
	cerr << "pitch shift (-p)   = " << popt << endl;
	cerr << "sinusoids (-s)     = " << sopt << endl;
	cerr << "voices (-v)        = " << vopt_ << endl;
	cerr << "midiPort (-m)      = " << midi_ << endl;
	cerr << "outFile  (-f)      = " << fileName << endl;

	int i =0;
	// soundfile input 
	string sfname;
	if (soundfiles.size() != 0)   
	{
		sfname = soundfiles[0];
		i = 1;
	}

	cout << "Phasevocoding " << sfname << endl;

	if(i == 1)//sound file input
	{
		cout << "Using sound file input" << endl;
		microphone_ = false;
		if (vopt_ == 1) 
		{
			phasevocSeries(sfname, fftSize_, winSize_, dopt, iopt, popt, fileName);
		}
		else
		{
			if (eopt_ == 0)
				phasevocPoly(sfname, fftSize_, winSize_, dopt, iopt, popt, fileName);
			else if (eopt_ == 1) 
				phasevocHeterophonics(sfname, fftSize_, winSize_, dopt, iopt, popt, fileName);      	  	    
			else if (eopt_ == 2) 
				phasevocConvolve(sfname, fftSize_, winSize_, dopt, iopt, popt, fileName);
			else if (eopt_ == 3) 
				phasevocCrossSynth(sfname, fftSize_, winSize_, dopt, iopt, popt, fileName);
			else if (eopt_ == 4) 
			{
				string sfname1 = soundfiles[0];
				string sfname2 = soundfiles[1];
				phasevocHeterophonicsRadioDrum(sfname1, sfname2, fftSize_, winSize_, dopt, iopt, popt, fileName);      	  	    
			}
			else 
				cout << "Not supported heterophonics epoch" << endl;
		}

	}
	if (i == 0) //micophone input
	{
		cout << "Using live microphone input" << endl;
		microphone_ = true;
		if (vopt_ == 1) 
			phasevocSeries("microphone", fftSize_, winSize_, dopt, iopt, popt, fileName);
		else
			if (eopt_ ==0) 
				phasevocPoly("microphone", fftSize_, winSize_, dopt, iopt, popt, fileName);      
			else if (eopt_ == 1) 
				phasevocHeterophonics("microphone", fftSize_, winSize_, dopt, iopt, popt, fileName);      	  
			else if (eopt_ == 2) 
				phasevocConvolve("microphone", fftSize_, winSize_, dopt, iopt, popt, fileName);      	  
			else if (eopt_ == 3) 
				phasevocCrossSynth("microphone", fftSize_, winSize_, dopt, iopt, popt, fileName);
			else if (eopt_ == 4) 
				phasevocHeterophonicsRadioDrum("microphone", "microphone", fftSize_, winSize_, dopt, iopt, popt, fileName);      	  	    
			else 
				cout << "Not supported heterophonics epoch" << endl;
	}

	exit(0);
}


