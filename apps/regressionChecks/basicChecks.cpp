// Basic Audio Processing checks
#include "common-reg.h"
// TODO: move srate into common-reg.

void
basic_vibrato(string infile, string outfile)
{
    cout << "Testing vibrato" << endl;

    MarSystem* pnet = mng.create("Series", "pnet");
    addSource( pnet, infile );
    pnet->addMarSystem(mng.create("Vibrato", "vib"));
    addDest( pnet, outfile);

    pnet->updctrl("Vibrato/vib/mrs_real/mod_freq", 10.0);
    while (pnet->getctrl("mrs_bool/notEmpty")->toBool())
    {
        pnet->tick();
    }
    delete pnet;
}


// currently NOT WORKING!!!  (very bad gp, it should only go here when
// it works)
void
basic_pitch(string infile)
{
    MarSystem* pnet = mng.create("Series", "pnet");
    addSource( pnet, infile );
    mrs_real srate =
        pnet->getctrl("SoundFileSource/src/mrs_real/osrate")->toReal();

    pnet->addMarSystem(mng.create("ShiftInput", "sfi"));
    pnet->addMarSystem(mng.create("PitchPraat", "pitch"));
    pnet->addMarSystem(mng.create("RealvecSink", "rvSink"));

	mrs_real lowPitch = 36;
	mrs_real highPitch = 79;
    mrs_real lowFreq = pitch2hertz(lowPitch);
    mrs_real highFreq = pitch2hertz(highPitch);
    mrs_natural lowSamples = hertz2samples(lowFreq, srate);
    mrs_natural highSamples = hertz2samples(highFreq, srate);

    pnet->updctrl("PitchPraat/pitch/mrs_natural/lowSamples", lowSamples);
    pnet->updctrl("PitchPraat/pitch/mrs_natural/highSamples", highSamples);

    //  The window should be just long enough to contain three periods
    //  (for pitch detection) of MinimumPitch. E.g. if MinimumPitch is
    //  75 Hz, the window length is 40 ms and padded with zeros to reach
    //  a power of two.
    mrs_real windowSize = 3.0 / lowPitch * srate;
    pnet->updctrl("mrs_natural/inSamples", 512);
    // pnet->updctrl("ShiftInput/sfi/mrs_natural/Decimation", 256);
    pnet->updctrl("ShiftInput/sfi/mrs_natural/WindowSize",
                  powerOfTwo(windowSize));
    //pnet->updctrl("ShiftInput/sfi/mrs_natural/WindowSize", 1024);

    while (pnet->getctrl("mrs_bool/notEmpty")->toBool())
    {
        pnet->tick();
    }
    pnet->updctrl("RealvecSink/rvSink/mrs_bool/done", true);
    realvec data =
        pnet->getctrl("RealvecSink/rvSink/mrs_realvec/data")->toVec();
    for (mrs_natural i=1; i<data.getSize(); i+=2)
    {
        cout<<samples2hertz(data(i), srate)<<endl;
    }
    delete pnet;
}

