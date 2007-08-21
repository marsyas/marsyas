
#include <cstdio>
#include<iostream>
#include<iomanip>
#include <string>

#include "Conversions.h"
#include "CommandLineOptions.h"
#include "MarSystemManager.h"
#include "FileName.h"

using namespace std;
using namespace Marsyas;

mrs_natural hopSize = 512;
mrs_natural windowSize = 1024;
mrs_natural lpcOrder = 10;

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
	input->addMarSystem(mng.create("ShiftInput", "si"));

	MarSystem* flowthru = mng.create("FlowThru", "flowthru");
	flowthru->addMarSystem(mng.create("Hamming", "hamAna")); 
	flowthru->addMarSystem(mng.create("LPC", "lpc"));
	input->addMarSystem(flowthru);
	input->addMarSystem(mng.create("Filter", "analysis"));


	MarSystem* audioSink = mng.create("SoundFileSink", "audioSink");


	input->addMarSystem(mng.create("NoiseSource", "ns"));

	//    input->addMarSystem(mng.create("Gain", "nsg1"));
	//  	input->addMarSystem(mng.create("PlotSink", "plot2"));
	input->addMarSystem(mng.create("Filter", "synthesis"));
	input->addMarSystem(mng.create("Gain", "nsg"));
	input->addMarSystem(mng.create("Windowing", "winSyn")); 
	input->addMarSystem(mng.create("OverlapAdd", "ova"));
	input->addMarSystem(audioSink);

	input->updctrl("SoundFileSource/src/mrs_string/filename", inName);
	input->updctrl("SoundFileSource/src/mrs_natural/inSamples", hopSize);
	//input->updctrl("ShiftInput/si/mrs_natural/Decimation", hopSize);
	input->updctrl("ShiftInput/si/mrs_natural/WindowSize", windowSize);
	input->updctrl("Windowing/winSyn/mrs_string/type", "Hanning");
	//input->updctrl("ShiftOutput/so/mrs_natural/Interpolation", hopSize);

	input->updctrl("FlowThru/flowthru/LPC/lpc/mrs_natural/order",lpcOrder);
	input->updctrl("FlowThru/flowthru/LPC/lpc/mrs_real/lambda",0.0);
	input->updctrl("FlowThru/flowthru/LPC/lpc/mrs_real/gamma",1.0);
	// input->updctrl("LPC/lpc/mrs_natural/featureMode", 0);

	input->linkctrl("Filter/analysis/mrs_realvec/ncoeffs",
		"FlowThru/flowthru/LPC/lpc/mrs_realvec/coeffs");
	input->linkctrl("Filter/synthesis/mrs_realvec/dcoeffs",
		"FlowThru/flowthru/LPC/lpc/mrs_realvec/coeffs");
	// link the power of the error with a gain
	input->linkctrl("Gain/nsg/mrs_real/gain",
		"FlowThru/flowthru/LPC/lpc/mrs_real/power");

	input->updctrl("SoundFileSink/audioSink/mrs_string/filename", outName);
	//input->updctrl("Gain/nsg1/mrs_real/gain", .1);
	input->updctrl("NoiseSource/ns/mrs_string/mode", "truc");

	int i = 0;
	while(input->getctrl("SoundFileSource/src/mrs_bool/notEmpty")->toBool())
	{
		input->tick();
		i++;
		//	cout << input->getctrl("Filter/analysis/mrs_realvec/ncoeffs")->toVec() << endl;
		//	cout << input->getctrl("LPC/lpc/mrs_realvec/coeffs")->toVec() << endl;
	}

	cout << endl << "LPC processing finished!";
	delete input;
	return 0;
}

void initOptions()
{
	cmd_options.addNaturalOption("order", "o", lpcOrder);
	cmd_options.addStringOption("outputDirectory", "O", outputDirectory);
}

void loadOptions()
{
	lpcOrder = cmd_options.getNaturalOption("order");
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

	cout << "vocalEffort Marsyas implementation" << endl;

	cout<<"LPC and LSP order: " <<lpcOrder <<endl;
	cout<<"hopSize: " <<hopSize <<endl;

	for (sfi=soundfiles.begin() ; sfi!=soundfiles.end() ; sfi++)
	{
		cout << "Sound to analyze: " << *sfi << endl;
		FileName Sfname(*sfi);
		outName = outputDirectory + "/" + Sfname.name();
		process(*sfi, outName);
	}

	cout << endl << "LPC and LSP processing finished!";
}
