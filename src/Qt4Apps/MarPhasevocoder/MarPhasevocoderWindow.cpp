
#include <QtGui>

#include "MarPhasevocoderWindow.h"

int dopt = 64;
int iopt = 64;
int sopt = 80;
int bopt = 128;
int vopt_ = 1;
mrs_real gopt_ = 1.0;
mrs_natural eopt_ = 0;

mrs_real popt = 1.0;

MarPhasevocoderWindow::MarPhasevocoderWindow(string inAudioFileName)
{
	QWidget *w = new QWidget;
	setCentralWidget(w);

	createActions();
	createMenus();  

	QLabel  *sinusoidsLabel  = new QLabel("Number of Sinusoids");
	QSlider *sinusoidsSlider = new QSlider(Qt::Horizontal);

	QLabel  *volumeLabel  = new QLabel("Volume");
	QSlider *volumeSlider = new QSlider(Qt::Horizontal);

	QLabel  *freqLabel1  = new QLabel("Frequency");
	QLabel  *freqLabel2  = new QLabel("Frequency");
	freqSlider_ = new QSlider(Qt::Horizontal);

	QLabel  *timeLabel  = new QLabel("Time");
	QSlider *timeSlider = new QSlider(Qt::Horizontal);

	QLabel *posLabel = new QLabel("Pos");
	posSlider_ = new QSlider(Qt::Horizontal);

	sinusoidsLabel->setMinimumWidth(150);
	volumeLabel->setMinimumWidth(150);

	freqSlider_->setValue(50);
	timeSlider->setValue(50);
	sinusoidsSlider->setValue(10);

	createNetwork(inAudioFileName);

	QGridLayout *gridLayout = new QGridLayout;

	gridLayout->addWidget(sinusoidsLabel, 0, 0);
	gridLayout->addWidget(sinusoidsSlider, 1, 0);

	gridLayout->addWidget(volumeLabel, 0, 1);
	gridLayout->addWidget(volumeSlider, 1, 1);

	gridLayout->addWidget(freqLabel1, 2, 0);
	gridLayout->addWidget(freqSlider_, 3, 0);

	gridLayout->addWidget(timeLabel, 2, 1);
	gridLayout->addWidget(timeSlider, 3, 1);

	gridLayout->addWidget(freqLabel2, 4, 0);
	gridLayout->addWidget(freqControl_, 5, 0);

	gridLayout->addWidget(posLabel, 4, 1);
	gridLayout->addWidget(posSlider_, 5, 1);

	gridLayout->addWidget(posControl_, 6, 0);
	

	connect(timeSlider, SIGNAL(valueChanged(int)), this, SLOT(timeChanged(int)));
	connect(volumeSlider, SIGNAL(valueChanged(int)), this, SLOT(volumeChanged(int)));
	connect(freqSlider_, SIGNAL(valueChanged(int)), this, SLOT(freqChanged(int)));
	connect(sinusoidsSlider, SIGNAL(valueChanged(int)), this, SLOT(sinusoidsChanged(int)));
	connect(posSlider_, SIGNAL(sliderReleased()), this, SLOT(posChanged()));
	
	connect(mwr_, SIGNAL(ctrlChanged(MarControlPtr)), this, SLOT(ctrlChanged(MarControlPtr)));

	w->setLayout(gridLayout);

	startNetwork();
	if (inAudioFileName != "")
	{
		
		mwr_->trackctrl(freqPtr_); 
		mwr_->trackctrl(posPtr_);
		
		mwr_->updctrl(fnamePtr_, inAudioFileName);
		mwr_->updctrl(initPtr_, true);
		
		mwr_->play();
	}
	
}

void 
MarPhasevocoderWindow::timeChanged(int value)
{
	float nval = iopt * ((50.0 + (100.0 - value)) / 100.0);
	mwr_->updctrl("PvOscBank/ob/mrs_natural/Interpolation", (mrs_natural) nval);
	mwr_->updctrl("ShiftOutput/so/mrs_natural/Interpolation", (mrs_natural)nval);
}

void 
MarPhasevocoderWindow::posChanged() 
{
	int value = posSlider_->sliderPosition();
	mwr_->updctrl(posPtr_, (mrs_natural) value * 512 * 40);    
}

void 
MarPhasevocoderWindow::sinusoidsChanged(int value)
{
	mwr_->updctrl("PvConvert/conv/mrs_natural/Sinusoids", 
		(mrs_natural) value);    
}

void 
MarPhasevocoderWindow::volumeChanged(int value)
{

}

