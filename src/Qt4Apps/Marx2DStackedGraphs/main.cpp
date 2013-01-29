#include <QApplication>
#include <QPushButton>

#include <cmath>
#include <iostream>

#include "common_source.h" //for PI

#include "Marx2DStackedGraphs.h"

using namespace std;
using namespace Marsyas;


class TopPanel : public QWidget
{
public:
  TopPanel(QWidget *parent = 0);
};

TopPanel::TopPanel(QWidget *parent)
  : QWidget(parent)
{
  int num = 128;

  realvec* x;
  x = new realvec(num, 4);
  for (int i=0; i<num; i++) {
    double val = sin( ((double)i/(double)(num-1)) * 2 * PI );
    
    (*x)(i, 0) = val;
    (*x)(i, 1) = val;
    (*x)(i, 2) = val;
    (*x)(i, 3) = val;
  }

//   realvec* x;
//   x = new realvec(num);
//   for (int i=0; i<num; i++) {
//     double val = sin( ((double)i/(double)(num-1)) * 2 * PI );
//     (*x)(i) = val;
//   }



  Marx2DStackedGraphs* graph1 = new Marx2DStackedGraphs(num, 4, 0);
  graph1->setBuffers( *x );
  graph1->setMarxGraphIntArgs(2, 
			      Marx2DStackedGraphs::setPlotType, 
			      Marx2DGraph::LINEAR_INTERPOLATION);
  graph1->setMarxGraphQColorArgs(3,
				 Marx2DStackedGraphs::setBGColor,
				 QColor( 0, 255, 0));
  graph1->setMarxGraphFloatArgs(2,
				Marx2DStackedGraphs::setGraphDataLineSize,
				50.0);
  graph1->setMarxGraphBooleanArgs(1,
				  Marx2DStackedGraphs::setShowAxisScale,
				  true);
  graph1->setMarxGraphStringArgs(3,
				 Marx2DStackedGraphs::setYAxisLabel,
				 "yah man");



//   Marx2DGraph* graph1 = new Marx2DGraph(num, 0);
//   graph1->setPlotType(Marx2DGraph::POLYNOMIAL_INTERPOLATION);
//   graph1->setBuffer( *x );
//   graph1->addLabel("Points (realvec)");
//   graph1->setXAxisLabel( "SAMPLES" );
//   graph1->setYAxisLabel( "AMPLITUDE" );
//   graph1->setXAxisLabelOn( true );
//   graph1->setYAxisLabelOn( true );

  QGridLayout *gridLayout = new QGridLayout;
  gridLayout->addWidget(graph1, 0, 0);
  setLayout(gridLayout);

}

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  TopPanel panel;
  panel.setGeometry(100, 100, 800, 600);
  panel.show();

  return app.exec();
}
