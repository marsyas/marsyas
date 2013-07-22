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

#ifndef MARSYAS_INSPECTOR_MARSYSTEM_VIEW_INCLUDED
#define MARSYAS_INSPECTOR_MARSYSTEM_VIEW_INCLUDED

#include <MarSystem.h>

#include <QObject>
#include <QtQml>
#include <QQmlComponent>
#include <QQmlPropertyMap>
#include <QQuickItem>
#include <QPointer>

Q_DECLARE_METATYPE(Marsyas::MarSystem*);

using namespace Marsyas;

class MarSystemItem : public QQuickItem
{
  Q_OBJECT
  Q_PROPERTY( Marsyas::MarSystem * system READ system WRITE setSystem )
  Q_PROPERTY( QQmlComponent * delegate READ delegate WRITE setDelegate )

public:
  MarSystemItem( QQuickItem * parent = 0 ):
    QQuickItem(parent),
    m_system(0),
    m_delegate(0)
  {}

  MarSystem *system() const { return m_system; }
  void setSystem( MarSystem * system );

  QQmlComponent *delegate() const { return m_delegate; }
  void setDelegate( QQmlComponent * );

private slots:
  void delegateStatusChanged(QQmlComponent::Status status);

protected:
  virtual void recreate( Marsyas::MarSystem * system, QQmlComponent * delegate ) = 0;

private:
  void try_recreate();

  Marsyas::MarSystem * m_system;
  QQmlComponent * m_delegate;
};

class MarSystemViewAttached;

class MarSystemView : public MarSystemItem
{
  Q_OBJECT
public:
  MarSystemView( QQuickItem * parent = 0 ): MarSystemItem(parent) {}
private:
  void recreate( Marsyas::MarSystem * system, QQmlComponent * delegate );
  void createItem( MarSystem *system, QQmlComponent * delegate, QQuickItem * parent = 0 );
  void clear();
private:
  QList<QObject*> m_items;

public:
  static MarSystemViewAttached *qmlAttachedProperties(QObject *object);
};

QML_DECLARE_TYPEINFO(MarSystemView, QML_HAS_ATTACHED_PROPERTIES);

class MarSystemViewAttached : public QObject
{
  Q_OBJECT
  Q_PROPERTY( QQuickItem * childrenArea READ childrenArea WRITE setChildrenArea NOTIFY childrenAreaChanged )
  Q_PROPERTY( QString path READ path NOTIFY pathChanged )
  Q_PROPERTY( bool hasChildren READ hasChildren NOTIFY hasChildrenChanged )
  Q_PROPERTY( Marsyas::MarSystem * system READ system NOTIFY systemChanged )
  Q_PROPERTY( QObject * controls READ controls NOTIFY controlsChanged )

public:
  MarSystemViewAttached( QObject * parent = 0 ):
    QObject(parent),
    m_system(0),
    m_has_children(false)
  {
    m_controls = new QQmlPropertyMap(this);
  }

  MarSystem * system() { return m_system; }
  void setSystem( MarSystem * system );

  QString path() const { return m_path; }
  void setPath( const QString & path )
  {
    m_path = path;
    emit pathChanged();
  }

  bool hasChildren() const { return m_has_children; }
  void setHasChildren( bool hasChildren )
  {
    m_has_children = hasChildren;
    emit hasChildrenChanged();
  }

  QQuickItem * childrenArea() const { return m_childrenArea; }
  void setChildrenArea( QQuickItem * item ) { m_childrenArea = item; emit childrenAreaChanged(); }

  QObject * controls() const { return m_controls; }
  //QQmlPropertyMap & controlMap() { return m_controls; }

signals:
  void childrenAreaChanged();
  void pathChanged();
  void hasChildrenChanged();
  void systemChanged();
  void controlsChanged();

private:
  MarSystem *m_system;
  QPointer<QQuickItem> m_childrenArea;
  QString m_path;
  bool m_has_children;
  QQmlPropertyMap * m_controls;
};


class MarSystemControlView : public MarSystemItem
{
  Q_OBJECT
public:
  MarSystemControlView( QQuickItem * parent = 0 ): MarSystemItem(parent) {}
private:
  void recreate( Marsyas::MarSystem * system, QQmlComponent * delegate );
private:
  QList<QObject*> m_items;

public:
  //static TreeItemData *qmlAttachedProperties(QObject *object);
};

#if 0
class MarSystemInfo : public QObject
{
  Q_PROPERTY( int inputObservations READ inputObservations NOTIFY inputObservationsChanged )
  Q_PROPERTY( int inputSamples READ inputSamples NOTIFY inputObservationsChanged )
  Q_PROPERTY( int outputObservations READ outputObservations )
  Q_PROPERTY( int outputSamples READ outputSamples )
};
#endif

#endif // MARSYAS_INSPECTOR_MARSYSTEM_VIEW_INCLUDED
