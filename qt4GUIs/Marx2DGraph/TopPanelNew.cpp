#include "TopPanelNew.h"


TopPanelNew::TopPanelNew(string au, QWidget *parent)
  : QWidget(parent)
{

  audio_file = au;

  // create the Marsyas 
  MarSystemManager mng;
  pnet = mng.create("Series", "pnet");
  pnet->addMarSystem(mng.create("SoundFileSource", "src"));
  pnet->addMarSystem(mng.create("AudioSink", "dest"));
  pnet->updctrl( "SoundFileSource/src/string/filename", au );
//   pnet->updctrl("SoundFileSource/src/string/filename", 
// 		"/usr/home/sardine/build/marsyas-0.2.4/Marx2DGraph/permiteme.au");
  
  nTicks = 500;
  

  // initialize graphs 
  int num = 512;

  graph2 = new Marx2DGraph(num, 0);
  //graph2->setPlotType(Marx2DGraph::POINTS);
  graph2->setPlotType(Marx2DGraph::POLYNOMIAL_INTERPOLATION);
  graph2->setGraphDataLineSize( 1.0 );

  graph2->addLabel( "Demo Graph");
  
//   graph3 = new Marx2DGraph(num, 0);
//   graph3->setPlotType(Marx2DGraph::LINEAR_INTERPOLATION);
//   graph3->addLabel("redundant graph for testing");

  
  QGridLayout *gridLayout = new QGridLayout;
  QPushButton *tickButton = new QPushButton(tr("Tick"));
  QSpinBox   *numTicksSpinBox = new QSpinBox();
  numTicksSpinBox->setRange(1, 1000);
  numTicksSpinBox->setValue(nTicks);

  gridLayout->addWidget(graph2, 0, 0, 1, 0, 0); 
//   gridLayout->addWidget(graph3, 0, 1); 
  gridLayout->addWidget(tickButton, 1, 0);
  gridLayout->addWidget(numTicksSpinBox, 1, 1);
  
  
  connect(tickButton, SIGNAL(clicked()), this, SLOT(tick()));
  connect(numTicksSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setTicks(int)));
  setLayout(gridLayout);




}
	

void 
TopPanelNew::setTicks(int v)
{
//   cout << "setTicks called" << endl;
  nTicks = v;
}


void 
TopPanelNew::tick()
{
//   cout << "Tick called" << endl;
  
  for (int i=0; i < nTicks; i++) 
    {
      pnet->tick();
      pnet->updctrl("bool/probe", true);
      realvec out(512);
      out = pnet->getctrl("realvec/input0").toVec();


      graph2->setBuffer( out );
//       graph3->setBuffer( out );
      
    }
  

  

  
  
}
