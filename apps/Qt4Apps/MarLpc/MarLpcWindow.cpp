
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
	QWidget *w = new QWidget;
	setCentralWidget(w);

	createActions();
	createMenus();  

	QLabel  *breathinessLabel  = new QLabel("breathiness");
	QSlider *breathinessSlider = new QSlider(Qt::Horizontal);

	QLabel  *cutOffLabel  = new QLabel("cutOff");
	QSlider *cutOffSlider = new QSlider(Qt::Horizontal);

	QLabel  *frequencyPoleLabel1  = new QLabel("frequencyPole");
	QLabel  *frequencyPoleLabel2  = new QLabel("frequencyPole");
	QSlider *frequencyPoleSlider_ = new QSlider(Qt::Horizontal);

	QLabel  *amplitudePoleLabel1  = new QLabel("amplitudePole");
	QLabel  *amplitudePoleLabel2  = new QLabel("amplitudePole");
	QSlider *amplitudePoleSlider_ = new QSlider(Qt::Horizontal);

	QLabel  *tiltLabel  = new QLabel("Tilt");
	QSlider *tiltSlider = new QSlider(Qt::Horizontal);

	QLabel *posLabel = new QLabel("Pos");
	posSlider_ = new QSlider(Qt::Horizontal);

	breathinessLabel->setMinimumWidth(150);
	cutOffLabel->setMinimumWidth(150);

	frequencyPoleSlider_->setValue(50);
	amplitudePoleSlider_->setValue(50);
	tiltSlider->setValue(10);

	createNetwork();

	QGridLayout *gridLayout = new QGridLayout;

	gridLayout->addWidget(breathinessLabel, 0, 0);
	gridLayout->addWidget(breathinessSlider, 1, 0);

	gridLayout->addWidget(cutOffLabel, 0, 1);
	gridLayout->addWidget(cutOffSlider, 1, 1);

	gridLayout->addWidget(frequencyPoleLabel1, 2, 0);
	gridLayout->addWidget(frequencyPoleSlider_, 3, 0);

	gridLayout->addWidget(amplitudePoleLabel1, 2, 1);
	gridLayout->addWidget(amplitudePoleSlider_, 3, 1);

	//gridLayout->addWidget(frequencyPoleLabel2, 4, 0);
	//gridLayout->addWidget(frequencyPoleSlider_, 5, 0);

	gridLayout->addWidget(posLabel, 4, 1);
	gridLayout->addWidget(posSlider_, 5, 1);
    gridLayout->addWidget(posControl_, 6, 0);

	//gridLayout->addWidget(frequencyPoleControl_, 6, 0);
    //gridLayout->addWidget(amplitudePoleControl_, 7, 0);

	connect(breathinessSlider, SIGNAL(valueChanged(int)), this, SLOT(breathinessChanged(int)));
	connect(cutOffSlider, SIGNAL(valueChanged(int)), this, SLOT(cutOffChanged(int)));
	connect(frequencyPoleSlider_, SIGNAL(valueChanged(int)), this, SLOT(frequencyPoleChanged(int)));
	connect(amplitudePoleSlider_, SIGNAL(valueChanged(int)), this, SLOT(amplitudePoleChanged(int)));
	connect(tiltSlider, SIGNAL(valueChanged(int)), this, SLOT(tiltChanged(int)));
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
	float nval = iopt * ((50.0 + (100.0 - value)) / 100.0);

}

void 
MarLpcWindow::frequencyPoleChanged(int value)
{

}

void 
MarLpcWindow::tiltChanged(int value)
{
	 
}

