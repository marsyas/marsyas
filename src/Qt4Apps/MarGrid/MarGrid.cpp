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
#include <sstream>

using namespace std;
using namespace Marsyas;

MarGrid::MarGrid(QWidget *parent)
  : QWidget(parent)
{
  setAcceptDrops(true);
  setMouseTracking(true);

  int winWidth, winHeight;

  cell_size = 50;
  som_width = 12;
  som_height = 12;
  initAudio_ = false;
  continuous_ = false;
  blackwhite_ = false;

  winWidth = cell_size * som_width;
  winHeight = cell_size * som_height;

  setMinimumSize(winWidth, winHeight);
  resetFilesVec(som_height, som_width);

  // Create playback network
  pnet_ = mng.create("Series", "pnet_");
  pnet_->addMarSystem(mng.create("SoundFileSource", "src"));
  // pnet_->addMarSystem(mng.create("Stereo2Mono", "s2m"));
  pnet_->addMarSystem(mng.create("Gain", "gain"));
  pnet_->addMarSystem(mng.create("AudioSink", "dest"));
  pnet_->linkControl("mrs_bool/hasData","SoundFileSource/src/mrs_bool/hasData");


  mwr_ = new MarSystemQtWrapper(pnet_);
  filePtr_ = mwr_->getctrl("SoundFileSource/src/mrs_string/filename");

  mwr_->start();
  setupTrain("margrid_train.mf");
}


void
MarGrid::setPlaybackMode(bool continuous)
{
  continuous_ = continuous;
}

void
MarGrid::setBlackWhiteMode(bool blackwhite)
{
  blackwhite_ = blackwhite;
  repaint();
}


