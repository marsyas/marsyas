#include "Map.h"

namespace Marsyas {

Map::Map(const std::string & name):
  MarSystem("Map", name)
{
  addControl("mrs_realvec/input", realvec(), m_input_ctl);
  addControl("mrs_realvec/output", realvec(), m_output_ctl);
}

Map::Map( const Map & other ):
  MarSystem(other)
{
  m_input_ctl = getControl("mrs_realvec/input");
  m_output_ctl = getControl("mrs_realvec/output");
}

void Map::myUpdate(MarControlPtr cause)
{
  MarSystem::myUpdate(cause);

  const bool do_not_update = false;

  MarControlAccessor access_input(m_input_ctl, do_not_update);
  MarControlAccessor access_output(m_output_ctl, do_not_update);

  realvec & input = access_input.to<realvec>();
  realvec & output = access_output.to<realvec>();

  if (input.getRows() != inObservations_ ||
      input.getCols() != inSamples_)
  {
    input.create(inObservations_, inSamples_);
  }

  if (output.getRows() != onObservations_ ||
      output.getCols() != onSamples_)
  {
    output.create(onObservations_, onSamples_);
  }
}

void Map::myProcess(realvec & in, realvec & out)
{
  {
    MarControlAccessor input_access(m_input_ctl);
    realvec & input_data = input_access.to<realvec>();

    assert(input_data.getRows() == in.getRows() &&
           input_data.getCols() == in.getCols());

    input_data = in;
  }

  const realvec & output_data = m_output_ctl->to<realvec>();

  assert(output_data.getRows() == out.getRows() &&
         output_data.getCols() == out.getCols());

  out = output_data;
}

} // namespace Marsyas
