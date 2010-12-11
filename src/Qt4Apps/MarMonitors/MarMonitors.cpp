/*
** Copyright (C) 1998-2010 George Tzanetakis <gtzan@cs.uvic.ca>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/


#include "MarMonitors.h"

using namespace std;
using namespace Marsyas;


MarMonitors::MarMonitors(string pluginName, string audioInputName)
{
	centralWidget_ = new QWidget;
	setCentralWidget(centralWidget_);
	nGraphs_ = 1;  
	createActions();
	createMenus();
  
	gridLayout_ = new QGridLayout;
	QPushButton *tickButton = new QPushButton(tr("Tick"));
	QPushButton *graphButton = new QPushButton(tr("New Graph"));
	QPushButton *setupButton = new QPushButton(tr("Setup"));
	QPushButton *tick10Button = new QPushButton(tr("Tick10"));
	QPushButton *tick100Button = new QPushButton(tr("Tick100"));
	

	gridLayout_->addWidget(tickButton, 0, 0);
	// gridLayout_->addWidget(graphButton, 0, 1);
	gridLayout_->addWidget(setupButton, 0, 1);
	// gridLayout_->addWidget(tick10Button, 1,0);
	// gridLayout_->addWidget(tick100Button, 1,1);
	

	connect(tickButton, SIGNAL(clicked()), this, SLOT(tick()));
	connect(setupButton, SIGNAL(clicked()), this, SLOT(setup()));
	centralWidget_->setLayout(gridLayout_);
  
	QString s1(pluginName.c_str());
	audioInputName_ = audioInputName;
	initNetwork(s1);
  
}


void 
MarMonitors::initNetwork(QString pluginName)
{
  
  
	// create the Marsyas 
	MarSystemManager mng;
  
	if (pluginName != "")
    {
		ifstream pluginStream(pluginName.toStdString().c_str());
		pnet_ = mng.getMarSystem(pluginStream);
      
		pnet_->updctrl("AudioSink/dest/mrs_bool/initAudio", true);

		if (pnet_ == 0) 
		{
			cout << "Manager does not support system " << endl;
			exit(1);
		}
    }
	else 
    {
		pnet_ = mng.create("Series", "pnet");
		pnet_->addMarSystem(mng.create("SoundFileSource", "src"));
		pnet_->addMarSystem(mng.create("Gain", "gain"));
		pnet_->addMarSystem(mng.create("Windowing", "windowing"));
		pnet_->addMarSystem(mng.create("Gain", "gain2"));
		pnet_->addMarSystem(mng.create("Windowing", "ham2"));
      
      
		pnet_->updctrl("SoundFileSource/src/mrs_string/filename", "/Users/gtzan/data/sound/music_speech/music/gravity.au");
		pnet_->updctrl("Windowing/windowing/mrs_bool/zeroPhasing", true);
		pnet_->updctrl("Gain/gain/mrs_real/gain", 2.0);
		pnet_->updctrl("Gain/gain2/mrs_real/gain", 3.0);
    }
  
	nTicks_ = 1;
  
}





void
MarMonitors::createMenus()
{
	fileMenu = menuBar()->addMenu(tr("&File"));
	fileMenu->addAction(openAct);
	menuBar()->addSeparator();
	helpMenu = menuBar()->addMenu(tr("&Help"));
	helpMenu->addAction(aboutAct);
}


void 
MarMonitors::createActions()
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
MarMonitors::open()
{
	QString fileName = QFileDialog::getOpenFileName(this);
	initNetwork(fileName);
}

void 
MarMonitors::about()
{
	// QMessageBox::about(this, tr("Marsyas MarMonitors"),  
	// tr("Marsyas MarPhasevocoder: A graphical user interface for viewing \n the intermediate results of dataflow calcualtion in arbitrary marsystems stored as plugins"));
   
}





void 
MarMonitors::graph(int graph_size, string xlabel, string label)
{
	nGraphs_ += 1;

	// Create new graph
	Marx2DGraph* graph = new Marx2DGraph(graph_size, 0);
	graph->setPlotType(Marx2DGraph::LINEAR_INTERPOLATION);
	graph->setGraphDataLineSize( 1.0 );
	graph->setShowAxisScale(true);
	graph->setXAxisLabel(xlabel);
	graph->setXAxisLabelOn(true);
	graph->addLabel(label);
	graphs.push_back(graph);
 
 


	gridLayout_->addWidget(graph, nGraphs_/2, (nGraphs_ % 2));
	probes_.push_back(label);
}


void 
MarMonitors::dialogDone()
{

	nTicks_ = graphNum->value();
	
	string cname = listWidget->currentItem()->text().toStdString();
	mrs_realvec foo = pnet_->getctrl(cname)->to<mrs_realvec>();
	pnet_->updctrl("mrs_string/filename", audioInputName_);
	if (foo.getCols() == 1)
		graph(foo.getRows(), "Observations", cname);
	else 
		graph(foo.getCols(), "Samples", cname);
  

}


void 
MarMonitors::setup()
{
	QDialog *setupDialog = new QDialog(this);
	setupDialog->setGeometry(400,200, 500, 400);
	
	
	QLabel* label = new QLabel(tr("Number of ticks"));
	QPushButton* doneButton = new QPushButton(tr("Done"));
  

	QGridLayout *layout = new QGridLayout;



	mycontrols_ = pnet_->getControls();
	std::map<std::string, MarControlPtr>::iterator myc;
	listWidget = new QListWidget();  
  
	for (myc = mycontrols_.begin(); myc != mycontrols_.end(); ++myc)
    {
		if ((myc->first).find("realvec") < myc->first.length())
		{
			QListWidgetItem *newItem = new QListWidgetItem(tr(myc->first.c_str()), listWidget);	  
		}
      
    }
  
	listWidget->setCurrentRow(0);
  

	graphNum = new QSpinBox();
	graphNum->setWrapping(true);
	graphNum->setMaximum(1000);
	graphNum->setValue(nTicks_);
	
	layout->addWidget(graphNum, 1, 0);  
	

	layout->addWidget(label, 0, 0);
	layout->addWidget(listWidget, 2, 0);
	layout->addWidget(doneButton, 3, 0);

	setupDialog->setLayout(layout);
  
	connect(doneButton, SIGNAL(clicked()), setupDialog, SLOT(accept()));
	connect(doneButton, SIGNAL(clicked()), this, SLOT(dialogDone()));  
	setupDialog->exec();
}


void 
MarMonitors::tick()
{
	pnet_->updctrl("AudioSink/dest/mrs_bool/mute", true);	
	for (int i=0; i<nTicks_; i++)
	  {
	    pnet_->tick();  

		


	    // Hack for specific case - needs to be removed 
		mrs_realvec processedData = pnet_->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();
		cout << processedData << endl;
		
		// mrs_realvec tempos(2);
	    // mrs_real bin = processedData(1) * 0.25;
		// tempos(0) = bin;
		
	    // cout << "BIN = " << bin << endl;
	    // pnet_->updctrl("BeatPhase/beatphase/mrs_realvec/tempos",tempos);
		
	  }

	for (int i = 0; i < graphs.size(); ++i) 
    {
		out_ = (mycontrols_[probes_[i]])->to<mrs_realvec>();

		graphs[i]->setYMaxMin(out_.maxval(), out_.minval());	  
		
		out_.normMaxMin();
		out_ -= 0.5;
		out_ *= 2.0;
		graphs[i]->setBuffer( out_ );
    }
	// second tick to silence audio 
	// pnet_->updctrl("AudioSink/dest/mrs_bool/mute", true);
	// pnet_->tick();
	
}
