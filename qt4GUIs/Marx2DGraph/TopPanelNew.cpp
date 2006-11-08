#include "TopPanelNew.h"

using namespace std;
using namespace Marsyas;


TopPanelNew::TopPanelNew(string au, QWidget *parent)
  : QWidget(parent)
{

  audio_file = au;
  nGraphs_ = 1;
  
  // create the Marsyas 
  MarSystemManager mng;
  pnet_ = mng.create("Series", "pnet");
  pnet_->addMarSystem(mng.create("SoundFileSource", "src"));
  pnet_->addMarSystem(mng.create("Gain", "gain"));
  pnet_->addMarSystem(mng.create("Gain", "gain2"));
  
  // pnet_->addMarSystem(mng.create("AudioSink", "dest"));
  pnet_->updctrl("SoundFileSource/src/mrs_string/filename", au );
  pnet_->updctrl("Gain/gain/mrs_real/gain", 2.0);
  cout << "pnet_ = " << *pnet_ << endl;
  pnet_->updctrl("mrs_bool/probe", true);

  
  cout << "pnet_ = " << *pnet_ << endl;
  


  
  nTicks = 500;
  

  // initialize graphs 
  int num = 512;



  // graph2->addLabel( "Demo Graph");
  
  graph3 = new Marx2DGraph(num, 0);
  graph3->setPlotType(Marx2DGraph::LINEAR_INTERPOLATION);
  // graph3->addLabel("redundant graph for testing");
  graph3->setGraphDataLineSize( 1.0 );

  
  gridLayout_ = new QGridLayout;
  QPushButton *tickButton = new QPushButton(tr("Tick"));
  QPushButton *graphButton = new QPushButton(tr("New Graph"));
  
  /* QSpinBox   *numTicksSpinBox = new QSpinBox();
  numTicksSpinBox->setRange(1, 1000);
  numTicksSpinBox->setValue(nTicks);
  */ 


  gridLayout_->addWidget(tickButton, 0, 1);
  gridLayout_->addWidget(graphButton, 0, 0);
  
  
  connect(graphButton, SIGNAL(clicked()), this, SLOT(graph()));  
  connect(tickButton, SIGNAL(clicked()), this, SLOT(tick()));

  // connect(numTicksSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setTicks(int)));
  setLayout(gridLayout_);


  //std::map<std::string, MarControlPtr>& mycontrols = pnet_->getControls();
  // std::map<std::string, MarControlPtr>::iterator myc;
  
  // for (myc = mycontrols.begin(); myc != mycontrols.end(); ++myc)
  // {
  // cout << "myc = " << myc->first();
  // }
  
      
  
  

}
	

void 
TopPanelNew::setTicks(int v)
{
//   cout << "setTicks called" << endl;
  nTicks = v;
}


void 
TopPanelNew::graph()
{
 int num = 512;
 Marx2DGraph* graph = new Marx2DGraph(num, 0);
 graphs.push_back(graph);
 
 nGraphs_ += 1;
 
 cout << "nGraphs_ " << nGraphs_ << endl;
 cout << "nGraphs_ %2 " << (nGraphs_ % 2) << endl;
 
 //graph2->setPlotType(Marx2DGraph::POINTS);
 graph->setPlotType(Marx2DGraph::LINEAR_INTERPOLATION);
 graph->setGraphDataLineSize( 1.0 );
 gridLayout_->addWidget(graph, nGraphs_/2, (nGraphs_ % 2));

 
 tick();
 
}


void 
TopPanelNew::tick()
{
      
      
  realvec out(512);  
  pnet_->tick();  
  for (int i = 0; i < graphs.size(); ++i) 
    {
      
      out = pnet_->getctrl("mrs_realvec/input0")->toVec();
      cout << "out " << out << endl;
      
      graphs[i]->setBuffer( out );
    }
  
  


  
  
}
