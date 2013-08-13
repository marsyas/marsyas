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

#include <MarSystem.h>
#include <QWidget>
#include <QListWidget>
#include <QLabel>

class DebugController;

class DebugWidget : public QWidget
{
  Q_OBJECT

public:
  DebugWidget( DebugController * debugger, QWidget * parent = 0 );
  void setSystem( Marsyas::MarSystem * system );

signals:
  void pathClicked( const QString & path );

public slots:
  void openRecording();
  void updateReport();
  void clear();

private slots:
  void onItemClicked( QListWidgetItem * );

private:
  enum DataRole {
    PathRole = Qt::UserRole
  };

  DebugController *m_debugger;
  QLabel *m_rec_label;
  QListWidget *m_bug_list;
};

#endif // MARSYAS_INSPECTOR_DEBUG_WIDGET_INCLUDED
