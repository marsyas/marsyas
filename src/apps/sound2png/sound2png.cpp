//
// sound2png
//
// A program that uses Marsyas to generate a PNG of an input audio
// file.  The PNG can be either the waveform or the spectrogram of the
// audio file.
//
// written by sness (c) 2009 - GPL - sness@sness.net
//

#include <cstdio>
#include "Collection.h"
#include "MarSystemManager.h"
#include "CommandLineOptions.h"
#ifdef MARSYAS_PNG
#include "pngwriter.h"
#endif MARSYAS_PNG 

#include <vector> 

using namespace std;
using namespace Marsyas;

//
// Global variables for various commandline options
//
int helpopt;
int usageopt;
int verboseopt;
int windowSize;
int hopSize;
mrs_real gain;
int maxFreq;
bool waveform;
int position;
int ticks;
bool histogram;
CommandLineOptions cmd_options;

void 
printUsage(string progName)
{
  MRSDIAG("sound2png.cpp - printUsage");
  cerr << "Usage : " << progName << " in.wav out.png" << endl;
  cerr << endl;
  cerr << "where : " << endl;
  cerr << "   in.wav is a sound file in a MARSYAS supported format" << endl;
  cerr << "   out.png is the name of the PNG file to be generated" << endl;
  exit(1);
}

void 
printHelp(string progName)
{
  MRSDIAG("sound2png.cpp - printHelp");
  cerr << "sound2png" << endl;
  cerr << "-------------------------------------------------------------" << endl;
  cerr << "Generate a PNG of an input audio file.  The PNG can either be" << endl;
  cerr << "the waveform or the spectrogram of the audio file" << endl;
  cerr << endl;
  cerr << "written by sness (c) 2009 GPL - sness@sness.net" << endl;
  cerr << endl;
  cerr << "Usage : " << progName << " in.wav out.png" << endl;
  cerr << endl;
  cerr << "where : " << endl;
  cerr << "   in.wav is a sound file in a MARSYAS supported format" << endl;
  cerr << "   out.png is the name of the PNG file to be generated" << endl;
  cerr << "Help Options:" << endl;
  cerr << "-u --usage        : display short usage info" << endl;
  cerr << "-h --help         : display this information " << endl;
  cerr << "-v --verbose      : verbose output" << endl;
  cerr << "-w --waveform     : draw a waveform instead of a spectrogram" << endl;
  cerr << "--ws --windowsize : windows size in samples " << endl;
  cerr << "--hs --hopsize    : hop size in samples (for spectrogram)" << endl;
  cerr << "-g --gain         : gain for spectrogram (for spectrogram)" << endl;
  cerr << "--mf --maxfreq    : maximum frequency (for spectrogram)" << endl;
  cerr << "-p --position     : position to start at in the audio file" << endl;
  cerr << "-t --ticks        : how many times to tick the network" << endl;
  cerr << "-hi --histogram   : output a histogram of the values for the spectrogram" << endl;
   
  exit(1);
}

void 
initOptions()
{
  cmd_options.addBoolOption("help", "h", false);
  cmd_options.addBoolOption("usage", "u", false);
  cmd_options.addBoolOption("verbose", "v", false);
  cmd_options.addBoolOption("waveform", "w", false);
  cmd_options.addNaturalOption("windowsize", "ws", 512);
  cmd_options.addNaturalOption("hopsize", "hs", 256);
  cmd_options.addRealOption("gain", "g", 1.5);
  cmd_options.addNaturalOption("maxfreq", "mf", 8000);
  cmd_options.addNaturalOption("ticks", "t", -1);
  cmd_options.addNaturalOption("position", "p", 0);
  cmd_options.addBoolOption("histogram", "hi", false);
}


void 
loadOptions()
{
  helpopt = cmd_options.getBoolOption("help");
  usageopt = cmd_options.getBoolOption("usage");
  verboseopt = cmd_options.getBoolOption("verbose");
  waveform = cmd_options.getBoolOption("waveform");
  windowSize = cmd_options.getNaturalOption("windowsize");
  hopSize = cmd_options.getNaturalOption("hopsize");
  gain = cmd_options.getRealOption("gain");
  maxFreq = cmd_options.getNaturalOption("maxfreq");
  position = cmd_options.getNaturalOption("position");
  ticks = cmd_options.getNaturalOption("ticks");
  histogram = cmd_options.getBoolOption("histogram");
}


