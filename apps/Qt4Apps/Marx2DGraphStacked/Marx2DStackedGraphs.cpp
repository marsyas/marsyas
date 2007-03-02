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

/**
   \class Marx2DStackedGraphs.cpp
   \brief 2D plotting of multiple graphs sharing an x-axis

Marx2DStackedGraphs is essentially a wrapper for Marx2DGraph that
allows multiple graphs to be plotted together along a shared x-axis,
ie. graph buffers should be of identitical lengths.
*/

#include "Marx2DStackedGraphs.h"

#include <QPushButton>

using namespace std;
using namespace Marsyas;


//! A default constructor
/*!
  A default constructor when buffer size nor number of graphs has been
  specified.

  \param *parent QWidget pointer to the parent widget
*/
Marx2DStackedGraphs::Marx2DStackedGraphs(QWidget *parent )
  : QWidget(parent)
{
  Marx2DStackedGraphs(0, 0, parent);
}


//! An overloaded constructor
/*!
  An overloaded constructor used when the number of graphs is known
  but buffer size has not been specified.

  \param n an unsigned int giving the number of graphs to plot
  \param *parent QWidget pointer to the parent widget
*/
Marx2DStackedGraphs::Marx2DStackedGraphs(unsigned int n, QWidget *parent )
  : QWidget(parent)
{
  Marx2DStackedGraphs(0, n, parent);
}


//! The real constructor
/*!
  This is the real constructor that ultimately is called by the defaults.

  \param s an int giving the buffer size
  \param n an unsigned int giving the number of graphs to plot
  \param *parent QWidget pointer to the parent widget
*/
Marx2DStackedGraphs::Marx2DStackedGraphs(int s, unsigned int n, QWidget *parent )
  : QWidget(parent)
{
  numgraphs = n;
  graphs.reserve(sizeof(int) * numgraphs);

  buffersize = s;

  for (int i=0; i<numgraphs; i++) {
    Marx2DGraph* marx = new Marx2DGraph(buffersize, 0);

    if (i !=  numgraphs-1) {
      marx->displayXaxis( false );
    }
    marx->setAxisDisplayType( Marx2DGraph::BROKEN );

    graphs.push_back( marx );
  }

  QGridLayout *gridLayout = new QGridLayout; 
  gridLayout->setSpacing( 0 );
  vector<Marx2DGraph*>::iterator vi;
  int i=0;
  for (vi = graphs.begin(); vi != graphs.end(); ++vi) {
    Marx2DGraph* marx = *vi;
    gridLayout->addWidget(marx, i, 0);  i++;
    setLayout(gridLayout);
  }
  
}



//! update the buffers
/*!
  Used to update all of the buffers of this stacked graph, ie. the
  realvec argument is multidimensional with col and rows equal to or
  greater than 1.

  \param realvec& a multidimensional realvec
  \return bool indicates if the buffer was successfully updated
*/
bool
Marx2DStackedGraphs::setBuffers( realvec& rv )
{
  if (rv.getCols() == numgraphs && rv.getRows() == buffersize) {
    
    vector<Marx2DGraph*>::iterator vi;
    int i=0;
    for (vi = graphs.begin(); vi != graphs.end(); ++vi) {
      Marx2DGraph* marx = *vi;

      // this doesn't really seem that efficient
      realvec* rt = new realvec(rv.getRows());
      for (int k=0; k<rv.getRows(); k++) {
	(*rt)(k) = rv(k, i);
      }
      
      marx->setBuffer( *rt );
      
      i++;
    }
    return true;
  }
  else {
    return false;
  }
}


//! Set per graph int args
/*!
  This method communicates its arguments to the individual graphs.
  The first argument is the graph reference number starting from
  zero.  The second argument is the name of Marx2DGraph method to
  call.  The final argument is the argument for the Marx2DGraph method
  to be communicated.  For instance, say one wanted to set the plot
  type for the second of three stacked graphs:

  myStackedGraph.setMarxGraphIntArgs(1, 
                                     setPlotType,
                                     LINEAR_INTERPOLATION);

  \param int g the graph reference
  \param int m an enum of possible Marx2DGraph methods
  \param int a an enum of arguments the Marx2DGraph method

  \return bool true if action appears successful
*/
bool 
Marx2DStackedGraphs::setMarxGraphIntArgs( int g, int m, int a )
{
  if (g >= (int)numgraphs){ return false; }

  switch (m) {
  case Marx2DStackedGraphs::setPlotType:
    graphs[g]->setPlotType( a );
    break;
  case Marx2DStackedGraphs::setAxisDisplayType:
    graphs[g]->setAxisDisplayType(a);
    break;
  case Marx2DStackedGraphs::setGraphDataPointType:
    graphs[g]->setGraphDataPointType(a);
    break;
  default:
    return false;
  }

  return true;
}


