#ifndef MARSYSAS_INSPECTOR_MARSYSTEM_ADAPTOR_INCLUDED
#define MARSYSAS_INSPECTOR_MARSYSTEM_ADAPTOR_INCLUDED

#include "marsyas_qml.h"

#include <MarSystem.h>

#include <QObject>
#include <QVariant>
#include <QQmlPropertyMap>

namespace MarsyasQml {

using namespace Marsyas;

class MarSystemAdaptor : public QObject
{
  Q_OBJECT
  Q_PROPERTY( Marsyas::MarSystem * system READ system WRITE setSystem NOTIFY systemChanged )
  Q_PROPERTY( QVariantList children READ children NOTIFY childrenChanged )
  Q_PROPERTY( QString name READ name NOTIFY systemChanged )
  Q_PROPERTY( QString type READ type NOTIFY systemChanged )
  Q_PROPERTY( QString path READ path NOTIFY systemChanged )
  Q_PROPERTY( QObject * defaultControls READ defaultControls NOTIFY systemChanged )
  Q_PROPERTY( bool hasChildren READ hasChildren NOTIFY systemChanged )

public:
  MarSystemAdaptor( Marsyas::MarSystem *system, QObject * parent = 0 );

  QString name() const
  {
    return QString::fromStdString( m_system->getPrefix() );
  }

  QString type() const
  {
    return QString::fromStdString( m_system->getType() );
  }

  QString path() const
  {
    return QString::fromStdString( m_system->getAbsPath() );
  }

  MarSystem * system() const { return m_system; }
  void setSystem( MarSystem * system );

  bool hasChildren() const
  {
    return !m_children.isEmpty();
  }

  QVariantList children() const;

  QObject *defaultControls();

signals:
  void systemChanged();
  void childrenChanged();

private:
  MarSystem *m_system;
  QList<MarSystemAdaptor*> m_children;
  QQmlPropertyMap *m_defaultControls;
};

} // namespace MarsyasQml

#endif // MARSYSAS_INSPECTOR_MARSYSTEM_ADAPTOR_INCLUDED