int getFileLengthForWaveform(string inFileName, int windowSize, double& min, double& max) {

  MarSystemManager mng;

  // A series to contain everything
  MarSystem* net = mng.create("Series", "net");
	
  // The sound file
  net->addMarSystem(mng.create("SoundFileSource", "src"));
  net->updctrl("SoundFileSource/src/mrs_string/filename", inFileName);
  net->updctrl("SoundFileSource/src/mrs_natural/pos", position);
  net->setctrl("mrs_natural/inSamples", windowSize);

  // Compute the AbsMax of this window
  net->addMarSystem(mng.create("AbsMax","absmax"));

  realvec processedData;

  int length = 0;

  while (net->getctrl("SoundFileSource/src/mrs_bool/notEmpty")->to<mrs_bool>() 
		 && (ticks == -1 || length < ticks))  {
	net->tick();
	length++;

	processedData = net->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();
	if (processedData(0) < min)
	  min = processedData(0);
	if (processedData(0) > max)
	  max = processedData(0);
  }


  delete net;

  if (verboseopt) {
    cout << "length=" << length << endl;
    cout << "max=" << max << endl;
    cout << "min=" << min << endl;
  }

  return length;
}

void outputWaveformPNG(string inFileName, string outFileName)
{
#ifdef MARSYAS_PNG
  int length;
  int height = 128;
  int middle_right;
  int middle_left;

  double min = 99999999999.9;
  double max = -99999999999.9;

  length = getFileLengthForWaveform(inFileName,windowSize,min,max);
  pngwriter png(length,height,0,outFileName.c_str());

  MarSystemManager mng;

  // A series to contain everything
  MarSystem* net = mng.create("Series", "net");
	
  // The sound file
  net->addMarSystem(mng.create("SoundFileSource", "src"));
  net->updctrl("SoundFileSource/src/mrs_string/filename", inFileName);
  net->updctrl("SoundFileSource/src/mrs_natural/pos", position);
  net->setctrl("mrs_natural/inSamples", windowSize);
  net->addMarSystem(mng.create("MaxMin","maxmin"));

  mrs_natural channels = net->getctrl("mrs_natural/onObservations")->to<mrs_natural>();

  if (verboseopt) {
	cout << "channels=" << channels << endl;
  }

  if (channels == 2) {
	middle_right = (height/4);
	middle_left = (height/2)+(height/4);
  } else {
	middle_left = (height/2);
  }

  realvec processedData;

  // Give it a white background
  png.invert();

  // A line across the middle of the plot
  png.line(0,middle_left,length,middle_left,0,0,0);
  if (channels == 2) {
	png.line(0,middle_right,length,middle_right,0,0,0);
  }
  
  double x = 0;

  double y_max_right = 0;
  double y_min_right = 0;
  double y_max_right_prev = 0;
  double y_min_right_prev = 0;

  double y_max_left = 0;
  double y_min_left = 0;
  double y_max_left_prev = 0;
  double y_min_left_prev = 0;

  double draw_color;

  // If we are just displaying individual samples, make the line dark blue.
  if (windowSize == 1) {
	draw_color = 0.0;
  } else {
	draw_color = 0.2;
  }
  
  while (net->getctrl("SoundFileSource/src/mrs_bool/notEmpty")->to<mrs_bool>()
		 && (ticks == -1 || x < ticks))  {
	net->tick();
	processedData = net->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();

	if (channels == 2) {
	  y_max_right = processedData(1,0) / 2.0 * height;
	  y_min_right = processedData(1,1) / 2.0 * height;
	}

   	y_max_left = processedData(0,0) / 2.0 * height;
   	y_min_left = processedData(0,1) / 2.0 * height;

	//
	// Draw the right waveform
	//
	if (channels == 2) {
	  // Draw a line from the maximum to the minimum value
	  png.line(x,middle_right+y_min_right,x,middle_right+y_max_right,0.0,0.0,1.0);
	
	  // Shade the middle part of the line lighter blue
	  double right_height = (y_max_right - y_min_right) / 4.0;
	  png.line(x,(middle_right+y_min_right)+right_height,x,(middle_right+y_max_right)-right_height,0.5,0.5,1.0);
	
	  // Fill in any missing segments with light blue
	  if (y_min_right_prev > y_max_right) {
		png.line(x,middle_right+y_min_right_prev,x,middle_right+y_max_right,draw_color,draw_color,1.0);
	  } else if (y_max_right_prev < y_min_right) {
		png.line(x,middle_right+y_max_right_prev,x,middle_right+y_min_right,draw_color,draw_color,1.0);
	  }
	}

	//
	// Draw the left waveform
	//
	// Draw a line from the maximum to the minimum value
	png.line(x,middle_left+y_min_left,x,middle_left+y_max_left,0.0,0.0,1.0);

	// Shade the middle part of the line lighter blue
	double left_height = (y_max_left - y_min_left) / 4.0;
	png.line(x,(middle_left+y_min_left)+left_height,x,(middle_left+y_max_left)-left_height,0.5,0.5,1.0);
	
	// Fill in any missing segments with light blue
	if (y_min_left_prev > y_max_left) {
	  png.line(x,middle_left+y_min_left_prev,x,middle_left+y_max_left,draw_color,draw_color,1.0);
	} else if (y_max_left_prev < y_min_left) {
	  png.line(x,middle_left+y_max_left_prev,x,middle_left+y_min_left,draw_color,draw_color,1.0);
	}
	

	y_max_right_prev = y_max_right;
	y_min_right_prev = y_min_right;
	y_max_left_prev = y_max_left;
	y_min_left_prev = y_min_left;

  	x++;

  }

  png.close();


  delete net;
#endif
}


