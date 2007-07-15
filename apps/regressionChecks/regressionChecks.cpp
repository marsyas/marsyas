#include "common-reg.h"
#include "CommandLineOptions.h"

#include "basicChecks.cpp"


CommandLineOptions cmd_options;
string testName;
mrs_bool helpOpt;

void
printUsage(string progName)
{
	MRSDIAG("regressionChecks.cpp - printUsage");
	cerr << "regressionChecks, MARSYAS" << endl;
	cerr << "-------------------------" << endl;
	cerr << "   Usage:" <<endl;
	cerr << progName << " -t testName file1 file2 ... fileN" << endl;
	cerr << "   To get a list of all tests, use" << endl;
	cerr << progName << " -h" << endl <<endl;
	exit(1);
}

void
printHelp(string progName)
{
	MRSDIAG("regressionChecks.cpp - printHelp");
	cerr << "regressionChecks, MARSYAS" << endl;
	cerr << "-------------------------" << endl;
	cerr << "   Usage:" << endl;
	cerr << progName << " -t testName file1 file2 file3" << endl;
	cerr << endl;
	cerr << "Supported tests:" << endl;
	cerr << "null            : no test" <<endl;
	cerr << "audiodevices    : test audio devices" << endl;
	exit(1);
}

void
initOptions()
{
	cmd_options.addBoolOption("help", "h", false);
	cmd_options.addBoolOption("usage", "u", false);
	cmd_options.addBoolOption("verbose", "v", false);
	cmd_options.addStringOption("testName", "t", EMPTYSTRING);
}

void
loadOptions()
{
	helpOpt = (cmd_options.getBoolOption("help") ||
	           cmd_options.getBoolOption("usage") ||
	           cmd_options.getBoolOption("verbose") );
	testName = cmd_options.getStringOption("testName");
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

	if (testName == "audiodevices")
		basic_audiodevices();


	else if (testName == "null")
		basic_null();
	else
	{
		cout << "Unsupported test " << endl;
		printHelp(progName);
	}
	exit(0);
}

