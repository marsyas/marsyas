#include "TopPanel.h"

using namespace std;
using namespace Marsyas;


TopPanel::TopPanel(QWidget *parent)
  : QWidget(parent)
{
  long num = 7;

  realvec x = realvec(num);
  for (int i=0; i<num; i++) {
	  x(i) = sin( ((float)i/(float)(num-1)) * 2 * M_PI );
  }

  

  Marx2DGraph* graph1 = new Marx2DGraph(num, 0);
  graph1->setPlotType(Marx2DGraph::POLYNOMIAL_INTERPOLATION);
  graph1->setBuffer( x);
  graph1->addLabel("Polynomial Interpolation");

  num = 32;
  x = realvec(num);
  for (int i=0; i<num; i++) {
	  x(i) = sin( ((float)i/(float)(num-1)) * 2 * M_PI );
  }

  Marx2DGraph* graph2 = new Marx2DGraph(num, 0);
  graph2->setPlotType(Marx2DGraph::POINTS);
  graph2->setBuffer(x);
  graph2->setGraphDataPointType( Marx2DGraph::XS );
  graph2->addLabel("Points -- XS");

  Marx2DGraph* graph3 = new Marx2DGraph(num, 0);
  graph3->setPlotType(Marx2DGraph::LINEAR_INTERPOLATION);
  graph3->setBuffer( x);
  graph3->addLabel("Linear Interpolation");

  Marx2DGraph* graph5 = new Marx2DGraph(num, 0);
  graph5->setPlotType(Marx2DGraph::POINTS);
  graph5->setBuffer( x);
  graph5->setGraphDataPointType( Marx2DGraph::CIRCLES );
  graph5->addLabel("Points -- CIRCLES");

  Marx2DGraph* graph6 = new Marx2DGraph(num, 0);
  graph6->setPlotType(Marx2DGraph::POINTS);
  graph6->setBuffer( x);
  graph6->setGraphDataPointType( Marx2DGraph::SQUARES );
  graph6->addLabel("Points -- SQUARES");

  num = 64;
  x = realvec(num);
  for (int i=0; i<num; i++) {
	  x(i) = sin( ((float)i/(float)(num-1)) * 2 * M_PI );
  }

  Marx2DGraph* graph4 = new Marx2DGraph(num, 0);
  graph4->setPlotType(Marx2DGraph::PRECISION);
  graph4->setBuffer( x);
  graph4->addLabel("Precision Mode");


  QGridLayout *gridLayout = new QGridLayout;
  gridLayout->addWidget(graph1, 0, 0);
  gridLayout->addWidget(graph2, 1, 0); 
  gridLayout->addWidget(graph3, 2, 0); 
  gridLayout->addWidget(graph4, 3, 0); 
  gridLayout->addWidget(graph5, 0, 1); 
  gridLayout->addWidget(graph6, 1, 1); 
//   gridLayout->setColumnStretch(1, 10);
  setLayout(gridLayout);

}
	
