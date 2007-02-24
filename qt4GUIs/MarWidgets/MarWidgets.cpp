/*
**
** Copyright (C) 2005 George Tzanetakis. All rights reserved.
**
*/ 


#include <QtGui>

#include "MarWidgets.h"

MarWidgets::MarWidgets()
{
}



void MarWidgets::setup()
{

  // create dataflow network 
  MarSystemManager mng;
  
  pnet = mng.create("Series", "pnet");

  MarSystem* mixer = mng.create("Fanout","mixer");
  
  // create two branches of gain controlled Sources 
  MarSystem* sbr = mng.create("Series", "sbr");
  sbr->addMarSystem(mng.create("SoundFileSource", "src1"));
  sbr->addMarSystem(mng.create("Gain", "gain1"));
  sbr->addMarSystem(mng.create("Gain", "gain11"));
  
  MarSystem* nbr = mng.create("Series", "nbr");
  nbr->addMarSystem(mng.create("SoundFileSource", "src2"));
  nbr->addMarSystem(mng.create("Gain", "gain2"));
  nbr->addMarSystem(mng.create("Gain", "gain21"));
  
  // add the branches 
  mixer->addMarSystem(sbr);
  mixer->addMarSystem(nbr);
  
  // sum them and send to output 
  pnet->addMarSystem(mixer);
  pnet->addMarSystem(mng.create("Sum", "sum"));
  pnet->addMarSystem(mng.create("AudioSink", "dest"));

  
  



  // create top-level links(alias) to the controls we want 
  pnet->linkctrl("mrs_string/fname1", "Fanout/mixer/Series/sbr/SoundFileSource/src1/mrs_string/filename");
  pnet->linkctrl("mrs_real/gain1", "Fanout/mixer/Series/sbr/Gain/gain1/mrs_real/gain");

  pnet->linkctrl("mrs_string/fname2", 
		 "Fanout/mixer/Series/nbr/SoundFileSource/src2/mrs_string/filename");
  pnet->linkctrl("mrs_real/gain2", "Fanout/mixer/Series/nbr/Gain/gain2/mrs_real/gain");

  pnet->updctrl("mrs_natural/inSamples", 512);


  cout << "created Marsystem" << endl;

  
  // create wrapper for MarSystem
  mwr = new MarSystemQtWrapper(pnet);
  mwr->trackctrl(pnet->getctrl("mrs_real/gain1"));
  mwr->trackctrl(pnet->getctrl("mrs_real/gain2"));
  mwr->trackctrl(pnet->getctrl("mrs_string/fname1"));
  mwr->trackctrl(pnet->getctrl("mrs_string/fname2"));
		 
  cout << "creating wrapper" << endl;

  // button for starting sound 
  playButton = new QPushButton("play", this);
  connect(playButton, SIGNAL(clicked()), mwr, SLOT(play()));

  stepButton = new QPushButton("step", this);
  probeButton = new QCheckBox("probe", this);
  


  cout << "created buttons" << endl;
  
  mwr->start();
  cout << *pnet << endl;
  mwr->play(); 

  
  

  // widget for controlling MarSystems 
  // notice the different widget types and how they link 
  // when they share the control 
  MarControlWidget *mywidget1 = 
    new MarControlWidget(mwr->getctrl("mrs_real/gain1"), "spinbox", mwr, this);

  MarControlWidget *mywidget2 =
	      new MarControlWidget(mwr->getctrl("mrs_real/gain2"), "slider", mwr, this);

 MarControlWidget *mywidget3 =
        new MarControlWidget(mwr->getctrl("mrs_real/gain1"), "spinbox", mwr, this);

  MarControlWidget *mywidget4 =
          new MarControlWidget(mwr->getctrl("mrs_real/gain2"), "spinbox", mwr, this);

  MarControlWidget *mywidget5 =
		    new MarControlWidget(mwr->getctrl("mrs_real/gain1"), "slider", mwr, this);

  MarControlWidget *mywidget6 =
		      new MarControlWidget(mwr->getctrl("mrs_string/fname1"), "lineedit", mwr, this);
   MarControlWidget *mywidget7 =
		        new MarControlWidget(mwr->getctrl("mrs_string/fname2"), "lineedit", mwr, this);
	    



  

  cout << "created widgets" << endl;
  
  // set initial values for controls - notice this sets 
  // automatically the MarControlWidgets 

  mwr->updctrl("mrs_real/gain1", 0.5f);
 mwr->updctrl("mrs_string/fname1", 
	       "/home/gtzan/data/sound/music_speech/music/bmarsalis.au");
  mwr->updctrl("mrs_string/fname2", 
		"/home/gtzan/data/sound/music_speech/music/unpoco.au");

  mwr->updctrl("mrs_real/gain2", 0.5f);
  
  mwr->updctrl("AudioSink/dest/mrs_bool/initAudio", true);
  // Add children to a grid layout 
  QGridLayout *mdichildlayout = new QGridLayout();
  mdichildlayout->addWidget(playButton,0,0);
  mdichildlayout->addWidget(stepButton,0,1);
  mdichildlayout->addWidget(probeButton,0,2);
  
  mdichildlayout->addWidget(mywidget1,0,3);
  mdichildlayout->addWidget(mywidget2,1,0);
  mdichildlayout->addWidget(mywidget3,1,1);
  mdichildlayout->addWidget(mywidget4,1,2);
  mdichildlayout->addWidget(mywidget5,1,3);
  mdichildlayout->addWidget(mywidget6,2,0);
  mdichildlayout->addWidget(mywidget7,2,1);
  // mdichildlayout->addWidget(mywidget8,2,2);
   
   
  // mdichildlayout->addWidget(mywidget9,2,3);
  
  this->setLayout(mdichildlayout);
  

}


