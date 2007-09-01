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
  pnet->addMarSystem(mng.create("Windowing", "hamming"));
  pnet->addMarSystem(mng.create("Spectrum","spk"));
  pnet->addMarSystem(mng.create("PowerSpectrum", "pspk"));
  pnet->updctrl("PowerSpectrum/pspk/mrs_string/spectrumType","power");  


  pnet->addMarSystem(mng.create("Memory", "mem"));
  //  pnet->addMarSystem(mng.create("AudioSink", "dest"));
  pnet->updctrl( "SoundFileSource/src/mrs_string/filename", au );
  pnet->updctrl( "Memory/mem/mrs_natural/memSize", 12);
  pnet->addMarSystem(mng.create("Gain", "gain"));
  pnet->updctrl("Gain/gain/mrs_real/gain", 2000.0);
  //pnet->addMarSystem(mng.create("PlotSink", "psink"));
  pnet->addMarSystem(mng.create("Gain", "gain1"));
  
  nTicks = 128;
  

  // initialize graphs 
  int num = 256;


  graph = new MarxGLMultiBufferGraph(this, num, 12);
  graph->setMinimumWidth( 500 );



  QGroupBox* model_transforms_box = new QGroupBox( tr("Model Transformations") );
  QGroupBox* projection_transforms_box = new QGroupBox( tr("Projection Transformations") );
  QGroupBox* tick_box = new QGroupBox( tr("Song Increment") );
  
  // MODEL TRANSFORMATION BOX
  QLabel* model_trans_label = new QLabel( "Model Translate" );
  QLabel* model_rot_label = new QLabel( "Model Rotate" ); 
  model_trans_label->setAlignment( Qt::AlignTop );
  model_rot_label->setAlignment( Qt::AlignTop );
  model_trans_label->setSizePolicy( QSizePolicy( QSizePolicy::Maximum,
						 QSizePolicy::Maximum) );
  model_rot_label->setSizePolicy( QSizePolicy( QSizePolicy::Maximum,
						 QSizePolicy::Maximum) );
  Marx3dSlider* m_translate_3d_slider = new Marx3dSlider;
  Marx3dSlider* m_rotation_3d_slider = new Marx3dSlider;

  m_translate_3d_slider->setXinterval( -1.5f, 1.5f );
  m_translate_3d_slider->setYinterval( -1.5f, 1.5f );
  m_translate_3d_slider->setZinterval( -3.f, 3.f );
  m_rotation_3d_slider->setXinterval( 0.f, 360.f );
  m_rotation_3d_slider->setYinterval( 0.f, 360.f );
  m_rotation_3d_slider->setZinterval( 0.f, 360.f );

  m_rotation_3d_slider->setXPos( 0.f  );
  m_rotation_3d_slider->setYPos( 0.f  );
  m_rotation_3d_slider->setZPos( 0.f  );
  m_translate_3d_slider->setXPos( 0.f );
  m_translate_3d_slider->setYPos( 0.f );
  m_translate_3d_slider->setZPos( 0.f );


  QVBoxLayout *model_trans_layout = new QVBoxLayout;
  model_trans_layout->addWidget( m_translate_3d_slider );
  model_trans_layout->addWidget( model_trans_label );

  QVBoxLayout *model_rot_layout = new QVBoxLayout;
  model_rot_layout->addWidget( m_rotation_3d_slider );
  model_rot_layout->addWidget( model_rot_label );
  
  QGridLayout *modelTransfromLayout = new QGridLayout; 
  modelTransfromLayout->addLayout( model_trans_layout, 0, 0 );
  modelTransfromLayout->setColumnMinimumWidth( 1, 10 ); // padding
  modelTransfromLayout->addLayout( model_rot_layout, 0, 2 );


  // PROJECTION TRANSFORMATION BOX
  QLabel* projection_trans_label = new QLabel( "Projection Translate" );
  QLabel* projection_rot_label = new QLabel( "Projection Rotate" ); 
  projection_trans_label->setAlignment( Qt::AlignTop );
  projection_rot_label->setAlignment( Qt::AlignTop );
  projection_trans_label->setSizePolicy( QSizePolicy( QSizePolicy::Maximum,
						 QSizePolicy::Maximum) );
  projection_rot_label->setSizePolicy( QSizePolicy( QSizePolicy::Maximum,
						 QSizePolicy::Maximum) );
  Marx3dSlider* r_translate_3d_slider = new Marx3dSlider;
  Marx3dSlider* r_rotation_3d_slider = new Marx3dSlider;

  r_translate_3d_slider->setXinterval( -1.5f, 1.5f );
  r_translate_3d_slider->setYinterval( -1.5f, 1.5f );
  r_translate_3d_slider->setZinterval( -3.f, 3.f );
  r_rotation_3d_slider->setXinterval( 0.f, 360.f );
  r_rotation_3d_slider->setYinterval( 0.f, 360.f );
  r_rotation_3d_slider->setZinterval( 0.f, 360.f );

  r_translate_3d_slider->setXPos( graph->getXProjectionTranslation() );
  r_translate_3d_slider->setYPos( graph->getYProjectionTranslation() );
  r_translate_3d_slider->setZPos( graph->getYProjectionTranslation() );
  r_rotation_3d_slider->setXPos( graph->getXProjectionRotation() );
  r_rotation_3d_slider->setYPos( graph->getYProjectionRotation() );
  r_rotation_3d_slider->setZPos( graph->getZProjectionRotation() );


  QVBoxLayout *projection_trans_layout = new QVBoxLayout;
  projection_trans_layout->addWidget( r_translate_3d_slider );
  projection_trans_layout->addWidget( projection_trans_label );

  QVBoxLayout *projection_rot_layout = new QVBoxLayout;
  projection_rot_layout->addWidget( r_rotation_3d_slider );
  projection_rot_layout->addWidget( projection_rot_label );
  
  QGridLayout *projectionTransfromLayout = new QGridLayout; 
  projectionTransfromLayout->addLayout( projection_trans_layout, 0, 0 );
  projectionTransfromLayout->setColumnMinimumWidth( 1, 10 ); // padding
  projectionTransfromLayout->addLayout( projection_rot_layout, 0, 2 );

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
  connect(m_translate_3d_slider, SIGNAL(positionChanged(float, float, float)), 
	  this, SLOT(xyzModelTranslation(float, float, float)) );
  connect(m_rotation_3d_slider, SIGNAL(positionChanged(float, float, float)), 
	  this, SLOT(xyzModelRotation(float, float, float))  );
  connect(r_translate_3d_slider, SIGNAL(positionChanged(float, float, float)), 
	  this, SLOT(xyzProjectionTranslation(float, float, float))   );
  connect(r_rotation_3d_slider, SIGNAL(positionChanged(float, float, float)), this, 
	  SLOT(xyzProjectionRotation(float, float, float))  );


  // put all the boxes together
  model_transforms_box->setLayout( modelTransfromLayout );
  projection_transforms_box->setLayout( projectionTransfromLayout );
  tick_box->setLayout( tickLayout );

  // bound their maximum width
  model_transforms_box->setMaximumWidth( 300 );
  projection_transforms_box->setMaximumWidth( 300 );
  tick_box->setMaximumWidth( 300 );
  tick_box->setMaximumHeight( 100 );

  QVBoxLayout *right_layout = new QVBoxLayout;
  right_layout->addWidget( model_transforms_box );
  right_layout->addWidget( projection_transforms_box );
  right_layout->addWidget( tick_box );

  QGridLayout *layout = new QGridLayout;  
  layout->addWidget( graph, 0, 0 );
  layout->addLayout( right_layout, 0, 1 );

  setLayout(layout);

}