void
MarGrid::setupTrain(QString fname)
{
  // Build network for feature extraction
  MarSystem* extractNet = mng.create("Series", "extractNet");
  extractNet->addMarSystem(mng.create("SoundFileSource", "src"));
  extractNet->addMarSystem(mng.create("Stereo2Mono", "s2m"));
  // extractNet->addMarSystem(mng.create("AudioSink", "dest"));

  MarSystem* spectralNet = mng.create("Series", "spectralNet");
  spectralNet->addMarSystem(mng.create("Windowing", "ham"));
  spectralNet->addMarSystem(mng.create("Spectrum", "spk"));
  spectralNet->addMarSystem(mng.create("PowerSpectrum", "pspk"));

  MarSystem* featureFanout = mng.create("Fanout", "featureFanout");
  featureFanout->addMarSystem(mng.create("Centroid", "centroid"));
  featureFanout->addMarSystem(mng.create("Rolloff", "rolloff"));
  featureFanout->addMarSystem(mng.create("Flux", "flux"));
  featureFanout->addMarSystem(mng.create("MFCC", "mfcc"));

  spectralNet->addMarSystem(featureFanout);
  extractNet->addMarSystem(spectralNet);
  extractNet->addMarSystem(mng.create("Memory", "mem"));

  MarSystem* stats = mng.create("Fanout", "stats");
  stats->addMarSystem(mng.create("Mean", "mn1"));
  stats->addMarSystem(mng.create("StandardDeviation", "std1"));
  extractNet->addMarSystem(stats);

  MarSystem* acc = mng.create("Accumulator", "acc");
  acc->updControl("mrs_natural/nTimes", 1200);
  acc->addMarSystem(extractNet);

  total_ = mng.create("Series", "total");
  total_->addMarSystem(acc);
  MarSystem* stats2 = mng.create("Fanout", "stats2");
  stats2->addMarSystem(mng.create("Mean", "mn2"));
  stats2->addMarSystem(mng.create("StandardDeviation", "std2"));

  total_->addMarSystem(stats2);
  total_->addMarSystem(mng.create("Annotator", "ann"));

  // total_->updControl("Accumulator/acc/Series/extractNet/AudioSink/dest/mrs_bool/initAudio", true);


  total_->linkControl("mrs_string/filename",
                      "Accumulator/acc/Series/extractNet/SoundFileSource/src/mrs_string/filename");


  total_->linkControl("mrs_string/currentlyPlaying",
                      "Accumulator/acc/Series/extractNet/SoundFileSource/src/mrs_string/currentlyPlaying");


  total_->linkControl("mrs_bool/shuffle",
                      "Accumulator/acc/Series/extractNet/SoundFileSource/src/mrs_bool/shuffle");

  total_->linkControl("mrs_natural/pos",
                      "Accumulator/acc/Series/extractNet/SoundFileSource/src/mrs_natural/pos");

  total_->linkControl("mrs_real/repetitions",
                      "Accumulator/acc/Series/extractNet/SoundFileSource/src/mrs_real/repetitions");


  total_->linkControl("mrs_natural/cindex",
                      "Accumulator/acc/Series/extractNet/SoundFileSource/src/mrs_natural/cindex");

  total_->linkControl("mrs_natural/numFiles",
                      "Accumulator/acc/Series/extractNet/SoundFileSource/src/mrs_natural/numFiles");

  total_->linkControl("mrs_string/allfilenames",
                      "Accumulator/acc/Series/extractNet/SoundFileSource/src/mrs_string/allfilenames");

  total_->linkControl("mrs_natural/numFiles",
                      "Accumulator/acc/Series/extractNet/SoundFileSource/src/mrs_natural/numFiles");


  total_->linkControl("mrs_bool/hasData",
                      "Accumulator/acc/Series/extractNet/SoundFileSource/src/mrs_bool/hasData");
  total_->linkControl("mrs_natural/advance",
                      "Accumulator/acc/Series/extractNet/SoundFileSource/src/mrs_natural/advance");

  total_->linkControl("mrs_bool/memReset",
                      "Accumulator/acc/Series/extractNet/Memory/mem/mrs_bool/reset");

  total_->linkControl("mrs_natural/label",
                      "Annotator/ann/mrs_natural/label");



  total_->updControl("mrs_natural/inSamples", 512);


  trainFname = fname;
  predictFname = "margrid_train.mf";
  total_->updControl("mrs_string/filename", trainFname.toStdString());
  total_->updControl("mrs_real/repetitions", 1.0);


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

  som_fmatrix.create(total_onObservations+2,
                     numFiles);

  // calculate features
  cout << "Calculating features" << endl;



  for (index=0; index < numFiles; index++)
  {
    total_->updControl("mrs_natural/label", index);
    total_->updControl("mrs_bool/memReset", true);
    total_->updControl("mrs_natural/cindex", index);



    total_->process(som_in,som_res);
    string current = total_->getctrl("mrs_string/currentlyPlaying")->to<mrs_string>();

    cout << current  << " - ";
    cout << "Processed " << index << " files " << endl;

    for (int o=0; o < total_onObservations; o++)
      som_fmatrix(o, index) = som_res(o, 0);

    // this is where the initialization locations should be provided
    for (int o=total_onObservations; o< total_onObservations+2; o++)
    {
      /* if ((index > 120)&&(index <140))
         som_fmatrix(o, index) = 0.0;
         else
      */
      som_fmatrix(o, index) = -1.0;
    }

    total_->updControl("mrs_natural/advance", 1);
  }

  ofstream oss;
  oss.open("som_fmatrix.txt");
  oss << som_fmatrix << endl;
  oss.close();
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
  norm_->updControl("mrs_natural/inSamples", train_som_fmatrix.getCols());
  norm_->updControl("mrs_natural/inObservations", train_som_fmatrix.getRows());
  norm_->updControl("mrs_natural/ignoreLast", 3);
  norm_->updControl("mrs_string/mode", "train");
  norm_->process(train_som_fmatrix, norm_som_fmatrix);
  norm_->updControl("mrs_string/mode", "predict");
  norm_->process(train_som_fmatrix, norm_som_fmatrix);



  // Create netork for training the self-organizing map
  som_ = mng.create("SOM", "som");
  som_->updControl("mrs_natural/grid_width", som_width);
  som_->updControl("mrs_natural/grid_height", som_height);
  som_->updControl("mrs_natural/inSamples", norm_som_fmatrix.getCols());
  som_->updControl("mrs_natural/inObservations", norm_som_fmatrix.getRows());
  som_->updControl("mrs_string/mode", "train");

  realvec som_fmatrixres;
  som_fmatrixres.create(som_->getctrl("mrs_natural/onObservations")->to<mrs_natural>(),
                        som_->getctrl("mrs_natural/onSamples")->to<mrs_natural>());

  cout << "Starting training" << endl;

  for (int i=0; i < 200; i ++)
  {
    cout << "Training iteration" << i << endl;
    norm_som_fmatrix.shuffle();
    som_->process(norm_som_fmatrix, som_fmatrixres);
  }

  cout << "Training done" << endl;
  som_->updControl("mrs_bool/done", true);
  som_->tick();

  // write the trained som network and the feature normalization networks
  ofstream oss;
  oss.open("som.mpl");
  oss << *som_ << endl;
  delete som_;

  ofstream noss;
  noss.open("norm.mpl");
  noss << *norm_ << endl;
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
  norm_ = mng.getMarSystem(niss);

  resetPredict();
  cout << "Starting prediction" << endl;
  som_->updControl("mrs_string/mode", "predict");

  Collection l1;
  l1.read(predictFname.toStdString());
  cout << "Read collection" << endl;

  total_->updControl("mrs_natural/pos", 0);
  total_->updControl("mrs_natural/advance", 0);
  total_->updControl("mrs_string/filename", predictFname.toStdString());

  som_->updControl("mrs_natural/inSamples", 1);

  realvec predict_res(som_->getctrl("mrs_natural/onObservations")->to<mrs_natural>(),
                      som_->getctrl("mrs_natural/onSamples")->to<mrs_natural>());
  norm_->updControl("mrs_natural/inSamples", 1);


  realvec som_in;
  realvec som_res;
  realvec som_res1;

  realvec norm_som_res;

  mrs_natural inObs = total_->getctrl("mrs_natural/inObservations")->to<mrs_natural>();
  mrs_natural inSms = total_->getctrl("mrs_natural/inSamples")->to<mrs_natural>();

  mrs_natural onObs = total_->getctrl("mrs_natural/onObservations")->to<mrs_natural>();
  mrs_natural onSms = total_->getctrl("mrs_natural/onSamples")->to<mrs_natural>();

  som_in.create(inObs, inSms);
  som_res.create(onObs, onSms);

  som_res1.create(onObs+2, onSms);

  norm_som_res.create(onObs+2, onSms);

  for (unsigned int index = 0; index < l1.size(); index++)
  {
    total_->updControl("mrs_natural/label", index);
    total_->updControl("mrs_bool/memReset", true);
    total_->updControl("mrs_natural/cindex", index);


    total_->process(som_in, som_res);
    string current = total_->getctrl("mrs_string/currentlyPlaying")->to<mrs_string>();


    cout << "CURRENT = " << current << endl;

    for (int o=0; o < onObs; o++)
      som_res1(o, 0) = som_res(o, 0);

    norm_->process(som_res1, norm_som_res);
    som_->process(norm_som_res, predict_res);



    grid_x = predict_res(0);
    grid_y = predict_res(1);
    addFile(grid_x,grid_y, current);
    repaint();
    total_->updControl("mrs_natural/advance", 1);
  }



  cout << "end_prediction" << endl;


}



