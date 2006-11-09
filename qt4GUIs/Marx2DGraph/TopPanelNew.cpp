#include "TopPanelNew.h"

using namespace std;
using namespace Marsyas;


TopPanelNew::TopPanelNew(string au, QWidget *parent)
  : QWidget(parent)
{

  audio_file = au;
  nGraphs_ = 2;

  out_.create(512);  
  
  // create the Marsyas 
  MarSystemManager mng;
  
  /* ifstream pluginStream("test2.mpl");
  pnet_ = mng.getMarSystem(pluginStream);

  if (pnet_ == 0) 
    {
      cout << "Manager does not support system " << endl;
      exit(1);
    }
  */ 
  
  pnet_ = mng.create("Series", "pnet");
  pnet_->addMarSystem(mng.create("SoundFileSource", "src"));
  pnet_->addMarSystem(mng.create("Gain", "gain"));
  pnet_->addMarSystem(mng.create("Windowing", "windowing"));
  pnet_->addMarSystem(mng.create("Gain", "gain2"));
  pnet_->addMarSystem(mng.create("Hamming", "ham2"));
  
  
  pnet_->updctrl("SoundFileSource/src/mrs_string/filename", au );
  pnet_->updctrl("Windowing/windowing/mrs_natural/zeroPhasing", 1);
  pnet_->updctrl("Gain/gain/mrs_real/gain", 2.0);
  pnet_->updctrl("Gain/gain2/mrs_real/gain", 3.0);
  pnet_->updctrl("mrs_bool/probe", true);
  
  cout << *pnet_ << endl;
  
  
  nTicks = 500;
  

  // initialize graphs 
  int num = 512;



  // graph2->addLabel( "Demo Graph");
  
  graph3 = new Marx2DGraph(num, 0);
  graph3->setPlotType(Marx2DGraph::LINEAR_INTERPOLATION);
  graph3->setPlotType(Marx2DGraph::POINTS);
  // graph3->addLabel("redundant graph for testing");
  graph3->setGraphDataLineSize( 1.0 );

  
  gridLayout_ = new QGridLayout;
  QPushButton *tickButton = new QPushButton(tr("Tick"));
  QPushButton *graphButton = new QPushButton(tr("New Graph"));
  QPushButton *setupButton = new QPushButton(tr("Setup"));
  

  /* QSpinBox   *numTicksSpinBox = new QSpinBox();
  numTicksSpinBox->setRange(1, 1000);
  numTicksSpinBox->setValue(nTicks);
  */ 


  gridLayout_->addWidget(tickButton, 0, 1);
  gridLayout_->addWidget(graphButton, 0, 0);
  gridLayout_->addWidget(setupButton, 0, 2);
  
  
  connect(graphButton, SIGNAL(clicked()), this, SLOT(graph()));  
  connect(tickButton, SIGNAL(clicked()), this, SLOT(tick()));
  connect(setupButton, SIGNAL(clicked()), this, SLOT(setup()));

  // connect(numTicksSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setTicks(int)));
  setLayout(gridLayout_);


  
      


  

}
	



void 
TopPanelNew::graph()
{
 int num = 512;
 nGraphs_ += 1;

 // Create new graph
 Marx2DGraph* graph = new Marx2DGraph(num, 0);
 graph->setPlotType(Marx2DGraph::LINEAR_INTERPOLATION);
 graph->setGraphDataLineSize( 1.0 );
 graphs.push_back(graph);
 
 gridLayout_->addWidget(graph, nGraphs_/3, (nGraphs_ % 3));

 probes_.push_back("/Series/pnet/mrs_realvec/input0");
 tick();
 
}


void 
TopPanelNew::dialogDone()
{
  if (graphs.size() == 0) 
    return;
  else 
    {
      
      probes_[graphNum->value()] = listWidget->currentItem()->text().toStdString();
      
    }
  
  tick();
  
}


void 
TopPanelNew::setup()
{
  QDialog *setupDialog = new QDialog(this);

  QLabel* label = new QLabel(tr("Setup probe for Graph"));
  QPushButton* doneButton = new QPushButton(tr("Done"));
  

  QGridLayout *layout = new QGridLayout;



  std::map<std::string, MarControlPtr> mycontrols = pnet_->getControls();
  std::map<std::string, MarControlPtr>::iterator myc;

  listWidget = new QListWidget();  
  
  for (myc = mycontrols.begin(); myc != mycontrols.end(); ++myc)
    {
      if ((myc->first).find("realvec") < myc->first.length())
	{
	  QListWidgetItem *newItem = new QListWidgetItem(tr(myc->first.c_str()), listWidget);	  
	}
      
    }
  
  listWidget->setCurrentRow(0);
  

  if (graphs.size() > 0)
    {
      graphNum = new QSpinBox();
      graphNum->setWrapping(true);
      graphNum->setMaximum(graphs.size()-1);
      layout->addWidget(graphNum, 1, 0);  
    }
  

  layout->addWidget(label, 0, 0);
  layout->addWidget(listWidget, 2, 0);

  layout->addWidget(doneButton, 3, 0);

  setupDialog->setLayout(layout);
  connect(doneButton, SIGNAL(clicked()), setupDialog, SLOT(accept()));
  connect(doneButton, SIGNAL(clicked()), this, SLOT(dialogDone()));  
  
  setupDialog->exec();
}


void 
TopPanelNew::tick()
{

  pnet_->tick();  
  for (int i = 0; i < graphs.size(); ++i) 
    {
      
      out_ = pnet_->getControl(probes_[i])->toVec();
      graphs[i]->setBuffer( out_ );
    }
  
  


  
  
}
