/****************************************************************************
**
** Copyright (C) 2005-2005 Trolltech AS. All rights reserved.
**
** This file is part of the example classes of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.trolltech.com/products/qt/opensource.html
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://www.trolltech.com/products/qt/licensing.html or contact the
** sales department at sales@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include <QtGui>

#include "MarGrid.h"

using namespace std;
using namespace Marsyas;

MarGrid::MarGrid(QWidget *parent)
  : QWidget(parent)
{
  setAcceptDrops(true);
  setMouseTracking(true);

  int winWidth, winHeight;
  
  cell_size = 50;
  som_width = 12;;
  som_height = 12;
   
  winWidth = cell_size * som_width;
  winHeight = cell_size * som_height;

  setMinimumSize(winWidth, winHeight);

  for (int i=0; i < som_width; i++)
    for (int j=0; j < som_height; j++) 
      {
	QList<string> empty;
	files.push_back(empty);
	counters.push_back(0);
	counterSizes.push_back(0);
	labels.push_back(0);
      }
  
  
  // Create playback network
  pnet_ = mng.create("Series", "pnet_");
  pnet_->addMarSystem(mng.create("SoundFileSource", "src"));
  pnet_->addMarSystem(mng.create("Gain", "gain"));
  pnet_->addMarSystem(mng.create("AudioSink", "dest"));
  pnet_->linkctrl("mrs_bool/notEmpty","SoundFileSource/src/mrs_bool/notEmpty");

  pnet_->updctrl("AudioSink/dest/mrs_bool/initAudio", true);

  mwr_ = new MarSystemQtWrapper(pnet_);
  mwr_->start();
  
  filePtr_ = mwr_->getctrl("SoundFileSource/src/mrs_string/filename");


  setupTrain("music.mf");
}



void 
MarGrid::setupTrain(QString fname)
{
  // Build network for feature extraction 
  MarSystem* extractNet = mng.create("Series", "extractNet");
  extractNet->addMarSystem(mng.create("SoundFileSource", "src"));
  
  MarSystem* spectralNet = mng.create("Series", "spectralNet");
  spectralNet->addMarSystem(mng.create("Windowing", "ham"));
  spectralNet->addMarSystem(mng.create("Spectrum", "spk"));
  spectralNet->addMarSystem(mng.create("PowerSpectrum", "pspk"));
  
  MarSystem* featureFanout = mng.create("Fanout", "featureFanout");
  featureFanout->addMarSystem(mng.create("Centroid", "centroid"));
  featureFanout->addMarSystem(mng.create("Rolloff", "rolloff"));
  featureFanout->addMarSystem(mng.create("MFCC", "mfcc"));
  
  spectralNet->addMarSystem(featureFanout);
  extractNet->addMarSystem(spectralNet);
  extractNet->addMarSystem(mng.create("Memory", "mem"));
  
  MarSystem* stats = mng.create("Fanout", "stats");
  stats->addMarSystem(mng.create("Mean", "mn1"));
  stats->addMarSystem(mng.create("StandardDeviation", "std1"));
  extractNet->addMarSystem(stats);
  
  MarSystem* acc = mng.create("Accumulator", "acc");
  acc->updctrl("mrs_natural/nTimes", 1200);
  acc->addMarSystem(extractNet);
  
  total_ = mng.create("Series", "total");
  total_->addMarSystem(acc);
  MarSystem* stats2 = mng.create("Fanout", "stats2");
  stats2->addMarSystem(mng.create("Mean", "mn2"));
  stats2->addMarSystem(mng.create("StandardDeviation", "std2"));

  total_->addMarSystem(stats2);
  total_->addMarSystem(mng.create("Annotator", "ann"));
  


  total_->linkctrl("mrs_string/filename",
		  "Accumulator/acc/Series/extractNet/SoundFileSource/src/mrs_string/filename");  
  

  total_->linkctrl("mrs_string/currentlyPlaying",
		  "Accumulator/acc/Series/extractNet/SoundFileSource/src/mrs_string/currentlyPlaying");  


  total_->linkctrl("mrs_bool/shuffle",
		  "Accumulator/acc/Series/extractNet/SoundFileSource/src/mrs_bool/shuffle");  
  
  total_->linkctrl("mrs_natural/pos",
		  "Accumulator/acc/Series/extractNet/SoundFileSource/src/mrs_natural/pos");  

  total_->linkctrl("mrs_real/repetitions",
		  "Accumulator/acc/Series/extractNet/SoundFileSource/src/mrs_real/repetitions");  


  total_->linkctrl("mrs_natural/cindex",
		  "Accumulator/acc/Series/extractNet/SoundFileSource/src/mrs_natural/cindex");  

  total_->linkctrl("mrs_natural/numFiles",
		  "Accumulator/acc/Series/extractNet/SoundFileSource/src/mrs_natural/numFiles");  
  
  total_->linkctrl("mrs_string/allfilenames",
		   "Accumulator/acc/Series/extractNet/SoundFileSource/src/mrs_string/allfilenames");  
  
  total_->linkctrl("mrs_natural/numFiles",
		   "Accumulator/acc/Series/extractNet/SoundFileSource/src/mrs_natural/numFiles");  
  
  
  total_->linkctrl("mrs_bool/notEmpty",
		  "Accumulator/acc/Series/extractNet/SoundFileSource/src/mrs_bool/notEmpty");  
  total_->linkctrl("mrs_bool/advance",
		  "Accumulator/acc/Series/extractNet/SoundFileSource/src/mrs_bool/advance");  
  
  total_->linkctrl("mrs_bool/memReset",
		  "Accumulator/acc/Series/extractNet/Memory/mem/mrs_bool/reset");  
  
  total_->linkctrl("mrs_natural/label",
		  "Annotator/ann/mrs_natural/label");
  
  
  
  total_->updctrl("mrs_natural/inSamples", 512);
  
  trainFname = fname;
  predictFname = "test.mf";
  total_->updctrl("mrs_string/filename", trainFname.toStdString());
  total_->updctrl("mrs_real/repetitions", 1.0);
  

}

  

void 
MarGrid::extract()
{
  // EXTRACT FEATURES 
  int index= 0;
  int numFiles = total_->getctrl("mrs_natural/numFiles")->to<mrs_natural>();

  realvec som_in;
  realvec som_res;
  realvec som_fmatrix;

  mrs_natural total_onObservations = 
    total_->getctrl("mrs_natural/onObservations")->to<mrs_natural>();
  
  som_in.create(total_->getctrl("mrs_natural/inObservations")->to<mrs_natural>(), 
		total_->getctrl("mrs_natural/inSamples")->to<mrs_natural>());
  
  som_res.create(total_onObservations, 
		 total_->getctrl("mrs_natural/onSamples")->to<mrs_natural>());
  
  som_fmatrix.create(total_onObservations, 
		     numFiles);

  // calculate features 
  cout << "Calculating features" << endl;
  for (index=0; index < numFiles; index++)
    {
      total_->updctrl("mrs_natural/label", index);
      total_->updctrl("mrs_bool/memReset", true);
      total_->updctrl("mrs_natural/cindex", index);
      
      string current = total_->getctrl("mrs_string/currentlyPlaying")->to<mrs_string>();
      cout << current  << " - ";
      
      cout << "Processed " << index << " files " << endl;	  
      total_->process(som_in,som_res);
      
      for (int o=0; o < total_onObservations; o++) 
	som_fmatrix(o, index) = som_res(o, 0);
    }

  ofstream oss;
  oss.open("som_fmatrix.txt");
  oss << som_fmatrix << endl;
}



void
MarGrid::train()
{
  // Read the feature matrix from file som_fmatrix.txt 
  realvec train_som_fmatrix;
  ifstream iss;
  iss.open("som_fmatrix.txt");
  iss >> train_som_fmatrix;

  // Normalize the feature matrix so that all features are between 0 and 1
  norm_som_fmatrix.create(train_som_fmatrix.getRows(),
			  train_som_fmatrix.getCols());
  norm_ = mng.create("NormMaxMin", "norm");
  norm_->updctrl("mrs_natural/inSamples", train_som_fmatrix.getCols());
  norm_->updctrl("mrs_natural/inObservations", 
		total_->getctrl("mrs_natural/onObservations")->to<mrs_natural>());
  norm_->updctrl("mrs_string/mode", "train");
  norm_->process(train_som_fmatrix, norm_som_fmatrix);
  norm_->updctrl("mrs_string/mode", "predict");  
  norm_->process(train_som_fmatrix, norm_som_fmatrix);

  // Create netork for training the self-organizing map 
  som_ = mng.create("SOM", "som");  
  som_->updctrl("mrs_natural/grid_width", som_width);
  som_->updctrl("mrs_natural/grid_height", som_height);
  som_->updctrl("mrs_natural/inSamples", norm_som_fmatrix.getCols());
  som_->updctrl("mrs_natural/inObservations", norm_som_fmatrix.getRows());  
  som_->updctrl("mrs_string/mode", "train");

  realvec som_fmatrixres;
  som_fmatrixres.create(som_->getctrl("mrs_natural/onObservations")->to<mrs_natural>(), 
			som_->getctrl("mrs_natural/onSamples")->to<mrs_natural>());
  
  cout << "Starting training" << endl;
  
  for (int i=0; i < 2000; i ++) 
    {
      cout << "Training iteration" << i << endl;
      norm_som_fmatrix.shuffle();
      som_->process(norm_som_fmatrix, som_fmatrixres);
    }
  
  cout << "Training done" << endl;
  som_->updctrl("mrs_bool/done", true);
  som_->tick();

  // write the trained som network and the feature normalization networks 
  ofstream oss;
  oss.open("som.mpl");
  oss << *som_ << endl;
  delete som_;


  ofstream noss;
  noss.open("norm.mpl");
  oss << *norm_ << endl;
  delete norm_;
  
}


void
MarGrid::setupPredict(QString fname)
{
  predictFname = fname;
}

void
MarGrid::predict()
{
  MarSystemManager mng;

  // read trained som network from file som.mpl and normalization network norm.mpl 
  ifstream iss;
  iss.open("som.mpl");
  som_ = mng.getMarSystem(iss);

  ifstream niss;
  niss.open("norm.mpl");
  norm_ = mng.getMarSystem(iss);

  resetPredict();
  cout << "Starting prediction" << endl;
  som_->updctrl("mrs_string/mode", "predict");  
  
  Collection l1;
  l1.read(predictFname.toStdString());
  cout << "Read collection" << endl;

  total_->updctrl("mrs_natural/pos", 0);  
  total_->updctrl("mrs_string/filename", predictFname.toStdString());    
  
  som_->updctrl("mrs_natural/inSamples", 1);

  realvec predict_res(som_->getctrl("mrs_natural/onObservations")->to<mrs_natural>(), 
		      som_->getctrl("mrs_natural/onSamples")->to<mrs_natural>());

  norm_->updctrl("mrs_natural/inSamples", 1);
  
  realvec som_in;
  realvec som_res;
  realvec norm_som_res;
  
  
  som_in.create(total_->getctrl("mrs_natural/inObservations")->to<mrs_natural>(), 
		total_->getctrl("mrs_natural/inSamples")->to<mrs_natural>());
  
  som_res.create(total_->getctrl("mrs_natural/onObservations")->to<mrs_natural>(), 
		 total_->getctrl("mrs_natural/onSamples")->to<mrs_natural>());

  norm_som_res.create(total_->getctrl("mrs_natural/onObservations")->to<mrs_natural>(), 
		      total_->getctrl("mrs_natural/onSamples")->to<mrs_natural>());

  
  for (int index = 0; index < l1.size(); index++)
    {
      total_->updctrl("mrs_natural/label", index);
      total_->updctrl("mrs_bool/memReset", true);
      total_->updctrl("mrs_natural/cindex", index);
      string current = total_->getctrl("mrs_string/currentlyPlaying")->to<mrs_string>();
      
      total_->process(som_in, som_res);
      norm_->process(som_res, norm_som_res);
      som_->process(norm_som_res, predict_res);
      grid_x = predict_res(0);
      grid_y = predict_res(1);
      addFile(grid_x,grid_y, current);      
      
      repaint();
    }

  cout << "end_prediction" << endl;

  
}



void MarGrid::clear()
{
    pieceLocations.clear();
    piecePixmaps.clear();
    pieceRects.clear();
    inPlace = 0;
    update();
}

void 
MarGrid::resetPredict()
{
  cout << "RESET PREDICT" << endl;
  
  for (int i=0; i < som_width; i++) 
    for (int j=0; j < som_height; j++) 
      {
	int k = i * som_height + j;
	files[k].clear();
	counterSizes[k] = 0;
	labels[k] = 0;
      }
  repaint();
  
}


void 
MarGrid::addFile(int grid_x, int grid_y, string filename)
{
  cout << "grid_x = " << grid_x << endl;
  cout << "grid_y = " << grid_y << endl;
  
  cout << "adding file " << filename << endl;
  
  int k = grid_x * som_height + grid_y;
  files[k].push_back(filename);
  counterSizes[k]++;
}

void MarGrid::mousePressEvent(QMouseEvent *event)
{
  

  cout << "mouse Press Event" << endl;
  
  grid_x = event->pos().x() / cell_size;
  grid_y = event->pos().y() / cell_size;
  cout << "grid_x = " << grid_x << endl;
  cout << "grid_y = " << grid_y << endl;
  
  
  int k = grid_x * som_height + grid_y;
  QList<string> posFiles = files[k];
  

  int counterSize = counterSizes[k];
  if (counterSize > 0) 
    counters[k] = (counters[k] + 1) % counterSize;  
  int counter = counters[k];
  
  cout << "*********" << endl;
  if (posFiles.size() != 0) 
    {
      cout << "Playing:" << posFiles[counter] << endl;
      emit playingFile(posFiles[counter].c_str());
      
      mwr_->updctrl(filePtr_, posFiles[counter]);
      mwr_->play();
      
    }
  else 
      mwr_->pause();
  
  cout << "Playlist: " << endl;
  for (int i=0; i < posFiles.size(); i++) 
    cout << posFiles[i] << endl;

  repaint();
  
}


void 
MarGrid::mouseMoveEvent(QMouseEvent* event)
{
  
  if ((event->pos().x() >= 600)|| (event->pos().y() >= 600))
    {
      return;
    }
  
  grid_x = event->pos().x() / cell_size;
  grid_y = event->pos().y() / cell_size;
  
  int k = grid_x * som_height + grid_y;
  QList<string> posFiles = files[k];
  
  int counter = counters[k];

  if (posFiles.size() != 0) 
    {
      cout << "*********" << endl;
      cout << "Playing: " << posFiles[counter] << endl;
      emit playingFile(posFiles[counter].c_str());
      mwr_->updctrl(filePtr_, posFiles[counter]);
      mwr_->play();
      
    }
   else 
     mwr_->pause();
  
  cout << "Playlist: " << endl;
  for (int i=0; i < posFiles.size(); i++) 
    cout << posFiles[i] << endl;
  
  repaint();
  
}




void 
MarGrid::paintEvent(QPaintEvent *event)
{
  QPainter painter;
  painter.begin(this);
  
  
  QRegExp qrp("classical+");
  QRegExp qrp1("metal+");
  QRegExp qrp2("hiphop+");
  QRegExp qrp3("blues+");
  QRegExp qrp4("jazz+");

  int maxDensity = 0;
  int minDensity = 10000;
  for (int i=0; i < files.size(); i++) {
    int count = files[i].size();

    if ( count > maxDensity ) {
      maxDensity = count;
    }
    if ( count < minDensity ) {
       minDensity = count;
    }
  }  

  for (int i=0; i < som_width; i++) 
    for (int j=0; j < som_height; j++) 
      {
	
	int k = i * som_height + j;
	
	QRect   myr(i*cell_size,j*cell_size,cell_size,cell_size);	
	QLine   myl1(i*cell_size,j*cell_size, i*cell_size, j*cell_size + cell_size);
	QLine   myl2(i*cell_size,j*cell_size, i*cell_size+cell_size, j*cell_size );
	

	QList<string> posFiles = files[k];

	QVector<int> labelvotes;
	labelvotes << 0 << 0 << 0 << 0 << 0;


	
	for (int i = 0; i < posFiles.size(); ++i) 
	  {
	    QString curFile(posFiles.at(i).c_str());
	    
	    if (qrp.indexIn(curFile) >=  0)
	      {
		labelvotes[0]++;
	      }
	    if (qrp1.indexIn(curFile) >= 0) 
	      {
		labelvotes[1]++;
	      }

	    if (qrp2.indexIn(curFile) >= 0) 
	      {
		labelvotes[2]++;
	      }


	    if (qrp3.indexIn(curFile) >= 0) 
	      {
		labelvotes[3]++;
	      }

	    if (qrp4.indexIn(curFile) >= 0) 
	      {
		labelvotes[4]++;
	      }
	    
	    
	  }

	int majority_label = 0;
	int max =0;
	
	for (int l=0; l < 5; l++) 
	  {
	    if (labelvotes[l] > max) 
	      {
		max = labelvotes[l];
		majority_label = l;
	      }
	  }
	
	if (max > 0) 
	  labels[k] = majority_label +1;
	
	
	/* if (labels[k] == 0) 
	  painter.setBrush(QColor("#ffffff"));
	else if (labels[k] == 1) 
	  painter.setBrush(QColor("#ffcccc"));
	else if (labels[k] == 2) 
	  painter.setBrush(QColor("#1111ff"));	  
	else if (labels[k] == 3) 
	  painter.setBrush(QColor("#cccccc"));
	else if (labels[k] == 4) 
	  painter.setBrush(QColor("#ccaacc"));	  
	else  
	  painter.setBrush(QColor("#fcaaac"));	  
	*/ 

	// For grey scale colouring
	if ( posFiles.size() > 0 ) {
          int color = 255 - int(posFiles.size() / float(maxDensity) * 255);
          painter.setBrush( QColor(color, color, color) );
	} else {
	  painter.setBrush(QColor("#ffffff"));	  
        }
	
	painter.setPen(Qt::NoPen);
	painter.drawRect(myr);
	
	painter.setPen(Qt::red);
	painter.drawLine(myl1);
	painter.drawLine(myl2);
	
	painter.setBrush(Qt::red);
	QRect newr(grid_x*cell_size+cell_size/4,grid_y*cell_size+cell_size/4,cell_size-cell_size/2,cell_size-cell_size/2);	
	painter.drawRect(newr);
	
	

	
      }
  
  painter.end();
}


