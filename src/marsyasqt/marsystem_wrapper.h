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
#include "realtime/runner.h"
#include "realtime/any.h"

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QDebug>

namespace MarsyasQt {

using namespace Marsyas;
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
  {}

  void start()
  {
    m_runner.start();
  }

  void stop()
  {
    m_runner.stop();
  }

  bool isRunning()
  {
    return m_runner.isRunning();
  }

#if 0
  QVariant controlValue( const QString & path )
  {
    any value = thread.controlValue( path.toStdString() );
    QVariant variant =
  }

  void setControlValue( const QString & path, const QVariant & variant )
  {
    any value =
        thread.setControlValue( path.toStdString(), value );
  }
#endif

  Control * control( const QString & path );

  void update()
  {
    m_runner.update();
  }

private slots:
  void removeControl( QObject * object );

private:
  friend class Control;

  Control * createControl( const QString & path );

  Marsyas::RealTime::Runner m_runner;
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

  virtual void setValue( const QVariant & value, bool update = true )
  {
    m_thread_control->setValue( any_from_variant(value), update );
  }

  QString path() const { return QString::fromStdString( m_thread_control->path() ); }

protected:
  friend class System;

  Control( Marsyas::RealTime::Control *thread_control, System * parent ):
    QObject(parent),
    m_thread_control(thread_control)
  {}

  virtual ~Control() {}

  virtual QVariant variant_from_any( const any & value ) = 0;
  virtual any any_from_variant( const QVariant & value ) = 0;

private:
  Marsyas::RealTime::Control *m_thread_control;
};

class ControlBool : public Control
{
  Q_OBJECT
public slots:
  void setValue(bool value)
  {
    Control::setValue( QVariant(value) );
  }
signals:
  void valueChanged(bool value);

protected:
  friend class System;

  ControlBool( Marsyas::RealTime::Control *thread_control, System * parent ):
    Control(thread_control, parent)
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
    Control::setValue( QVariant(value) );
  }
signals:
  void valueChanged(int value);

protected:
  friend class System;

  ControlNatural( Marsyas::RealTime::Control *thread_control, System * parent ):
    Control(thread_control, parent)
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
    Control::setValue( QVariant(value) );
  }
signals:
  void valueChanged(double value);

protected:
  friend class System;

  ControlReal( Marsyas::RealTime::Control *thread_control, System * parent ):
    Control(thread_control, parent)
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
public slots:
  void setValue(const realvec & value)
  {
    Control::setValue( QVariant::fromValue(value) );
  }
signals:
  void valueChanged(const realvec & value);

protected:
  friend class System;

  ControlRealvec( Marsyas::RealTime::Control *thread_control, System * parent ):
    Control(thread_control, parent)
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
    Control::setValue( QVariant(value) );
  }
signals:
  void valueChanged(const QString & value);

protected:
  friend class System;

  ControlString( Marsyas::RealTime::Control *thread_control, System * parent ):
    Control(thread_control, parent)
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

