
#include <QtGui>

#include "MarLpcWindow.h"

int dopt = 64;
int iopt = 64;
int sopt = 80;
int bopt = 128;
int vopt_ = 1;
mrs_real gopt_ = 1.0;
mrs_natural eopt_ = 0;

mrs_real popt = 1.0;

MarLpcWindow::MarLpcWindow()
{
	frequencyPole_ = 0;
	amplitudePole_ = .85;

	QWidget *w = new QWidget;
	setCentralWidget(w);

	createActions();
	createMenus();  

	QLabel  *breathinessLabel  = new QLabel("breathiness");
	breathinessSlider_ = new QSlider(Qt::Horizontal);

	QLabel  *cutOffLabel  = new QLabel("cutOff");
	QSlider *cutOffSlider = new QSlider(Qt::Horizontal);

	QLabel  *frequencyPoleLabel1  = new QLabel("frequencyPole");
	QLabel  *frequencyPoleLabel2  = new QLabel("frequencyPole");
	frequencyPoleSlider_ = new QSlider(Qt::Horizontal);

	QLabel  *amplitudePoleLabel1  = new QLabel("amplitudePole");
	QLabel  *amplitudePoleLabel2  = new QLabel("amplitudePole");
	amplitudePoleSlider_ = new QSlider(Qt::Horizontal);

	QLabel  *tiltLabel  = new QLabel("Tilt");
	tiltSlider_ = new QSlider(Qt::Horizontal);

	QLabel *posLabel = new QLabel("Pos");
	posSlider_ = new QSlider(Qt::Horizontal);

	breathinessLabel->setMinimumWidth(150);
	cutOffLabel->setMinimumWidth(150);

	frequencyPoleSlider_->setValue(50);
	amplitudePoleSlider_->setValue(50);
	tiltSlider_->setValue(50);

	createNetwork();

	QGridLayout *gridLayout = new QGridLayout;

	gridLayout->addWidget(breathinessLabel, 0, 0);
	gridLayout->addWidget(breathinessSlider_, 1, 0);

	gridLayout->addWidget(tiltLabel, 0, 1);
	gridLayout->addWidget(tiltSlider_, 1, 1);

	gridLayout->addWidget(frequencyPoleLabel1, 2, 0);
	gridLayout->addWidget(frequencyPoleSlider_, 3, 0);

	gridLayout->addWidget(amplitudePoleLabel1, 2, 1);
	gridLayout->addWidget(amplitudePoleSlider_, 3, 1);

	gridLayout->addWidget(posLabel, 5, 0);
	gridLayout->addWidget(posSlider_, 6, 0);
	gridLayout->addWidget(posControl_, 6, 1);

	gridLayout->addWidget(frequencyPoleControl_, 4, 0);
	gridLayout->addWidget(amplitudePoleControl_, 4, 1);

	connect(breathinessSlider_, SIGNAL(valueChanged(int)), this, SLOT(breathinessChanged(int)));
	connect(cutOffSlider, SIGNAL(valueChanged(int)), this, SLOT(cutOffChanged(int)));
	connect(frequencyPoleSlider_, SIGNAL(valueChanged(int)), this, SLOT(frequencyPoleChanged(int)));
	connect(amplitudePoleSlider_, SIGNAL(valueChanged(int)), this, SLOT(amplitudePoleChanged(int)));
	connect(tiltSlider_, SIGNAL(valueChanged(int)), this, SLOT(tiltChanged(int)));
	connect(posSlider_, SIGNAL(sliderReleased()), this, SLOT(posChanged()));

	connect(mwr_, SIGNAL(ctrlChanged(MarControlPtr)), this, SLOT(ctrlChanged(MarControlPtr)));

	w->setLayout(gridLayout);

	startNetwork();
}


void 
MarLpcWindow::posChanged() 
{
	int value = posSlider_->sliderPosition();
	mwr_->updctrl(posPtr_, (mrs_natural) value * 512 * 40);    
}

void 
MarLpcWindow::amplitudePoleChanged(int value)
{
	amplitudePole_ = .5+.5*value/100.0;

	updateResonanceFilter();
}

void 
MarLpcWindow::frequencyPoleChanged(int value)
{
	frequencyPole_ =  value/100.0*.4*PI;
	updateResonanceFilter();
}

void 
MarLpcWindow::tiltChanged(int value)
{
	realvec dcoeffs(2);
	dcoeffs(0)= 1;
	dcoeffs(1)= -.8-.2*value/100.0;

	mwr_->updctrl("FanOutIn/fanoutin/Series/aSeries/Filter/tilt/mrs_realvec/dcoeffs",  dcoeffs);
}

