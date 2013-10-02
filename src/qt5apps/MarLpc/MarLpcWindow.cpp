#include "MarLpcWindow.h"

#include <marsyas/system/MarSystemManager.h>

#include <QLabel>
#include <QGridLayout>
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QFileDialog>
#include <QMessageBox>

#include <cmath>

using namespace Marsyas;
using namespace MarsyasQt;

int dopt = 64;
int iopt = 64;
int sopt = 80;
int bopt = 128;
int vopt_ = 1;
mrs_real gopt_ = 1.0;
mrs_natural eopt_ = 0;

mrs_real popt = 1.0;

MarLpcWindow::MarLpcWindow():
  m_song_len(0),
  m_song_sr(0)
{
  createNetwork();

  QWidget *w = new QWidget;
  setCentralWidget(w);

  createActions();
  createMenus();

  m_breathinessSlider = new QSlider(Qt::Horizontal);

  //QSlider *cutOffSlider = new QSlider(Qt::Horizontal);

  m_frequencyPoleSlider = new QSlider(Qt::Horizontal);

  m_amplitudePoleSlider = new QSlider(Qt::Horizontal);

  m_tiltSlider = new QSlider(Qt::Horizontal);

  m_posSlider = new QSlider(Qt::Horizontal);
  m_posSlider->setTracking(false);

  m_posDisplay = new QTimeEdit;
  m_posDisplay->setMinimumTime(QTime(0,0));
  m_posDisplay->setMaximumTime(QTime(5,0));
  m_posDisplay->setReadOnly(true);
  m_posDisplay->setDisplayFormat("mm:ss:zzz");
  m_posDisplay->setButtonSymbols(QTimeEdit::NoButtons);

  m_tiltFilterView = new RealvecView("Tilt Filter");
  m_resonanceFilterView = new RealvecView("Resonance Filter");

  m_breathinessSlider->setValue(50);
  m_frequencyPoleSlider->setValue(50);
  m_amplitudePoleSlider->setValue(50);
  m_tiltSlider->setValue(50);

  QGridLayout *gridLayout = new QGridLayout;

  gridLayout->addWidget( new QLabel("breathiness"), 0, 0);
  gridLayout->addWidget(m_breathinessSlider, 1, 0);

  gridLayout->addWidget(new QLabel("Tilt"), 0, 1);
  gridLayout->addWidget(m_tiltSlider, 1, 1);

  gridLayout->addWidget(new QLabel("frequencyPole"), 2, 0);
  gridLayout->addWidget(m_frequencyPoleSlider, 3, 0);

  gridLayout->addWidget(new QLabel("amplitudePole"), 2, 1);
  gridLayout->addWidget(m_amplitudePoleSlider, 3, 1);

  gridLayout->addWidget(new QLabel("Pos"), 5, 0);
  gridLayout->addWidget(m_posSlider, 6, 0);
  gridLayout->addWidget(m_posDisplay, 6, 1);

  gridLayout->addWidget(m_tiltFilterView, 4, 0);
  gridLayout->addWidget(m_resonanceFilterView, 4, 1);

  connect(m_breathinessSlider, SIGNAL(valueChanged(int)), this, SLOT(breathinessChanged(int)));
  //connect(cutOffSlider, SIGNAL(valueChanged(int)), this, SLOT(cutOffChanged(int)));
  connect(m_frequencyPoleSlider, SIGNAL(valueChanged(int)), this, SLOT(updateResonanceFilter()));
  connect(m_amplitudePoleSlider, SIGNAL(valueChanged(int)), this, SLOT(updateResonanceFilter()));
  connect(m_tiltSlider, SIGNAL(valueChanged(int)), this, SLOT(updateTiltFilter()));
  connect(m_posSlider, SIGNAL(actionTriggered(int)), this, SLOT(posChanged()));
  connect(&m_updateTimer, SIGNAL(timeout()), this, SLOT(updateControls()));

  w->setLayout(gridLayout);

  updateResonanceFilter();
  updateTiltFilter();
  breathinessChanged(m_breathinessSlider->value());
}

