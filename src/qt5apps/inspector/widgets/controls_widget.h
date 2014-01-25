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

#ifndef MARSYAS_INSPECTOR_CONTROLS_LIST_WIDGET_INCLUDED
#define MARSYAS_INSPECTOR_CONTROLS_LIST_WIDGET_INCLUDED

#include <marsyas/system/MarSystem.h>

#include <QSortFilterProxyModel>
#include <QTreeView>
#include <QLineEdit>
#include <QCheckBox>

namespace MarsyasQt {
  class ControlModel;
}

class ControlFilterModel : public QSortFilterProxyModel
{
  Q_OBJECT

public:
  ControlFilterModel( QObject * parent = 0 ):
    QSortFilterProxyModel(parent),
    m_show_private(false)
  {}

  bool filterAcceptsRow(int row,
                        const QModelIndex & parent) const;

public slots:
  void setShowPrivate(bool visible)
  {
    m_show_private = visible;
    invalidateFilter();
  }

private:
  bool m_show_private;
};

class ControlsWidget : public QWidget
{
  Q_OBJECT
public:
  ControlsWidget( QWidget * parent = 0 );
  void setSystem( Marsyas::MarSystem * system );
  Marsyas::MarSystem * system() const { return m_system; }

  QSize sizeHint() const { return QSize(500,300); }

public slots:
    void refresh();

signals:
    void pathChanged( const QString & );
    void controlClicked( const QString & path );

private slots:
    void onItemClicked(const QModelIndex & index);

private:
  enum DataColumns {
    NameColumn = 0,
    ValueColumn,
    TypeColumn,
    PathColumn
  };

  Marsyas::MarSystem * m_system;
  MarsyasQt::ControlModel *m_model;
  ControlFilterModel *m_filter_model;
  QTreeView *m_tree;
  QLineEdit *m_filter_edit;
  QCheckBox *m_private_toggle;
};

#endif // MARSYAS_INSPECTOR_CONTROLS_LIST_WIDGET_INCLUDED
