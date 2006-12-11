
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




MarPhasevocoderWindow::MarPhasevocoderWindow()
{

  


  QWidget *w = new QWidget;
  setCentralWidget(w);

  createActions();
  createMenus();  

  QLabel  *sinusoidsLabel  = new QLabel("Number of Sinusoids");
  QSlider *sinusoidsSlider = new QSlider(Qt::Horizontal);

  QLabel  *volumeLabel  = new QLabel("Volume");
  QSlider *volumeSlider = new QSlider(Qt::Horizontal);

  QLabel  *freqLabel  = new QLabel("Frequency");
  QSlider *freqSlider = new QSlider(Qt::Horizontal);

  QLabel  *timeLabel  = new QLabel("Time");
  QSlider *timeSlider = new QSlider(Qt::Horizontal);

  sinusoidsLabel->setMinimumWidth(150);
  volumeLabel->setMinimumWidth(150);
  
  freqSlider->setValue(50);
  timeSlider->setValue(50);
  sinusoidsSlider->setValue(10);
  


  QGridLayout *gridLayout = new QGridLayout;

  gridLayout->addWidget(sinusoidsLabel, 0, 0);
  gridLayout->addWidget(sinusoidsSlider, 1, 0);

  gridLayout->addWidget(volumeLabel, 0, 1);
  gridLayout->addWidget(volumeSlider, 1, 1);

  gridLayout->addWidget(freqLabel, 2, 0);
  gridLayout->addWidget(freqSlider, 3, 0);

  gridLayout->addWidget(timeLabel, 2, 1);
  gridLayout->addWidget(timeSlider, 3, 1);


  connect(timeSlider, SIGNAL(valueChanged(int)), this, SLOT(timeChanged(int)));
  connect(volumeSlider, SIGNAL(valueChanged(int)), this, SLOT(volumeChanged(int)));
  connect(freqSlider, SIGNAL(valueChanged(int)), this, SLOT(freqChanged(int)));
  connect(sinusoidsSlider, SIGNAL(valueChanged(int)), this, SLOT(sinusoidsChanged(int)));
  
  
  

  
  w->setLayout(gridLayout);

  createNetwork();
  

}


void 
MarPhasevocoderWindow::timeChanged(int value)
{
  cout << "Time Stretching Changed" << endl;
  cout << "value " << value << endl;
  
  float nval = iopt * (value / 100.0);
  
  cout << "(int) nval = " << (mrs_natural) nval << endl;
  

  mwr_->updctrl("PvOscBank/ob/mrs_natural/Interpolation", (mrs_natural) nval);
  mwr_->updctrl("ShiftOutput/so/mrs_natural/Interpolation", (mrs_natural)nval);
  


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
  cout << "Volume Changed" << endl;
  cout << "value " << value << endl;

}

void 
MarPhasevocoderWindow::freqChanged(int value)
{
  // cout << "Frequency Changed" << endl;
  // cout << "value " << value << endl;
  
  float pitchShift = value * 1.0 / 50.0;
  cout << "pitchShift" << pitchShift << endl;
  
  mwr_->updctrl("PvOscBank/ob/mrs_real/PitchShift", pitchShift);  

}




void 
MarPhasevocoderWindow::createNetwork()
{
  cout << "CreateNetwork" << endl;
  
  mrs_natural N = 512;
  mrs_natural Nw = 512;
  mrs_natural I = iopt;
  mrs_natural P = popt;
  mrs_natural D = dopt;
  string sfName("vlobos.au");
  


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

  pvoc_->updctrl("SoundFileSource/src/mrs_string/filename", sfName);
  pvoc_->updctrl("SoundFileSource/src/mrs_natural/inSamples", D);
  pvoc_->updctrl("SoundFileSource/src/mrs_natural/inObservations", 1);

  pvoc_->updctrl("SoundFileSource/src/mrs_real/repetitions", -1.0);
  pvoc_->updctrl("ShiftInput/si/mrs_natural/Decimation", D);
  pvoc_->updctrl("ShiftInput/si/mrs_natural/WindowSize", Nw);
  pvoc_->updctrl("PvFold/fo/mrs_natural/FFTSize", N);
  pvoc_->updctrl("PvFold/fo/mrs_natural/WindowSize", Nw);
  pvoc_->updctrl("PvFold/fo/mrs_natural/Decimation", D);
  pvoc_->updctrl("PvConvert/conv/mrs_natural/Decimation",D);      
  pvoc_->updctrl("PvConvert/conv/mrs_natural/Sinusoids", (mrs_natural) sopt);  
  pvoc_->updctrl("PvOscBank/ob/mrs_natural/Interpolation", I);
  pvoc_->updctrl("PvOscBank/ob/mrs_real/PitchShift", P);
  pvoc_->updctrl("ShiftOutput/so/mrs_natural/Interpolation", I);
  pvoc_->updctrl("ShiftOutput/so/mrs_natural/WindowSize", Nw);      
  pvoc_->updctrl("ShiftOutput/so/mrs_natural/Decimation", D);
  pvoc_->updctrl("Gain/gain/mrs_real/gain", gopt_);
 
  pvoc_->updctrl("AudioSink/dest/mrs_bool/initAudio", true);

  mwr_ = new MarSystemWrapper(pvoc_);
  mwr_->start();  

  mwr_->play();

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
  
  mwr_->updctrl("SoundFileSource/src/mrs_string/filename", fileName.toStdString());
}



void 
MarPhasevocoderWindow::about()
{
  QMessageBox::about(this, tr("Marsyas MarPhasevocoder"),  tr("Marsyas MarPhasevocoder: A graphical user interface for real-time manipulation \n using the Marsyas implementation of the phasevocoder. \n (based on the phasevocoder described in \n the Elements of Computer Music by F.R.Moore)"));
  
}
