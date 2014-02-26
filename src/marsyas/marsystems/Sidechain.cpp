#include "Sidechain.h"

namespace Marsyas {

Sidechain::Sidechain(std::string name):
  MarSystem("Sidechain", name)
{
  isComposite_ = true;
}

void Sidechain::myUpdate(MarControlPtr cause)
{
  MarSystem::myUpdate(cause);

  if (!marsystems_.size())
    return;

  MarSystem *child = marsystems_[0];
  child->setControl("mrs_natural/inObservations", inObservations_);
  child->setControl("mrs_natural/inSamples", inSamples_);
  child->setControl("mrs_real/israte", israte_);
  child->setControl("mrs_string/inObsNames", inObsNames_);
  child->update();

  m_child_out.create( child->getControl("mrs_natural/onObservations")->to<mrs_natural>(),
                      child->getControl("mrs_natural/onSamples")->to<mrs_natural>() );
}

void Sidechain::myProcess(realvec& in, realvec& out)
{
  out = in;

  if (!marsystems_.size())
    return;

  MarSystem *child = marsystems_[0];
  child->process(in, m_child_out);
}

} // namespace Marsyas
