#include <QLCDNumber>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "glwidget.h"
#include "window.h"

Window::Window()
{
  mainWidget = new QWidget;

  setCentralWidget(mainWidget);

  glWidget = new GLWidget;

  glWidget->setMinimumSize(800,400);
  glWidget->setMaximumSize(800,400);

  createActions();
  createMenus();

  // Create the x,y,z rotation sliders
  xRotSlider = createRotationSlider(-180,180);
  yRotSlider = createRotationSlider(-255,255);
//   zRotSlider = createRotationSlider();

//   // Create the x,y,z translation sliders
  xTransSlider = createTranslationSlider();
  yTransSlider = createTranslationSlider();
//   zTransSlider = createTranslationSlider();

  // Fog sliders
//   fogStartSlider = createTranslationSlider();
//   fogEndSlider = createTranslationSlider();

//   fogStartSlider = createSlider(-100,-30,1,10,10);
//   fogEndSlider = createSlider(-100,-30,1,10,10);

  fogStartSlider = createSlider(-130,-80,1,10,10);
  fogEndSlider = createSlider(-130,-80,1,10,10);

  // Data display sliders
  magnitudeCutoffSlider = createSlider(0,100,1,10,10);
  numVerticesSlider = createSlider(3,20,1,10,10);
  dotSizeSlider = createSlider(1,100,1,10,10);

  // Song position
  posSlider = createSlider(0,100,1,10,10);

//   // How fast the display moves
//   displaySpeedSlider = createSlider(0,100,1,10,10);

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

  // A combo box for choosing the fft bin size
  fftBinsCombo = new QComboBox;
  fftBinsCombo->addItem(tr("32"));
  fftBinsCombo->addItem(tr("64"));
  fftBinsCombo->addItem(tr("128"));
  fftBinsCombo->addItem(tr("256"));
  fftBinsCombo->addItem(tr("512"));
  fftBinsCombo->addItem(tr("1024"));
  fftBinsCombo->addItem(tr("2048"));
  fftBinsCombo->addItem(tr("4096"));
  fftBinsCombo->addItem(tr("8192"));
  fftBinsCombo->addItem(tr("16384"));
  fftBinsCombo->addItem(tr("32768"));

  // A combo box for choosing the background color
  backgroundCombo = new QComboBox;
  backgroundCombo->addItem(tr("Black"));
  backgroundCombo->addItem(tr("Black"));
  backgroundCombo->addItem(tr("Dark Blue"));
  backgroundCombo->addItem(tr("Light Blue"));

//     waterfallCheckBox = new QCheckBox(tr("&Waterfall"));
//     waterfallCheckBox->setChecked(true);

  // 	// Connect a click signal on the go button to a slot to start the rotation time
  connect(playpause_button, SIGNAL(clicked()), glWidget, SLOT(playPause()));

  // Connect up the x,y,z rotation sliders with slots to set the rotation values
  connect(xRotSlider, SIGNAL(valueChanged(int)), glWidget, SLOT(setXRotation(int)));
  connect(glWidget, SIGNAL(xRotationChanged(int)), xRotSlider, SLOT(setValue(int)));
  connect(yRotSlider, SIGNAL(valueChanged(int)), glWidget, SLOT(setYRotation(int)));
  connect(glWidget, SIGNAL(yRotationChanged(int)), yRotSlider, SLOT(setValue(int)));
//   connect(zRotSlider, SIGNAL(valueChanged(int)), glWidget, SLOT(setZRotation(int)));
//   connect(glWidget, SIGNAL(zRotationChanged(int)), zRotSlider, SLOT(setValue(int)));

//   // Connect up the x,y,z rotation sliders with slots to set the rotation values
  connect(xTransSlider, SIGNAL(valueChanged(int)), glWidget, SLOT(setXTranslation(int)));
  connect(yTransSlider, SIGNAL(valueChanged(int)), glWidget, SLOT(setYTranslation(int)));
//   connect(zTransSlider, SIGNAL(valueChanged(int)), glWidget, SLOT(setZTranslation(int)));

  // Connect up the other sliders
  connect(fogStartSlider, SIGNAL(valueChanged(int)), glWidget, SLOT(setFogStart(int)));
  connect(fogEndSlider, SIGNAL(valueChanged(int)), glWidget, SLOT(setFogEnd(int)));

  connect(magnitudeCutoffSlider, SIGNAL(valueChanged(int)), glWidget, SLOT(setMagnitudeCutoff(int)));
  connect(numVerticesSlider, SIGNAL(valueChanged(int)), glWidget, SLOT(setNumVertices(int)));
  connect(dotSizeSlider, SIGNAL(valueChanged(int)), glWidget, SLOT(setDotSize(int)));
//   connect(displaySpeedSlider, SIGNAL(valueChanged(int)), glWidget, SLOT(setDisplaySpeed(int)));

  // Connect up the FFT bins slider
  connect(fftBinsCombo, SIGNAL(currentIndexChanged(int)), glWidget, SLOT(setFFTBins(int)));

  // Connect up the background color combo
  // NOT IMPLEMENTED.

  // Current playback position
//   connect(posSlider, SIGNAL(valueChanged(int)), glWidget, SLOT(setSongPosition(int)));
  connect(posSlider, SIGNAL(sliderReleased()), this, SLOT(seekPos()));
  connect(glWidget, SIGNAL(posChanged(int)), this, SLOT(positionSlider(int)));


//   // Scale sliders
//   connect(yScaleSlider, SIGNAL(valueChanged(int)), glWidget, SLOT(setYScale(int)));

//   connect(powerSpectrumModeCombo, SIGNAL(currentIndexChanged(int)), glWidget, SLOT(powerSpectrumModeChanged(int)));

//   connect(waterfallCheckBox, SIGNAL(toggled(bool)), glWidget, SLOT(setWaterfallVisible(bool)));

  // A main layout to hold everything
  QHBoxLayout *layout = new QHBoxLayout;

  // The OpenGL window and the sliders to move it interactively
  QVBoxLayout *gl_layout = new QVBoxLayout;
  gl_layout->addWidget(glWidget);


//   QHBoxLayout *z_rot_slider_layout = new QHBoxLayout;
//   QLabel *z_rot_slider_label = new QLabel(("Z Rot"));
//   z_rot_slider_layout->addWidget(z_rot_slider_label);
//   z_rot_slider_layout->addWidget(zRotSlider);
//   gl_layout->addLayout(z_rot_slider_layout);



//   QHBoxLayout *z_trans_slider_layout = new QHBoxLayout;
//   QLabel *z_trans_slider_label = new QLabel(("Z Trans"));
//   z_trans_slider_layout->addWidget(z_trans_slider_label);
//   z_trans_slider_layout->addWidget(zTransSlider);
//   gl_layout->addLayout(z_trans_slider_layout);


  layout->addLayout(gl_layout);

  // All the controls on the right side of the window
  QVBoxLayout *controls_layout = new QVBoxLayout;
  layout->addLayout(controls_layout);

  QHBoxLayout *song_position_slider_layout = new QHBoxLayout;
  QLabel *song_position_label = new QLabel(("Song Position"));
  song_position_slider_layout->addWidget(song_position_label);
  song_position_slider_layout->addWidget(posSlider);
  controls_layout->addLayout(song_position_slider_layout);

  QHBoxLayout *x_rot_slider_layout = new QHBoxLayout;
  QLabel *x_rot_slider_label = new QLabel(("X Rot"));
  x_rot_slider_layout->addWidget(x_rot_slider_label);
  x_rot_slider_layout->addWidget(xRotSlider);
  controls_layout->addLayout(x_rot_slider_layout);

  QHBoxLayout *y_rot_slider_layout = new QHBoxLayout;
  QLabel *y_rot_slider_label = new QLabel(("Y Rot"));
  y_rot_slider_layout->addWidget(y_rot_slider_label);
  y_rot_slider_layout->addWidget(yRotSlider);
  controls_layout->addLayout(y_rot_slider_layout);

  QHBoxLayout *x_trans_slider_layout = new QHBoxLayout;
  QLabel *x_trans_slider_label = new QLabel(("X Trans"));
  x_trans_slider_layout->addWidget(x_trans_slider_label);
  x_trans_slider_layout->addWidget(xTransSlider);
  controls_layout->addLayout(x_trans_slider_layout);

  QHBoxLayout *y_trans_slider_layout = new QHBoxLayout;
  QLabel *y_trans_slider_label = new QLabel(("Y Trans"));
  y_trans_slider_layout->addWidget(y_trans_slider_label);
  y_trans_slider_layout->addWidget(yTransSlider);
  controls_layout->addLayout(y_trans_slider_layout);

  QHBoxLayout *fog_start_slider_layout = new QHBoxLayout;
  QLabel *fog_start_slider_label = new QLabel(("Fog Start"));
  fog_start_slider_layout->addWidget(fog_start_slider_label);
  fog_start_slider_layout->addWidget(fogStartSlider);
  controls_layout->addLayout(fog_start_slider_layout);

  QHBoxLayout *fog_end_slider_layout = new QHBoxLayout;
  QLabel *fog_end_slider_label = new QLabel(("Fog End"));
  fog_end_slider_layout->addWidget(fog_end_slider_label);
  fog_end_slider_layout->addWidget(fogEndSlider);
  controls_layout->addLayout(fog_end_slider_layout);

//   // The scaling sliders
//   QVBoxLayout *scale_layout = new QVBoxLayout;
//   QLabel *scale_label = new QLabel(("Scale"));
//   scale_layout->addWidget(scale_label);
//   scale_layout->addWidget(yScaleSlider);
//   controls_layout->addLayout(scale_layout);

  // Controls for the animation
  QHBoxLayout *fft_bins_layout = new QHBoxLayout;
  fftBinsLabel = new QLabel(tr("Number of FFT Bins:"));
//   fftBinsLabel->setBuddy(fftBinsCombo);
  fft_bins_layout->addWidget(fftBinsLabel);
  fft_bins_layout->addWidget(fftBinsCombo);
  controls_layout->addLayout(fft_bins_layout);

  QHBoxLayout *magnitude_cutoff_slider_layout = new QHBoxLayout;
  QLabel *magnitude_cutoff_label = new QLabel(("Magnitude Cutoff"));
  magnitude_cutoff_slider_layout->addWidget(magnitude_cutoff_label);
  magnitude_cutoff_slider_layout->addWidget(magnitudeCutoffSlider);
  controls_layout->addLayout(magnitude_cutoff_slider_layout);

  QHBoxLayout *num_vertices_slider_layout = new QHBoxLayout;
  QLabel *num_vertices_label = new QLabel(("Dot quality"));
  num_vertices_slider_layout->addWidget(num_vertices_label);
  num_vertices_slider_layout->addWidget(numVerticesSlider);
  controls_layout->addLayout(num_vertices_slider_layout);

  QHBoxLayout *dot_size_slider_layout = new QHBoxLayout;
  QLabel *dot_size_label = new QLabel(("Dot Size"));
  dot_size_slider_layout->addWidget(dot_size_label);
  dot_size_slider_layout->addWidget(dotSizeSlider);
  controls_layout->addLayout(dot_size_slider_layout);

//   QHBoxLayout *display_speed_slider_layout = new QHBoxLayout;
//   QLabel *display_speed_label = new QLabel(("Display Speed"));
//   display_speed_slider_layout->addWidget(display_speed_label);
//   display_speed_slider_layout->addWidget(displaySpeedSlider);
//   controls_layout->addLayout(display_speed_slider_layout);

  QHBoxLayout *playpause_button_layout = new QHBoxLayout;
  playpause_button_layout->addWidget(playpause_button);
  controls_layout->addLayout(playpause_button_layout);

  // Set the layout for this widget to the layout we just created
  mainWidget->setLayout(layout);

  // Set some nice defaults for all the sliders
  xRotSlider->setValue(100);
  yRotSlider->setValue(0);
//   zRotSlider->setValue(0);

  xTransSlider->setValue(0);
  yTransSlider->setValue(97);
//   zTransSlider->setValue(61);

//    fogStartSlider->setValue(-24);
//    fogEndSlider->setValue(-49);

  fogStartSlider->setValue(-94);
  fogEndSlider->setValue(-117);

  posSlider->setValue(0);

  magnitudeCutoffSlider->setValue(50);
  numVerticesSlider->setValue(10);
  dotSizeSlider->setValue(32);
//    displaySpeedSlider->setValue(50);

  fftBinsCombo->setCurrentIndex(4);

//   yScaleSlider->setValue(350);

  setWindowTitle(tr("MarPanning"));
}

void Window::play(const QString & fileName)
{
  glWidget->play(fileName);
}

//
// A handy function to create a generic slider for the x,y,z positions
//
QSlider *Window::createRotationSlider(int min, int max)
{
  QSlider *slider = new QSlider(Qt::Horizontal);
  slider->setRange(min,max);
  slider->setSingleStep(1);
  slider->setPageStep(10);
  slider->setTickInterval(10);
  slider->setTickPosition(QSlider::TicksRight);
  return slider;
}

#if 0
MarSystemQtWrapper* Window::getMarSystemQtWrapper()
{
  return glWidget->getMarSystemQtWrapper();
}
#endif


QSlider *Window::createTranslationSlider()
{
  QSlider *slider = new QSlider(Qt::Horizontal);
  slider->setRange(-150, 150);
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
  return QSize(826, 544);
}

// The maximum size of the widget
QSize Window::sizeHint() const
{
  return QSize(826, 544);
}

void
Window::positionSlider(int val)
{
  if (posSlider->isSliderDown() == false)
    posSlider->setValue(val);
}

void Window::seekPos() {
  glWidget->setPos(posSlider->sliderPosition());
}
