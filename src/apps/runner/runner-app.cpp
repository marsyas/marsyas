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
#include <sstream>

using namespace Marsyas;
using namespace std;

int set_control(MarControlPtr & control, const string & text)
{
  string type = control->getType();

  if (type == "mrs_bool")
  {
    if (text == "true")
      control->setValue(true);
    else if (text == "false")
      control->setValue(false);
    else
      return 1;
  }
  else if (type == "mrs_natural")
  {
    mrs_natural i;
    size_t count = 0;
    try {
      i = std::stol(text, &count);
    }
    catch (invalid_argument) {}
    if (count == text.size())
    {
      control->setValue(i);
    }
    else
      return 1;
  }
  else if (type == "mrs_real")
  {
    mrs_real r;
    size_t count = 0;
    try {
      r = std::stod(text, &count);
    }
    catch (invalid_argument) {}
    if (count == text.size())
    {
      control->setValue(r);
    }
    else
      return 1;
  }
  else if (type == "mrs_string")
  {
    control->setValue(text);
  }
  else
    return 1;

  return 0;
}

MarControlPtr find_control(MarSystem *owner, const string & control_name)
{
  const map<string, MarControlPtr>& controls = owner->getLocalControls();
  map<string, MarControlPtr>::const_iterator control_itr;
  for(const auto & control_mapping : controls)
  {
    const MarControlPtr & control = control_mapping.second;
    string name = control->getName();
    name = name.substr( name.find('/') + 1 );

    if (name == control_name)
      return control;
  }
  return MarControlPtr();
}

static int apply_controls( MarSystem *system,
                           const vector<pair<string,string>> & controls )
{
  for( const auto & mapping : controls )
  {
    const string & control_name = mapping.first;
    const string & control_value_text = mapping.second;

    MarControlPtr control = find_control(system, control_name);
    if (control.isInvalid())
    {
      MRSERR("Can not set control - invalid name: " << control_name);
      return 1;
    }
    int err = set_control(control, control_value_text);
    if (err)
    {
      MRSERR("Can not set control - invalid value: "
             << control_name << " = " << control_value_text);
      return err;
    }
  }

  return 0;
}

static void tokenize_control_description( const string & description,
                                          vector<pair<string,string>> & controls )
{
  string::size_type pos = 0;
  while(pos < description.size())
  {
    string control_name;
    string control_value;
    string::size_type next_pos;

    next_pos = description.find('=', pos);
    control_name = description.substr(pos, next_pos - pos);

    if (next_pos == string::npos)
      break;
    pos = next_pos + 1;

    next_pos = description.find(':', pos);
    control_value = description.substr(pos, next_pos - pos);

    controls.push_back( make_pair(control_name, control_value) );

    if (next_pos == string::npos)
      break;
    pos = next_pos + 1;
  }
}

static int apply_control_options( MarSystem * system,
                                  const CommandLineOptions & opt )
{
  mrs_real sr = opt.value<mrs_real>("samplerate");
  mrs_natural block = opt.value<mrs_natural>("block");
  string control_description = opt.value<mrs_string>("controls");

  if (sr > 0)
    system->setControl("mrs_real/israte", sr);

  if (block > 0)
    system->setControl("mrs_natural/inSamples", block);

  vector<pair<string,string>> controls;
  tokenize_control_description(control_description, controls);
  int error = apply_controls(system, controls);

  if (error)
    return error;

  system->update();

  return 0;
}

static MarSystem *load_network( const string system_filename )
{

  auto string_ends_with =
      []( const string & str, const string & ending ) -> bool
  {
    if (str.length() < ending.length())
      return false;
    return str.compare( str.length() - ending.length(), ending.length(), ending ) == 0;
  };

  MarSystem* system = 0;

  if (string_ends_with(system_filename, ".mrs"))
  {
#ifdef MARSYAS_HAS_SCRIPT
    system = system_from_script(system_filename);
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

  return system;
}

int run( MarSystem *system, const CommandLineOptions & opt )
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

    bool realtime = opt.value<bool>("realtime");

    RealTime::Runner runner(system);
    runner.setRtPriorityEnabled(realtime);

    runner.start((unsigned int)ticks);
    runner.wait();

    return 0;
}

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

int main( int argc, const char *argv[] )
{
    CommandLineOptions opt;
    opt.define<bool>("usage", 'u', "", "Print usage.");
    opt.define<bool>("help", 'h', "", "Print usage and options.");
    opt.define<bool>("realtime", 'r', "", "Use real-time thread priority.");
    opt.define<int>("count", 'N', "<number>", "Perform <number> amount of ticks.");
    opt.define<mrs_real>("samplerate", 's', "<number>", "Override sampling rate.");
    opt.define<mrs_natural>("block", 'b', "<samples>", "Block size in samples.");
    opt.define<mrs_string>("controls", 'c', "", "A series of control definitions of form \"name=value:name=value:...\"");

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

    MarSystem *system = load_network(system_filename);
    if (!system) {
        cerr << "Could not load network definition file: "
             << system_filename << endl;
        return 1;
    }

    int error = apply_control_options(system, opt);
    if (error)
      return error;

    return run(system, opt);
}
