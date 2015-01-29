#include <marsyas/system/MarSystemManager.h>
#include <marsyas/CommandLineOptions.h>

#include <iostream>

#define STR(x) #x
#define QUOTE(x) STR(x)

#ifndef MARSYAS_VERSION
#define MARSYAS_VERSION Unknown
#endif

using namespace std;
using namespace Marsyas;

ostream & message(ostream & stream = cout)
{
  stream << "marsyas-info: ";
  return stream;
}

ostream & error(ostream & stream = cerr)
{
  stream << "marsyas-info: *** error: ";
  return stream;
}

void print_help()
{
  message() << "Available commands: " << endl
            << "help : Print this help." << endl
            << "version : Print Marsyas version." << endl
            << "list marsystems : List all MarSystems." << endl
            << "list controls <MarSystem type> : List controls of a MarSystem type." << endl
               ;
}

void print_version()
{
  message() << "Marsyas version " << QUOTE(MARSYAS_VERSION) << endl;
}

int list_marsystems()
{
  typedef std::map<std::string, MarSystem*> registry_type;

  MarSystemManager manager;

  const registry_type & registry = manager.registry();
  for (registry_type::const_iterator it = registry.begin(); it != registry.end(); ++it)
  {
    string marsystem_type = it->first;
    cout << marsystem_type << endl;
  }

  return 0;
}

int list_controls(int argc, char **argv)
{
  if (argc < 1)
  {
    error() << "You asked to list controls, but you didn't say of which MarSystem." << endl
            << "I expect: marsyas-info list controls <MarSystem type>" << endl;
    return 1;
  }

  MarSystemManager mng;

  string marsystem_type(argv[0]);
  MarSystem *system = mng.getPrototype(marsystem_type);
  if (!system)
  {
    error() << "No such MarSystem type: \"" << marsystem_type << "\"" << endl;
    return 1;
  }

  typedef std::map<std::string, MarControlPtr> control_map_t;

  const control_map_t &controls = system->controls();
  for(control_map_t::const_iterator it = controls.begin(); it != controls.end(); ++it)
  {
    const MarControlPtr & control = it->second;
    cout << control->id() << " (";
    string type = control->getType();
    if (type == "mrs_bool")
      cout << "boolean, default=" << (control->to<bool>() ? "true" : "false");
    else if (type == "mrs_natural")
      cout << "integer, default=" << control->to<mrs_natural>();
    else if (type == "mrs_real")
      cout << "real, default=" << control->to<mrs_real>();
    else if (type == "mrs_realvec")
      cout << "array";
    else if (type == "mrs_string")
      cout << "string, default=\"" << control->to<string>() << "\"";
    else
      cout << "type: " << type;
    cout << ")" << endl;
  }

  return 0;
}

int list(int argc, char **argv)
{
  if (argc < 1)
  {
    error() << "You asked to list something, but you didn't say what." << endl
            << "I expect: marsyas-info list <object>" << endl;
    return 1;
  }

  string object(argv[0]);
  if (object == "marsystems")
    return list_marsystems();
  if (object == "controls")
    return list_controls(argc-1, argv+1);
  else
  {
    error() << "I don't know how to list \"" << object << "\"" << endl;
    return 1;
  }
}

int main(int argc, char **argv)
{
  if (argc < 2)
  {
    print_version();
    return 0;
  }

  string command(argv[1]);

  if (command == "help" || command == "-h" || command == "--help")
  {
    print_help();
  }
  else if (command == "version" || command == "-v" || command == "--version")
  {
    cout << QUOTE(MARSYAS_VERSION) << endl;
  }
  else if (command == "list")
  {
    return list(argc-2, argv+2);
  }
  else
  {
    error() << "Unknown command: " << command << endl;
    return 1;
  }

  return 0;
}