int getFileLengthForSpectrogram(string inFileName, double& min, double& max, double& average) {

  realvec processedData;
  double dataLength = 0;
  double dataTotal = 0.0;

  MarSystemManager mng;
  MarSystem* net = mng.create("Series", "net");
  net->addMarSystem(mng.create("SoundFileSource", "src"));
  net->addMarSystem(mng.create("ShiftInput", "si"));
  net->addMarSystem(mng.create("Spectrum","spk"));
  net->addMarSystem(mng.create("PowerSpectrum","pspk"));
  net->updctrl("PowerSpectrum/pspk/mrs_string/spectrumType", "decibels");
  net->updctrl("SoundFileSource/src/mrs_string/filename", inFileName);
  net->updctrl("SoundFileSource/src/mrs_natural/pos", position);
  net->updctrl("SoundFileSource/src/mrs_natural/inSamples", hopSize);
  net->updctrl("ShiftInput/si/mrs_natural/winSize", windowSize);
  net->updctrl("mrs_natural/inSamples", int(hopSize));

  int length = 0;
  while ( net->getctrl("SoundFileSource/src/mrs_bool/notEmpty")->to<mrs_bool>() 
		  && (ticks == -1 || length < ticks)) {
	net->tick();
	length++;

	processedData = net->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();
	for (int i = 0; i < processedData.getRows(); i++) {
	  for (int j = 0; j < processedData.getCols(); j++) {
 		if (processedData(i,j) < min)
		  min = processedData(i,j);
 		if (processedData(i,j) > max)
		  max = processedData(i,j);
		dataLength += 1;
		dataTotal += processedData(i,j);
	  }
	}
  }

  delete net;

  average = dataTotal / dataLength;

  if (verboseopt) {
	cout << "length=" << length << endl;
	cout << "max=" << max << endl;
	cout << "min=" << min << endl;
	cout << "average=" << average << endl;
  }

  return length;
}