void
TopPanelNew::xyzModelTranslation( float x, float y, float z )
{
  graph->modelTranslate( MarxGLMultiBufferGraph::X, x );
  graph->modelTranslate( MarxGLMultiBufferGraph::Y, y );
  graph->modelTranslate( MarxGLMultiBufferGraph::Z, z );
}


void
TopPanelNew::xyzModelRotation( float x, float y, float z )
{
  graph->modelRotate( MarxGLMultiBufferGraph::X, x );
  graph->modelRotate( MarxGLMultiBufferGraph::Y, y );
  graph->modelRotate( MarxGLMultiBufferGraph::Z, z );
}


void
TopPanelNew::xyzProjectionTranslation( float x, float y, float z )
{
  graph->projectionTranslate( MarxGLMultiBufferGraph::X, x );
  graph->projectionTranslate( MarxGLMultiBufferGraph::Y, y );
  graph->projectionTranslate( MarxGLMultiBufferGraph::Z, z );
}


void
TopPanelNew::xyzProjectionRotation( float x, float y, float z )
{
  graph->projectionRotate( MarxGLMultiBufferGraph::X, x );
  graph->projectionRotate( MarxGLMultiBufferGraph::Y, y );
  graph->projectionRotate( MarxGLMultiBufferGraph::Z, z );
}


void 
TopPanelNew::setTicks(int v)
{
  nTicks = v;
}


void 
TopPanelNew::tick()
{
  
  for (int i=0; i < nTicks; i++) 
    {
      pnet->tick();
      pnet->updctrl("mrs_bool/probe", true);

//       realvec out(512);
//       out = pnet->getctrl("mrs_realvec/input0")->to<mrs_realvec>();

      realvec out(256, 12);
//       cout << out.getRows() << "\t";
//       cout << out.getCols() << "\n";
      out = pnet->getctrl("mrs_realvec/input5")->to<mrs_realvec>();

//       out.setval(0.0);
//       for (int g=0; g < 12; g++)
//         for (int t=0; t < 256; t++)
// 	  out(t, g) = 0.7;
      //cout << out << endl;
//       cout << out.getRows() << "\t";
//       cout << out.getCols() << "\n";

      graph->setBuffer( out );
      
    }
  
}
