/*
** Copyright (C) 1998-2013 George Tzanetakis <gtzan@cs.uvic.ca>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#ifndef MARSYASQT_QTMARSYSTEM_INCLUDED
#define MARSYASQT_QTMARSYSTEM_INCLUDED

#include "marsyasqt_common.h"
#include <marsyas/realtime/runner.h>
#include <marsyas/realtime/any.h>
#include <marsyas/realtime/controller.h>
#include <marsyas/realvec.h>

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QDebug>

namespace MarsyasQt {

using namespace Marsyas;
using Marsyas::mrs_natural;
using Marsyas::mrs_real;
using Marsyas::mrs_realvec;
using Marsyas::RealTime::any;
using Marsyas::RealTime::any_cast;
using Marsyas::RealTime::bad_any_cast;

template<typename T>
QVariant variant_from_any( const any & value )
{
  try {
    const T & typed_value = any_cast<T>(value);
    return QVariant::fromValue(typed_value);
  } catch (bad_any_cast) {
    return QVariant();
  }
}

template<typename T>
any any_from_variant( const QVariant & value )
{
  T typed_value = value.value<T>();
  return any(typed_value);
}

template<typename T>
T safe_any_cast( const any & value )
{
  try {
    const T & typed_value = any_cast<T>(value);
    return typed_value;
  } catch (bad_any_cast) {
    return T();
  }
}

////////////////////////////////

class Control;

/**
 * @brief New convenience MarSystem wrapper for use with Qt,
 * based on Marsyas::RealTime::Runner.
 */
class System : public QObject
{
  Q_OBJECT

public:


public:
  System( Marsyas::MarSystem * system, QObject * parent = 0 ):
    QObject(parent),
    m_runner(system)
  {
    m_runner.addController( &m_controller );
  }

  void setRtPriorityEnabled( bool on )
  {
    m_runner.setRtPriorityEnabled(on);
  }

  void start()
  {
    m_runner.start();
  }

  void stop()
  {
    m_runner.stop();
    m_controller.clearQueue();
  }

  bool isRunning()
  {
    return m_runner.isRunning();
  }

  Control * control( const QString & path );

  void update()
  {
    qWarning() << "Qt wrapper: TODO: System::update().";
  }

  MarSystem * marsystem() { return m_runner.system(); }

private slots:
  void removeControl( QObject * object );

private:
  friend class Control;

  Control * createControl( const QString & path );

  Marsyas::RealTime::Runner m_runner;
  Marsyas::RealTime::Controller m_controller;
  QMap<QString, Control*> m_controls;
};

///////////////////////////////

class Control : public QObject
{
  Q_OBJECT
public:
  virtual QVariant value()
  {
    return variant_from_any( m_thread_control->value() );
  }

  virtual void setValue( const QVariant & value, bool update = true ) = 0;

  QString path() const { return QString::fromStdString( m_thread_control->path() ); }

protected:
  friend class System;

  Control( Marsyas::RealTime::Control *thread_control,
           Marsyas::RealTime::Controller *controller,
           System * parent ):
    QObject(parent),
    m_system(parent),
    m_controller(controller),
    m_thread_control(thread_control)
  {
    MarControlPtr control = m_system->marsystem()->getControl( path().toStdString() );
    if (!control.isInvalid())
    {
      m_short_path = QByteArray( control->path().c_str() );
    }
    else
    {
      qCritical() << "Qt wrapper: could not find control:" << path() << endl;
    }
  }

  virtual ~Control() {}

  virtual QVariant variant_from_any( const any & value ) = 0;
  virtual any any_from_variant( const QVariant & value ) = 0;

  template <typename T> void controller_set( const T & value )
  {
    if (m_system->isRunning())
    {
      m_controller->set( m_short_path.constData(), value );
    }
    else
    {
      m_system->marsystem()->updControl( path().toStdString(), value );
    }
  }

private:
  System * m_system;
  Marsyas::RealTime::Controller *m_controller;
  Marsyas::RealTime::Control *m_thread_control;
  QByteArray m_short_path;
};

