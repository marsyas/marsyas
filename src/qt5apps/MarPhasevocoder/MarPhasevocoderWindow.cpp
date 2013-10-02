
#include "MarPhasevocoderWindow.h"

#include <marsyas/system/MarSystemManager.h>

#include <QLabel>
#include <QGridLayout>
#include <QTimer>
#include <QMenuBar>
#include <QFileDialog>
#include <QMessageBox>

int dopt = 64;
int iopt = 64;
int sopt = 80;
int bopt = 128;
int vopt_ = 1;
mrs_real gopt_ = 1.0;
mrs_natural eopt_ = 0;

mrs_real popt = 1.0;

MarPhasevocoderWindow::MarPhasevocoderWindow(const QString & inAudioFileName):
  m_song_length(0)
{
  QWidget *w = new QWidget;
  setCentralWidget(w);

  createActions();
  createMenus();

  QSlider *sinusoidsSlider = new QSlider(Qt::Horizontal);
  QSlider *volumeSlider = new QSlider(Qt::Horizontal);
  QSlider *freqSlider = new QSlider(Qt::Horizontal);
  m_timeSlider = new QSlider(Qt::Horizontal);
  m_posSlider = new QSlider(Qt::Horizontal);
  m_posSlider->setTracking(false);

  volumeSlider->setValue(100);
  freqSlider->setValue(50);
  m_timeSlider->setValue(50);
  sinusoidsSlider->setValue(100);

  m_posDisplay = new QTimeEdit();
  m_posDisplay->setMinimumTime(QTime(0,0));
  m_posDisplay->setMaximumTime(QTime(5,0));
  m_posDisplay->setReadOnly(true);
  m_posDisplay->setDisplayFormat("mm:ss:zzz");
  m_posDisplay->setButtonSymbols(QTimeEdit::NoButtons);

  createNetwork();

  QGridLayout *gridLayout = new QGridLayout;
  gridLayout->setColumnMinimumWidth(0, 150);
  gridLayout->setColumnMinimumWidth(1, 150);

  gridLayout->addWidget(new QLabel("Number of Sinusoids"), 0, 0);
  gridLayout->addWidget(sinusoidsSlider, 1, 0);

  gridLayout->addWidget(new QLabel("Volume"), 0, 1);
  gridLayout->addWidget(volumeSlider, 1, 1);

  gridLayout->addWidget(new QLabel("Frequency"), 2, 0);
  gridLayout->addWidget(freqSlider, 3, 0);

  gridLayout->addWidget(new QLabel("Time"), 2, 1);
  gridLayout->addWidget(m_timeSlider, 3, 1);

  gridLayout->addWidget(new QLabel("Position"), 4, 0);
  gridLayout->addWidget(m_posSlider, 5, 0);

  gridLayout->addWidget(m_posDisplay, 5, 1);

  w->setLayout(gridLayout);

  connect(volumeSlider, SIGNAL(valueChanged(int)), this, SLOT(volumeChanged(int)));
  connect(freqSlider, SIGNAL(valueChanged(int)), this, SLOT(freqChanged(int)));
  connect(sinusoidsSlider, SIGNAL(valueChanged(int)), this, SLOT(sinusoidsChanged(int)));
  connect(m_posSlider, SIGNAL(actionTriggered(int)), this, SLOT(posChanged()));

  connect(&m_updateTimer, SIGNAL(timeout()), this, SLOT(updateControls()));

  play(inAudioFileName);
}

void
MarPhasevocoderWindow::open()
{
  QString fileName = QFileDialog::getOpenFileName(this);
  play(fileName);
}


void MarPhasevocoderWindow::play( const QString & fileName )
{
  if (fileName.isEmpty())
    return;

  m_updateTimer.stop();
  m_system->stop();

  static const bool NO_UPDATE = false;
  m_filenameControl->setValue(fileName, NO_UPDATE);
  m_initAudioControl->setValue(true, NO_UPDATE);
  m_system->update();

  m_song_length = m_sizeControl->value().toInt();
  m_song_sr = m_sampleRateControl->value().toDouble();

  applyTimeStretch(m_timeSlider->value());

  m_system->start();
  m_updateTimer.start(150);
}

void
MarPhasevocoderWindow::applyTimeStretch(int value)
{
  // Changing these controls changes block size and is
  // therefore time-consuming and not realtime safe!
  // So rather do it at m_updateTimer tick than whenever
  // the related slider is moved.
  float nval = iopt * ((50.0 + (100.0 - value)) / 100.0);
  m_interpolationControl1->setValue((int)nval, false);
  m_interpolationControl2->setValue((int)nval, false);
  m_system->update();
}

void
MarPhasevocoderWindow::posChanged()
{
  int position = m_posSlider->sliderPosition() / 100.0 * m_song_length;
  m_posControl->setValue(position);
}

void
MarPhasevocoderWindow::sinusoidsChanged(int value)
{
  m_sinesControl->setValue(value);
}