//! update a color feature of a graph
/*!
  This method works similar to setMarxGraphIntArgs except that it
  operates on QColor arguments instead of integers.

  \param int g graph number starting at zero
  \param int m the method given in the enum
  \param QColor the QColor argument

  \return bool was this method successful?
  
  \sa setMarxGraphIntArgs
*/
bool 
Marx2DStackedGraphs::setMarxGraphQColorArgs( int g, int m, QColor c)
{
  if (g >= (int)numgraphs){ return false; }

  switch (m) {
  case Marx2DStackedGraphs::setGraphDataColor:
    graphs[g]->setGraphDataColor( c );
    break;
  case  Marx2DStackedGraphs::setGraphLabelsAndAxisColor:
    graphs[g]->setGraphLabelsAndAxisColor( c );
    break;
  case Marx2DStackedGraphs::setBGColor:
    graphs[g]->setBGColor( c );
    break;
  default:
    return false;
  }

  return true;
}


//! update a float feature of a Marx2DGraph
/*!
  This method works similar to setMarxGraphIntArgs except that it
  operates on float arguments instead of integers.

  \param int g graph number starting at zero
  \param int m the method given in the enum
  \param float f

  \return bool was this method successful?
  
  \sa setMarxGraphIntArgs
*/
bool 
Marx2DStackedGraphs::setMarxGraphFloatArgs( int g, int m, float f)
{
  if (g >= (int)numgraphs){ return false; }

  switch (m) {
  case Marx2DStackedGraphs::setGraphDataPointSize:
    graphs[0]->setGraphDataPointSize( f );
    break;
  case Marx2DStackedGraphs::setGraphDataLineSize:
    graphs[0]->setGraphDataLineSize( f );
    break;
  default:
    return false;
  }

  return true;
}


//! update a boolean feature of a Marx2DGraph
/*!
  This method works similar to setMarxGraphIntArgs except that it
  operates on boolean arguments instead of integers.

  \param int g graph number starting at zero
  \param int m the method given in the enum
  \param boolean tf argument to Marx2DGraph method

  \return bool was this method successful?
  
  \sa setMarxGraphIntArgs
*/
bool 
Marx2DStackedGraphs::setMarxGraphBooleanArgs( int g, int m, bool tf )
{
  if (g >= (int)numgraphs){ return false; }

  switch (m) {
  case Marx2DStackedGraphs::setShowAxisScale:
    graphs[0]->setShowAxisScale( tf );
    break;
  case Marx2DStackedGraphs::setAntialias:
    graphs[0]->setAntialias( tf );
    break;
  case Marx2DStackedGraphs::setXAxisLabelOn:
    graphs[0]->setXAxisLabelOn( tf );
    break;
  case Marx2DStackedGraphs::setYAxisLabelOn:
    graphs[0]->setYAxisLabelOn( tf );
    break;
  default:
    return false;
  }

  return true;
}


//! update a string feature of a Marx2DGraph
/*!
  This method works similar to setMarxGraphIntArgs except that it
  operates on string arguments instead of integers.

  \param int g graph number starting at zero
  \param int m the method given in the enum
  \param string label argument to Marx2DGraph method

  \return bool was this method successful?
  
  \sa setMarxGraphIntArgs
*/
bool 
Marx2DStackedGraphs::setMarxGraphStringArgs( int g, int m, string s)
{
  if (g >= (int)numgraphs){ return false; }

  switch (m) {
  case Marx2DStackedGraphs::setXAxisLabel:
    graphs[0]->setXAxisLabel( s );
    break;
  case Marx2DStackedGraphs::setYAxisLabel:
    graphs[0]->setYAxisLabel( s );
    break;    
  case Marx2DStackedGraphs::addLabel:
    graphs[0]->addLabel( s );
    break;
  default:
    return false;
  }

  return true;
}
