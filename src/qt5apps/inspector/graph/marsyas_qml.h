#ifndef MARSYAS_QML_COMMON_INCLUDED
#define MARSYAS_QML_COMMON_INCLUDED

#include <MarSystem.h>
#include <MarControl.h>
#include <QMetaType>
#include <QString>
#include <QVariant>

Q_DECLARE_METATYPE(Marsyas::MarSystem*);

namespace MarsyasQml {

QVariant variantFromControl ( const Marsyas::MarControlPtr & control );

} // namespace MarsyasQml

#endif // MARSYAS_QML_COMMON_INCLUDED
