#include "marsystem_adaptor.h"
#include "marsyas_qml.h"

using namespace std;
using namespace Marsyas;

namespace MarsyasQml {

MarSystemAdaptor::MarSystemAdaptor( Marsyas::MarSystem *system, QObject * parent ):
  QObject(parent),
  m_system(system),
  m_defaultControls(0)
{
  vector<MarSystem*> children = m_system->getChildren();
  int count = children.size();
  for (int idx = 0; idx < count; ++idx)
  {
    m_children.append( new MarSystemAdaptor( children[idx], this ) );
  }
}

QVariantList MarSystemAdaptor::children() const
{
  QVariantList children_varlist;
  foreach( MarSystemAdaptor * system, m_children )
  {
    children_varlist << QVariant::fromValue<QObject*>(system);
  }
  return children_varlist;
}

QObject *MarSystemAdaptor::defaultControls()
{
  if (!m_defaultControls)
  {
    QQmlPropertyMap *defaultControls = new QQmlPropertyMap;
    defaultControls->insert("inSamples", variantFromControl(m_system->getControl("mrs_natural/inSamples")));
    defaultControls->insert("onSamples", variantFromControl(m_system->getControl("mrs_natural/onSamples")));
    defaultControls->insert("inObservations", variantFromControl(m_system->getControl("mrs_natural/inObservations")));
    defaultControls->insert("onObservations", variantFromControl(m_system->getControl("mrs_natural/onObservations")));
    defaultControls->insert("israte", variantFromControl(m_system->getControl("mrs_real/israte")));
    defaultControls->insert("osrate", variantFromControl(m_system->getControl("mrs_real/osrate")));
    defaultControls->insert("inStabilizingDelay", variantFromControl(m_system->getControl("mrs_natural/inStabilizingDelay")));
    defaultControls->insert("onStabilizingDelay", variantFromControl(m_system->getControl("mrs_natural/onStabilizingDelay")));
    defaultControls->insert("inObsNames", variantFromControl(m_system->getControl("mrs_string/inObsNames")));
    defaultControls->insert("onObsNames", variantFromControl(m_system->getControl("mrs_string/onObsNames")));
    defaultControls->insert("active", variantFromControl(m_system->getControl("mrs_bool/active")));
    defaultControls->insert("mute", variantFromControl(m_system->getControl("mrs_bool/mute")));
    defaultControls->insert("debug", variantFromControl(m_system->getControl("mrs_bool/debug")));
    defaultControls->insert("verbose", variantFromControl(m_system->getControl("mrs_bool/verbose")));
    defaultControls->insert("processedData", variantFromControl(m_system->getControl("mrs_realvec/processedData")));
    m_defaultControls = defaultControls;
  }
  return m_defaultControls;
}

} // namespace MarsyasQml
