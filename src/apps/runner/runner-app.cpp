#include <marsyas/CommandLineOptions.h>
#include <marsyas/core/MarSystemManager.h>
#include <marsyas/core/MarSystem.h>
#include <marsyas/realtime/runner.h>

#include <fstream>
#include <iostream>

using namespace Marsyas;
using namespace std;

int printUsage()
{
    cout << "Usage: marsyas-run <plugin-file> [options]" << endl;
    return 0;
}

int run( const string system_filename, const CommandLineOptions & opt );

int main( int argc, const char *argv[] )
{
    CommandLineOptions opt;
    opt.addBoolOption("realtime", "r", false);
    opt.addBoolOption("finite", "f", false);
    opt.addNaturalOption("ticks", "t", 0);
    opt.addRealOption("samplerate", "s", 0);
    opt.addNaturalOption("block", "b", 0);
    opt.readOptions(argc, argv);

    const vector<string> & arguments = opt.getRemaining();
    if (arguments.size() < 1)
        return printUsage();

    string system_filename = arguments[0];

    return run(system_filename, opt);
}

int run( const string system_filename, const CommandLineOptions & opt )
{
    ifstream system_istream(system_filename);
    MarSystemManager mng;
    MarSystem* system = mng.getMarSystem(system_istream);
    if (!system) {
        cerr << "Could not load filesystem file:" << system;
        return 1;
    }

    bool realtime = opt.getBoolOption("realtime");
    unsigned int ticks = std::max((mrs_natural)0, opt.getNaturalOption("ticks"));
    //bool finite = opt.getBoolOption("finite");
    mrs_real sr = opt.getRealOption("samplerate");
    mrs_natural block = opt.getNaturalOption("block");

    if (sr > 0)
      system->setControl("mrs_real/israte", sr);
    if (block > 0)
      system->setControl("mrs_natural/inSamples", block);
    system->update();

    RealTime::Runner runner(system);
    runner.setRtPriorityEnabled(realtime);

    runner.start(ticks);
    runner.wait();

    return 0;
}
