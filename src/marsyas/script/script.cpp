#include "script.h"
#include "parser.h"
#include <marsyas/system/MarSystemManager.h>
#include <cassert>
#include <string>
#include <sstream>
#include <map>
#include <iostream>
using namespace std;
using std::string;
using std::stringstream;
using std::map;

namespace Marsyas {

class script_translator
{
  MarSystemManager & m_manager;
  std::vector< std::pair<MarSystem*, node> > m_controls;

  MarSystem *translate_actor( const node & n )
  {
    if (n.tag != ACTOR_NODE)
      return 0;

    assert(n.components.size() == 3);
    assert(n.components[0].tag == STRING_NODE || n.components[0].tag == GENERIC_NODE);
    assert(n.components[1].tag == STRING_NODE );
    assert(n.components[2].tag == GENERIC_NODE);

    std::string name, type;

    if (n.components[0].tag == STRING_NODE)
      name = std::move(n.components[0].s);

    type = std::move(n.components[1].s);

    MarSystem *system = m_manager.create(type, name);

    const node & system_def = n.components[2];
    if(system_def.components.size() == 2)
    {
      const node & controls = system_def.components[0];
      const node & children = system_def.components[1];

      m_controls.push_back( std::make_pair(system, controls) );

      int child_idx = 0;
      for( const node & child : children.components )
      {
        MarSystem *child_system = translate_actor(child);
        if (child_system)
        {
          if (child_system->getName().empty())
          {
            stringstream name;
            name << "child" << child_idx;
            child_system->setName(name.str());
          }
          system->addMarSystem(child_system);
          child_idx++;
        }
      }
    }

    return system;
  }

  void apply_controls()
  {
    for( const auto & mapping : m_controls )
    {
      MarSystem * system = mapping.first;
      const node & controls = mapping.second;

      for( const node & control : controls.components)
      {
        assert(control.tag == CONTROL_NODE);
        assert(control.components.size() == 2);
        assert(control.components[0].tag == STRING_NODE);

        const std::string & control_name = control.components[0].s;
        const node & control_value = control.components[1];

        //cout << "setting: " << system->getAbsPath() << ": " << control_name << endl;

        switch(control_value.tag)
        {
        case BOOL_NODE:
          //cout << "bool" << control_value.v.b << endl;
          system->setControl(std::string("mrs_bool/") + control_name,
                             control_value.v.b);
          break;
        case INT_NODE:
          //cout << "int" << endl;
          system->setControl(std::string("mrs_natural/") + control_name,
                             (mrs_natural) control_value.v.i);
          break;
        case REAL_NODE:
          //cout << "real" << endl;
          system->setControl(std::string("mrs_real/") + control_name,
                             (mrs_real) control_value.v.r);
          break;
        case STRING_NODE:
          //cout << "string" << endl;
          system->setControl(std::string("mrs_string/") + control_name,
                             control_value.s);
          break;
        default:
          bool control_value_is_valid = false;
          assert(control_value_is_valid);
        }
      }

      system->update();
    }

    m_controls.clear();
  }

public:

  script_translator( MarSystemManager & manager ):
    m_manager(manager)
  {}

  MarSystem *translate( const node & syntax_tree )
  {
    MarSystem *system = translate_actor(syntax_tree);

    if (system) apply_controls();

    return system;
  }
};

MarSystem *system_from_script(std::istream & script_stream)
{
  Parser parser(script_stream);
  parser.parse();
  const node &tree = parser.parsed();

  MarSystemManager manager;
  script_translator translator(manager);
  MarSystem *system = translator.translate(tree);

  if (system && system->getName().empty())
    system->setName("network");

  return system;
}

}