void
MarGrid::clear()
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
MarGrid::resetFilesVec(int height, int width)
{
  for (int i=0; i < width; i++)
  {
    for (int j=0; j < height; j++)
    {
      QList<string> empty;
      files.push_back(empty);
      counters.push_back(0);
      counterSizes.push_back(0);
      labels.push_back(0);
    }
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

void
MarGrid::mousePressEvent(QMouseEvent *event)
{

  if ((event->pos().x() >= som_width * cell_size)|| (event->pos().y() >= som_height * cell_size))
  {
    return;
  }
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
    if (initAudio_ == false)
    {
      mwr_->updctrl("AudioSink/dest/mrs_bool/initAudio", true);
      initAudio_ = true;
    }

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

  if ((event->pos().x() >= som_width * cell_size)|| (event->pos().y() >= som_height * cell_size))
  {
    return;
  }

  grid_x = event->pos().x() / cell_size;
  grid_y = event->pos().y() / cell_size;

  int k = grid_x * som_height + grid_y;
  QList<string> posFiles = files[k];

  int counter = counters[k];

  if (continuous_)
  {

    if (posFiles.size() != 0)
    {
      cout << "*********" << endl;
      cout << "Playing: " << posFiles[counter] << endl;
      emit playingFile(posFiles[counter].c_str());
      mwr_->updctrl(filePtr_, posFiles[counter]);

      if (initAudio_ == false)
      {
        mwr_->updctrl("AudioSink/dest/mrs_bool/initAudio", true);
        initAudio_ = true;
      }

      mwr_->play();
    }
    else
      mwr_->pause();

    cout << "Playlist: " << endl;
    for (int i=0; i < posFiles.size(); i++)
      cout << posFiles[i] << endl;
  }

  repaint();

}

void
MarGrid::setXGridSize(QString size)
{
  som_width = size.toInt();
  resetFilesVec(som_height, som_width);
  clear();
}

void
MarGrid::setYGridSize(QString size)
{
  som_height = size.toInt();
  resetFilesVec(som_height, som_width);
  clear();
}

void
MarGrid::openPredictionGrid(QString fname)
{
  fstream file_ip((fname.toStdString()).c_str(), ios::in);
  if (!file_ip) {
    cerr << "Can't open input file " << endl;
    return;
  }

  resetPredict();

  int vecSize = 0;
  int index = 0;
  int splitIndex = 0;
  string numLines = "";
  string line = "";
  string listFname = "";
  string vecIndex = "";

  cout << "doing file stuff" << endl;

  // get the stored size of the vector
  getline(file_ip,numLines);
  istringstream intBuffer(numLines);
  intBuffer >> vecSize;

  while(!file_ip.eof())
  {
    getline(file_ip,line);
    splitIndex = line.find_first_of(',');
    vecIndex = line.substr(0,splitIndex);
    listFname = line.substr(splitIndex + 1);

    istringstream buffer(vecIndex);
    buffer >> index;
    files[index] += listFname;
    counterSizes[index]++;
  }
}

void
MarGrid::savePredictionGrid(QString fname)
{
  fstream file_op((fname.toStdString()).c_str(), ios::out);

  //first line is the size of files
  file_op << files.size() << endl;

  for (int i = 0; i < files.size(); i++)
  {
    // output the vector index and filename
    QList<std::string> temp = files[i];
    for(int j = 0; j < files[i].size(); j++ )
    {
      file_op << i;
      file_op << "," + temp.takeFirst() << endl;

    }
  }
  file_op.close();


}


void
MarGrid::paintEvent(QPaintEvent *event)
{
  QPainter painter;
  painter.begin(this);
  cell_size = this->width() / som_width-2;

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

  for (int i=0; i <= som_width; i++)
  {
    QLine vertical(i*cell_size, 0, i * cell_size, som_height * cell_size);
    painter.setPen(Qt::red);
    painter.drawLine(vertical);
  }


  for (int j=0; j <= som_width; j++)
  {
    QLine vertical(0, j*cell_size, som_width * cell_size, j * cell_size);
    painter.setPen(Qt::red);
    painter.drawLine(vertical);
  }



  for (int i=0; i < som_width; i++)
    for (int j=0; j < som_height; j++)
    {
      int k = i * som_height + j;

      QRect   myr(i*cell_size,j*cell_size,cell_size,cell_size);
      QList<string> posFiles = files[k];


      // For grey scale colouring
      if ( posFiles.size() > 0 ) {
        if (blackwhite_) {
          painter.setBrush(QColor("#000000"));
        } else {
          int color = 255 - int(posFiles.size() / float(maxDensity) * 255);
          painter.setBrush( QColor(color, color, color) );
        }
      }
      else
      {
        painter.setBrush(QColor("#ffffff"));
      }

      QRect fillRect(i*cell_size+1, j*cell_size+1, cell_size-1,cell_size-1);

      painter.setPen(Qt::NoPen);
      painter.drawRect(fillRect);


      painter.setBrush(Qt::red);
      QRect selectRect(grid_x*cell_size+cell_size/4,grid_y*cell_size+cell_size/4,
                       cell_size-cell_size/2,cell_size-cell_size/2);
      painter.drawRect(selectRect);
    }

  painter.end();
}


