#include "json_io.h"
#include <json.hh>
#include <string>
#include <iostream>
#include <sstream>

namespace Marsyas {

using std::string;

static MarSystem *system_from_json( const JSON::Value &json, MarSystemManager & manager )
{
  if (json.type() != JSON::OBJECT)
    return 0;

  JSON::Object object( std::move(json) );

  string system_type = object["type"];
  string system_name = object["name"];

  if (system_type.empty())
    return 0;

  MarSystem *system = manager.create(system_type, system_name);

  if (!system)
    return 0;

  JSON::Object controls( std::move(object["controls"]) );
  for( auto control : controls )
  {
      const std::string & ctl_name = control.first;
      const JSON::Value & ctl_value = control.second;
      switch(ctl_value.type())
      {
      case JSON::BOOL:
        system->setControl(string("mrs_bool/") + ctl_name, (bool) ctl_value);
        break;
      case JSON::INT:
        system->setControl(string("mrs_natural/") + ctl_name, (mrs_natural) (int) ctl_value);
        break;
      case JSON::FLOAT:
        system->setControl(string("mrs_real/") + ctl_name, (mrs_real) ctl_value);
        break;
      case JSON::STRING:
        system->setControl(string("mrs_string/") + ctl_name, (mrs_string) ctl_value);
        break;
      // case JSON::OBJECT:
        // TODO: link controls
      default:
        throw std::runtime_error("Unsupported control value type.");
      }
  }
  system->update();

  JSON::Array children( std::move(object["children"]) );
  int i = 0;
  for( auto value : children )
  {
    ++i;
    MarSystem *child = system_from_json(value, manager);
    if (child)
    {
      if (child->getName().empty())
      {
        std::ostringstream name;
        name << "_" << i << "_";
        child->setName(name.str());
      }
      system->addMarSystem(child);
    }
  }

  return system;
}

MarSystem *system_from_json_string( const std::string & json_string )
{
  MarSystemManager manager;
  JSON::Value data = parse_string(json_string.c_str());
  return system_from_json(data, manager);
}

MarSystem *system_from_json_file( const std::string & json_file_name )
{
  MarSystemManager manager;
  JSON::Value data = parse_file(json_file_name.c_str());
  return system_from_json(data, manager);
}

}
