#include "window.h"
#include "glwidget.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QAction>
#include <QMenu>
#include <QMenuBar>

Window::Window(const QString & inAudioFileName)
{

  mainWidget = new QWidget();

  setCentralWidget(mainWidget);

  glWidget = new GLWidget(inAudioFileName);

  glWidget->setMinimumSize(500,500);
  glWidget->setMaximumSize(500,500);

  createActions();
  createMenus();

  // Create the x,y,z sliders
  xSlider = createRotationSlider();
  ySlider = createRotationSlider();
  zSlider = createRotationSlider();

  // The y-scale slider
  yScaleSlider = createSlider(0,1000,10,100,50);

  // A play/pause button
  playpause_button  = new QPushButton(tr("Play/Pause"));

  // A combo box for choosing the power spectrum calculation
  powerSpectrumModeCombo = new QComboBox;
  powerSpectrumModeCombo->addItem(tr("Power"));
  powerSpectrumModeCombo->addItem(tr("Magnitude"));
  powerSpectrumModeCombo->addItem(tr("Decibels"));
  powerSpectrumModeCombo->addItem(tr("Power Density"));

  powerSpectrumModeLabel = new QLabel(tr("&PowerSpectrum mode:"));
  powerSpectrumModeLabel->setBuddy(powerSpectrumModeCombo);

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

  // Connect up the x,y,z sliders with slots to set the rotation values
  connect(xSlider, SIGNAL(valueChanged(int)), glWidget, SLOT(setXRotation(int)));
  connect(glWidget, SIGNAL(xRotationChanged(int)), xSlider, SLOT(setValue(int)));
  connect(ySlider, SIGNAL(valueChanged(int)), glWidget, SLOT(setYRotation(int)));
  connect(glWidget, SIGNAL(yRotationChanged(int)), ySlider, SLOT(setValue(int)));
  connect(zSlider, SIGNAL(valueChanged(int)), glWidget, SLOT(setZRotation(int)));
  connect(glWidget, SIGNAL(zRotationChanged(int)), zSlider, SLOT(setValue(int)));

  // Scale sliders
  connect(yScaleSlider, SIGNAL(valueChanged(int)), glWidget, SLOT(setYScale(int)));

  connect(powerSpectrumModeCombo, SIGNAL(currentIndexChanged(int)), glWidget, SLOT(powerSpectrumModeChanged(int)));

//   connect(waterfallCheckBox, SIGNAL(toggled(bool)), glWidget, SLOT(setWaterfallVisible(bool)));

  // A main layout to hold everything
  QHBoxLayout *layout = new QHBoxLayout;

  // The OpenGL window and the sliders to move it interactively
  QVBoxLayout *gl_layout = new QVBoxLayout;
  gl_layout->addWidget(glWidget);

  QHBoxLayout *x_slider_layout = new QHBoxLayout;
  QLabel *x_slider_label = new QLabel(("X"));
  x_slider_layout->addWidget(x_slider_label);
  x_slider_layout->addWidget(xSlider);
  gl_layout->addLayout(x_slider_layout);

  QHBoxLayout *y_slider_layout = new QHBoxLayout;
  QLabel *y_slider_label = new QLabel(("Y"));
  y_slider_layout->addWidget(y_slider_label);
  y_slider_layout->addWidget(ySlider);
  gl_layout->addLayout(y_slider_layout);

  QHBoxLayout *z_slider_layout = new QHBoxLayout;
  QLabel *z_slider_label = new QLabel(("Z"));
  z_slider_layout->addWidget(z_slider_label);
  z_slider_layout->addWidget(zSlider);
  gl_layout->addLayout(z_slider_layout);

  layout->addLayout(gl_layout);

  // All the controls on the right side of the window
  QVBoxLayout *controls_layout = new QVBoxLayout;
  layout->addLayout(controls_layout);

  // The scaling sliders
  QVBoxLayout *scale_layout = new QVBoxLayout;
  QLabel *scale_label = new QLabel(("Scale"));
  scale_layout->addWidget(scale_label);
  scale_layout->addWidget(yScaleSlider);
  controls_layout->addLayout(scale_layout);

  // Controls for the animation
  QVBoxLayout *buttons_layout = new QVBoxLayout;
  buttons_layout->addWidget(powerSpectrumModeLabel);
  buttons_layout->addWidget(powerSpectrumModeCombo);
  buttons_layout->addWidget(playpause_button);
  controls_layout->addLayout(buttons_layout);

  // Set the layout for this widget to the layout we just created
  mainWidget->setLayout(layout);

  // Set some nice defaults for all the sliders
  xSlider->setValue(0 * 16);
  ySlider->setValue(0 * 16);
  zSlider->setValue(0 * 16);

  yScaleSlider->setValue(350);

  setWindowTitle(tr("MarSndPeek"));
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
  QMessageBox::about(this, tr("Marsyas MarSndPeek"),
                     tr("Marsyas MarSndPeek : A graphical user interface for real-time visualization \n of audio data, including waterfall spectrograms, waveform, and statistics. \n \n Inspired by sndpeek :  \n http://soundlab.cs.princeton.edu/software/sndpeek/ \n \n \n written by sness (c) 2009 GPL - sness@sness.net"));

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
