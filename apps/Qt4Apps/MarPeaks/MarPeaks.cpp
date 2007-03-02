
#include <QtGui>

#include "MarPeaks.h"

MarPeaks::MarPeaks()
{
	QWidget *w = new QWidget;
	setCentralWidget(w);

	plot_ = false;
	spectrogram_ = new Plot(this);
	spectrogram_->setData(data_);

	posSlider_ = new QSlider(Qt::Horizontal);

	createActions();
	createMenus();  
	
	createNetwork();

	QGridLayout *gridLayout = new QGridLayout;
	gridLayout->addWidget(spectrogram_, 0, 0);
	gridLayout->addWidget(posSlider_, 1, 0);
	w->setLayout(gridLayout);
	
	connect(mwr_, SIGNAL(ctrlChanged(MarControlPtr)), this, SLOT(ctrlChanged(MarControlPtr)));

	startNetwork();
}

void 
MarPeaks::createNetwork()
{
	MarSystemManager mng;

	// create the phasevocoder network
	msys_ = mng.create("Series", "pnet");
	msys_->addMarSystem(mng.create("SoundFileSource", "src"));
	//msys_->addMarSystem(mng.create("Spectrum", "spk"));
	
	MarSystem* powerSpect = mng.create("Series", "pspect");
	powerSpect->addMarSystem(mng.create("Hamming", "win"));
	powerSpect->addMarSystem(mng.create("Spectrum", "spk"));
	powerSpect->addMarSystem(mng.create("PowerSpectrum", "pspk"));
	//powerSpect->addMarSystem(mng.create("Memory", "mem"));

 	MarSystem* fanout = mng.create("Fanout", "fanout");
	fanout->addMarSystem(powerSpect);
	fanout->addMarSystem(mng.create("AudioSink", "dest"));
 	
	msys_->addMarSystem(fanout);

	msys_->updctrl("SoundFileSource/src/mrs_natural/inSamples", 1024);
	msys_->updctrl("SoundFileSource/src/mrs_natural/inObservations", 1);
	msys_->updctrl("SoundFileSource/src/mrs_real/repetitions", 1.0);
	msys_->updctrl("Fanout/fanout/Series/pspect/PowerSpectrum/pspk/mrs_string/spectrumType", "decibels");
	//msys_->updctrl("Fanout/fanout/Series/pspect/Memory/mem/mrs_natural/memSize", 100);

	mwr_ = new MarSystemQtWrapper(msys_);

	ctrl_Spectrum_ = mwr_->getctrl("Fanout/fanout/Series/pspect/PowerSpectrum/pspk/mrs_realvec/outTick");
	//ctrl_Spectrum_ = mwr_->getctrl("Fanout/fanout/Series/pspect/Memory/mem/mrs_realvec/outTick");
	
	ctrl_initAudio_ = mwr_->getctrl("Fanout/fanout/AudioSink/dest/mrs_bool/initAudio");

	ctrl_pos_ = mwr_->getctrl("SoundFileSource/src/mrs_natural/pos");
	ctrl_notEmpty_ = mwr_->getctrl("SoundFileSource/src/mrs_bool/notEmpty");

	mwr_->trackctrl(ctrl_Spectrum_);
	mwr_->trackctrl(ctrl_pos_);
	mwr_->trackctrl(ctrl_notEmpty_);
}

void 
MarPeaks::startNetwork()
{
	mwr_->start();
	mwr_->pause();
}

void
MarPeaks::createMenus()
{
	fileMenu = menuBar()->addMenu(tr("&File"));
	fileMenu->addAction(openAct);
	menuBar()->addSeparator();
	helpMenu = menuBar()->addMenu(tr("&Help"));
	helpMenu->addAction(aboutAct);
}

void 
MarPeaks::createActions()
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
MarPeaks::about()
{
	QMessageBox::about(this, tr("Marsyas MarPhasevocoder"),  
		tr("Marsyas MarPeaker: A very dummy graphical user interface for real-time spectrograms)"));
}

void 
MarPeaks::open()
{
	QString fileName = QFileDialog::getOpenFileName(this);
	mwr_->updctrl("SoundFileSource/src/mrs_string/filename", fileName.toStdString());

	mrs_natural size = mwr_->getctrl("SoundFileSource/src/mrs_natural/size")->toNatural();
	posSlider_->setMaximum(size);

	// setup marsyas network for just reading the entire file and 
	// calculate the spectrogram (i.e. disable audio playing for now)
// 	msys_->updctrl("Fanout/fanout/mrs_natural/enable", 0); //enable spectrum calculation
// 	msys_->updctrl("Fanout/fanout/mrs_natural/disable", 1); //disable audio playback
// 
// 	//clear spectrogram data container
// 	data_.clear();
// 	spectrogram_->setData(data_);
// 	
// 	//run network for spectrogram calculation of entire file
// 	//(with audio output disabled)
// 	plot_ = true;
// 	mwr_->play();
// 	//while(ctrl_notEmpty_->isTrue()) {;} //reading a control without mutex!!! DeadLocks and data corruption!!! [!]
// 	//mwr_->pause();
//  	//plot_ = false;
 
// 	//rewind SoundFileSource so we can then play it from the beginning
// 	//(this is a hack for doing a rewind!!!! Not nice... :-\)
// 	mwr_->updctrl("SoundFileSource/src/mrs_string/filename", "");
// 	mwr_->updctrl("SoundFileSource/src/mrs_string/filename", fileName.toStdString());
// 	
// 	//enable playback and disable spectrogram calculation
 	msys_->updctrl("Fanout/fanout/mrs_natural/disable", 0);//disable spectrum calculation
 	msys_->updctrl("Fanout/fanout/mrs_natural/enable", 1); //enable audio playback
 
 	mwr_->updctrl(ctrl_initAudio_, true);
 	mwr_->play();
}

void 
MarPeaks::ctrlChanged(MarControlPtr ctrl)
{
// 	if (ctrl.isEqual(ctrl_pos_))
// 	{
// 		//mrs_real fval = ctrl->to<mrs_natural>()  / (512.0 * 40.0);
// 		if (posSlider_->isSliderDown() == false)
// 		{
// 			posSlider_->blockSignals(true);
// 			posSlider_->setValue((int)ctrl->to<mrs_natural>());
// 			posSlider_->blockSignals(false);
// 		}
// 	}

// 	if (ctrl.isEqual(ctrl_Spectrum_) && plot_)
// 	{
// 		//data_.addSpectrum(&(ctrl_Spectrum_->to<mrs_realvec>()));
// 		//spectrogram_->setData(data_);
// 	}

// 	if(ctrl.isEqual(ctrl_notEmpty_))
// 	{
// 		if(!ctrl_notEmpty_->toBool())
// 		{
// 			ctrl_pos_->setValue(0);
// 			spectrogram_->setData(data_);
// 		}
// 	}
}