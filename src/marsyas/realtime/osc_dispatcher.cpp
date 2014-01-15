#include "osc_dispatcher.h"

#include <string>
#include <algorithm>

using namespace std;

namespace Marsyas {
namespace RealTime {

void OscDispatcher::run()
{
  size_t packet_size;

  while( (packet_size = m_queue->pop(m_buffer, m_buffer_size)) )
  {
    if (packet_size > m_buffer_size)
    {
      MRSWARN("OSC Dispatcher: dropped too large OSC packet.");
      continue;
    }
    ProcessPacket(m_buffer, packet_size, m_endpoint);
  }
}

void OscDispatcher::ProcessMessage( const osc::ReceivedMessage& message,
                                    const IpEndpointName& )
{
  const char * path = message.AddressPattern();
  if (path[0] == '/') ++path;

  // FIXME: Constructing std::string is not real-time-safe.
  MarControlPtr control = find_control(path);
  if (control.isInvalid())
  {
    MRSWARN("OSC dispatcher: no control for path: " << path);
    return;
  }

  try
  {
    osc::ReceivedMessage::const_iterator it = message.ArgumentsBegin();
    if (it == message.ArgumentsEnd())
      throw std::runtime_error("Message has no arguments.");

    char tag = it->TypeTag();
    switch(tag)
    {
    case osc::TRUE_TYPE_TAG:
    case osc::FALSE_TYPE_TAG:
      control->setValue(it->AsBoolUnchecked());
      break;
    case osc::INT32_TYPE_TAG:
      control->setValue(it->AsInt32Unchecked());
      break;
    case osc::FLOAT_TYPE_TAG:
      control->setValue((mrs_real) it->AsFloatUnchecked());
      break;
    case osc::DOUBLE_TYPE_TAG:
      control->setValue((mrs_real) it->AsDoubleUnchecked());
      break;
    case osc::STRING_TYPE_TAG:
      control->setValue(it->AsStringUnchecked());
      break;
    default:
      throw std::runtime_error("Unsupported message argument type.");
    }
  }
  catch ( std::exception & e )
  {
    MRSWARN("OSC dispatcher: error while parsing message: " << e.what());
  }
}

MarSystem *find_child_system(MarSystem * parent, const char * name, size_t count)
{
  const std::vector<MarSystem*> & children = parent->children();
  for( MarSystem * child : children )
  {
    const string & child_name = child->getName();
    if (child_name.size() == count &&
        equal(name, name + count, child_name.data()))
    {
      return child;
    }
  }

  return 0;
}

MarControlPtr find_local_control(MarSystem * system, const char *name, size_t count)
{
  const auto & controls = system->controls();
  for ( const auto & mapping : controls )
  {
    const MarControlPtr & control = mapping.second;
    const string & id = control->id();
    if (id.size() == count &&
        equal(name, name + count, id.data()))
    {
      return control;
    }
  }

  return MarControlPtr();
}

MarControlPtr OscDispatcher::find_control( const char * path )
{
  size_t path_len = strlen(path);
  const char * path_end = path + path_len;
  const char * component = path;

  MarSystem * system = m_system;

  while(system)
  {
    const char * separator = std::find(component, path_end, '/');
    size_t component_len = separator - component;
    if (separator != path_end)
    {
      system = find_child_system(system, component, component_len );
    }
    else
    {
      return find_local_control(system, component, component_len);
    }
    component = separator + 1;
  }

  return MarControlPtr();
}

}
}
