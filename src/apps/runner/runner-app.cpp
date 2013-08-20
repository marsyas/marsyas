#include <marsyas/CommandLineOptions.h>
#include <marsyas/core/MarSystemManager.h>
#include <marsyas/core/MarSystem.h>
#include <marsyas/realtime/runner.h>

#include <fstream>
#include <iostream>

using namespace Marsyas;
using namespace std;

static void print_usage()
{
    cout << "Usage: marsyas-run <plugin-file> [options]" << endl;
}

static void print_help( const CommandLineOptions & opt )
{
  print_usage();
  cout << "Options:" << endl;
  opt.print();
}

int run( const string system_filename, const CommandLineOptions & opt );

int main( int argc, const char *argv[] )
{
    CommandLineOptions opt;
    opt.define<bool>("usage", 'u', "", "Print usage.");
    opt.define<bool>("help", 'h', "", "Print usage and options.");
    opt.define<bool>("realtime", 'r', "", "Use real-time thread priority.");
    opt.define<int>("count", 'N', "<number>", "Perform <number> amount of ticks.");
    opt.define<mrs_real>("samplerate", 's', "<number>", "Override sampling rate.");
    opt.define<mrs_natural>("block", 'b', "<samples>", "Block size in samples.");

    if (!opt.readOptions(argc, argv))
      return 1;

    if (opt.value<bool>("usage"))
    {
      print_usage();
      return 0;
    }

    if (opt.value<bool>("help"))
    {
      print_help(opt);
      return 0;
    }

    const vector<string> & arguments = opt.getRemaining();
    if (arguments.size() < 1)
    {
        print_usage();
        return 0;
    }

    string system_filename = arguments[0];

    return run(system_filename, opt);
}

int run( const string system_filename, const CommandLineOptions & opt )
{
    int ticks = 0;
    if (opt.has("count"))
    {
      ticks = opt.value<int>("count");
      if (ticks < 1)
      {
        cerr << "Invalid value for option 'count' (must be > 0)." << endl;
        return 1;
      }
    }

    ifstream system_istream(system_filename);
    MarSystemManager mng;
    MarSystem* system = mng.getMarSystem(system_istream);
    if (!system) {
        cerr << "Could not load filesystem file:" << system;
        return 1;
    }

    bool realtime = opt.value<bool>("realtime");
    mrs_real sr = opt.value<mrs_real>("samplerate");
    mrs_natural block = opt.value<mrs_natural>("block");

    if (sr > 0)
      system->setControl("mrs_real/israte", sr);
    if (block > 0)
      system->setControl("mrs_natural/inSamples", block);
    system->update();

    RealTime::Runner runner(system);
    runner.setRtPriorityEnabled(realtime);

    runner.start((unsigned int)ticks);
    runner.wait();

    return 0;
}