void 
MarLpcWindow::breathinessChanged(int value)
{
mrs_real volume = value / 100.0;

	mwr_->updctrl("Fanin/fanin/Series/nSeries/Gain/noiseLevel/mrs_real/gain",  volume);
	mwr_->updctrl("Fanin/fanin/Series/aSeries/Gain/residualLevel/mrs_real/gain", (mrs_real) (1-volume));

	cout << volume << " " << mwr_->getctrl("Fanin/fanin/Series/aSeries/Gain/residualLevel/mrs_real/gain")->toReal() << endl;

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

	/*if (name == "mrs_real/PitchShift") 
	{
		freqSlider_->blockSignals(true);
		mrs_real fval = cname->to<mrs_real>();
		int val = (int)(fval * 50.0);
		freqSlider_->setValue(val);
		freqSlider_->blockSignals(false);
		frequencyPoleControl_->updControl(cname);
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
mrs_natural orderFirstStage = 3;
mrs_natural orderSecondStage = 10;

	MarSystemManager mng;

	// create the phasevocoder network
	lpc_ = mng.create("Series", "lpcSeries");
	lpc_->addMarSystem(mng.create("SoundFileSource", "src"));
	lpc_->addMarSystem(mng.create("ShiftInput", "si"));

	MarSystem *fanin = mng.create("Fanin", "fanin");
	lpc_->addMarSystem(fanin);
	// analysis branch (generates the residual)
	MarSystem* flowthru = mng.create("FlowThru", "flowthru");
	flowthru->addMarSystem(mng.create("Windowing", "hamAna")); 
	flowthru->addMarSystem(mng.create("LPC", "lpc"));
	MarSystem* aSeries = mng.create("Series", "aSeries");
	aSeries->addMarSystem(flowthru);
	aSeries->addMarSystem(mng.create("Filter", "analysis"));
	aSeries->addMarSystem(mng.create("Gain", "residualLevel"));
	fanin->addMarSystem(aSeries);

	// noise branch (generates noise)
	MarSystem* nSeries = mng.create("Series", "nSeries");
    nSeries->addMarSystem(mng.create("NoiseSource", "ns"));
	nSeries->addMarSystem(mng.create("Gain", "nsg"));
	nSeries->addMarSystem(mng.create("Gain", "noiseLevel"));
	fanin->addMarSystem(nSeries);

	lpc_->addMarSystem(mng.create("Filter", "synthesis"));
	lpc_->addMarSystem(mng.create("Windowing", "winSyn")); 
	lpc_->addMarSystem(mng.create("OverlapAdd", "ova"));

	MarSystem* dest = mng.create("AudioSink", "dest");
	lpc_->addMarSystem(dest);

	lpc_->updctrl("mrs_natural/inSamples", D);
	lpc_->updctrl("mrs_natural/inObservations", 1);
	lpc_->updctrl("SoundFileSource/src/mrs_real/repetitions", -1.0);
    lpc_->updctrl("ShiftInput/si/mrs_natural/Decimation", D);
	lpc_->updctrl("ShiftInput/si/mrs_natural/WindowSize", Nw);

	lpc_->updctrl("Windowing/winSyn/mrs_string/type", "Hanning");

	lpc_->updctrl("Fanin/fanin/Series/aSeries/FlowThru/flowthru/LPC/lpc/mrs_natural/order",orderSecondStage);
	lpc_->updctrl("Fanin/fanin/Series/aSeries/FlowThru/flowthru/LPC/lpc/mrs_real/lambda",0.0);
	lpc_->updctrl("Fanin/fanin/Series/aSeries/FlowThru/flowthru/LPC/lpc/mrs_real/gamma",1.0);
	lpc_->updctrl("Fanin/fanin/Series/nSeries/NoiseSource/ns/mrs_string/mode", "truc");

	lpc_->linkctrl("Fanin/fanin/Series/aSeries/Filter/analysis/mrs_realvec/ncoeffs",
		"Fanin/fanin/Series/aSeries/FlowThru/flowthru/LPC/lpc/mrs_realvec/coeffs");
	lpc_->linkctrl("Filter/synthesis/mrs_realvec/dcoeffs",
		"Fanin/fanin/Series/aSeries/FlowThru/flowthru/LPC/lpc/mrs_realvec/coeffs");
	// link the power of the error with a gain
	lpc_->linkctrl("Fanin/fanin/Series/nSeries/Gain/nsg/mrs_real/gain",
		"Fanin/fanin/Series/aSeries/FlowThru/flowthru/LPC/lpc/mrs_real/power");


	mwr_ = new MarSystemQtWrapper(lpc_, true);
//	frequencyPolePtr_ = mwr_->getctrl("PvOscBank/ob/mrs_real/PitchShift");
	posPtr_ = mwr_->getctrl("SoundFileSource/src/mrs_natural/pos");
	initPtr_ = mwr_->getctrl("AudioSink/dest/mrs_bool/initAudio");
	fnamePtr_ = mwr_->getctrl("SoundFileSource/src/mrs_string/filename");
//	frequencyPoleControl_ = new MarControlGUI(frequencyPolePtr_, mwr_, this);
//	amplitudePoleControl_ = new MarControlGUI(amplitudePolePtr_, mwr_, this);
	posControl_ = new MarControlGUI(posPtr_, mwr_, this);
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

	//mwr_->trackctrl(frequencyPolePtr_); 
	//mwr_->trackctrl(amplitudePolePtr_); 
	mwr_->trackctrl(posPtr_);

	mwr_->updctrl(fnamePtr_, fileName.toStdString());
	mwr_->updctrl(initPtr_, true);

	mwr_->play();
}

void 
MarLpcWindow::about()
{
	QMessageBox::about(this, tr("Marsyas MarPhasevocoder"),  
		tr("Marsyas MarLpc: A graphical user interface for real-time manipulation \n using the Marsyas implementation of the Linear Prediction analysis/synthesis chain. \n"));
}
