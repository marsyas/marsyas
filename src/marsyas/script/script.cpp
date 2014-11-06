#include "script.h"
#include "manager.h"
#include "parser.h"
#include "operation_processor.hpp"
#include <marsyas/system/MarSystemManager.h>
#include <marsyas/FileName.h>
#include <marsyas/common_source.h>

#include <cassert>
#include <string>
#include <sstream>
#include <map>
#include <stack>
#include <deque>
#include <algorithm>
#include <iostream>

using namespace std;

namespace Marsyas {

class script_translator
{
  struct control_mapping {
    control_mapping(MarSystem *t, const node & d):
      target(t), data(d)
    {}
    MarSystem * target;
    node data;
  };

  typedef std::vector<control_mapping> control_map_t;
  typedef map<string, node> prototype_map_t;
  typedef map<string, MarSystem*> system_map_t;

  std::deque<prototype_map_t> m_prototype_stack;
  std::deque<MarSystem*> m_system_scope_stack;
  std::deque<control_map_t> m_control_scope_stack;

  MarSystemManager * m_manager;
  std::string m_working_dir;
  const bool m_use_register;

  MarSystem * this_system_scope() { return m_system_scope_stack.back(); }
  control_map_t & this_control_scope() { return m_control_scope_stack.back(); }

  bool add_prototype( const string & name, const node & proto_node )
  {
    //MRSMSG("Registering prototype: " << name);

    assert(!name.empty());
    assert(!m_prototype_stack.empty());

    prototype_map_t & prototypes = m_prototype_stack.back();

    if (prototypes.find(name) != prototypes.end())
    {
      MRSERR("Prototype with name '" << name << "'"
             << " already registered in this scope!");
      return false;
    }

    prototypes[name] = proto_node;

    return true;
  }

  const node * resolve_prototype( const std::string & name )
  {
    assert(!name.empty());

    for ( auto it = m_prototype_stack.rbegin(); it != m_prototype_stack.rend(); ++it )
    {
      const prototype_map_t & prototypes = *it;
      try {
        return &prototypes.at(name);
      }
      catch (...)
      {
        continue;
      }
    }
    return nullptr;
  }

  MarSystem * instantiate_system( const std::string & type, const std::string & name )
  {
    assert(!type.empty());

    const node *prototype_node = resolve_prototype(type);
    if (prototype_node)
    {
      MarSystem *system = translate_actor(*prototype_node, true);
      if (system)
      {
        system->setType(system->getName());
        system->setName(name);
        return system;
      }
      else
      {
        return nullptr;
      }
    }

    return m_manager->create(type, name);
  }

  bool handle_directives( const node & directives_node )
  {
    for( const node & directive_node : directives_node.components)
    {
      if (!handle_directive( directive_node ))
        return false;
    }
    return true;
  }