void 
MarLpcWindow::breathinessChanged(int value)
{
	mrs_real volume = value / 100.0;

	mwr_->updctrl("FanOutIn/fanoutin/Series/nSeries/Gain/noiseLevel/mrs_real/gain",  volume);
	mwr_->updctrl("FanOutIn/fanoutin/Series/aSeries/Gain/residualLevel/mrs_real/gain", (mrs_real) (1-volume));
}

void 
MarLpcWindow::cutOffChanged(int value)
{

}

void 
MarLpcWindow::ctrlChanged(MarControlPtr cname)
{
	string name = cname->getName();
	if (cname.isEqual(posPtr_))
	{
		mrs_real fval = cname->to<mrs_natural>()  / (512.0 * 40.0);
		if (posSlider_->isSliderDown() == false)
		{
			posSlider_->blockSignals(true);
			posSlider_->setValue((int)fval);
			posSlider_->blockSignals(false);
		}
		posControl_->updControl(cname); 
	}

	/*if (cname.isEqual(frequencyPolePtr_)) 
	{
	frequencyPoleSlider_->blockSignals(true);
	mrs_real fval = cname->to<mrs_real>();
	int val = (int)(fval * 50.0);
	frequencyPoleSlider_->setValue(val);
	frequencyPoleSlider_->blockSignals(false);
	frequencyPoleControl_->updControl(cname);
	}

	if (cname.isEqual(amplitudePolePtr_)) 
	{
	amplitudePoleSlider_->blockSignals(true);
	mrs_real fval = cname->to<mrs_real>();
	int val = (int)(fval * 50.0);
	amplitudePoleSlider_->setValue(val);
	amplitudePoleSlider_->blockSignals(false);
	amplitudePoleControl_->updControl(cname);
	}*/
}

void 
MarLpcWindow::createNetwork()
{
	//mrs_natural N = 256;
	mrs_natural Nw = 512;
	mrs_natural I = iopt;
	mrs_real P = popt;
	mrs_natural D = 256;
	mrs_natural emphasisOrder = 3;
	mrs_natural formantOrder = 20;
	mrs_real emphasisBe = .9;
	mrs_real formantBe = .975;

	MarSystemManager mng;

	// create the overall network
	lpc_ = mng.create("Series", "lpcSeries");
	lpc_->addMarSystem(mng.create("SoundFileSource", "src"));
	lpc_->addMarSystem(mng.create("ShiftInput", "si"));

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
	lpc_->addMarSystem(fanoutin);

	// filtering modified residual with formant filter
	lpc_->addMarSystem(mng.create("Filter", "synthesis"));
	lpc_->addMarSystem(mng.create("Windowing", "winSyn")); 
	lpc_->addMarSystem(mng.create("OverlapAdd", "ova"));
	lpc_->addMarSystem(mng.create("CompExp", "norm"));

	MarSystem* dest = mng.create("AudioSink", "dest");
	lpc_->addMarSystem(dest);

	// setting up flow parameters
	lpc_->updctrl("mrs_natural/inSamples", D);
	lpc_->updctrl("mrs_natural/inObservations", 1);
	lpc_->updctrl("SoundFileSource/src/mrs_real/repetitions", -1.0);

	lpc_->updctrl("ShiftInput/si/mrs_natural/WindowSize", Nw);

	// setting up emphasis analysis parameters
	lpc_->updctrl("FanOutIn/fanoutin/Series/aSeries/FlowThru/emphasisFlowthru/LPC/emphasisLpc/mrs_natural/order", emphasisOrder);
	lpc_->updctrl("FanOutIn/fanoutin/Series/aSeries/FlowThru/emphasisFlowthru/LPC/emphasisLpc/mrs_real/lambda",0.0);
	lpc_->updctrl("FanOutIn/fanoutin/Series/aSeries/FlowThru/emphasisFlowthru/LPC/emphasisLpc/mrs_real/gamma",emphasisBe);

	lpc_->linkctrl("FanOutIn/fanoutin/Series/aSeries/Filter/emphasisAnalysis/mrs_realvec/ncoeffs",
		"FanOutIn/fanoutin/Series/aSeries/FlowThru/emphasisFlowthru/LPC/emphasisLpc/mrs_realvec/coeffs");

	// setting up emphasis analysis parameters
	lpc_->updctrl("FanOutIn/fanoutin/Series/aSeries/FlowThru/formantFlowthru/LPC/formantLpc/mrs_natural/order", formantOrder);
	lpc_->updctrl("FanOutIn/fanoutin/Series/aSeries/FlowThru/formantFlowthru/LPC/formantLpc/mrs_real/lambda",0.0);
	lpc_->updctrl("FanOutIn/fanoutin/Series/aSeries/FlowThru/formantFlowthru/LPC/formantLpc/mrs_real/gamma",formantBe);

	lpc_->linkctrl("FanOutIn/fanoutin/Series/aSeries/Filter/formantAnalysis/mrs_realvec/ncoeffs",
		"FanOutIn/fanoutin/Series/aSeries/FlowThru/formantFlowthru/LPC/formantLpc/mrs_realvec/coeffs");

	// setting up synthesis parameters
	lpc_->updctrl("Windowing/winSyn/mrs_string/type", "Hanning");

	//lpc_->linkctrl("FanOutIn/fanoutin/Series/aSeries/Filter/tilt/mrs_realvec/dcoeffs",
	//	"FanOutIn/fanoutin/Series/aSeries/FlowThru/emphasisFlowthru/LPC/emphasisLpc/mrs_realvec/coeffs");

	lpc_->linkctrl("Filter/synthesis/mrs_realvec/dcoeffs",
		"FanOutIn/fanoutin/Series/aSeries/FlowThru/formantFlowthru/LPC/formantLpc/mrs_realvec/coeffs");

	// link the LPCs gain estimations with gains
	lpc_->linkctrl("FanOutIn/fanoutin/Series/nSeries/FanOutIn/nFanOutIn/Gain/nEmphasis/mrs_real/gain",
		"FanOutIn/fanoutin/Series/aSeries/FlowThru/formantFlowthru/LPC/formantLpc/mrs_real/power");
	lpc_->linkctrl("FanOutIn/fanoutin/Series/nSeries/FanOutIn/nFanOutIn/Gain/nFormant/mrs_real/gain",
		"FanOutIn/fanoutin/Series/aSeries/FlowThru/formantFlowthru/LPC/formantLpc/mrs_real/power");


	// creating shortcuts for osc manipulation
	lpc_->linkctrl("mrs_natural/formantOrder",
		"FanOutIn/fanoutin/Series/aSeries/FlowThru/formantFlowthru/LPC/formantLpc/mrs_natural/order");

	mwr_ = new MarSystemQtWrapper(lpc_, true);

	posPtr_ = mwr_->getctrl("SoundFileSource/src/mrs_natural/pos");
	posControl_ = new MarControlGUI(posPtr_, mwr_, this);

	initPtr_ = mwr_->getctrl("AudioSink/dest/mrs_bool/initAudio");
	fnamePtr_ = mwr_->getctrl("SoundFileSource/src/mrs_string/filename");

	frequencyPolePtr_ = mwr_->getctrl("FanOutIn/fanoutin/Series/aSeries/Filter/tilt/mrs_realvec/dcoeffs");
	amplitudePolePtr_ = mwr_->getctrl("FanOutIn/fanoutin/Series/aSeries/Filter/resonance/mrs_realvec/dcoeffs");
	frequencyPoleControl_ = new MarControlGUI(frequencyPolePtr_, mwr_, this);
	amplitudePoleControl_ = new MarControlGUI(amplitudePolePtr_, mwr_, this);
}

