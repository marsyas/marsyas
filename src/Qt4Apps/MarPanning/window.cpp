#include <QtGui>
#include <QLCDNumber>

#include "glwidget.h"
#include "window.h"

Window::Window(string inAudioFileName)
{

  mainWidget = new QWidget();

  setCentralWidget(mainWidget);

  glWidget = new GLWidget(inAudioFileName);

  glWidget->setMinimumSize(700,700);
  glWidget->setMaximumSize(700,700);

  createActions();
  createMenus();  

  // Create the x,y,z rotation sliders
  xRotSlider = createRotationSlider();
  yRotSlider = createRotationSlider();
  zRotSlider = createRotationSlider();

  // Create the x,y,z translation sliders
  xTransSlider = createTranslationSlider();
  yTransSlider = createTranslationSlider();
  zTransSlider = createTranslationSlider();

  // Fog sliders
  fogStartSlider = createTranslationSlider();
  fogEndSlider = createTranslationSlider();

//   // The y-scale slider
//   yScaleSlider = createSlider(0,1000,10,100,50);

  // A play/pause button
  playpause_button  = new QPushButton(tr("Play/Pause"));

//   // A combo box for choosing the power spectrum calculation
//   powerSpectrumModeCombo = new QComboBox;
//   powerSpectrumModeCombo->addItem(tr("Power"));
//   powerSpectrumModeCombo->addItem(tr("Magnitude"));
//   powerSpectrumModeCombo->addItem(tr("Decibels"));
//   powerSpectrumModeCombo->addItem(tr("Power Density"));

//   powerSpectrumModeLabel = new QLabel(tr("&PowerSpectrum mode:"));
//   powerSpectrumModeLabel->setBuddy(powerSpectrumModeCombo);

  //   // A combo box for choosing the fft bin size
  //   fftBinsModeCombo = new QComboBox;
  //   fftBinsModeCombo->addItem(tr("32"));
  //   fftBinsModeCombo->addItem(tr("64"));
  //   fftBinsModeCombo->addItem(tr("128"));
  //   fftBinsModeCombo->addItem(tr("256"));
  //   fftBinsModeCombo->addItem(tr("1024"));
  //   fftBinsModeCombo->addItem(tr("2048"));
  //   fftBinsModeCombo->addItem(tr("4096"));
  //   fftBinsModeCombo->addItem(tr("8192"));
  //   fftBinsModeCombo->addItem(tr("16384"));
  //   fftBinsModeCombo->addItem(tr("32768"));

  //   fftBinsModeLabel = new QLabel(tr("&Number of FFT Bins:"));
  //   fftBinsModeLabel->setBuddy(fftBinsModeCombo);

//     waterfallCheckBox = new QCheckBox(tr("&Waterfall"));
//     waterfallCheckBox->setChecked(true);

  // 	// Connect a click signal on the go button to a slot to start the rotation time
  connect(playpause_button, SIGNAL(clicked()), glWidget, SLOT(playPause()));

  // Connect up the x,y,z rotation sliders with slots to set the rotation values
  connect(xRotSlider, SIGNAL(valueChanged(int)), glWidget, SLOT(setXRotation(int)));
  connect(glWidget, SIGNAL(xRotationChanged(int)), xRotSlider, SLOT(setValue(int)));
  connect(yRotSlider, SIGNAL(valueChanged(int)), glWidget, SLOT(setYRotation(int)));
  connect(glWidget, SIGNAL(yRotationChanged(int)), yRotSlider, SLOT(setValue(int)));
  connect(zRotSlider, SIGNAL(valueChanged(int)), glWidget, SLOT(setZRotation(int)));
  connect(glWidget, SIGNAL(zRotationChanged(int)), zRotSlider, SLOT(setValue(int)));

  // Connect up the x,y,z rotation sliders with slots to set the rotation values
  connect(xTransSlider, SIGNAL(valueChanged(int)), glWidget, SLOT(setXTranslation(int)));
//   connect(glWidget, SIGNAL(xTranslationChanged(int)), xTransSlider, SLOT(setValue(int)));
  connect(yTransSlider, SIGNAL(valueChanged(int)), glWidget, SLOT(setYTranslation(int)));
//   connect(glWidget, SIGNAL(yTranslationChanged(int)), yTransSlider, SLOT(setValue(int)));
  connect(zTransSlider, SIGNAL(valueChanged(int)), glWidget, SLOT(setZTranslation(int)));
//   connect(glWidget, SIGNAL(zTranslationChanged(int)), zTransSlider, SLOT(setValue(int)));

  connect(fogStartSlider, SIGNAL(valueChanged(int)), glWidget, SLOT(setFogStart(int)));
  connect(fogEndSlider, SIGNAL(valueChanged(int)), glWidget, SLOT(setFogEnd(int)));


//   // Scale sliders
//   connect(yScaleSlider, SIGNAL(valueChanged(int)), glWidget, SLOT(setYScale(int)));

//   connect(powerSpectrumModeCombo, SIGNAL(currentIndexChanged(int)), glWidget, SLOT(powerSpectrumModeChanged(int)));

//   connect(waterfallCheckBox, SIGNAL(toggled(bool)), glWidget, SLOT(setWaterfallVisible(bool)));

  // A main layout to hold everything
  QHBoxLayout *layout = new QHBoxLayout;

  // The OpenGL window and the sliders to move it interactively
  QVBoxLayout *gl_layout = new QVBoxLayout;
  gl_layout->addWidget(glWidget);

  QHBoxLayout *x_rot_slider_layout = new QHBoxLayout;
  QLabel *x_rot_slider_label = new QLabel(("X Rot"));
  x_rot_slider_layout->addWidget(x_rot_slider_label);
  x_rot_slider_layout->addWidget(xRotSlider);
  gl_layout->addLayout(x_rot_slider_layout);
	
  QHBoxLayout *y_rot_slider_layout = new QHBoxLayout;
  QLabel *y_rot_slider_label = new QLabel(("Y Rot"));
  y_rot_slider_layout->addWidget(y_rot_slider_label);
  y_rot_slider_layout->addWidget(yRotSlider);
  gl_layout->addLayout(y_rot_slider_layout);

  QHBoxLayout *z_rot_slider_layout = new QHBoxLayout;
  QLabel *z_rot_slider_label = new QLabel(("Z Rot"));
  z_rot_slider_layout->addWidget(z_rot_slider_label);
  z_rot_slider_layout->addWidget(zRotSlider);
  gl_layout->addLayout(z_rot_slider_layout);

  QHBoxLayout *x_trans_slider_layout = new QHBoxLayout;
  QLabel *x_trans_slider_label = new QLabel(("X Trans"));
  x_trans_slider_layout->addWidget(x_trans_slider_label);
  x_trans_slider_layout->addWidget(xTransSlider);
  gl_layout->addLayout(x_trans_slider_layout);
	
  QHBoxLayout *y_trans_slider_layout = new QHBoxLayout;
  QLabel *y_trans_slider_label = new QLabel(("Y Trans"));
  y_trans_slider_layout->addWidget(y_trans_slider_label);
  y_trans_slider_layout->addWidget(yTransSlider);
  gl_layout->addLayout(y_trans_slider_layout);

  QHBoxLayout *z_trans_slider_layout = new QHBoxLayout;
  QLabel *z_trans_slider_label = new QLabel(("Z Trans"));
  z_trans_slider_layout->addWidget(z_trans_slider_label);
  z_trans_slider_layout->addWidget(zTransSlider);
  gl_layout->addLayout(z_trans_slider_layout);

  QHBoxLayout *fog_start_slider_layout = new QHBoxLayout;
  QLabel *fog_start_slider_label = new QLabel(("Fog Start"));
  fog_start_slider_layout->addWidget(fog_start_slider_label);
  fog_start_slider_layout->addWidget(fogStartSlider);
  gl_layout->addLayout(fog_start_slider_layout);

  QHBoxLayout *fog_end_slider_layout = new QHBoxLayout;
  QLabel *fog_end_slider_label = new QLabel(("Fog End"));
  fog_end_slider_layout->addWidget(fog_end_slider_label);
  fog_end_slider_layout->addWidget(fogEndSlider);
  gl_layout->addLayout(fog_end_slider_layout);


  layout->addLayout(gl_layout);

  // All the controls on the right side of the window
  QVBoxLayout *controls_layout = new QVBoxLayout;
  layout->addLayout(controls_layout);

//   // The scaling sliders
//   QVBoxLayout *scale_layout = new QVBoxLayout;
//   QLabel *scale_label = new QLabel(("Scale"));
//   scale_layout->addWidget(scale_label);
//   scale_layout->addWidget(yScaleSlider);
//   controls_layout->addLayout(scale_layout);

  // Controls for the animation
  QVBoxLayout *buttons_layout = new QVBoxLayout;
//   buttons_layout->addWidget(powerSpectrumModeLabel);
//   buttons_layout->addWidget(powerSpectrumModeCombo);
  buttons_layout->addWidget(playpause_button);
  controls_layout->addLayout(buttons_layout);

  // Set the layout for this widget to the layout we just created
  mainWidget->setLayout(layout);

  // Set some nice defaults for all the sliders
  xRotSlider->setValue(0);
  yRotSlider->setValue(0 * 16);
  zRotSlider->setValue(0 * 16);

  xTransSlider->setValue(0);
  yTransSlider->setValue(24);
  zTransSlider->setValue(61);

   fogStartSlider->setValue(-24);
   fogEndSlider->setValue(-49);

//   yScaleSlider->setValue(350);
	
  setWindowTitle(tr("MarPanning"));
}

