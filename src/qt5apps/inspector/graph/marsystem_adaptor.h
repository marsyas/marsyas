#ifndef MARSYSAS_INSPECTOR_MARSYSTEM_ADAPTOR_INCLUDED
#define MARSYSAS_INSPECTOR_MARSYSTEM_ADAPTOR_INCLUDED

#include "marsyas_qml.h"

#include <marsyas/system/MarSystem.h>

#include <QObject>
#include <QVariant>
#include <QQmlPropertyMap>

namespace MarsyasQml {

using namespace Marsyas;

class MarSystemAdaptor : public QObject
{
  Q_OBJECT
  Q_PROPERTY( Marsyas::MarSystem * system READ system CONSTANT )
  Q_PROPERTY( QString name READ name CONSTANT )
  Q_PROPERTY( QString type READ type CONSTANT )
  Q_PROPERTY( QString path READ path CONSTANT )
  Q_PROPERTY( int level READ level CONSTANT )
  Q_PROPERTY( bool hasChildren READ hasChildren NOTIFY childrenChanged )
  Q_PROPERTY( int childCount READ childCount NOTIFY childrenChanged )
  Q_PROPERTY( QVariantList children READ children NOTIFY childrenChanged )
  Q_PROPERTY( QObject * defaultControls READ defaultControls NOTIFY controlsChanged )

public:
  MarSystemAdaptor( Marsyas::MarSystem *system, QObject * parent = 0 );

  MarSystem * system() const { return m_system; }

  QString name() const
  {
    return QString::fromStdString( m_system->getName() );
  }

  QString type() const
  {
    return QString::fromStdString( m_system->getType() );
  }

  QString path() const
  {
    return QString::fromStdString( m_system->getAbsPath() );
  }

  int level() const
  {
    MarSystem *system = m_system;
    int level = 0;
    while(system->getParent())
    {
      system = system->getParent();
      ++level;
    }
    return level;
  }

  bool hasChildren() const
  {
    return !m_children.isEmpty();
  }

  int childCount() const
  {
    return m_children.count();
  }

  QVariantList children() const;

  QObject *defaultControls();

signals:
  void childrenChanged();
  void controlsChanged();

private:
  MarSystem *m_system;
  QList<MarSystemAdaptor*> m_children;
  QQmlPropertyMap *m_defaultControls;
};

} // namespace MarsyasQml

#endif // MARSYSAS_INSPECTOR_MARSYSTEM_ADAPTOR_INCLUDED
