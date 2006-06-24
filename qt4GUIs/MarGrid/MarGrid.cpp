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
  setMinimumSize(400, 400);

  som_width = 40;
  som_height = 40;
  cell_size = 400 / som_width;
  
  for (int i=0; i < som_width; i++)
    for (int j=0; j < som_height; j++) 
      {
	QList<string> empty;
	files.push_back(empty);
	counters.push_back(0);
	counterSizes.push_back(0);
	labels.push_back(0);
      }
  
  

  pnet_ = mng.create("Series", "pnet_");
  pnet_->addMarSystem(mng.create("SoundFileSource", "src"));
  pnet_->addMarSystem(mng.create("Gain", "gain"));
  pnet_->addMarSystem(mng.create("AudioSink", "dest"));
  
  pnet_->linkctrl("mrs_bool/notEmpty", "SoundFileSource/src/mrs_bool/notEmpty");
  mwr_ = new MarSystemWrapper(pnet_);

  mwr_->start();
  


  // addFile(0,0, "/home/gtzan/data/sound/genres/classical/classical.00000.au");        
  // addFile(0,1, "/home/gtzan/data/sound/genres/metal/metal.00000.au");        
  // addFile(1,0, "/home/gtzan/data/sound/genres/hiphop/hiphop.00000.au");        
  // addFile(2,0, "/home/gtzan/data/sound/genres/jazz/jazz.00000.au");        
  // addFile(2,2, "/home/gtzan/data/sound/genres/blues/blues.00000.au");        
  // initSOM();
  
}




  

void 
MarGrid::extract()
{
  // EXTRACT FEATURES 
  MarSystem* extractNet = mng.create("Series", "extractNet");
  extractNet->addMarSystem(mng.create("SoundFileSource", "src"));
  
  MarSystem* spectralNet = mng.create("Series", "spectralNet");
  spectralNet->addMarSystem(mng.create("Hamming", "ham"));
  spectralNet->addMarSystem(mng.create("Spectrum", "spk"));
  spectralNet->addMarSystem(mng.create("PowerSpectrum", "pspk"));
  
  MarSystem* featureFanout = mng.create("Fanout", "featureFanout");
  featureFanout->addMarSystem(mng.create("Centroid", "centroid"));
  featureFanout->addMarSystem(mng.create("Rolloff", "rolloff"));
  
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
  // stats2->addMarSystem(mng.create("StandardDeviation", "std2"));

  total_->addMarSystem(stats2);
  total_->addMarSystem(mng.create("Annotator", "ann"));
  
  Collection l;
  l.read("music.mf");

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
  
  
  
  total_->updctrl("Accumulator/acc/Series/extractNet/SoundFileSource/src/mrs_natural/inSamples", 512);
  
  total_->updctrl("mrs_string/filename", "music.mf");
  total_->updctrl("mrs_real/repetitions", 1.0);
  
  int index= 0;


  realvec som_in;
  realvec som_res;
  realvec norm_som_res;
  
  realvec som_fmatrix;
  
  som_in.create(total_->getctrl("mrs_natural/inObservations").toNatural(), 
		total_->getctrl("mrs_natural/inSamples").toNatural());
  
  som_res.create(total_->getctrl("mrs_natural/onObservations").toNatural(), 
		 total_->getctrl("mrs_natural/onSamples").toNatural());

  norm_som_res.create(total_->getctrl("mrs_natural/onObservations").toNatural(), 
		      total_->getctrl("mrs_natural/onSamples").toNatural());

  som_fmatrix.create(total_->getctrl("mrs_natural/onObservations").toNatural(), 
		     l.size());

  norm_som_fmatrix.create(total_->getctrl("mrs_natural/onObservations").toNatural(), 
		     l.size());
  
  mrs_natural total_onObservations = total_->getctrl("mrs_natural/onObservations").toNatural();
  

  

  // calculate features 
  cout << "Calculating features" << endl;
  for (index=0; index < l.size(); index++)
    {
      total_->updctrl("mrs_natural/label", index);
      total_->updctrl("mrs_bool/memReset", true);
      total_->updctrl("mrs_natural/cindex", index);
      
      string current = total_->getctrl("mrs_string/currentlyPlaying").toString();
      cout << current  << " - ";
      
      cout << "Processed " << index << " files " << endl;	  
      total_->process(som_in,som_res);
      cout << som_res(0) << endl;
      cout << som_res(1) << endl;
      
      for (int o=0; o < total_onObservations; o++) 
	som_fmatrix(o, index) = som_res(o, 0);
    }
  
  norm_ = mng.create("NormMaxMin", "norm");
  norm_->updctrl("mrs_natural/inSamples", l.size());
  norm_->updctrl("mrs_natural/inObservations", 
		total_->getctrl("mrs_natural/onObservations").toNatural());
  norm_->updctrl("mrs_bool/train", true);
  norm_->process(som_fmatrix, norm_som_fmatrix);
  norm_->updctrl("mrs_bool/train", false);  
  norm_->process(som_fmatrix, norm_som_fmatrix);
  
  som_ = mng.create("SOM", "som");  
  som_->updctrl("mrs_natural/grid_width", som_width);
  som_->updctrl("mrs_natural/grid_height", som_height);
  som_->updctrl("mrs_natural/inSamples", norm_som_fmatrix.getCols());
  som_->updctrl("mrs_natural/inObservations", norm_som_fmatrix.getRows());
  
}



