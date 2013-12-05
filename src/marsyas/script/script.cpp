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
  typedef std::pair<MarSystem*, std::vector<node>> control_mapping_t;
  typedef std::vector<control_mapping_t> control_map_t;

  struct context
  {
    context(MarSystem *system): root_system(system) {}
    MarSystem *root_system;
    control_map_t control_map;
  };

  enum context_policy
  {
    own_context,
    inherit_context
  };

  MarSystemManager & m_manager;
  std::stack<context> m_context_stack;

  context & this_context() { return m_context_stack.top(); }

  bool handle_directive( const node & directive_node )
  {
    //cout << "handling directive: " << directive_node.tag << endl;

    switch(directive_node.tag)
    {
    case INCLUDE_DIRECTIVE:
      return handle_include_directive(directive_node);
    default:
      MRSERR("Invalid directive: " << directive_node.tag);
    }
    return false;
  }

  bool handle_include_directive( const node & directive_node )
  {
    int component_count = (int) directive_node.components.size();
    assert(component_count == 1 || component_count == 2);

    string filename, id;

    assert(directive_node.components[0].tag == STRING_NODE);
    filename = directive_node.components[0].s;
    assert(!filename.empty());

    if (directive_node.components.size() > 1)
    {
      assert(directive_node.components[1].tag == ID_NODE);
      id = directive_node.components[1].s;
    }

    ifstream file(filename.c_str());
    if (!file.is_open())
    {
      MRSERR("Could not open included file: " << filename);
      return false;
    }

    MarSystem *system = system_from_script(file);
    if (!system)
    {
      MRSERR("Error in included file: " << filename);
      return false;
    }

    if (id.empty())
      id = system->getName();

    if (id.empty())
    {
      MRSERR("Included network has no name and no alias provided in include statement.");
      delete system;
      return false;
    }

    //cout << "Registering prototype: " << system->getName() << " as " << id << endl;
    m_manager.registerPrototype(id, system);
    return true;
  }

  MarSystem *translate_actor( const node & n, context_policy policy )
  {
    //cout << "handling actor: " << n.tag << endl;

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

    MarSystem *system = m_manager.create(type, name);

    if (!system)
      return nullptr;

    if (policy == own_context)
      m_context_stack.emplace(system);
    else
      assert(!m_context_stack.empty());

    const node & system_def = n.components[2];
    int child_idx = 0;

    this_context().control_map.emplace_back(system, vector<node>());
    int control_map_index = this_context().control_map.size() - 1;

    for( const node & system_def_element : system_def.components )
    {
      switch(system_def_element.tag)
      {
      case ACTOR_NODE:
      {
        MarSystem *child_system = translate_actor(system_def_element, inherit_context);
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
        break;
      }
      case PROTOTYPE_NODE:
      {
        translate_actor(system_def_element, own_context);
        break;
      }
      case STATE_NODE:
      {
        this_context().control_map[control_map_index].second.push_back(system_def_element);
        break;
      }
      case CONTROL_NODE:
        this_context().control_map[control_map_index].second.push_back(system_def_element);
        break;
      default:
        assert(false);
      }
    }

    //current_context().control_map.emplace_back(system, std::move(controls));

    if (policy == own_context)
    {
      apply_controls(this_context().control_map);
      m_context_stack.pop();
    }

    if (n.tag == PROTOTYPE_NODE)
    {
      assert(!name.empty());
      m_manager.registerPrototype(name, system);
    }

    return system;
  }


  void apply_controls( const control_map_t & control_map )
  {
    for( const auto & mapping : control_map )
    {
      MarSystem * system = mapping.first;
      const auto & controls = mapping.second;

      //cout << "Applying controls for: " << system->getAbsPath() << endl;

      for( const node & control_node : controls)
      {
        switch(control_node.tag)
        {
        case CONTROL_NODE:
          apply_control(system, control_node);
          break;
        case STATE_NODE:
          translate_state(system, control_node);
          break;
        default:
          assert(false);
        }
      }

      system->update();
    }
  }

  void translate_state( MarSystem *system, const node & state_node )
  {
    //cout << "Translating state..." << endl;

    assert(state_node.tag == STATE_NODE);
    assert(state_node.components.size() == 3);

    const node & condition_node = state_node.components[0];
    const node & when_node = state_node.components[1];
    const node & else_node = state_node.components[2];

    if (when_node.components.empty() && else_node.components.empty())
    {
      //cout << ".. Both when and else states are empty." << endl;
      return;
    }

    MarControlPtr condition_control =
        translate_complex_value(system, condition_node, system);

    if (!when_node.components.empty())
    {
      //cout << ".. Got when state." << endl;
      ScriptStateProcessor *when_processor = translate_state_definition(system, when_node);
      when_processor->getControl("mrs_bool/condition")->linkTo(condition_control, false);
      when_processor->update();
      system->attachMarSystem(when_processor);
    }

    if (!else_node.components.empty())
    {
      //cout << ".. Got else state." << endl;
      ScriptStateProcessor *else_processor = translate_state_definition(system, else_node);
      else_processor->getControl("mrs_bool/condition")->linkTo(condition_control, false);
      else_processor->setControl("mrs_bool/inverse", true);
      else_processor->update();
      system->attachMarSystem(else_processor);
    }
  }

  ScriptStateProcessor * translate_state_definition( MarSystem *system, const node & state_node  )
  {
    ScriptStateProcessor *state_processor = new ScriptStateProcessor("state_processor");

    for ( const node & mapping_node : state_node.components )
    {
      //cout << "Translating a mapping..." << endl;

      assert(mapping_node.tag == CONTROL_NODE);
      assert(mapping_node.components.size() == 2);
      assert(mapping_node.components[0].tag == ID_NODE);

      const std::string & dst_path = mapping_node.components[0].s;
      const node & src_node = mapping_node.components[1];

      MarControlPtr src_control = translate_complex_value(system, src_node, state_processor);
      if (src_control.isInvalid()) {
        MRSERR("Invalid value for control: " << dst_path);
        continue;
      }

      MarControlPtr dst_control = find_remote_control(system, dst_path);
      if (dst_control.isInvalid()) {
        MRSERR("Invalid destination control: " << dst_path);
        continue;
      }

      state_processor->addMapping( dst_control, src_control );
    }

    return state_processor;
  }

  void apply_control( MarSystem * system,
                      const node & control_node )
  {
    assert(control_node.tag == CONTROL_NODE);
    assert(control_node.components.size() == 2);
    assert(control_node.components[0].tag == ID_NODE);

    const std::string & dst_description = control_node.components[0].s;
    assert(!dst_description.empty());

    const node & src_node = control_node.components[1];
    MarControlPtr source_control = translate_complex_value(system, src_node, system);
    if (source_control.isInvalid()) {
      MRSERR("Can not set control - invalid value: "
             << system->getAbsPath() << dst_description);
      return;
    }

    string control_name = dst_description;
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

  MarControlPtr translate_complex_value( MarSystem *anchor,
                                         const node & value_node,
                                         MarSystem *owner )
  {
    if (value_node.tag == OPERATION_NODE)
    {
        // cout << "Translating expression..." << endl;

        ScriptOperationProcessor::operation *op = translate_operation(anchor, value_node);
        if (!op)
          return MarControlPtr();

        ScriptOperationProcessor *processor = new ScriptOperationProcessor("processor");
        processor->setOperation(op);
        owner->attachMarSystem(processor);

        MarControlPtr src_control = find_control(processor, "result");
        return src_control;
    }
    else
    {
      // cout << "Translating control value..." << endl;
      MarControlPtr src_control = translate_simple_value(anchor, value_node);
      return src_control;
    }
  }

  MarControlPtr translate_simple_value( MarSystem * anchor, const node & control_value )
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

      ScriptOperationProcessor::operator_type op =
          ScriptOperationProcessor::operator_for_text(op_node.s);

      auto opn = new ScriptOperationProcessor::operation(left_operand, op, right_operand);

      if (!op) {
        MRSERR("Invalid operator: '" << op_node.s << "'");
        delete opn;
        return nullptr;
      }

      return opn;
    }
    else
    {
      //cout << "-- Translating value..." << endl;
      MarControlPtr value = translate_simple_value(anchor, op_node);
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
         << "-- -- under " << current_context().root_system->getAbsPath() << endl;
#endif
    if (path.empty())
      return MarControlPtr();

    vector<string> path_elements;
    string control_name;
    bool absolute;
    split_control_path(path, path_elements, control_name, absolute);

    if (control_name.empty())
      return MarControlPtr();

    MarSystem *parent = absolute ? this_context().root_system : anchor;
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
    m_manager(manager)
  {}

  bool handle_directives( const node & directives_node )
  {
    for( const node & directive_node : directives_node.components)
    {
      if (!handle_directive( directive_node ))
        return false;
    }
    return true;
  }

  MarSystem *translate( const node & syntax_tree )
  {
    MarSystem *system = translate_actor(syntax_tree, own_context);
    return system;
  }
};

MarSystem *system_from_script(std::istream & script_stream)
{
  Parser parser(script_stream);
  parser.parse();

  const node &directives = parser.directives();
  const node &actor = parser.actor();

  MarSystemManager manager;
  script_translator translator(manager);

  if (!translator.handle_directives(directives))
    return nullptr;

  MarSystem *system = translator.translate(actor);

  if (system && system->getName().empty())
    system->setName("network");

  return system;
}

}
