#include <marsyas/CommandLineOptions.h>
#include <marsyas/system/MarSystemManager.h>
#include <marsyas/system/MarSystem.h>
#include <marsyas/realtime/runner.h>
#ifdef MARSYAS_HAS_SCRIPT
# include <marsyas/script/script.h>
#endif
#ifdef MARSYAS_HAS_JSON
# include <marsyas/json_io.h>
#endif

#include <fstream>
#include <iostream>
#include <functional>

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

    auto string_ends_with =
        []( const string & str, const string & ending ) -> bool
    {
      if (str.length() < ending.length())
        return false;
      return str.compare( str.length() - ending.length(), ending.length(), ending ) == 0;
    };

    MarSystem* system = 0;

    if (string_ends_with(system_filename, ".mars"))
    {
#ifdef MARSYAS_HAS_SCRIPT
      ifstream system_istream(system_filename);
      system = system_from_script(system_istream);
#endif
    }
    else if (string_ends_with(system_filename, ".mpl"))
    {
      ifstream system_istream(system_filename);
      MarSystemManager mng;
      system = mng.getMarSystem(system_istream);
    }
    else if (string_ends_with(system_filename, ".json"))
    {
#ifdef MARSYAS_HAS_JSON
      system = system_from_json_file(system_filename);
#endif
    }

    if (!system) {
        cerr << "Could not load network definition file: "
             << system_filename << endl;
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