void MarLpcWindow::play( const QString & fileName )
{
  if (fileName.isEmpty())
    return;

  m_updateTimer.stop();
  m_system->stop();

  static const bool NO_UPDATE = false;
  m_fileNameControl->setValue(fileName, NO_UPDATE);
  m_initAudioControl->setValue(true, NO_UPDATE);
  m_system->update();

  m_song_len = m_sizeControl->value().toInt();
  m_song_sr = m_sampleRateControl->value().toDouble();

  m_system->start();
  m_updateTimer.start(200);
}

void MarLpcWindow::open()
{
  QString fileName = QFileDialog::getOpenFileName(this);
  play(fileName);
}

void MarLpcWindow::posChanged()
{
  if (m_song_len <= 0)
    return;

  int position = m_posSlider->sliderPosition() / 100.0 * m_song_len;
  m_posControl->setValue(position);
}

void MarLpcWindow::updateResonanceFilter()
{
  double amp_pole = .5 + .5 * m_amplitudePoleSlider->value() / 100.0;
  double freq_pole = m_frequencyPoleSlider->value() / 100.0 * .4 * PI;

  realvec dcoeffs(3);
  dcoeffs(0)= 1;
  dcoeffs(1)= -2 * amp_pole * std::cos(freq_pole);
  dcoeffs(2)= amp_pole * amp_pole;

  m_resonanceFilterControl->setValue(QVariant::fromValue(dcoeffs));
  m_resonanceFilterView->setData(dcoeffs);
}

void MarLpcWindow::updateTiltFilter()
{
  realvec dcoeffs(2);
  dcoeffs(0)= 1;
  dcoeffs(1)= -.8 - .2 * m_tiltSlider->value() / 100.0;

  m_tiltFilterControl->setValue(QVariant::fromValue(dcoeffs));
  m_tiltFilterView->setData(dcoeffs);
}

void MarLpcWindow::breathinessChanged(int value)
{
  mrs_real volume = value / 100.0;

  m_noiseLevelControl->setValue(volume);
  m_residualLevelControl->setValue((mrs_real)(1.0-volume));
}

void MarLpcWindow::cutOffChanged(int value)
{

}

void MarLpcWindow::updateControls()
{
  int pos = m_posControl->value().toInt();
  int pos_percent = (m_song_len > 0) ? (pos * 100.0 / m_song_len) : 0;
  double pos_ms = (m_song_sr > 0) ? ((double) pos / m_song_sr) * 1000.0 : 0.0;
  QTime time = QTime(0,0).addMSecs(pos_ms);

  if (!m_posSlider->isSliderDown())
    m_posSlider->setValue(pos_percent);
  m_posDisplay->setTime(time);
}

