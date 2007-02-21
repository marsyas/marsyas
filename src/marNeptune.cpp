
#include <cstdio>
#include<iostream>
#include<iomanip>
#include <string>

#include "Conversions.h"
#include "CommandLineOptions.h"
#include "MarSystemManager.h"
#include "FileName.h"

#define EMPTYSTRING "MARSYAS_EMPTY" 

using namespace std;
using namespace Marsyas;

mrs_natural hopSize = 512;
mrs_natural windowSize = 1024;

string inName = EMPTYSTRING;
string outName = EMPTYSTRING;
string outputDirectory = EMPTYSTRING;

CommandLineOptions cmd_options;

int process(string inName, string outName)
{
	MarSystemManager mng;
	// setting up the network
	MarSystem* input = mng.create("Series", "input");

	input->addMarSystem(mng.create("SoundFileSource","src"));
  //input->addMarSystem(mng.create("ShiftInput", "si"));
  
	
	input->updctrl("SoundFileSource/src/mrs_string/filename", inName);
	input->updctrl("SoundFileSource/src/mrs_natural/inSamples", hopSize);
	//input->updctrl("ShiftInput/si/mrs_natural/Decimation", hopSize);
	//input->updctrl("ShiftInput/si/mrs_natural/WindowSize", windowSize);

 
	int i = 0;
	while(input->getctrl("SoundFileSource/src/mrs_bool/notEmpty")->toBool())
	{
		input->tick();
		i++;
	}

	delete input;
	return 0;
}

void initOptions()
{
cmd_options.addNaturalOption("windowSize", "w", windowSize);
cmd_options.addStringOption("outputDirectory", "O", outputDirectory);
}

void loadOptions()
{
windowSize = cmd_options.getNaturalOption("windowSize");
outputDirectory = cmd_options.getStringOption("outputDirectory");
}

int
main(int argc, const char **argv)
{
	initOptions();
	cmd_options.readOptions(argc, argv);
	loadOptions();  

	vector<string> soundfiles = cmd_options.getRemaining();
	vector<string>::iterator sfi;

  cout << "Marsyas for Neptune Segmentation" << endl;
 
  cout<<"windowSize: " << windowSize <<endl;
  cout<<"hopeSize: " << hopSize <<endl;
	
	for (sfi=soundfiles.begin() ; sfi!=soundfiles.end() ; sfi++)
	{
		cout << "Sound to segment: " << *sfi << endl;
		FileName Sfname(*sfi);
		outName = outputDirectory + "/" + Sfname.name();
		process(*sfi, outName);
	}

	cout << endl << "segmentation finished!";
}