#include "CommandLineOptions.h"

using namespace std;
using namespace Marsyas;

CommandLineOptions cmd_options;

int helpOpt;
int usageOpt;
mrs_natural naturalOpt;
mrs_real realOpt;
mrs_string stringOpt;

void
printUsage()
{
	MRSDIAG("commandOptions.cpp - printUsage");
	cerr << "Usage: commandOptions " << "file1 file2 file3" << endl;
	cerr << endl;
	cerr << "where file1, ..., fileN are sound files in a MARSYAS supported format" << endl;
	exit(1);
}

void
printHelp()
{
	MRSDIAG("commandOptions.cpp - printHelp");
	cerr << "commandOptions: Sample Program"<< endl;
	cerr << "------------------------------" << endl;
	cerr << endl;
	cerr << "Usage: commandOptions file1 file2 file3" << endl;
	cerr << endl;
	cerr << "where file1, ..., fileN are sound files in a Marsyas supported format" << endl;
	cerr << "Help Options:" << endl;
	cerr << "-u --usage		 : display short usage info" << endl;
	cerr << "-h --help		 : display this information " << endl;
	cerr << "-n --natural		 : sets a `natural' variable " << endl;
	cerr << "-r --real		 : sets a `real' variable " << endl;
	cerr << "-s --string		 : sets a `string' variable " << endl;
	exit(1);
}

void
initOptions()
{
	cmd_options.addBoolOption("help", "h", false);
	cmd_options.addBoolOption("usage", "u", false);
	cmd_options.addNaturalOption("natural", "n", 9);
	cmd_options.addRealOption("real", "r", 3.1415927);
	cmd_options.addStringOption("string", "s", "hello world");
}

void
loadOptions()
{
	helpOpt = cmd_options.getBoolOption("help");
	usageOpt = cmd_options.getBoolOption("usage");
	naturalOpt = cmd_options.getNaturalOption("natural");
	realOpt = cmd_options.getRealOption("real");
	stringOpt = cmd_options.getStringOption("string");
}



void doStuff(string printMe)
{
	cout<<printMe<<endl;
}

int main(int argc, const char **argv)
{
	initOptions();
	cmd_options.readOptions(argc,argv);
	loadOptions();

	vector<string> soundfiles = cmd_options.getRemaining();

	if (helpOpt)
		printHelp();

	if ( (usageOpt) || (argc==1) )
		printUsage();

	cout<<"Command-line options were:"<<endl;
	cout<<"		--natural: "<<naturalOpt<<endl;
	cout<<"		--real: "<<realOpt<<endl;
	cout<<"		--string: "<<stringOpt<<endl;
	cout<<"(these may simply be the default values)"<<endl;
	cout<<endl;
	cout<<"The rest of the command-line arguments were: "<<endl;

	vector<string>::iterator sfi;
	for (sfi = soundfiles.begin(); sfi != soundfiles.end(); ++sfi)
	{
		doStuff( *sfi );
	}
}

