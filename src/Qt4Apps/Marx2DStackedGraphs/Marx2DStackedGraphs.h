/*
** Copyright (C) 1998-2006 George Tzanetakis <gtzan@cs.uvic.ca>
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

#ifndef MARX2DSTACKEDGRAPHS_H
#define MARX2DSTACKEDGRAPHS_H 

#include <QWidget>
#include <QGridLayout>
#include <QPaintEvent>
#include <QGridLayout>

#include "Marx2DGraph.h"
using namespace MarsyasQt;


class Marx2DStackedGraphs : public QWidget
{

 public:
  Marx2DStackedGraphs(QWidget *parent = 0);//warning C4520: 'Marx2DGraph' : multiple default constructors specified	[!]
  Marx2DStackedGraphs(unsigned int n=0, QWidget *parent = 0);//warning C4520: 'Marx2DGraph' : multiple default constructors specified	[!]
  Marx2DStackedGraphs(int s=0, unsigned int n=0, QWidget *parent = 0 );//warning C4520: 'Marx2DGraph' : multiple default constructors specified	[!]

  bool setBuffers( Marsyas::realvec& );

  enum { setPlotType, setAxisDisplayType, setGraphDataPointType };
  bool setMarxGraphIntArgs( int, int, int );

  enum { setGraphDataColor, setGraphLabelsAndAxisColor, setBGColor };
  bool setMarxGraphQColorArgs( int, int, QColor );

  enum { setGraphDataPointSize, setGraphDataLineSize };
  bool setMarxGraphFloatArgs( int, int, float );

  enum { setShowAxisScale, setAntialias, setXAxisLabelOn, setYAxisLabelOn };
  bool setMarxGraphBooleanArgs( int, int, bool );

  enum { setXAxisLabel, setYAxisLabel, addLabel }; 
  bool setMarxGraphStringArgs( int, int, std::string );

 protected:


 private:
  std::vector<Marx2DGraph*> graphs;
  unsigned int numgraphs;

  int buffersize;
  float *buffers;

};


#endif
