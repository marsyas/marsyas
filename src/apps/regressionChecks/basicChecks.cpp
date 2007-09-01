// Basic Audio Processing checks
#include "common-reg.h"

// TODO: move
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
basic_delay(string infile, string outfile)
{
	MarSystem* pnet = mng.create("Series", "pnet");
	addSource( pnet, infile );
	pnet->addMarSystem(mng.create("Delay", "delay"));
	pnet->updctrl("Delay/delay/mrs_natural/delaySamples", 16);
	pnet->updctrl("Delay/delay/mrs_real/feedback", (mrs_real) 0.5);
	addDest( pnet, outfile);

	while (pnet->getctrl("mrs_bool/notEmpty")->to<mrs_bool>())
	{
		pnet->tick();
	}
	delete pnet;
}


// Produces "settings that are not supported in Marsyas" .au files.  :(
void
basic_downsample(string infile, string outfile)
{
	MarSystem* pnet = mng.create("Series", "pnet");
	addSource( pnet, infile );
	pnet->addMarSystem(mng.create("DownSampler", "down"));
	addDest( pnet, outfile);

	while (pnet->getctrl("mrs_bool/notEmpty")->to<mrs_bool>())
	{
		pnet->tick();
	}
	delete pnet;
}

void
basic_negative(string infile, string outfile)
{
	MarSystem* pnet = mng.create("Series", "pnet");
	addSource( pnet, infile );
	pnet->addMarSystem(mng.create("Negative", "inv"));
	addDest( pnet, outfile);

	while (pnet->getctrl("mrs_bool/notEmpty")->to<mrs_bool>())
	{
		pnet->tick();
	}
	delete pnet;
}

void
basic_shifter(string infile, string outfile)
{
	MarSystem* pnet = mng.create("Series", "pnet");
	addSource( pnet, infile );
	pnet->addMarSystem(mng.create("Shifter", "shift"));
	pnet->updctrl("Shifter/shift/mrs_natural/shift", 16);
	addDest( pnet, outfile);

	while (pnet->getctrl("mrs_bool/notEmpty")->to<mrs_bool>())
	{
		pnet->tick();
	}
	delete pnet;
}

void
basic_shiftInput(string infile, string outfile)
{
cout<<"CURRENTLY BROKEN!";
/*
	MarSystem* pnet = mng.create("Series", "pnet");
	addSource( pnet, infile );
	pnet->addMarSystem(mng.create("ShiftInput", "shift"));
	pnet->updctrl("mrs_natural/inSamples", 256);
	pnet->updctrl("ShiftInput/shift/mrs_natural/WindowSize", 512);
	addDest( pnet, outfile);

	while (pnet->getctrl("mrs_bool/notEmpty")->to<mrs_bool>())
	{
		pnet->tick();
	}
	delete pnet;
*/
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
