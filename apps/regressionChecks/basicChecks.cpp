// Basic Audio Processing checks
#include "common-reg.h"


void
basic_vibrato(string infile, string outfile)
{
    cout << "Testing vibrato" << endl;

    MarSystem* pnet = mng.create("Series", "pnet");

	addSource( pnet, infile );
    pnet->addMarSystem(mng.create("Vibrato", "vib"));
	addDest( pnet, outfile);

//    pnet->linkControl("mrs_natural/pos", "SoundFileSource/src/mrs_natural/pos");

    pnet->updctrl("Vibrato/vib/mrs_real/mod_freq", 10.0);
    mrs_bool isEmpty;
    while (isEmpty = pnet->getctrl("mrs_bool/notEmpty")->toBool())
    {
        pnet->tick();
    }
    delete pnet;
}