void 
MarPhasevocoderWindow::ctrlChanged(MarControlPtr cname)
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

	if (name == "mrs_real/PitchShift") 
	{
		freqSlider_->blockSignals(true);
		mrs_real fval = cname->to<mrs_real>();
		int val = (int)(fval * 50.0);
		freqSlider_->setValue(val);
		freqSlider_->blockSignals(false);
		freqControl_->updControl(cname);
	}
}

void 
MarPhasevocoderWindow::freqChanged(int value)
{
	float pitchShift = value * 1.0 / 50.0;
	mwr_->updctrl(freqPtr_, pitchShift);
}

void 
MarPhasevocoderWindow::createNetwork(string inAudioFileName)
{
	mrs_natural N = 512;
	mrs_natural Nw = 512;
	mrs_natural I = iopt;
	mrs_real P = popt;
	mrs_natural D = dopt;

	MarSystemManager mng;

	// create the phasevocoder network
	pvoc_ = mng.create("Series", "pvoc_");
	pvoc_->addMarSystem(mng.create("SoundFileSource", "src"));
	pvoc_->addMarSystem(mng.create("ShiftInput", "si"));
	pvoc_->addMarSystem(mng.create("PvFold", "fo"));
	pvoc_->addMarSystem(mng.create("Spectrum", "spk"));
	pvoc_->addMarSystem(mng.create("PvConvert", "conv"));
	pvoc_->addMarSystem(mng.create("PvOscBank", "ob"));
	pvoc_->addMarSystem(mng.create("ShiftOutput", "so"));
	pvoc_->addMarSystem(mng.create("Gain", "gain"));
	MarSystem* dest = mng.create("AudioSink", "dest");
	pvoc_->addMarSystem(dest);

	pvoc_->updctrl("mrs_natural/inSamples", D);
	pvoc_->updctrl("mrs_natural/inObservations", 1);
	pvoc_->updctrl("SoundFileSource/src/mrs_real/repetitions", -1.0);
	pvoc_->updctrl("ShiftInput/si/mrs_natural/winSize", Nw);
	pvoc_->updctrl("PvFold/fo/mrs_natural/FFTSize", N);
	pvoc_->updctrl("PvConvert/conv/mrs_natural/Decimation",D);      
	pvoc_->updctrl("PvConvert/conv/mrs_natural/Sinusoids", (mrs_natural) sopt);  
	pvoc_->updctrl("PvOscBank/ob/mrs_natural/Interpolation", I);
	pvoc_->updctrl("PvOscBank/ob/mrs_real/PitchShift", P);
	pvoc_->updctrl("ShiftOutput/so/mrs_natural/Interpolation", I);
	pvoc_->updctrl("Gain/gain/mrs_real/gain", gopt_);

	// mwr_ = new MarSystemQtWrapper(pvoc_, true);
	mwr_ = new MarSystemQtWrapper(pvoc_);
	freqPtr_ = mwr_->getctrl("PvOscBank/ob/mrs_real/PitchShift");
	posPtr_ = mwr_->getctrl("SoundFileSource/src/mrs_natural/pos");
	initPtr_ = mwr_->getctrl("AudioSink/dest/mrs_bool/initAudio");
	fnamePtr_ = mwr_->getctrl("SoundFileSource/src/mrs_string/filename");
	if (inAudioFileName != "")
		mwr_->updctrl("SoundFileSource/src/mrs_string/filename", inAudioFileName);
	
	freqControl_ = new MarControlGUI(freqPtr_, mwr_, this);
	posControl_ = new MarControlGUI(posPtr_, mwr_, this);
}

void 
MarPhasevocoderWindow::startNetwork()
{
	mwr_->tickForever();
	mwr_->pause();
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
MarPhasevocoderWindow::open()
{
	QString fileName = QFileDialog::getOpenFileName(this);

	mwr_->trackctrl(freqPtr_); 
	mwr_->trackctrl(posPtr_);
	
	mwr_->updctrl(fnamePtr_, fileName.toStdString());
	mwr_->updctrl(initPtr_, true);

	mwr_->play();
}

void 
MarPhasevocoderWindow::about()
{
	QMessageBox::about(this, tr("Marsyas MarPhasevocoder"),  
		tr("Marsyas MarPhasevocoder: A graphical user interface for real-time manipulation \n using the Marsyas implementation of the phasevocoder. \n (based on the phasevocoder described in \n the Elements of Computer Music by F.R.Moore)"));
}
