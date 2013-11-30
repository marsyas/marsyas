#include "script.h"
#include "parser.h"
#include "operation_processor.hpp"
#include <marsyas/system/MarSystemManager.h>

#include <cassert>
#include <string>
#include <sstream>
#include <map>
#include <stack>
#include <algorithm>
#include <iostream>

using namespace std;

namespace Marsyas {

class script_translator
{
  typedef std::vector< std::pair<MarSystem*, node> > control_map_t;
  MarSystemManager & m_manager;
  std::stack<control_map_t> m_controls;
  MarSystem *m_root_system;

  MarSystem *translate_actor( const node & n )
  {
    if (n.tag != ACTOR_NODE && n.tag != PROTOTYPE_NODE)
      return 0;

    assert(n.components.size() == 3);
    assert(n.components[0].tag == ID_NODE || n.components[0].tag == GENERIC_NODE);
    assert(n.components[1].tag == ID_NODE );
    assert(n.components[2].tag == GENERIC_NODE);

    std::string name, type;

    if (n.components[0].tag == ID_NODE)
      name = std::move(n.components[0].s);

    type = std::move(n.components[1].s);

    if (n.tag == PROTOTYPE_NODE && name.empty())
    {
      MRSERR("Prototype must be given a name!");
      return 0;
    }

    MarSystem *system = m_manager.create(type, name);

    if (n.tag == PROTOTYPE_NODE)
      m_controls.emplace();

    assert(m_controls.size());
    control_map_t & control_map = m_controls.top();

    const node & system_def = n.components[2];
    if(system_def.components.size() == 2)
    {
      const node & controls = system_def.components[0];
      const node & children = system_def.components[1];

      control_map.emplace_back(system, controls);

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

    if (n.tag == PROTOTYPE_NODE)
    {
      assert(!name.empty());
      m_root_system = system;
      apply_controls(control_map);
      m_controls.pop();
      m_manager.registerPrototype(name, system);
      m_root_system = 0;
      return 0;
    }

    return system;
  }

  void apply_controls( const control_map_t & controls )
  {
    for( const auto & mapping : controls )
    {
      MarSystem * system = mapping.first;
      const node & controls = mapping.second;

      for( const node & control : controls.components)
      {
        assert(control.tag == CONTROL_NODE);
        assert(control.components.size() == 2);
        assert(control.components[0].tag == ID_NODE);

        const std::string & control_name = control.components[0].s;
        const node & control_value = control.components[1];

        set_control(system, control_name, control_value);
      }

      system->update();
    }
  }



  void set_control( MarSystem * system,
                    const std::string & control_description,
                    const node & control_value )
  {
    assert(!control_description.empty());

    MarControlPtr source_control;

    if (control_value.tag == OPERATION_NODE)
    {
        // cout << "Creating operation processor..." << endl;

        ScriptOperationProcessor::operation *op = translate_operation(system, control_value);
        if (!op)
          return;

        // cout << "Processor created." << endl;

        ScriptOperationProcessor *processor = new ScriptOperationProcessor("processor");
        processor->setOperation(op);
        source_control = find_control(processor, "result");

        system->attachMarSystem(processor);
    }
    else
    {
      // cout << "Translating control value..." << endl;

      source_control = translate_control_value(system, control_value);
    }

    if (source_control.isInvalid()) {
      MRSERR("Can not set control - invalid value: "
             << system->getAbsPath() << control_description);
      return;
    }

    string control_name = control_description;
    bool create = control_name[0] == '+';
    if (create)
      control_name = control_name.substr(1);

    bool link = source_control->getMarSystem() != nullptr;

    static const bool do_not_update = false;

    std::string control_path = source_control->getType() + '/' + control_name;
    MarControlPtr control = system->getControl( control_path );

    if (create)
    {
      //cout << "Creating:" << system->getAbsPath() << control_path << endl;
      if (!control.isInvalid())
      {
        MRSERR("ERROR: Can not add control - "
               << "same control already exists: " << system->getAbsPath() << control_path);
        return;
      }
      bool created = system->addControl(control_path, *source_control, control);
      if (!created)
      {
        MRSERR("ERROR: Failed to create control: " << system->getAbsPath() << control_path);
        return;
      }
      if (link)
      {
        control->linkTo(source_control, do_not_update);
      }
    }
    else
    {
      /*
      cout << "Setting:" << system->getAbsPath() << control_path
           << " = " << source_control
           << endl;
           */
      if (control.isInvalid())
      {
        MRSERR("ERROR: Can not set control - "
               << "it does not exist: " << system->getAbsPath() << control_path);
        return;
      }
      if (link)
      {
        control->linkTo(source_control, do_not_update);
      }
      else
      {
        control->setValue( source_control );
      }
    }
  }

  MarControlPtr translate_control_value( MarSystem * anchor, const node & control_value )
  {
    switch(control_value.tag)
    {
    case BOOL_NODE:
      return MarControlPtr(control_value.v.b);
    case INT_NODE:
      return MarControlPtr((mrs_natural) control_value.v.i);
    case REAL_NODE:
      return MarControlPtr((mrs_real) control_value.v.r);
    case STRING_NODE:
    {
      return MarControlPtr(control_value.s);
    }
    case MATRIX_NODE:
    {
      mrs_natural row_count = 0, column_count = 0;
      row_count = (mrs_natural) control_value.components.size();
      for( const auto & row : control_value.components )
      {
        mrs_natural row_column_count = (mrs_natural) row.components.size();
        column_count = std::max(column_count, row_column_count);
      }
      realvec matrix(row_count, column_count);
      for(mrs_natural r = 0; r < row_count; ++r)
      {
        const auto & row = control_value.components[r];
        mrs_natural row_column_count = (mrs_natural) row.components.size();
        for(mrs_natural c = 0; c < row_column_count; ++c)
        {
          static const bool invalid_matrix_value = false;
          switch(row.components[c].tag)
          {
          case REAL_NODE:
            matrix(r, c) = row.components[c].v.r; break;
          case INT_NODE:
            matrix(r, c) = (mrs_real) row.components[c].v.i; break;
          default:
            assert(invalid_matrix_value);
          }
        }
      }
      return MarControlPtr(matrix);
    }
    case ID_NODE:
    {
      string link_path = control_value.s;
      assert(!link_path.empty());
      return find_remote_control(anchor, link_path);
    }
    default:
      bool control_value_is_valid = false;
      assert(control_value_is_valid);
    }
    return MarControlPtr();
  }

  ScriptOperationProcessor::operation *translate_operation( MarSystem *anchor, const node & op_node )
  {
    if (op_node.tag == OPERATION_NODE)
    {
      assert(op_node.s.size());
      assert(op_node.components.size() == 2);

      //cout << "-- Translating operation: " << op_node.s[0] << endl;

      auto left_operand = translate_operation(anchor, op_node.components[0]);
      auto right_operand = translate_operation(anchor, op_node.components[1]);

      if (!left_operand || !right_operand)
        return nullptr;

      auto op = new ScriptOperationProcessor::operation;
      op->op = op_node.s[0];
      op->left_operand = left_operand;
      op->right_operand = right_operand;
      return op;
    }
    else
    {
      //cout << "-- Translating value..." << endl;
      MarControlPtr value = translate_control_value(anchor, op_node);
      if (value.isInvalid())
      {
        MRSERR("Can not parse expression: invalid control value!");
        return nullptr;
      }
      auto op = new ScriptOperationProcessor::operation;
      op->value = value;
      return op;
    }

    return nullptr;
  }

  MarControlPtr find_remote_control( MarSystem *anchor, const string & path )
  {
#if 0
    cout << "-- Searching for remote control: " << endl
         << "-- -- " << path << endl
         << "-- -- at " << anchor->getAbsPath() << endl
         << "-- -- under " << m_root_system->getAbsPath() << endl;
#endif
    if (path.empty())
      return MarControlPtr();

    vector<string> path_elements;
    string control_name;
    bool absolute;
    split_control_path(path, path_elements, control_name, absolute);

    if (control_name.empty())
      return MarControlPtr();

    MarSystem *parent = absolute ? m_root_system : anchor;
    MarSystem *control_owner = find_child(parent, path_elements);
    if (!control_owner)
        return MarControlPtr();

    // cout << "-- Got control owner" << endl;
    return find_control(control_owner, control_name);
  }

  MarControlPtr find_control(MarSystem *owner, const string & control_name)
  {
#if 0
    cout << "-- Searching for local control: " << endl
         << "-- -- " << control_name << endl
         << "-- -- at " << owner->getAbsPath() << endl;
#endif
    const map<string, MarControlPtr>& controls = owner->getLocalControls();
    map<string, MarControlPtr>::const_iterator control_itr;
    for(const auto & control_mapping : controls)
    {
      const MarControlPtr & control = control_mapping.second;
      string name = control->getName();
      //cout << "-- Comparing control: " << name << endl;
      name = name.substr( name.find('/') + 1 );

      if (name == control_name)
        return control;
    }
    return MarControlPtr();
  }

  MarSystem * find_child(MarSystem *parent,
                         const vector<string> & path_elements )
  {
#if 0
    cout << "-- Searching for child:" << endl
         << "-- -- ";
    for(const auto & elem : path_elements) cout << (elem + "/");
    cout << endl;
    cout << "-- -- " << "under " << parent->getAbsPath() << endl;
#endif

    MarSystem *system = parent;
    for(const auto & path_element : path_elements)
    {
      MarSystem *matching_child = 0;
      vector<MarSystem*> children = system->getChildren();
      for(MarSystem *child : children)
      {
        if (child->getName() == path_element)
        {
          matching_child = child;
          break;
        }
      }
      if (matching_child)
        system = matching_child;
      else
        return 0;
    }
    return system;
  }

  void split_control_path(const string & path,
                          vector<string> & components,
                          string & last_component,
                          bool & is_absolute)
  {
    is_absolute = !path.empty() && path[0] == '/';
    string::size_type pos = is_absolute ? 1 : 0;

    while(pos < path.length())
    {
      string::size_type separator_pos = path.find('/', pos);

      if (separator_pos == string::npos)
        break;
      else
        components.push_back( path.substr(pos, separator_pos - pos) );

      pos = separator_pos + 1;
    }

    last_component = path.substr(pos, path.size() - pos);
  }

public:

  script_translator( MarSystemManager & manager ):
    m_manager(manager),
    m_root_system(0)
  {}

  MarSystem *translate( const node & syntax_tree )
  {
    m_controls.emplace();

    m_root_system = translate_actor(syntax_tree);

    assert(m_controls.size());

    if (m_root_system)
      apply_controls( m_controls.top() );

    return m_root_system;
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
