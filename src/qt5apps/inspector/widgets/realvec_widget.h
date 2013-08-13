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
#include <QLineEdit>
#include <QStackedLayout>
#include <QComboBox>

namespace MarsyasQt {
class RealvecTableWidget;
}

class DebugController;

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

  RealvecWidget( DebugController *debugger, QWidget * parent = 0 );
  QSize sizeHint() const { return QSize(500,400); }

public slots:
  void setDisplayType( int type );
  void displayControl( Marsyas::MarSystem *system, const QString & path );
  void displayDebugValue( const QString & path );
  void refresh();
  void clear();

private:
  void refreshFromControl();
  void refreshFromDebugger();
  void clearData();

  DebugController *m_debugger;

  bool m_display_debugger;
  QString m_path;
  Marsyas::MarSystem * m_system;

  QStackedLayout * m_stack;
  QComboBox * m_display_type_selector;

  QLineEdit *m_label;
  MarsyasQt::RealvecTableWidget *m_table;
  MarsyasQt::Marx2DGraph *m_graph;

  QImage m_image;
};

#endif // MARSYAS_INSPECTOR_REALVEC_WIDGET_INCLUDED
