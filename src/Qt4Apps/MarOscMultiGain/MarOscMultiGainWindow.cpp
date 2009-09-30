
#include <QtGui>

#include "MarOscMultiGainWindow.h"


MarOscMultiGainWindow::MarOscMultiGainWindow(string fileName)
{
	QWidget *w = new QWidget;
	setCentralWidget(w);

	QLabel  *gain1Label  = new QLabel("gain");
	gain1Slider_ = new QSlider(Qt::Horizontal);
	gain1Slider_->setValue(100);

	QLabel  *gain2Label  = new QLabel("gain");
	gain2Slider_ = new QSlider(Qt::Horizontal);
	gain2Slider_->setValue(100);

	QLabel  *gain3Label  = new QLabel("gain");
	gain3Slider_ = new QSlider(Qt::Horizontal);
	gain3Slider_->setValue(100);

	createNetwork(fileName);

	QGridLayout *gridLayout = new QGridLayout;

	gridLayout->addWidget(gain1Label, 1, 1);
	gridLayout->addWidget(gain1Slider_, 2, 1);

	gridLayout->addWidget(gain2Label, 3, 1);
	gridLayout->addWidget(gain2Slider_, 4, 1);

	gridLayout->addWidget(gain3Label, 5, 1);
	gridLayout->addWidget(gain3Slider_, 6, 1);

	connect(gain1Slider_, SIGNAL(valueChanged(int)), this, SLOT(gain1Changed(int)));
	connect(gain2Slider_, SIGNAL(valueChanged(int)), this, SLOT(gain2Changed(int)));
	connect(gain3Slider_, SIGNAL(valueChanged(int)), this, SLOT(gain3Changed(int)));

	w->setLayout(gridLayout);

	startNetwork();

 	cout << "Playing file=(" << fileName << ")" << endl;
//   	play(fileName);

}

void 
MarOscMultiGainWindow::gain1Changed(int value)
{
  mrs_real amp = value / 100.0;
  cout << "gain1 amp=" << amp << endl;
  mwr_->updctrl("Gain/gain1/mrs_real/gain",amp);
}

void 
MarOscMultiGainWindow::gain2Changed(int value)
{
  mrs_real amp = value / 100.0;
  cout << "gain2 amp=" << amp << endl;
  mwr_->updctrl("Gain/gain2/mrs_real/gain",amp);
}

void 
MarOscMultiGainWindow::gain3Changed(int value)
{
  mrs_real amp = value / 100.0;
  cout << "gain3 amp=" << amp << endl;
  mwr_->updctrl("Gain/gain3/mrs_real/gain",amp);
}


void 
MarOscMultiGainWindow::ctrlChanged(MarControlPtr cname)
{
	string name = cname->getName();
}

void 
MarOscMultiGainWindow::createNetwork(string fileName)
{
	MarSystemManager mng;

	// create the overall network
	net_ = mng.create("Series", "net");
	net_->addMarSystem(mng.create("SoundFileSource", "src"));
	net_->addMarSystem(mng.create("Gain", "gain1"));
	net_->addMarSystem(mng.create("Gain", "gain2"));
	net_->addMarSystem(mng.create("Gain", "gain3"));

	MarSystem* dest = mng.create("AudioSink", "dest");
	net_->addMarSystem(dest);
	mwr_ = new MarSystemQtWrapper(net_, true);

	initPtr_ = mwr_->getctrl("AudioSink/dest/mrs_bool/initAudio");
	fnamePtr_ = mwr_->getctrl("SoundFileSource/src/mrs_string/filename");

	gain1Ptr_ = mwr_->getctrl("Gain/gain1/mrs_real/gain");
	gain2Ptr_ = mwr_->getctrl("Gain/gain2/mrs_real/gain");
	gain3Ptr_ = mwr_->getctrl("Gain/gain3/mrs_real/gain");

	mwr_->updctrl("Gain/gain1/mrs_real/gain",1.0);
	mwr_->updctrl("Gain/gain2/mrs_real/gain",1.0);
	mwr_->updctrl("Gain/gain3/mrs_real/gain",1.0);
}

void 
MarOscMultiGainWindow::startNetwork()
{
 	mwr_->tickForever();
}

void 
MarOscMultiGainWindow::play(string fileName)
{
	mwr_->trackctrl(gain1Ptr_); 
	mwr_->trackctrl(gain2Ptr_); 
	mwr_->trackctrl(gain3Ptr_); 
	mwr_->updctrl(fnamePtr_, fileName);
	mwr_->updctrl(initPtr_, true);
	mwr_->play();
}