void MarLpcWindow::createNetwork()
{
  //mrs_natural N = 256;
  mrs_natural Nw = 512;
  //mrs_natural I = iopt;
  //mrs_real P = popt;
  mrs_natural D = 256;
  mrs_natural emphasisOrder = 3;
  mrs_natural formantOrder = 20;
  mrs_real emphasisBe = .9;
  mrs_real formantBe = .975;

  MarSystemManager mng;

  // create the overall network
  m_lpc = mng.create("Series", "lpcSeries");
  m_lpc->addMarSystem(mng.create("SoundFileSource", "src"));
  m_lpc->addMarSystem(mng.create("ShiftInput", "si"));

  MarSystem *fanoutin = mng.create("FanOutIn", "fanoutin");
  // analysis branch (generates the residual)
  MarSystem* aSeries = mng.create("Series", "aSeries");
  // first stage of Lpc for emphasis analysis
  MarSystem* emphasisFlowthru = mng.create("FlowThru", "emphasisFlowthru");
  emphasisFlowthru->addMarSystem(mng.create("Windowing", "emphasisWindow"));
  emphasisFlowthru->addMarSystem(mng.create("LPC", "emphasisLpc"));
  aSeries->addMarSystem(emphasisFlowthru);
  aSeries->addMarSystem(mng.create("Filter", "emphasisAnalysis"));
  // second stage of Lpc for formant analysis
  MarSystem* formantFlowthru = mng.create("FlowThru", "formantFlowthru");
  formantFlowthru->addMarSystem(mng.create("Windowing", "hamAna"));
  formantFlowthru->addMarSystem(mng.create("LPC", "formantLpc"));
  aSeries->addMarSystem(formantFlowthru);
  aSeries->addMarSystem(mng.create("Filter", "formantAnalysis"));
  // adjust residual level (Qt)
  aSeries->addMarSystem(mng.create("Gain", "residualLevel"));
  // apply syntheitic emphasis filters (Qt)
  aSeries->addMarSystem(mng.create("Filter", "tilt"));
  aSeries->addMarSystem(mng.create("Filter", "resonance"));
  fanoutin->addMarSystem(aSeries);
  // noise branch (generates noise)
  MarSystem* nSeries = mng.create("Series", "nSeries");
  nSeries->addMarSystem(mng.create("NoiseSource", "ns"));
  // adjust noise level from LPCs analysis (Qt)
  MarSystem* nFanOutIn = mng.create("FanOutIn", "nFanOutIn");
  nFanOutIn->addMarSystem(mng.create("Gain", "nEmphasis"));
  nFanOutIn->addMarSystem(mng.create("Gain", "nFormant"));
  nSeries->addMarSystem(nFanOutIn);
  // adjust noise level (Qt)
  nSeries->addMarSystem(mng.create("Gain", "noiseLevel"));
  fanoutin->addMarSystem(nSeries);
  m_lpc->addMarSystem(fanoutin);

  // filtering modified residual with formant filter
  m_lpc->addMarSystem(mng.create("Filter", "synthesis"));
  m_lpc->addMarSystem(mng.create("Windowing", "winSyn"));
  m_lpc->addMarSystem(mng.create("OverlapAdd", "ova"));
  m_lpc->addMarSystem(mng.create("CompExp", "norm"));

  MarSystem* dest = mng.create("AudioSink", "dest");
  m_lpc->addMarSystem(dest);

  // setting up flow parameters
  m_lpc->updControl("mrs_natural/inSamples", D);
  m_lpc->updControl("mrs_natural/inObservations", 1);
  m_lpc->updControl("SoundFileSource/src/mrs_real/repetitions", -1.0);

  m_lpc->updControl("ShiftInput/si/mrs_natural/winSize", Nw);

  // setting up emphasis analysis parameters
  m_lpc->updControl("FanOutIn/fanoutin/Series/aSeries/FlowThru/emphasisFlowthru/LPC/emphasisLpc/mrs_natural/order", emphasisOrder);
  m_lpc->updControl("FanOutIn/fanoutin/Series/aSeries/FlowThru/emphasisFlowthru/LPC/emphasisLpc/mrs_real/lambda",0.0);
  m_lpc->updControl("FanOutIn/fanoutin/Series/aSeries/FlowThru/emphasisFlowthru/LPC/emphasisLpc/mrs_real/gamma",emphasisBe);

  m_lpc->linkControl("FanOutIn/fanoutin/Series/aSeries/Filter/emphasisAnalysis/mrs_realvec/ncoeffs",
                     "FanOutIn/fanoutin/Series/aSeries/FlowThru/emphasisFlowthru/LPC/emphasisLpc/mrs_realvec/coeffs");

  // setting up emphasis analysis parameters
  m_lpc->updControl("FanOutIn/fanoutin/Series/aSeries/FlowThru/formantFlowthru/LPC/formantLpc/mrs_natural/order", formantOrder);
  m_lpc->updControl("FanOutIn/fanoutin/Series/aSeries/FlowThru/formantFlowthru/LPC/formantLpc/mrs_real/lambda",0.0);
  m_lpc->updControl("FanOutIn/fanoutin/Series/aSeries/FlowThru/formantFlowthru/LPC/formantLpc/mrs_real/gamma",formantBe);

  m_lpc->linkControl("FanOutIn/fanoutin/Series/aSeries/Filter/formantAnalysis/mrs_realvec/ncoeffs",
                     "FanOutIn/fanoutin/Series/aSeries/FlowThru/formantFlowthru/LPC/formantLpc/mrs_realvec/coeffs");

  // setting up synthesis parameters
  m_lpc->updControl("Windowing/winSyn/mrs_string/type", "Hanning");

  //lpc_->linkControl("FanOutIn/fanoutin/Series/aSeries/Filter/tilt/mrs_realvec/dcoeffs",
  //	"FanOutIn/fanoutin/Series/aSeries/FlowThru/emphasisFlowthru/LPC/emphasisLpc/mrs_realvec/coeffs");

  m_lpc->linkControl("Filter/synthesis/mrs_realvec/dcoeffs",
                     "FanOutIn/fanoutin/Series/aSeries/FlowThru/formantFlowthru/LPC/formantLpc/mrs_realvec/coeffs");

  // link the LPCs gain estimations with gains
  m_lpc->linkControl("FanOutIn/fanoutin/Series/nSeries/FanOutIn/nFanOutIn/Gain/nEmphasis/mrs_real/gain",
                     "FanOutIn/fanoutin/Series/aSeries/FlowThru/formantFlowthru/LPC/formantLpc/mrs_real/power");
  m_lpc->linkControl("FanOutIn/fanoutin/Series/nSeries/FanOutIn/nFanOutIn/Gain/nFormant/mrs_real/gain",
                     "FanOutIn/fanoutin/Series/aSeries/FlowThru/formantFlowthru/LPC/formantLpc/mrs_real/power");

  // set up compressor
  m_lpc->updControl("CompExp/norm/mrs_real/thresh", 0.8);
  m_lpc->updControl("CompExp/norm/mrs_real/slope", 1/60.0);

  // creating shortcuts for osc manipulation
  m_lpc->linkControl("mrs_natural/formantOrder",
                     "FanOutIn/fanoutin/Series/aSeries/FlowThru/formantFlowthru/LPC/formantLpc/mrs_natural/order");

  m_system = new MarsyasQt::System(m_lpc);
  m_fileNameControl = m_system->control("SoundFileSource/src/mrs_string/filename");
  m_sampleRateControl = m_system->control("mrs_real/osrate");
  m_sizeControl = m_system->control("SoundFileSource/src/mrs_natural/size");
  m_posControl = m_system->control("SoundFileSource/src/mrs_natural/pos");
  m_initAudioControl = m_system->control("AudioSink/dest/mrs_bool/initAudio");
  m_tiltFilterControl = m_system->control("FanOutIn/fanoutin/Series/aSeries/Filter/tilt/mrs_realvec/dcoeffs");
  m_resonanceFilterControl = m_system->control("FanOutIn/fanoutin/Series/aSeries/Filter/resonance/mrs_realvec/dcoeffs");
  m_noiseLevelControl = m_system->control("FanOutIn/fanoutin/Series/nSeries/Gain/noiseLevel/mrs_real/gain");
  m_residualLevelControl = m_system->control("FanOutIn/fanoutin/Series/aSeries/Gain/residualLevel/mrs_real/gain");
}

void MarLpcWindow::createMenus()
{
  fileMenu = menuBar()->addMenu(tr("&File"));
  fileMenu->addAction(openAct);
  menuBar()->addSeparator();
  helpMenu = menuBar()->addMenu(tr("&Help"));
  helpMenu->addAction(aboutAct);
}

void MarLpcWindow::createActions()
{
  openAct = new QAction(tr("&Open..."), this);
  openAct->setShortcut(tr("Ctrl+O"));
  openAct->setStatusTip(tr("Open an existing file"));
  connect(openAct, SIGNAL(triggered()), this, SLOT(open()));
  aboutAct = new QAction(tr("&About"), this);
  aboutAct->setStatusTip(tr("Show the application's About box"));
  connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));
}

void MarLpcWindow::about()
{
  QMessageBox::about(this, tr("Marsyas MarPhasevocoder"),
                     tr("Marsyas MarLpc: A graphical user interface for real-time manipulation \n using the Marsyas implementation of the Linear Prediction analysis/synthesis chain. \n"));
}
