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


#ifndef MARSYAS_INSPECTOR_DEBUG_WIDGET_INCLUDED
#define MARSYAS_INSPECTOR_DEBUG_WIDGET_INCLUDED

#include <marsyas/debug/record.h>

#include <marsyas/system/MarSystem.h>
#include <QWidget>
#include <QTreeWidget>

class DebugController;
class ActionManager;

class StatisticsWidget : public QWidget
{
  Q_OBJECT

public:
  StatisticsWidget( ActionManager *, DebugController * debugger,
               QWidget * parent = 0 );

  void setSystem( Marsyas::MarSystem * );

signals:
  void pathClicked( const QString & path );

public slots:
  void updateReport();

private slots:
  void onItemClicked( QTreeWidgetItem *, int );

private:
  enum DataRole {
    AbsolutePathRole = Qt::UserRole,
    RelativePathRole
  };

  void recursiveAddSystem( Marsyas::MarSystem *, QTreeWidgetItem * parent );
  void recursiveUpdateChildItems( QTreeWidgetItem *parent,
                                  const Marsyas::Debug::Record * );

  DebugController *m_debugger;
  QTreeWidget *m_report_view;
};

#endif // MARSYAS_INSPECTOR_DEBUG_WIDGET_INCLUDED
