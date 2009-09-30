
#include <QtGui>

#include "MarOscMultiGainWindow.h"


MarOscMultiGainWindow::MarOscMultiGainWindow(string fileName1, string fileName2, string fileName3)
{
	QWidget *w = new QWidget;
	setCentralWidget(w);

	QLabel  *gain1Label  = new QLabel(QString(fileName1.c_str()));
	gain1Slider_ = new QSlider(Qt::Horizontal);
	gain1Slider_->setValue(0);

	QLabel  *gain2Label  = new QLabel(QString(fileName2.c_str()));
	gain2Slider_ = new QSlider(Qt::Horizontal);
	gain2Slider_->setValue(0);

	QLabel  *gain3Label  = new QLabel(QString(fileName3.c_str()));
	gain3Slider_ = new QSlider(Qt::Horizontal);
	gain3Slider_->setValue(0);

	createNetwork(fileName1,fileName2,fileName3);

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

// 	startNetwork();

 	cout << "Playing files=(" << fileName1 << ")" << "(" << fileName2 << ")" << "(" << fileName3 << ")" << endl;
	play(fileName1,fileName2,fileName3);

}

void 
MarOscMultiGainWindow::gain1Changed(int value)
{
  mrs_real amp = value / 100.0;
  cout << "gain1 amp=" << amp << endl;
  mwr_->updctrl("Fanout/fanout/Series/series1/Gain/gain/mrs_real/gain",amp);
}

void 
MarOscMultiGainWindow::gain2Changed(int value)
{
  mrs_real amp = value / 100.0;
  cout << "gain2 amp=" << amp << endl;
  mwr_->updctrl("Fanout/fanout/Series/series2/Gain/gain/mrs_real/gain",amp);
}

void 
MarOscMultiGainWindow::gain3Changed(int value)
{
  mrs_real amp = value / 100.0;
  cout << "gain3 amp=" << amp << endl;
  mwr_->updctrl("Fanout/fanout/Series/series3/Gain/gain/mrs_real/gain",amp);
}


void 
MarOscMultiGainWindow::ctrlChanged(MarControlPtr cname)
{
	string name = cname->getName();
}

void 
MarOscMultiGainWindow::createNetwork(string fileName1,string fileName2,string fileName3)
{
	MarSystemManager mng;

	// create the overall network
	net_ = mng.create("Series", "net");

	MarSystem* fanout = mng.create("Fanout", "fanout");
	net_->addMarSystem(fanout);

	MarSystem* series1 = mng.create("Series", "series1");
	MarSystem* series2 = mng.create("Series", "series2");
	MarSystem* series3 = mng.create("Series", "series3");

	series1->addMarSystem(mng.create("SoundFileSource", "src"));
	series1->addMarSystem(mng.create("Gain", "gain"));

	series2->addMarSystem(mng.create("SoundFileSource", "src"));
	series2->addMarSystem(mng.create("Gain", "gain"));

	series3->addMarSystem(mng.create("SoundFileSource", "src"));
	series3->addMarSystem(mng.create("Gain", "gain"));

	fanout->addMarSystem(series1);
	fanout->addMarSystem(series2);
	fanout->addMarSystem(series3);

	net_->addMarSystem(mng.create("Sum", "sum"));

	MarSystem* dest = mng.create("AudioSink", "dest");
	net_->addMarSystem(dest);
	mwr_ = new MarSystemQtWrapper(net_, true);

	initPtr_ = mwr_->getctrl("AudioSink/dest/mrs_bool/initAudio");

	fname1Ptr_ = mwr_->getctrl("Fanout/fanout/Series/series1/SoundFileSource/src/mrs_string/filename");
	fname2Ptr_ = mwr_->getctrl("Fanout/fanout/Series/series2/SoundFileSource/src/mrs_string/filename");
	fname3Ptr_ = mwr_->getctrl("Fanout/fanout/Series/series3/SoundFileSource/src/mrs_string/filename");

	gain1Ptr_ = mwr_->getctrl("Fanout/fanout/Series/series1/Gain/gain/mrs_real/gain");
	gain2Ptr_ = mwr_->getctrl("Fanout/fanout/Series/series2/Gain/gain/mrs_real/gain");
	gain3Ptr_ = mwr_->getctrl("Fanout/fanout/Series/series3/Gain/gain/mrs_real/gain");

	mwr_->updctrl("Fanout/fanout/Series/series1/Gain/gain/mrs_real/gain",0.0);
	mwr_->updctrl("Fanout/fanout/Series/series2/Gain/gain/mrs_real/gain",0.0);
	mwr_->updctrl("Fanout/fanout/Series/series3/Gain/gain/mrs_real/gain",0.0);

	mwr_->updctrl("Fanout/fanout/Series/series1/SoundFileSource/src/mrs_real/repetitions",-1.0);
	mwr_->updctrl("Fanout/fanout/Series/series2/SoundFileSource/src/mrs_real/repetitions",-1.0);
	mwr_->updctrl("Fanout/fanout/Series/series3/SoundFileSource/src/mrs_real/repetitions",-1.0);
}

void 
MarOscMultiGainWindow::startNetwork()
{
//  	mwr_->tickForever();
}

void 
MarOscMultiGainWindow::play(string fileName1,string fileName2,string fileName3)
{
	mwr_->trackctrl(gain1Ptr_); 
	mwr_->trackctrl(gain2Ptr_); 
	mwr_->trackctrl(gain3Ptr_); 
	mwr_->updctrl(fname1Ptr_, fileName1);
	mwr_->updctrl(fname2Ptr_, fileName2);
	mwr_->updctrl(fname3Ptr_, fileName3);
	mwr_->updctrl(initPtr_, true);
	mwr_->start();
	mwr_->play();
}
