#include "backend.h"

// usage: ./regressionQtChecks infile.wav
int main(int argc, const char **argv)
{
	string infile = argv[1];
	string outfile = "qtcheck.au";

	MarBackend* marBackend = new MarBackend(infile, outfile);

// temp, to give it time to finish.
// doesn't work on win32 ?  should be replaced with something
// else.
	sleep(2);

	delete marBackend;
}

