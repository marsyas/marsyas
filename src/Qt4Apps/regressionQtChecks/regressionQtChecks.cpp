#include "backend.h"

int main(int argc, const char **argv)
{
	string infile = argv[1];
	string outfile = "qtcheck.au";

	MarBackend* marBackend = new MarBackend(infile, outfile);
// temp, to give it time to finish.
	sleep(2);

	delete marBackend;
}

