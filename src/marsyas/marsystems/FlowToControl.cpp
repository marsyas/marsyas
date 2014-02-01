#include "FlowToControl.h"

namespace Marsyas {

FlowToControl::FlowToControl(std::string name):
  MarSystem("FlowToControl", name)
{
  addControl("mrs_natural/row", 0, m_row_ctl);
  addControl("mrs_natural/column", 0, m_col_ctl);
  addControl("mrs_real/value", 0.0, m_value_ctl);
}

FlowToControl::FlowToControl(const FlowToControl & other):
  MarSystem(other)
{
  m_row_ctl = getControl("mrs_natural/row");
  m_col_ctl = getControl("mrs_natural/column");
  m_value_ctl = getControl("mrs_real/value");
}

FlowToControl::~FlowToControl() {}

MarSystem* FlowToControl::clone() const
{
  return new FlowToControl(*this);
}

void FlowToControl::myUpdate(MarControlPtr sender)
{
  if (sender() == m_row_ctl() || sender() == m_col_ctl())
    return;

  MarSystem::myUpdate(sender);
}

void FlowToControl::myProcess(realvec &in, realvec &out)
{
  out = in;

  mrs_natural row = m_row_ctl->to<mrs_natural>();
  mrs_natural col = m_col_ctl->to<mrs_natural>();

  if (row >= 0 && row < in.getRows() &&
      col >= 0 && col < in.getCols() )
  {
    m_value_ctl->setValue(in(row,col));
  }
}

} // namespace Marsyas