void 
MarLpcWindow::startNetwork()
{
	mwr_->tickForever();
	mwr_->pause();
}

void
MarLpcWindow::createMenus()
{
	fileMenu = menuBar()->addMenu(tr("&File"));
	fileMenu->addAction(openAct);
	menuBar()->addSeparator();
	helpMenu = menuBar()->addMenu(tr("&Help"));
	helpMenu->addAction(aboutAct);
}

void 
MarLpcWindow::createActions()
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
MarLpcWindow::open()
{
	QString fileName = QFileDialog::getOpenFileName(this);

	play(fileName.toStdString());
}

void 
MarLpcWindow::play(string fileName)
{
	mwr_->trackctrl(frequencyPolePtr_); 
	mwr_->trackctrl(amplitudePolePtr_); 
	mwr_->trackctrl(posPtr_);

	mwr_->updctrl(fnamePtr_, fileName);
	mwr_->updctrl(initPtr_, true);

	mwr_->play();
}

void 
MarLpcWindow::about()
{
	QMessageBox::about(this, tr("Marsyas MarPhasevocoder"),  
		tr("Marsyas MarLpc: A graphical user interface for real-time manipulation \n using the Marsyas implementation of the Linear Prediction analysis/synthesis chain. \n"));
}

void MarLpcWindow::updateResonanceFilter()
{
	realvec dcoeffs(3);
	dcoeffs(0)= 1;
	dcoeffs(1)= -2*amplitudePole_*cos(frequencyPole_);
	dcoeffs(2)= amplitudePole_*amplitudePole_;

	mwr_->updctrl("FanOutIn/fanoutin/Series/aSeries/Filter/resonance/mrs_realvec/dcoeffs",  dcoeffs);
}