void
MarGrid::train()
{
  

  som_->updctrl("mrs_string/mode", "train");



  realvec som_fmatrixres;
  som_fmatrixres.create(som_->getctrl("mrs_natural/onObservations").toNatural(), 
			som_->getctrl("mrs_natural/onSamples").toNatural());
  
  cout << "Starting training" << endl;
  // cout << "som = " << *som_ << endl;
  



  
  for (int i=0; i < 200; i ++) 
    {
      norm_som_fmatrix.shuffle();
      som_->process(norm_som_fmatrix, som_fmatrixres);
    }
  
  
  cout << "Training done" << endl;
  som_->updctrl("mrs_bool/done", true);
  som_->tick();
  cout << *som_ << endl;
  
}



void
MarGrid::predict()

{
  resetPredict();
  
  
  cout << "Starting prediction" << endl;
  som_->updctrl("mrs_string/mode", "predict");  


  cout << "som = " << *som_ << endl;
  
  
  Collection l1;
  l1.read("test.mf");
  total_->updctrl("mrs_natural/pos", 0);
  total_->updctrl("mrs_string/filename", "test.mf");    
  
  som_->updctrl("mrs_natural/inSamples", 1);
  
  realvec predict_res(som_->getctrl("mrs_natural/onObservations").toNatural(), 
		      som_->getctrl("mrs_natural/onSamples").toNatural());
  

  norm_->updctrl("mrs_natural/inSamples", 1);


  realvec som_in;
  realvec som_res;
  realvec norm_som_res;
  
  
  som_in.create(total_->getctrl("mrs_natural/inObservations").toNatural(), 
		total_->getctrl("mrs_natural/inSamples").toNatural());
  
  som_res.create(total_->getctrl("mrs_natural/onObservations").toNatural(), 
		 total_->getctrl("mrs_natural/onSamples").toNatural());

  norm_som_res.create(total_->getctrl("mrs_natural/onObservations").toNatural(), 
		      total_->getctrl("mrs_natural/onSamples").toNatural());

  int grid_x, grid_y;
  
  for (int index = 0; index < l1.size(); index++)
    {
      total_->updctrl("mrs_natural/label", index);
      total_->updctrl("mrs_bool/memReset", true);
      total_->updctrl("mrs_natural/cindex", index);
      string current = total_->getctrl("mrs_string/currentlyPlaying").toString();
      
      total_->process(som_in, som_res);
      norm_->process(som_res, norm_som_res);
      som_->process(norm_som_res, predict_res);
      grid_x = predict_res(0);
      grid_y = predict_res(1);
      addFile(grid_x,grid_y, current);      
    }

  cout << "end_prediction" << endl;
  repaint();
  
}



void MarGrid::clear()
{
    pieceLocations.clear();
    piecePixmaps.clear();
    pieceRects.clear();
    highlightedRect = QRect();
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
  highlightedRect = targetSquare(event->pos());
  
  int grid_x = event->pos().x() / cell_size;
  int grid_y = event->pos().y() / cell_size;
  cout << "grid_x = " << grid_x << endl;
  cout << "grid_y = " << grid_y << endl;
  
  
  cout << "Starting to play" << endl;
  int k = grid_x * som_height + grid_y;
  cout << "calculated k " << endl;
  QList<string> posFiles = files[k];
  cout << "retrieved posFiles " << endl;
  
  int counter = counters[k];
  cout << "retrieved counters " << endl;
  int counterSize = counterSizes[k];
  cout << "retrieved countersizes " << endl;
  if (counterSize > 0) 
    counters[k] = (counters[k] + 1) % counterSize;
  
  

  if (posFiles.size() != 0) 
    {
      cout << "updating file" << posFiles[counter] << endl;
      
      mwr_->updctrl("SoundFileSource/src/mrs_string/filename", posFiles[counter]);
      mwr_->play();
    }
  
  for (int i=0; i < posFiles.size(); i++) 
    cout << "posFiles = " << posFiles[i] << endl;
  


  repaint();
  
  
}

void MarGrid::paintEvent(QPaintEvent *event)
{
  cout << "Paint event called" << endl;
  
  QPainter painter;
  painter.begin(this);
  
  cout << "starting the painting" << endl;
  
  QRegExp qrp("classical+");
  QRegExp qrp1("metal+");
  QRegExp qrp2("hiphop+");
  QRegExp qrp3("blues+");
  QRegExp qrp4("jazz+");

  
  

  for (int i=0; i < som_width; i++) 
    for (int j=0; j < som_height; j++) 
      {
	
	int k = i * som_height + j;
	
	QRect   myr(i*cell_size,j*cell_size,cell_size,cell_size);	
	

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
	
	
	if (labels[k] == 0) 
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

	
	painter.setPen(Qt::NoPen);
	painter.drawRect(myr);

	
      }
  cout << "Out of loop " << endl;
  
  
  
  painter.end();
}



const QRect MarGrid::targetSquare(const QPoint &position) const
{
    return QRect(position.x()/40 * 40, position.y()/40 * 40, 40, 40);
}
