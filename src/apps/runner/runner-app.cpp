#include <marsyas/CommandLineOptions.h>
#include <marsyas/system/MarSystemManager.h>
#include <marsyas/system/MarSystem.h>
#include <marsyas/realtime/runner.h>
#include <marsyas/realtime/udp_transmitter.h>
#include <marsyas/realtime/udp_receiver.h>
#include <marsyas/common_source.h>

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
#include <string>

using namespace Marsyas;
using namespace Marsyas::RealTime;
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

static int apply_controls( MarSystem *system,
                           const vector<pair<string,string>> & controls )
{
  for( const auto & mapping : controls )
  {
    const string & control_path = mapping.first;
    const string & control_value_text = mapping.second;

    MarControlPtr control = system->remoteControl(control_path);
    if (control.isInvalid())
    {
      MRSERR("Can not set control - invalid path: " << control_path);
      return 1;
    }
    int err = set_control(control, control_value_text);
    if (err)
    {
      MRSERR("Can not set control - invalid value: "
             << control_path << " = " << control_value_text);
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

struct osc_endpoint
{
  string host;
  int ip;
};

static void parse_osc_subscriptions( const string & text,
                                     vector< pair<osc_endpoint, vector<string> > > & subscriptions )
{
  istringstream stream(text);
  string host_text;
  while(getline(stream, host_text, ';'))
  {
    osc_endpoint endpoint;

    istringstream host_stream(host_text);
    if (!getline(host_stream, endpoint.host, ':'))
      continue;

    string ip_text;
    if (!getline(host_stream, ip_text, ':'))
      continue;
    size_t len = 0;
    try {
      endpoint.ip = stoi(ip_text, &len);
    } catch (...) {}
    if (len != ip_text.size())
      continue;

    vector<string> paths;
    string path;
    while(getline(host_stream, path, ','))
    {
      paths.push_back(path);
    }

    if (!paths.empty())
      subscriptions.emplace_back( endpoint, std::move(paths) );
  }
}

static void apply_osc_subscriptions( RealTime::Runner & runner,
                                     const CommandLineOptions & opt,
                                     std::vector<UdpTransmitter*> & subscribers )
{
  vector< pair<osc_endpoint, vector<string> > > subscriptions;
  parse_osc_subscriptions(opt.value<string>("OSC-send"), subscriptions);

  for ( const auto & host : subscriptions )
  {
    const osc_endpoint & endpoint = host.first;

    UdpTransmitter *subscriber = nullptr;
    for ( UdpTransmitter *transmitter : subscribers )
    {
      if (transmitter->hasDestination(endpoint.host.c_str(), endpoint.ip))
      {
        subscriber = transmitter;
        break;
      }
    }
    if (!subscriber)
    {
      subscriber = new UdpTransmitter(endpoint.host.c_str(), endpoint.ip);
      subscribers.push_back(subscriber);
    }

    for ( const string & path : host.second )
    {
      bool ok = runner.subscribe(path, subscriber);
      if (ok)
      {
        MRSDEBUG("OSC subscription: " << path << " -> " << endpoint.host << ":" << endpoint.ip);
      }
      else
      {
        MRSERR("Failed OSC subscription: " << path << " -> " << endpoint.host << ":" << endpoint.ip);
      }
    }
  }
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

    std::vector<UdpTransmitter*> osc_subscribers;
    UdpReceiver *osc_controller = nullptr;

    RealTime::Runner runner(system);
    runner.setRtPriorityEnabled(realtime);

    if (opt.has("OSC-send"))
      apply_osc_subscriptions(runner, opt, osc_subscribers);

    if (opt.has("OSC-receive"))
    {
      int port = opt.value<int>("OSC-receive");
      osc_controller = new UdpReceiver("localhost", port);
      runner.addController(osc_controller);
      osc_controller->start();
    }

    runner.start((unsigned int)ticks);
    runner.wait();

    if (osc_controller)
    {
      osc_controller->stop();
      delete osc_controller;
    }

    for ( UdpTransmitter * subscriber : osc_subscribers )
      delete subscriber;

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
    opt.define<mrs_string>("controls", 'c', "definitions",
                           "A series of control definitions of form \"name=value:name=value:...\"");
    opt.define<int>("OSC-receive", "<port>", "UDP port on which to receive OSC control messages.");
    opt.define<mrs_string>("OSC-send", "<subscriptions>",
                           "Subscriptions for OSC control change notifications, of form: "
                           "\"host:port:control,control,...;host...\".");

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