class ControlBool : public Control
{
  Q_OBJECT
public slots:
  void setValue(bool value)
  {
    controller_set(value);
  }
  void setValue( const QVariant & value, bool )
  {
    controller_set(value.value<bool>());
  }

signals:
  void valueChanged(bool value);

protected:
  friend class System;

  ControlBool( Marsyas::RealTime::Control *thread_control,
               Marsyas::RealTime::Controller *controller,
               System * parent ):
    Control(thread_control, controller, parent)
  {}

  QVariant variant_from_any( const any & value )
  {
    return MarsyasQt::variant_from_any<bool>(value);
  }
  any any_from_variant( const QVariant & value )
  {
    return MarsyasQt::any_from_variant<bool>(value);
  }
};

class ControlNatural : public Control
{
  Q_OBJECT
public slots:
  void setValue(int value)
  {
    controller_set(value);
  }
  void setValue( const QVariant & value, bool )
  {
    controller_set(value.value<int>());
  }

signals:
  void valueChanged(int value);

protected:
  friend class System;

  ControlNatural( Marsyas::RealTime::Control *thread_control,
                  Marsyas::RealTime::Controller *controller,
                  System * parent ):
    Control(thread_control, controller, parent)
  {}
  QVariant variant_from_any( const any & value )
  {
    return MarsyasQt::variant_from_any<mrs_natural>(value);
  }
  any any_from_variant( const QVariant & value )
  {
    return MarsyasQt::any_from_variant<mrs_natural>(value);
  }
};

class ControlReal : public Control
{
  Q_OBJECT
public slots:
  void setValue(double value)
  {
    controller_set((mrs_real) value);
  }
  void setValue( const QVariant & value, bool )
  {
    controller_set(value.value<mrs_real>());
  }

signals:
  void valueChanged(double value);

protected:
  friend class System;

  ControlReal( Marsyas::RealTime::Control *thread_control,
               Marsyas::RealTime::Controller *controller,
               System * parent ):
    Control(thread_control, controller, parent)
  {}
  QVariant variant_from_any( const any & value )
  {
    return MarsyasQt::variant_from_any<mrs_real>(value);
  }
  any any_from_variant( const QVariant & value )
  {
    return MarsyasQt::any_from_variant<mrs_real>(value);
  }
};

class ControlRealvec : public Control
{
  Q_OBJECT
  void setValue( const QVariant &, bool )
  {
    //old_controller_set( value, update );
    qCritical() << "Qt wrapper: Setting realvec controls not yet implemented.";
  }

signals:
  void valueChanged(const mrs_realvec & value);

protected:
  friend class System;

  ControlRealvec( Marsyas::RealTime::Control *thread_control,
                  Marsyas::RealTime::Controller *controller,
                  System * parent ):
    Control(thread_control, controller, parent)
  {}
  QVariant variant_from_any( const any & value )
  {
    return MarsyasQt::variant_from_any<mrs_realvec>(value);
  }
  any any_from_variant( const QVariant & value )
  {
    return MarsyasQt::any_from_variant<mrs_realvec>(value);
  }
};

class ControlString : public Control
{
  Q_OBJECT
public slots:
  void setValue(const QString & value)
  {
    controller_set( value.toLocal8Bit().constData() );
  }
  void setValue( const QVariant & value, bool )
  {
    controller_set( value.value<QString>().toLocal8Bit().constData() );
  }

signals:
  void valueChanged(const QString & value);

protected:
  friend class System;

  ControlString( Marsyas::RealTime::Control *thread_control,
                 Marsyas::RealTime::Controller *controller,
                 System * parent ):
    Control(thread_control, controller, parent)
  {}
  QVariant variant_from_any( const any & value )
  {
    std::string string_value = safe_any_cast<mrs_string>(value);
    return QVariant( QString::fromStdString(string_value) );
  }
  any any_from_variant( const QVariant & value )
  {
    return any( value.toString().toStdString() );
  }
};

} // namespace MarsyasQt

#endif // MARSYASQT_QTMARSYSTEM_INCLUDED

