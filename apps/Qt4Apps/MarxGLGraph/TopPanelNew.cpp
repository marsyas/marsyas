#include "TopPanelNew.h"

#include <QGridLayout>

using namespace std;
using namespace Marsyas;


TopPanelNew::TopPanelNew(string au, QWidget *parent)
  : QWidget(parent)
{

  audio_file = au;

  // create the Marsyas 
  MarSystemManager mng;
  pnet = mng.create("Series", "pnet");
  pnet->addMarSystem(mng.create("SoundFileSource", "src"));
  pnet->addMarSystem(mng.create("AudioSink", "dest"));
  pnet->updctrl( "SoundFileSource/src/mrs_string/filename", au );
//   pnet->updctrl("SoundFileSource/src/mrs_string/filename", 
// 		"/usr/home/sardine/build/marsyas-0.2.4/Marx2DGraph/permiteme.au");
  
  nTicks = 500;
  

  // initialize graphs 
  int num = 512;

  graph = new MarxGLColorGraph(this, num);

  
  QGridLayout *gridLayout = new QGridLayout;
  QPushButton *tickButton = new QPushButton(tr("Tick"));
  QSpinBox   *numTicksSpinBox = new QSpinBox();
  QSlider *contrastSlider = new QSlider(Qt::Horizontal, this);

  numTicksSpinBox->setRange(1, 1000);
  numTicksSpinBox->setValue(nTicks);

  contrastSlider->setMinimum( 0 );
  contrastSlider->setMaximum( 20 );
  contrastSlider->setSliderPosition( 20 );
  contrastSlider->setTickPosition( QSlider::TicksBelow );
  contrastSlider->setTickInterval( 1 );

  gridLayout->addWidget(graph, 0, 0, 1, 0, 0); 
  gridLayout->addWidget(contrastSlider, 1, 0, 1, 0, 0); 
  gridLayout->addWidget(tickButton, 2, 0);
  gridLayout->addWidget(numTicksSpinBox, 2, 1);
  
  
  connect(tickButton, SIGNAL(clicked()), this, SLOT(tick()));
  connect(numTicksSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setTicks(int)));
  connect(contrastSlider, SIGNAL(valueChanged(int)), this, SLOT(setContrast(int)));
  setLayout(gridLayout);




}
	

void
TopPanelNew::setContrast( int c )
{
  graph->setContrast( (float)c/10.0 );
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
      pnet->updctrl("mrs_bool/probe", true);
      realvec out(512);
      out = pnet->getctrl("mrs_realvec/input0")->to<mrs_realvec>();


      graph->setBuffer( out );
      
    }
  

  

  
  
}