void
MarPhasevocoderWindow::freqChanged(int value)
{
  float pitchShift = value * 1.0 / 50.0;
  m_freqControl->setValue(pitchShift);
}

void
MarPhasevocoderWindow::volumeChanged(int value)
{
  m_gainControl->setValue( value / 100.0 );
}

void
MarPhasevocoderWindow::updateControls()
{
  applyTimeStretch(m_timeSlider->value());

  int pos = m_posControl->value().toInt();
  int pos_percent = (m_song_length > 0) ? (pos * 100.0 / m_song_length) : 0;
  double pos_ms = (m_song_sr > 0) ? (pos / m_song_sr) * 1000.0 : 0.0;
  QTime time = QTime(0,0).addMSecs(pos_ms);

  if (!m_posSlider->isSliderDown())
    m_posSlider->setValue(pos_percent);
  m_posDisplay->setTime(time);
}

void
MarPhasevocoderWindow::createNetwork()
{
  mrs_natural N = 512;
  mrs_natural Nw = 512;
  mrs_natural I = iopt;
  mrs_real P = popt;
  mrs_natural D = dopt;

  MarSystemManager mng;

  // create the phasevocoder network
  MarSystem *sys;
  sys = mng.create("Series", "pvoc_");
  sys->addMarSystem(mng.create("SoundFileSource", "src"));
  sys->addMarSystem(mng.create("ShiftInput", "si"));
  sys->addMarSystem(mng.create("PvFold", "fo"));
  sys->addMarSystem(mng.create("Spectrum", "spk"));
  sys->addMarSystem(mng.create("PvConvert", "conv"));
  sys->addMarSystem(mng.create("PvOscBank", "ob"));
  sys->addMarSystem(mng.create("ShiftOutput", "so"));
  sys->addMarSystem(mng.create("Gain", "gain"));
  MarSystem* dest = mng.create("AudioSink", "dest");
  sys->addMarSystem(dest);

  sys->updControl("mrs_natural/inSamples", D);
  sys->updControl("mrs_natural/inObservations", 1);
  sys->updControl("SoundFileSource/src/mrs_real/repetitions", -1.0);
  sys->updControl("ShiftInput/si/mrs_natural/winSize", Nw);
  sys->updControl("PvFold/fo/mrs_natural/FFTSize", N);
  sys->updControl("PvConvert/conv/mrs_natural/Decimation",D);
  sys->updControl("PvConvert/conv/mrs_natural/Sinusoids", (mrs_natural) sopt);
  sys->updControl("PvOscBank/ob/mrs_natural/Interpolation", I);
  sys->updControl("PvOscBank/ob/mrs_real/PitchShift", P);
  sys->updControl("ShiftOutput/so/mrs_natural/Interpolation", I);
  sys->updControl("Gain/gain/mrs_real/gain", gopt_);

  m_marsystem = sys;
  m_system = new MarsyasQt::System(sys);

  m_filenameControl = m_system->control("SoundFileSource/src/mrs_string/filename");
  m_gainControl = m_system->control("Gain/gain/mrs_real/gain");
  m_posControl = m_system->control("SoundFileSource/src/mrs_natural/pos");
  m_sizeControl = m_system->control("SoundFileSource/src/mrs_natural/size");
  m_sampleRateControl = m_system->control("mrs_real/osrate");
  m_freqControl = m_system->control("PvOscBank/ob/mrs_real/PitchShift");
  m_interpolationControl1 = m_system->control("PvOscBank/ob/mrs_natural/Interpolation");
  m_interpolationControl2 = m_system->control("ShiftOutput/so/mrs_natural/Interpolation");
  m_sinesControl = m_system->control("PvConvert/conv/mrs_natural/Sinusoids");
  m_initAudioControl = m_system->control("AudioSink/dest/mrs_bool/initAudio");
}


void
MarPhasevocoderWindow::createMenus()
{
  fileMenu = menuBar()->addMenu(tr("&File"));
  fileMenu->addAction(openAct);
  menuBar()->addSeparator();
  helpMenu = menuBar()->addMenu(tr("&Help"));
  helpMenu->addAction(aboutAct);
}

void
MarPhasevocoderWindow::createActions()
{
  openAct = new QAction(tr("&Open..."), this);
  openAct->setShortcut(tr("Ctrl+O"));
  openAct->setStatusTip(tr("Open an existing file"));
  connect(openAct, SIGNAL(triggered()), this, SLOT(open()));
  aboutAct = new QAction(tr("&About"), this);
  aboutAct->setStatusTip(tr("Show the application's About box"));
  connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));
}

void
MarPhasevocoderWindow::about()
{
  QMessageBox::about(this, tr("Marsyas MarPhasevocoder"),
                     tr("Marsyas MarPhasevocoder: A graphical user interface for real-time manipulation \n using the Marsyas implementation of the phasevocoder. \n (based on the phasevocoder described in \n the Elements of Computer Music by F.R.Moore)"));
}