  bool handle_directive( const node & directive_node )
  {
    //cout << "handling directive: " << directive_node.tag << endl;

    switch(directive_node.tag)
    {
    //case INCLUDE_DIRECTIVE:
      //return handle_include_directive(directive_node);
    default:
      MRSERR("Invalid directive: " << directive_node.tag);
    }
    return false;
  }
#if 0
  bool handle_include_directive( const node & directive_node )
  {
    assert(directive_node.components.size() == 1 ||
           directive_node.components.size() == 2);

    string filename, id;

    assert(directive_node.components[0].tag == STRING_NODE);
    filename = directive_node.components[0].s;
    assert(!filename.empty());

    if (directive_node.components.size() > 1)
    {
      assert(directive_node.components[1].tag == ID_NODE);
      id = directive_node.components[1].s;
    }

    MarSystem *system = translate_script(filename);

    if (!system)
    {
      MRSERR("Failed to include file: " << filename);
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
    return add_prototype(id, system);
  }
#endif
  string resolve_filename( const string & filename )
  {
    string abs_filename(filename);
    FileName file_info(abs_filename);
    if (!file_info.isAbsolute() && !m_working_dir.empty())
      abs_filename = m_working_dir + abs_filename;
    return abs_filename;
  }

  MarSystem *translate_script( const string & filename )
  {
    ScriptTranslator translator(m_manager);
    if (m_use_register)
      return translator.translateRegistered( resolve_filename(filename) );
    else
      return translator.translateFile( resolve_filename(filename) );
  }

  MarSystem *translate_actor( const node & n, bool independent_scope )
  {
    //cout << "handling actor: " << n.tag << endl;

    if (n.tag != ACTOR_NODE && n.tag != PROTOTYPE_NODE)
    {
      MRSERR("Node is not an actor!");
      return 0;
    }

    assert(n.components.size() == 3);

    const node & name_node = n.components[0];
    const node & type_node = n.components[1];
    const node & def_node = n.components[2];

    assert(name_node.tag == ID_NODE || name_node.tag == GENERIC_NODE);
    assert(type_node.tag == ID_NODE || type_node.tag == STRING_NODE);
    assert(def_node.tag == GENERIC_NODE);

    std::string name, type_name;

    if (name_node.tag == ID_NODE)
      name = std::move(name_node.s);

    type_name = std::move(type_node.s);

    MarSystem *system;

    switch (type_node.tag)
    {
    case ID_NODE:
      system = instantiate_system(type_name, name);
      break;
    case STRING_NODE:
      // represents a filename
      system = translate_script(type_name);
      if (!system)
      {
        MRSERR("Failed to translate script: " << type_name);
      }
      else if(!name.empty())
      {
        system->setName(name);
      }
      break;
    default:
      assert(false);
      system = nullptr;
    }

    if (!system)
      return nullptr;

    m_prototype_stack.emplace_back();

    if (independent_scope)
    {
      m_system_scope_stack.push_back(system);
      m_control_scope_stack.emplace_back();
    }
    else if (!name.empty())
    {
      try
      {
        system->addToScope( this_system_scope() );
      }
      catch (std::exception & e)
      {
        MRSERR("Failed to add '" << name
               << "' to scope '" << this_system_scope()->getName() << "'."
               << endl
               << "Reason: " << e.what());
      }

      m_system_scope_stack.push_back(system);
    }

    int child_idx = 0;

    for( const node & system_def_element : def_node.components )
    {
      switch(system_def_element.tag)
      {
      case ACTOR_NODE:
      {
        MarSystem *child_system = translate_actor(system_def_element, false);
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
        else
        {
          MRSERR("Failed to instantiate a child.");
        }
        break;
      }
      case PROTOTYPE_NODE:
      {
        const node & proto_node = system_def_element;
        assert(proto_node.components.size() == 3);
        const node & name_node = proto_node.components[0];
        assert(name_node.tag == ID_NODE);
        string proto_name = name_node.s;

        add_prototype(proto_name, system_def_element);

        break;
      }
      case STATE_NODE:
      {
        this_control_scope().emplace_back( system, system_def_element);
        break;
      }
      case CONTROL_DEF_NODE:
        this_control_scope().emplace_back( system, system_def_element);
        break;
      default:
        assert(false);
      }
    }

    if (independent_scope)
    {
      apply_controls( this_control_scope() );
      m_control_scope_stack.pop_back();
    }

    if (this_system_scope() == system)
    {
      m_system_scope_stack.pop_back();
    }

    m_prototype_stack.pop_back();

    return system;
  }


  void apply_controls( const control_map_t & control_map )
  {
    for( const auto & mapping : control_map )
    {
      MarSystem * target = mapping.target;
      const auto & data_node = mapping.data;

      //cout << "Applying controls for: " << system->getAbsPath() << endl;

      m_system_scope_stack.push_back( target );

      switch(data_node.tag)
      {
      case CONTROL_DEF_NODE:
        apply_control(target, data_node);
        break;
      case STATE_NODE:
        translate_state(target, data_node);
        break;
      default:
        assert(false);
      }

      m_system_scope_stack.pop_back();
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
        translate_complex_value(condition_node, system);

    if (!when_node.components.empty())
    {
      //cout << ".. Got when state." << endl;
      ScriptStateProcessor *when_processor = translate_state_definition(when_node);
      when_processor->getControl("mrs_bool/condition")->linkTo(condition_control, false);
      when_processor->update();
      system->attachMarSystem(when_processor);
    }

    if (!else_node.components.empty())
    {
      //cout << ".. Got else state." << endl;
      ScriptStateProcessor *else_processor = translate_state_definition(else_node);
      else_processor->getControl("mrs_bool/condition")->linkTo(condition_control, false);
      else_processor->setControl("mrs_bool/inverse", true);
      else_processor->update();
      system->attachMarSystem(else_processor);
    }
  }

  ScriptStateProcessor * translate_state_definition( const node & state_node  )
  {
    ScriptStateProcessor *state_processor = new ScriptStateProcessor("state_processor");

    for ( const node & mapping_node : state_node.components )
    {
      //cout << "Translating a mapping..." << endl;

      assert(mapping_node.tag == CONTROL_ASSIGNMENT_NODE);
      assert(mapping_node.components.size() == 2);
      assert(mapping_node.components[0].tag == ID_NODE);

      const std::string & dst_path = mapping_node.components[0].s;
      const node & src_node = mapping_node.components[1];

      MarControlPtr src_control = translate_complex_value(src_node, state_processor);
      if (src_control.isInvalid()) {
        MRSERR("Invalid value for control: " << dst_path);
        continue;
      }

      MarControlPtr dst_control = this_system_scope()->remoteControl(dst_path);
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
    assert(control_node.tag == CONTROL_DEF_NODE);
    assert(control_node.components.size() >= 3);
    assert(control_node.components[0].tag == BOOL_NODE);
    assert(control_node.components[1].tag == BOOL_NODE);
    assert(control_node.components[2].tag == ID_NODE);

    bool do_create = control_node.components[0].v.b;
    bool is_public = control_node.components[1].v.b;
    bool has_value = control_node.components.size() >= 4;

    MarControlPtr control;

    if (has_value)
    {
      control = assign_control
          ( system, control_node.components[2], control_node.components[3], do_create );
    }
    else
    {
      const string control_name = control_node.components[2].s;
      assert(!control_name.empty());
      control = system->control( control_name );
      if (control.isInvalid())
      {
        MRSERR("Can not set control access - control does not exist: " << control_name);
      }
    }

    if (control.isInvalid())
      return;

    if (is_public)
    {
      control->setPublic(true);
    }
  }

  MarControlPtr assign_control( MarSystem * system,
                                const node & dst_node, const node & src_node,
                                bool create )
  {
    string dst_name = dst_node.s;

    MarControlPtr dst_control = system->control( dst_name );

    MarControlPtr src_control = translate_complex_value(src_node, system);
    if (src_control.isInvalid())
    {
      MRSERR("Can not set control '" << system->path() << dst_name
             << "' - invalid value.");
      return MarControlPtr();
    }

    bool link = src_control->getMarSystem() != nullptr;

    if (create)
    {
      //cout << "Creating:" << system->getAbsPath() << control_path << endl;
      if (!dst_control.isInvalid())
      {
        MRSERR("ERROR: Can not add control - "
               << "same control already exists: " << system->path() << dst_name);
        return MarControlPtr();
      }
      string dst_descriptor = src_control->getType() + '/' + dst_name;
      bool created = system->addControl(dst_descriptor, *src_control, dst_control);
      if (!created)
      {
        MRSERR("ERROR: Failed to create control: " << system->path() << dst_name);
        return MarControlPtr();
      }
      if (link)
      {
        dst_control->linkTo( src_control );
      }
    }
    else
    {
      /*
      cout << "Setting:" << system->getAbsPath() << control_path
           << " = " << source_control
           << endl;
           */
      if (dst_control.isInvalid())
      {
        MRSERR("ERROR: Can not set control - "
               << "it does not exist: " << system->path() << dst_name);
        return MarControlPtr();
      }
      if (link)
      {
        dst_control->linkTo( src_control );
      }
      else
      {
        dst_control->setValue( src_control );
      }
    }

    return dst_control;
  }

  MarControlPtr translate_complex_value( const node & value_node,
                                         MarSystem *owner )
  {
    if (value_node.tag == OPERATION_NODE)
    {
        // cout << "Translating expression..." << endl;

        ScriptOperationProcessor::operation *op = translate_operation(value_node);
        if (!op)
          return MarControlPtr();

        ScriptOperationProcessor *processor = new ScriptOperationProcessor("processor");
        processor->setOperation(op);
        owner->attachMarSystem(processor);

        MarControlPtr src_control = processor->control("result");
        return src_control;
    }
    else
    {
      // cout << "Translating control value..." << endl;
      MarControlPtr src_control = translate_simple_value(value_node);
      return src_control;
    }
  }

  MarControlPtr translate_simple_value( const node & control_value )
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
          switch(row.components[c].tag)
          {
          case REAL_NODE:
            matrix(r, c) = row.components[c].v.r; break;
          case INT_NODE:
            matrix(r, c) = (mrs_real) row.components[c].v.i; break;
          default:
            assert(false);
          }
        }
      }
      return MarControlPtr(matrix);
    }
    case ID_NODE:
    {
      string link_path = control_value.s;
      assert(!link_path.empty());
      MarControlPtr link_control = this_system_scope()->remoteControl(link_path);
      if (link_control.isInvalid())
        MRSERR("Invalid control path: " << link_path);
      return link_control;
    }
    default:
      assert(false);
    }
    return MarControlPtr();
  }

  ScriptOperationProcessor::operation *translate_operation( const node & op_node )
  {
    if (op_node.tag == OPERATION_NODE)
    {
      assert(op_node.s.size());
      assert(op_node.components.size() == 2);

      //cout << "-- Translating operation: " << op_node.s[0] << endl;

      auto left_operand = translate_operation(op_node.components[0]);
      auto right_operand = translate_operation(op_node.components[1]);

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
      MarControlPtr value = translate_simple_value(op_node);
      if (value.isInvalid())
      {
        MRSERR("Can not parse expression: invalid control value!");
        return nullptr;
      }
      auto op = new ScriptOperationProcessor::operation(value);
      return op;
    }

    return nullptr;
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

  script_translator( MarSystemManager * manager,
                     const string & working_dir,
                     bool from_register ):
    m_manager(manager),
    m_working_dir(working_dir),
    m_use_register(from_register)
  {}

  MarSystem *translate(std::istream & script_stream)
  {
    Parser parser(script_stream);
    parser.parse();

    const node &directives = parser.directives();
    const node &actor = parser.actor();

    if (!handle_directives(directives))
    {
      return nullptr;
    }

    MarSystem *system = translate_actor(actor, true);

    if (system && system->getName().empty())
      system->setName("network");

    return system;
  }
};

