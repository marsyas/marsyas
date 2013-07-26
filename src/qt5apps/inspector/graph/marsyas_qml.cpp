#include "marsyas_qml.h"

namespace MarsyasQml {

using namespace Marsyas;

QVariant variantFromControl ( const Marsyas::MarControlPtr & control )
{
  QVariant value;

  std::string type = control->getType();
  if (type == "mrs_real")
    value = QString::number( control->to<mrs_real>() );
  else if (type == "mrs_natural")
    value = QString::number( control->to<mrs_natural>() );
  else if (type == "mrs_bool")
    value = QVariant( control->to<mrs_bool>() ).convert(QVariant::String);
  else if (type == "mrs_string")
    value = QString::fromStdString(control->to<mrs_string>());
  else if (type == "mrs_realvec")
    value = QString("[...]");
  else
    value = QString("<unknown>");

  return value;
}

} // namespace MarsyasQml
