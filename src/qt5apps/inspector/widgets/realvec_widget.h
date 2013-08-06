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

#ifndef MARSYAS_INSPECTOR_REALVEC_WIDGET_INCLUDED
#define MARSYAS_INSPECTOR_REALVEC_WIDGET_INCLUDED

#include <MarSystem.h>
#include <Marx2DGraph.h>
#include <QWidget>
#include <QTableView>
#include <QAbstractTableModel>
#include <QLabel>
#include <QStackedLayout>
#include <QComboBox>

namespace MarsyasQt {
class RealvecTableWidget;
}

class RealvecWidget : public QWidget
{
  Q_OBJECT
public:
  enum DisplayType {
    Table,
    Points,
    Precision,
    LinearInterpolation,
    PolynomialInterpolation
  };

  RealvecWidget( QWidget * parent = 0 );
  void setSystem( Marsyas::MarSystem * system );

public slots:
  void displayControl( const QString & path );
  void setDisplayType( int type );
  void refresh();

private:
  void clear();
  void refreshTable( const Marsyas::realvec & );
  void refreshGraph( const Marsyas::realvec & );

  Marsyas::MarSystem * m_system;
  QString m_control_path;

  QStackedLayout * m_stack;
  QComboBox * m_display_type_selector;

  QLabel *m_label;

  MarsyasQt::RealvecTableWidget *m_table;

  MarsyasQt::Marx2DGraph *m_graph;

  QImage m_image;
};

#endif // MARSYAS_INSPECTOR_REALVEC_WIDGET_INCLUDED