void outputSpectrogramPNG(string inFileName, string outFileName)
{
#ifdef MARSYAS_PNG
  double fftBins = windowSize / 2.0 + 1;  // N/2 + 1

  double min = 99999999999.9;
  double max = -99999999999.9;
  double average;

  int length = getFileLengthForSpectrogram(inFileName,min,max,average);

  MarSystemManager mng;
  MarSystem* net = mng.create("Series", "net");
  net->addMarSystem(mng.create("SoundFileSource", "src"));
  net->addMarSystem(mng.create("Stereo2Mono", "s2m"));
  net->addMarSystem(mng.create("ShiftInput", "si"));
  net->addMarSystem(mng.create("Spectrum","spk"));
  net->addMarSystem(mng.create("PowerSpectrum","pspk"));
  net->updctrl("PowerSpectrum/pspk/mrs_string/spectrumType", "decibels");
  net->updctrl("SoundFileSource/src/mrs_string/filename", inFileName);
  net->updctrl("SoundFileSource/src/mrs_natural/pos", position);
  net->updctrl("SoundFileSource/src/mrs_natural/inSamples", hopSize);
  net->updctrl("ShiftInput/si/mrs_natural/winSize", windowSize);
  net->updctrl("mrs_natural/inSamples", int(hopSize));

  mrs_real frequency = net->getctrl("SoundFileSource/src/mrs_real/osrate")->to<mrs_real>();
  double pngLength = length;
  double pngHeight = fftBins * (maxFreq / (frequency / 2.0));

  pngwriter png(int(pngLength),int(pngHeight),0,outFileName.c_str());

  realvec processedData;
  double normalizedData;

  // Iterate over the whole input file by ticking, outputting columns
  // of data to the .png file with each tick
  double x = 0;
  double y = 0;
  double colour = 0;
  while (net->getctrl("SoundFileSource/src/mrs_bool/notEmpty")->to<mrs_bool>()
		 && (ticks == -1 || x < ticks))  {
	net->tick();
	processedData = net->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();

 	for (int i = 0; i < pngHeight; i++) {
  	  double data_y = i;

 	  double data = processedData(int(data_y),0);

 	  normalizedData = ((data - min) / (max - min)) * gain;

	  // Make the spectrogram black on white instead of white on black
	  // TODO - Add the ability to generate different color maps, like Sonic Visualiser
 	  colour = 1.0 - normalizedData;
	  if (colour > 1.0) {
		colour = 1.0;
	  }
	  if (colour < 0.0) {
		colour = 0.0;
	  }

	  y = i;
	  png.plot(int(x),int(y),colour,colour,colour);
	}
	x++;

  }

  png.close();

  delete net;
#endif 
}

void fftHistogram(string inFileName)
{
  double fftBins = windowSize / 2.0 + 1;  // N/2 + 1

  MarSystemManager mng;
  MarSystem* net = mng.create("Series", "net");
  net->addMarSystem(mng.create("SoundFileSource", "src"));
  net->addMarSystem(mng.create("Stereo2Mono", "s2m"));
  net->addMarSystem(mng.create("ShiftInput", "si"));
  net->addMarSystem(mng.create("Spectrum","spk"));
  net->addMarSystem(mng.create("PowerSpectrum","pspk"));
  net->updctrl("PowerSpectrum/pspk/mrs_string/spectrumType", "decibels");
  net->updctrl("SoundFileSource/src/mrs_string/filename", inFileName);
  net->updctrl("SoundFileSource/src/mrs_natural/pos", position);
  net->updctrl("SoundFileSource/src/mrs_natural/inSamples", hopSize);
  net->updctrl("ShiftInput/si/mrs_natural/winSize", windowSize);
  net->updctrl("mrs_natural/inSamples", int(hopSize));

  mrs_real frequency = net->getctrl("SoundFileSource/src/mrs_real/osrate")->to<mrs_real>();
  double pngHeight = fftBins * (maxFreq / (frequency / 2.0));

  realvec processedData;
  double normalizedData;

  // Iterate over the whole input file by ticking, outputting columns
  // of data to the .png file with each tick
  double x = 0;
  double y = 0;
  double colour = 0;
  while (net->getctrl("SoundFileSource/src/mrs_bool/notEmpty")->to<mrs_bool>()
		 && (ticks == -1 || x < ticks))  {
	net->tick();
	processedData = net->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();

 	for (int i = 0; i < pngHeight; i++) {
  	  double data_y = i;

 	  double data = processedData(int(data_y),0);

	  cout << "data=" << data << endl;
	}
	x++;

  }

  delete net;
}


int
main(int argc, const char **argv)
{
  MRSDIAG("sfplay.cpp - main");

  string progName = argv[0];  
  if (argc == 1)
    printUsage(progName);

  // handling of command-line options 
  initOptions();
  cmd_options.readOptions(argc, argv);
  loadOptions();
  
  vector<string> files = cmd_options.getRemaining();
  if (helpopt) 
    printHelp(progName);
  
  if (usageopt)
    printUsage(progName);

  if (histogram) {
	fftHistogram(files[0]);
	exit(0);
  }

  if (files.size() != 2) {
	cerr << "You must specify two files on the command line." << endl;
	cerr << "One for the input audio file and one for the output PNG file" << endl;
    printUsage(progName); 
  }

#ifdef MARSYAS_PNG 
  // play the soundfiles/collections 
  if (waveform) {
	outputWaveformPNG(files[0],files[1]);
  } else {
	outputSpectrogramPNG(files[0],files[1]);
  }
  
  exit(0);
#else 
  cout << "sound2png requires Marsyas to be compiled with the WITH_PNG option" << endl; 
  exit(0);
#endif MARSYAS_PNG

}






