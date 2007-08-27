// Basic Audio Processing checks
#include "common-reg.h"

void
basic_vibrato(string infile, string outfile)
{
	MarSystem* pnet = mng.create("Series", "pnet");
	addSource( pnet, infile );
	pnet->addMarSystem(mng.create("Vibrato", "vib"));
	addDest( pnet, outfile);

	pnet->updctrl("Vibrato/vib/mrs_real/mod_freq", 10.0);
	while (pnet->getctrl("mrs_bool/notEmpty")->to<mrs_bool>())
	{
		pnet->tick();
	}
	delete pnet;
}


void
basic_windowing(string infile, string outfile)
{
	MarSystem* pnet = mng.create("Series", "pnet");
	addSource( pnet, infile );
	pnet->addMarSystem(mng.create("Windowing", "win"));
	pnet->updctrl("Windowing/win/mrs_string/type", "Hanning");
	addDest( pnet, outfile);

	while (pnet->getctrl("mrs_bool/notEmpty")->to<mrs_bool>())
	{
		pnet->tick();
	}
	delete pnet;
}
