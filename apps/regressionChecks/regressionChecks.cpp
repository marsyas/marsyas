#include "common-reg.h"
#include "CommandLineOptions.h"

#include "coreChecks.cpp"
#include "basicChecks.cpp"


CommandLineOptions cmd_options;
string testName;
string outputFilename;
string answerFilename;
mrs_bool helpOpt;

void
printUsage(string progName)
{
	MRSDIAG("regressionChecks.cpp - printUsage");
	cerr << "regressionChecks, MARSYAS" << endl;
	cerr << "-------------------------" << endl;
	cerr << "Usage:" <<endl;
	cerr << "    " << progName;
	cerr << " -t testName file1 file2 ... fileN" <<endl;
	cerr << "        -o outfile.wav      (optional)" << endl;
	cerr << "        -a answerfile.wav   (optional)" << endl;
	cerr << endl << "List of all tests:" << endl;
	cerr << "    " << progName << " -h" << endl <<endl;
	exit(1);
}

void
printHelp(string progName)
{
	MRSDIAG("regressionChecks.cpp - printHelp");
	cerr << "regressionChecks, MARSYAS" << endl;
	cerr << "-------------------------" << endl;
	cerr << "Usage:" << endl;
	cerr << "    " << progName << " -t testName file1 file2 file3" << endl;
	cerr << " -o outfile.wav -a answerfile.wav"<<endl;
	cerr << endl << "    *** Core System tests ***" << endl;
	cerr << "null            : no test" << endl;
	cerr << "audiodevices    : test audio devices" << endl;
	cerr << "isClose         : test if two sound files are (almost) equal" << endl;
	cerr << endl << "    *** Basic Audio Processing tests ***" << endl;
	cerr << "vibrato         : test vibrato" << endl;
	cerr << "pitch           : do pitch extraction" << endl;
	exit(1);
}

void
initOptions()
{
	cmd_options.addBoolOption("help", "h", false);
	cmd_options.addBoolOption("usage", "u", false);
	cmd_options.addBoolOption("verbose", "v", false);
	cmd_options.addStringOption("testName", "t", EMPTYSTRING);
	cmd_options.addStringOption("output", "o", EMPTYSTRING);
	cmd_options.addStringOption("answer", "a", EMPTYSTRING);
}

void
loadOptions()
{
	helpOpt = (cmd_options.getBoolOption("help") ||
	           cmd_options.getBoolOption("usage") ||
	           cmd_options.getBoolOption("verbose") );
	testName = cmd_options.getStringOption("testName");
	outputFilename = cmd_options.getStringOption("output");
	answerFilename = cmd_options.getStringOption("answer");
}

int
main(int argc, const char **argv)
{
	// get name (ie "regressionChecks" or "regressionChecks.exe")
	string progName = argv[0];
	if (argc == 1) printUsage(progName);

	// handling of command-line options
	initOptions();
	cmd_options.readOptions(argc, argv);
	loadOptions();
	if (helpOpt)
		printHelp(progName);

	// get sound files or collections
	vector<string> soundfiles = cmd_options.getRemaining();
	string fname0 = EMPTYSTRING;
	string fname1 = EMPTYSTRING;

	if (soundfiles.size() > 0)
		fname0 = soundfiles[0];
	if (soundfiles.size() > 1)
		fname1 = soundfiles[1];

	/*
		cout << "Marsyas test name = " << testName << endl;
		cout << "fname0 = " << fname0 << endl;
		cout << "fname1 = " << fname1 << endl;
	*/

	// Core system
	if (testName == "null")
		core_null();
	else if (testName == "audiodevices")
		core_audiodevices();
	else if (testName == "isClose")
		core_isClose(fname0, fname1);

	// Basic audio processing
	else if (testName == "vibrato")
		basic_vibrato(fname0, outputFilename);
	else if (testName == "pitch")
		basic_pitch(fname0);
	else if (testName == "windowing")
		basic_windowing(fname0,outputFilename);


	else
	{
		cout << "Unsupported test " << endl;
		printHelp(progName);
	}

	if (answerFilename != EMPTYSTRING)
	{
		core_isClose(outputFilename, answerFilename);
	}
	exit(0);
}

