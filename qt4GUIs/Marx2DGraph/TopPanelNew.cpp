#include "TopPanelNew.h"


TopPanelNew::TopPanelNew(QWidget *parent)
  : QWidget(parent)
{


  // create the Marsyas 
  MarSystemManager mng;
  pnet = mng.create("Series", "pnet");
  pnet->addMarSystem(mng.create("SoundFileSource", "src"));
  pnet->addMarSystem(mng.create("AudioSink", "dest"));
  pnet->updctrl("SoundFileSource/src/string/filename", 
		"/home/gtzan/data/sound/music_speech/music/gravity.au");
  
  nTicks = 1;
  

  // initialize graphs 
  int num;
  float* x;

  num = 512;
  x = new float[num];
  for (int i=0; i<num; i++) {
    x[i] = 0.0;
  }
  
  graph2 = new Marx2DGraph(num, 0);
  graph2->setPlotType(Marx2DGraph::POINTS);
  graph2->setBuffer(x, num);
  graph2->addLabel("Points");
  
  graph3 = new Marx2DGraph(num, 0);
  graph3->setPlotType(Marx2DGraph::LINEAR_INTERPOLATION);
  graph3->setBuffer(x, num);
  graph3->addLabel("Linear Interpolation");

  
  QGridLayout *gridLayout = new QGridLayout;
  QPushButton *tickButton = new QPushButton(tr("Tick"));
  QSpinBox   *numTicksSpinBox = new QSpinBox();
  numTicksSpinBox->setRange(1, 1000);
  numTicksSpinBox->setValue(1);

  gridLayout->addWidget(graph2, 0, 0); 
  gridLayout->addWidget(graph3, 0, 1); 
  gridLayout->addWidget(tickButton, 1, 0);
  gridLayout->addWidget(numTicksSpinBox, 1, 1);
  
  
  connect(tickButton, SIGNAL(clicked()), this, SLOT(tick()));
  connect(numTicksSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setTicks(int)));
  setLayout(gridLayout);




}
	

void 
TopPanelNew::setTicks(int v)
{
  cout << "setTicks called" << endl;
  nTicks = v;
}


void 
TopPanelNew::tick()
{
  cout << "Tick called" << endl;
  
  int num = 512;
  float* x = new float[num];
  
  for (int i=0; i < nTicks; i++) 
    {
      pnet->tick();
      pnet->updctrl("bool/probe", true);
      realvec out(512);
      out = pnet->getctrl("realvec/input0").toVec();
      for (int i=0; i<num; i++) {
	x[i] = out(i);
      }

      graph2->setBuffer(x, num);
      graph3->setBuffer(x, num);            
      
    }
  

  

  
  
}
