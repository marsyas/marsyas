
#include <QtGui>

#include "MarOscOutWindow.h"


MarOscOutWindow::MarOscOutWindow(string fileName)
{
	QWidget *w = new QWidget;
	setCentralWidget(w);

	QLabel  *gainLabel1  = new QLabel("gain");
	gainSlider_ = new QSlider(Qt::Horizontal);

	gainSlider_->setValue(0);

	createNetwork();

	QGridLayout *gridLayout = new QGridLayout;

	gridLayout->addWidget(gainLabel1, 2, 1);
	gridLayout->addWidget(gainSlider_, 3, 1);

	connect(gainSlider_, SIGNAL(valueChanged(int)), this, SLOT(gainChanged(int)));

	w->setLayout(gridLayout);

	startNetwork();

 	cout << "Playing file=(" << fileName << ")" << endl;
 	play(fileName);

}

void 
MarOscOutWindow::gainChanged(int value)
{
  mrs_real amp = value / 100.0;
  cout << "amp=" << amp << endl;
  mwr_->updctrl("Gain/gain/mrs_real/gain",amp);
}


void 
MarOscOutWindow::ctrlChanged(MarControlPtr cname)
{
	string name = cname->getName();
}

void 
MarOscOutWindow::createNetwork()
{
	MarSystemManager mng;

	// create the overall network
	net_ = mng.create("Series", "lpcSeries");
	net_->addMarSystem(mng.create("SoundFileSource", "src"));
	net_->addMarSystem(mng.create("Gain", "gain"));

	MarSystem* dest = mng.create("AudioSink", "dest");
	net_->addMarSystem(dest);
	mwr_ = new MarSystemQtWrapper(net_, true);

	initPtr_ = mwr_->getctrl("AudioSink/dest/mrs_bool/initAudio");
	fnamePtr_ = mwr_->getctrl("SoundFileSource/src/mrs_string/filename");

	gainPtr_ = mwr_->getctrl("Gain/gain/mrs_real/gain");

	mwr_->updctrl("Gain/gain/mrs_real/gain",0.0);
}

void 
MarOscOutWindow::startNetwork()
{
 	mwr_->tickForever();
}

void 
MarOscOutWindow::play(string fileName)
{
	mwr_->trackctrl(gainPtr_); 
	mwr_->updctrl(fnamePtr_, fileName);
	mwr_->updctrl(initPtr_, true);
	mwr_->play();
}
