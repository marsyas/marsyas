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
  
  nTicks = 128;
  

  // initialize graphs 
  int num = 512;
  float y_rot = 120.0;
  float x_tran = 0.4;
  float y_tran = -0.5;
  float z_tran = -1.0;
  float x_stretch = 12.0;
  float y_stretch = 2.5;

  graph = new MarxGL2Din3DSpaceGraph(this, num);
  graph->setXAxisStretch( x_stretch );
  graph->setYAxisStretch( y_stretch );

  graph->setXTranslation( x_tran );
  graph->setYTranslation(  y_tran );
  graph->setZTranslation( z_tran );

  graph->setYNormalizeFactor( 1.0f );
  graph->setYRotation( 0.0f );
  graph->setYRotation( y_rot );

  //graph->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );
  graph->setMinimumHeight( 300 );


  QGroupBox* rotation_box = new QGroupBox( tr("Y Rotation") );
  QGroupBox* stretch_box = new QGroupBox( tr("Model Stretch") );
  QGroupBox* translation_box = new QGroupBox( tr("Model Translation") );
  QGroupBox* tick_box = new QGroupBox( tr("Song Increment") );

  
  // ROTATION GROUP BOX
  QSlider* rotation_slider = new QSlider( Qt::Horizontal, rotation_box );
  rotation_slider->setMinimum( 0 );
  rotation_slider->setMaximum( 180 );
  rotation_slider->setSliderPosition( (int)y_rot );
  rotation_slider->setTickPosition( QSlider::TicksBelow );
  rotation_slider->setTickInterval( 1 );

  QGridLayout *rotationLayout = new QGridLayout;
  rotationLayout->addWidget( rotation_slider, 1, 0 );


  // TRANSLATION GROUP BOX
  QSlider* x_translation_slider = new QSlider( Qt::Horizontal, translation_box );
  x_translation_slider->setMinimum( -10 );
  x_translation_slider->setMaximum( 10 );
  x_translation_slider->setSliderPosition( (int)(x_tran*10) );
  x_translation_slider->setTickPosition( QSlider::TicksBelow );
  x_translation_slider->setTickInterval( 1 );  
  QLabel* x_tran_label = new QLabel( "X", translation_box );

  QHBoxLayout *xhlayout = new QHBoxLayout;
  xhlayout->addWidget( x_tran_label );
  xhlayout->addWidget( x_translation_slider );


  QSlider* y_translation_slider = new QSlider( Qt::Horizontal, translation_box );
  y_translation_slider->setMinimum( -10 );
  y_translation_slider->setMaximum( 10 );
  y_translation_slider->setSliderPosition( (int)(y_tran*10) );
  y_translation_slider->setTickPosition( QSlider::TicksBelow );
  y_translation_slider->setTickInterval( 1 );  
  QLabel* y_tran_label = new QLabel( "Y", translation_box );

  QHBoxLayout *yhlayout = new QHBoxLayout;
  yhlayout->addWidget( y_tran_label );
  yhlayout->addWidget( y_translation_slider );


  QSlider* z_translation_slider = new QSlider( Qt::Horizontal, translation_box );
  QLabel* z_tran_label = new QLabel( "Z", translation_box );
  z_translation_slider->setMinimum( -80 );
  z_translation_slider->setMaximum( 40 );
  z_translation_slider->setSliderPosition( (int)(z_tran*10) );
  z_translation_slider->setTickPosition( QSlider::TicksBelow );
  z_translation_slider->setTickInterval( 1 );  
  QHBoxLayout *zhlayout = new QHBoxLayout;
  zhlayout->addWidget( z_tran_label );
  zhlayout->addWidget( z_translation_slider );

  
  QGridLayout *translationLayout = new QGridLayout;
  translationLayout->addLayout( xhlayout, 0, 0 );
  translationLayout->addLayout( yhlayout, 1, 0 );
  translationLayout->addLayout( zhlayout, 2, 0 );



  // AXIS STRETCH BOX
  QSlider* x_stretch_slider = new QSlider( Qt::Horizontal, stretch_box );
  QLabel* x_stretch_label = new QLabel( "X", stretch_box );
  x_stretch_slider->setMinimum( 0 );
  x_stretch_slider->setMaximum( 80 );
  x_stretch_slider->setSliderPosition( (int)(x_stretch) );
  x_stretch_slider->setTickPosition( QSlider::TicksBelow );
  x_stretch_slider->setTickInterval( 1 );  
  QHBoxLayout *xslayout = new QHBoxLayout;
  xslayout->addWidget( x_stretch_label );
  xslayout->addWidget( x_stretch_slider ); 

  QSlider* y_stretch_slider = new QSlider( Qt::Horizontal, stretch_box );
  QLabel* y_stretch_label = new QLabel( "Y", stretch_box );
  y_stretch_slider->setMinimum( 0 );
  y_stretch_slider->setMaximum( 50 );
  y_stretch_slider->setSliderPosition( (int)(y_stretch*10) );
  y_stretch_slider->setTickPosition( QSlider::TicksBelow );
  y_stretch_slider->setTickInterval( 1 );  
  QHBoxLayout *yslayout = new QHBoxLayout;
  yslayout->addWidget( y_stretch_label );
  yslayout->addWidget( y_stretch_slider ); 

  QGridLayout *stretchLayout = new QGridLayout;
  stretchLayout->addLayout( xslayout, 0, 0 );
  stretchLayout->addLayout( yslayout, 1, 0 );



  // TICK GROUP BOX
  QGridLayout *tickLayout = new QGridLayout;
  QPushButton *tickButton = new QPushButton(tr("Tick"));
  QSpinBox   *numTicksSpinBox = new QSpinBox();

  numTicksSpinBox->setRange(1, 1000);
  numTicksSpinBox->setValue(nTicks);


  tickLayout->addWidget(tickButton, 1, 0);
  tickLayout->addWidget(numTicksSpinBox, 1, 1);
  
  
  // SIGNALS AND SLOTS
  connect(tickButton, SIGNAL(clicked()), this, SLOT(tick()));
  connect(numTicksSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setTicks(int)));
  connect(rotation_slider, SIGNAL(valueChanged(int)), this, SLOT(setYRotation(int)));
  connect( x_translation_slider, SIGNAL(valueChanged(int)), this, SLOT(setXTranslation(int)));
  connect( y_translation_slider, SIGNAL(valueChanged(int)), this, SLOT(setYTranslation(int)));
  connect( z_translation_slider, SIGNAL(valueChanged(int)), this, SLOT(setZTranslation(int)));
  connect( x_stretch_slider, SIGNAL(valueChanged(int)), this, SLOT(setXAxisStretch(int)));
  connect( y_stretch_slider, SIGNAL(valueChanged(int)), this, SLOT(setYAxisStretch(int)));

  // put all the boxes together
  rotation_box->setLayout( rotationLayout );
  translation_box->setLayout( translationLayout );
  stretch_box->setLayout( stretchLayout );
  tick_box->setLayout( tickLayout );

  QVBoxLayout *layout = new QVBoxLayout;
  layout->addWidget(graph); 
  layout->addWidget(rotation_box);
  layout->addWidget(translation_box);
  layout->addWidget(stretch_box);
  layout->addWidget(tick_box);
  setLayout(layout);

}


void 
TopPanelNew::setXAxisStretch(int v)
{
  graph->setXAxisStretch((float)v);
}


void 
TopPanelNew::setYAxisStretch(int v)
{
  graph->setYAxisStretch(((float)v)/10.0);
}


void 
TopPanelNew::setZTranslation(int v)
{
  graph->setZTranslation( ((float)v)/10.0 );
}

	
void 
TopPanelNew::setYTranslation(int v)
{
  graph->setYTranslation( ((float)v)/10.0 );
}


void 
TopPanelNew::setXTranslation(int v)
{
  graph->setXTranslation( ((float)v)/10.0 );
}


void
TopPanelNew::setYRotation(int v)
{
  graph->setYRotation( (float)v );
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