MarSystem *system_from_script(std::istream & script_stream,
                              const std::string & working_directory,
                              MarSystemManager *mng)
{
  ScriptTranslator translator(mng);
  return translator.translateStream(script_stream, working_directory);
}

MarSystem *system_from_script(const std::string & filepath, MarSystemManager *mng)
{
  ScriptTranslator translator(mng);
  return translator.translateFile(filepath);
}


ScriptTranslator::ScriptTranslator( MarSystemManager * manager ):
  m_manager(manager),
  m_own_manager(manager == nullptr)
{
  if (m_own_manager)
    m_manager = new MarSystemManager;
}

ScriptTranslator::~ScriptTranslator()
{
  if (m_own_manager)
    delete m_manager;
}

MarSystem *ScriptTranslator::translateFile(const std::string & filename_string)
{
  FileName filename(filename_string);
  string path = filename.path();

  ifstream file(filename_string.c_str());
  if (!file.is_open())
  {
    MRSERR("Could not open file: " << filename_string);
    return nullptr;
  }

  script_translator translator(m_manager, path, false);
  return translator.translate(file);
}

MarSystem *ScriptTranslator::translateStream
(std::istream & script_stream, const std::string & working_directory)
{
  script_translator translator(m_manager, working_directory, false);
  return translator.translate(script_stream);
}

MarSystem *ScriptTranslator::translateRegistered(const std::string & file_path)
{
  string script;
  try {
    script = ScriptManager::get(file_path);
  }
  catch (...)
  {
    MRSERR("Could not find registered script: " << file_path);
    return nullptr;
  }

  istringstream script_stream(script);

  // Treat registered paths just like file paths:
  FileName filename(file_path);
  string path = filename.path();

  script_translator translator(m_manager, path, true);
  return translator.translate(script_stream);
}

}