//
// A handy function to create a generic slider for the x,y,z positions
//
QSlider *Window::createRotationSlider()
{
  QSlider *slider = new QSlider(Qt::Horizontal);
  slider->setRange(0, 360 * 16);
  slider->setSingleStep(16);
  slider->setPageStep(15 * 16);
  slider->setTickInterval(15 * 16);
  slider->setTickPosition(QSlider::TicksRight);
  return slider;
}

QSlider *Window::createTranslationSlider()
{
  QSlider *slider = new QSlider(Qt::Horizontal);
  slider->setRange(-100, 100);
  slider->setSingleStep(16);
  slider->setPageStep(15 * 16);
  slider->setTickInterval(15 * 16);
  slider->setTickPosition(QSlider::TicksRight);
  return slider;
}

QSlider *Window::createSlider(int range_start, int range_end, int single_step, int page_step, int tick_interval)
{
  QSlider *slider = new QSlider(Qt::Horizontal);
  slider->setRange(range_start, range_end);
  slider->setSingleStep(single_step);
  slider->setPageStep(page_step);
  slider->setTickInterval(tick_interval);
  slider->setTickPosition(QSlider::TicksRight);
  return slider;
}

void Window::createMenus()
{
  fileMenu = menuBar()->addMenu(tr("&File"));
  fileMenu->addAction(openAct);
  menuBar()->addSeparator();
  helpMenu = menuBar()->addMenu(tr("&Help"));
  helpMenu->addAction(aboutAct);
}
 
void Window::createActions()
{
  openAct = new QAction(tr("&Open..."), this);
  openAct->setShortcut(tr("Ctrl+O"));
  openAct->setStatusTip(tr("Open an existing file"));
  connect(openAct, SIGNAL(triggered()), glWidget, SLOT(open()));
  aboutAct = new QAction(tr("&About"), this);
  aboutAct->setStatusTip(tr("Show the application's About box"));
  connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));
}

void 
Window::about()
{
  QMessageBox::about(this, tr("Marsyas MarPanning"),  
					 tr("Marsyas MarPanning : Displays left/right panning of an audio file \n graphically in OpenGL. \n \n \n written by sness (c) 2009 GPL - sness@sness.net"));

}

// The minimum size of the widget
QSize Window::minimumSizeHint() const
{
  return QSize(600, 600);
}

// The maximum size of the widget
QSize Window::sizeHint() const
{
  return QSize(800, 800);
}
