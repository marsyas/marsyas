/*
** Copyright (C) 2000-2013 George Tzanetakis <gtzan@cs.uvic.ca>
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

#include "Grid.h"

Grid::Grid()
{
  _collection = MusicCollection::getInstance();

  som_ = NULL;
  norm_ = NULL;

  som_height = 12;
  som_width = 12;
  oldWidth_ = som_width;
  oldHeight_ = som_height;
  _gridX = 0;
  _gridY = 0;
  _gridX1 = -1;
  _gridY1 = -1;

  init_alpha_ = 0.98;
  init_neighbourhood_ = 0.97;
  init_std_factor_ = 0.17;
  init_iterations_ = 10;

  train_alpha_ = 0.98;
  train_neighbourhood_ = 0.97;
  train_std_factor_ = 0.17;
  train_iterations_ = 100;

  continuous_ = false;
  cancel_ = false;
  state_ = 0;
  init_ = false;
  featureHash = new multimap<string, realvec>();
  normFeatureHash = new multimap<string, realvec>();
  numFeatures = 0;
  initFileLocations.resize(som_height * som_width);
  playlist_ = "Library"; // by default select the full iTunes library

  // sness
  trainFile_ = "";
  // Some objects to receive OSC messages
  x0_ = new OSCReceiver();
  y0_ = new OSCReceiver();
  x1_ = new OSCReceiver();
  y1_ = new OSCReceiver();


  /* For use with genres only
  * The 1st dimension is the normal grid square array layout
  * The 2nd dimension of density uses the following values
  * 0 - blues
  * 1 - classical
  * 2 - country
  * 3 - disco
  * 4 - hiphop
  * 5 - jazz
  * 6 - metal
  * 7 - reggae
  * 8 - rock
  * 9 - pop
  */
  genreDensity = (int **)malloc(som_height * som_width * sizeof(int *));
  for (int i = 0; i < som_height * som_width; i++)
  {
    genreDensity[i] = (int *)malloc(10 * sizeof(int));
    for(int j = 0; j < 10; j++)
    {
      genreDensity[i][j] = 0;
    }
  }

  setup();
}

Grid::~Grid() {

  //TODO: DROP files_ POINTER
  for(int i =0; i < som_height * som_width; i++)
    free(genreDensity[i]);
  free(genreDensity);
}

void Grid::clear() {
  //TODO: WRITE CLEAR FUNCTION
}

void Grid::run()
{
  mutex.lock();
  while(true)
  {
    cout << "Thread running"  << endl;
    buttonPressed.wait(&mutex);
    switch(state_) {
    case 1:
      extract();
      clearMode();
      break;
    case 2:
      train();
      clearMode();
      break;
    case 3:
      predict();
      clearMode();
      break;
    case 4:
      init();
      clearMode();
      break;
    case 0:
      break;
    }
  }
  mutex.unlock();
}

void Grid::setup() {
  cout << "Setup " << endl;

  _cellSize = 50;	//size of cell in pixes

  QList<std::string> *ql;
  for(int i=0; i < som_height; i++) {
    for(int j=0; j < som_width; j++) {
      ql = new QList<std::string>();
      files_.push_back(*ql);
      counters.push_back(0);
      counterSizes.push_back(0);
      labels.push_back(0);
    }
  }
  setupNetworks();
}



void Grid::setupNetworks() {


  MarSystem* extractNet = mng.create("Series", "extractNet");
  extractNet->addMarSystem(mng.create("SoundFileSource", "src"));
  extractNet->addMarSystem(mng.create("Stereo2Mono", "s2m"));
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
  total_->addMarSystem(mng.create("WekaSink", "wsink"));





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

  total_->linkControl("WekaSink/wsink/mrs_natural/nLabels",
                      "Accumulator/acc/Series/extractNet/SoundFileSource/src/mrs_natural/numFiles");
  total_->updControl("WekaSink/wsink/mrs_natural/downsample", 1);


  total_->linkControl("WekaSink/wsink/mrs_string/labelNames",
                      "Accumulator/acc/Series/extractNet/SoundFileSource/src/mrs_string/allfilenames");

  total_->updControl("mrs_natural/inSamples", 512);
  total_->updControl("mrs_real/repetitions", 1.0);

  // Playback network
  MarSystem *pnet;
  pnet = mng.create("Series", "pnet_");
  pnet->addMarSystem(mng.create("SoundFileSource", "src"));
  // pnet_->addMarSystem(mng.create("Stereo2Mono", "s2m"));
  pnet->addMarSystem(mng.create("Gain", "gain"));
  pnet->addMarSystem(mng.create("AudioSink", "dest"));
  pnet->linkControl("mrs_bool/hasData","SoundFileSource/src/mrs_bool/hasData");

  m_playMarSystem = pnet;
  m_playSystem = new MarsyasQt::System(pnet);

  m_fileNameControl = m_playSystem->control("SoundFileSource/src/mrs_string/filename");
  m_initAudioControl = m_playSystem->control("AudioSink/dest/mrs_bool/initAudio");
}

/*
* ---------------------------------------------------
* Slots
* ---------------------------------------------------
*/
void Grid::extract() {
  // sness
  if ( _collection->getNumTracks() > 0 ) {
    // !!! use itunes to generate the collection file rather then using a file
    ofstream featureFile;
    featureFile.open("margrid_train.mf", std::ios::out | std::ios::binary);
    //featureFile.open("music.mf");
    _collection->generatePlayList(featureFile, playlist_.c_str());
    extractAction("margrid_train.mf");
  } else if (trainFile_.length() > 0) {
    cout << "Grid::extract() :: trainFile_.length() > 0" << endl;
    cout << "trainFile_=" << trainFile_ << endl;
    extractAction(trainFile_);
  } else
  {
    emit errorBox("Need to load a collection of audio files first!");
  }
}
void Grid::extractAction(std::string filename)
{
  total_->updControl("mrs_string/filename", filename);
  total_->updControl("WekaSink/wsink/mrs_string/filename", "margrid2.arff");

  int numFiles = total_->getctrl("mrs_natural/numFiles")->to<mrs_natural>();
  realvec som_in;
  realvec som_res;
  realvec som_fmatrix;
  multimap<std::string, realvec>::iterator it;


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
  for (int index=0; index < numFiles; index++)
  {
    if(cancel_)
    {
      cancel_ = false;
      return;
    }
    total_->updControl("mrs_natural/label", index);
    total_->updControl("mrs_bool/memReset", true);
    total_->updControl("mrs_natural/cindex", index);

    total_->process(som_in,som_res);
    string current = total_->getctrl("mrs_string/currentlyPlaying")->to<mrs_string>();
    cout << "Processed " << index + 1 << " file of "<<numFiles<<" files" << endl;
    cout << current  << "\n";
    // hash the resulting feature on file name
    if(featureHash->find(current) == featureHash->end()) {
      featureHash->insert(pair<string,realvec>(current,som_res));
    }

    for (int o=0; o < total_onObservations; o++)
      som_fmatrix(o, index) = som_res(o, 0);

    for (int o=total_onObservations; o< total_onObservations+2; o++)
    {
      som_fmatrix(o, index) = -1.0;
    }

    total_->updControl("mrs_natural/advance", 1);
  }

  ofstream oss;
  oss.open("som_fmatrix.txt");
  oss << som_fmatrix << endl;
  oss.close();

  // normalize and create hashmap of normalized feature vectors on their file name

  realvec norm_som_fmatrix;

  // Normalize the feature matrix so that all features are between 0 and 1
  norm_som_fmatrix.create(som_fmatrix.getRows(),
                          som_fmatrix.getCols());
  norm_ = mng.create("NormMaxMin", "norm");
  norm_->updControl("mrs_natural/inSamples", som_fmatrix.getCols());
  norm_->updControl("mrs_natural/inObservations",  som_fmatrix.getRows());
  norm_->updControl("mrs_natural/ignoreLast", 3);
  norm_->updControl("mrs_string/mode", "train");
  norm_->process(som_fmatrix, norm_som_fmatrix);
  norm_->updControl("mrs_string/mode", "predict");

  norm_->process(som_fmatrix, norm_som_fmatrix);


  numFeatures = norm_->getctrl("mrs_natural/onObservations")->to<mrs_natural>();

  // make the hashmap of filename and normalized feature vector
  ifstream inFeatureName;
  std::string featureName = "";
  int counter = 0;
  realvec* normFeature = new realvec();

  inFeatureName.open(filename.c_str());
  getline(inFeatureName, featureName);
  while(inFeatureName)
  {
    norm_som_fmatrix.getCol(counter, *normFeature);
    if(normFeatureHash->find(featureName) == normFeatureHash->end())
      normFeatureHash->insert(pair<string,realvec>(featureName, *normFeature));
    getline(inFeatureName, featureName);
    counter++;
  }


  oss.open("norm_som_fmatrix.txt");
  oss << norm_som_fmatrix << endl;
  oss.close();

  ofstream noss;
  noss.open("norm.mpl");
  noss << *norm_ << endl;
  noss.close();
  delete norm_;

  /*
  Save the feature hash
  Each realvec is a separate file as it is the only way they can be loaded
  each file is named in the following pattern "featureVec" where X is an integer
  The file keys.txt holds space separated pairs of the hash key
  and the filename of the corresponding realvec

  NOTE: QT is used for platform independence.
  */

  QDir dir;
  QFile *keys;
  QString keysFile;
  QString realvecFile;

  if(!dir.exists("extractFiles"))
  {
    dir.mkdir("extractFiles");
  }
  dir.setCurrent("extractFiles");
  keysFile = dir.filePath("keys.txt");
  keys = new QFile(keysFile);
  if(keys->open(QIODevice::Truncate | QIODevice::ReadWrite ))
  {
    QTextStream txtStr(keys);
    int counter = 0;
    for ( it=normFeatureHash->begin() ; it != normFeatureHash->end(); it++ )
    {
      // make the key entry
      txtStr << it->first.c_str();
      txtStr << " ";
      txtStr << counter << endl;

      // open the realvec file and write the value
      std::stringstream stringStream;
      stringStream << "featureVec" << counter << ".hsh";
      realvecFile = stringStream.str().c_str();
      realvecFile = dir.filePath( realvecFile );
      it->second.write( realvecFile.toStdString() );

      counter++;
    }
    keys->close();
    delete keys;
    dir.setCurrent("..");
  }
  else
  {
    cerr << "Could not open keys.txt for writing";
  }

}
/*
** Overloaded training function to provide backwards compatability
*/
void Grid::train() {
  train(false);
}

void Grid::train(bool skipTraining) {
  cout<<"********Grid::train init_: "<<init_<<endl;
  if ( _collection->getNumTracks() == 0 && trainFile_.length() == 0) {
    emit errorBox("Need to load a collection of audio files first!");
    return;
  }

  // Read the feature matrix from file som_fmatrix.txt
  realvec norm_som_fmatrix;
  ifstream iss;
  iss.open("norm_som_fmatrix.txt");
  iss >> norm_som_fmatrix;

  // Skip the SOM manipulation.
  if(!skipTraining)
  {
    if(init_)
    {

      ifstream iss1;
      iss1.open("som.mpl");
      som_ = mng.getMarSystem(iss1);
      iss1.close();
      som_->updControl("mrs_natural/inSamples", norm_som_fmatrix.getCols());

    }
    else
    {
      // Create netork for training the self-organizing map

      som_ = mng.create("SOM", "som");
      som_->updControl("mrs_natural/grid_width", som_width);
      som_->updControl("mrs_natural/grid_height", som_height);
      som_->updControl("mrs_natural/inSamples", norm_som_fmatrix.getCols());
      som_->updControl("mrs_natural/inObservations", norm_som_fmatrix.getRows());
    }

    som_->updControl("mrs_real/alpha_decay_train", train_alpha_);
    som_->updControl("mrs_real/neighbourhood_decay_train", train_neighbourhood_);
    som_->updControl("mrs_real/std_factor_train", train_std_factor_);
    som_->updControl("mrs_string/mode", "train");


    realvec som_fmatrixres;
    som_fmatrixres.create(som_->getctrl("mrs_natural/onObservations")->to<mrs_natural>(),
                          som_->getctrl("mrs_natural/onSamples")->to<mrs_natural>());

    ofstream oss1;
    oss1.open("som2.mpl");
    oss1 << *som_;
    oss1.close();

    cout << "Starting training" << endl;


    bool done_ = false;
    for (int i=0; i < train_iterations_; i ++)
    {
      if(cancel_)
      {
        cancel_ = false;
        done_ = false;
        break;
      }
      cout << "Training iteration" << i << endl;
      norm_som_fmatrix.shuffle();
      som_->process(norm_som_fmatrix, som_fmatrixres);
      cout<<"x:"<<som_fmatrixres(0,0)<<"y:"<<som_fmatrixres(1,0)<<endl;
      done_ = true;
    }

    oss1.open("som3.mpl");
    oss1 << *som_ << endl;
    oss1.close();

    som_->updControl("mrs_bool/done", done_);
    som_->tick();
    cout << "Training done" << endl;

    // write the trained som network and the feature normalization networks
    oss1.open("som.mpl");
    oss1 << *som_ << endl;
    delete som_;
  }




}
void Grid::predict() {
  std::string fileName;

  if ( _collection->getNumTracks() > 0 ) {
    fileName = "margrid_train.mf";
  } else if (trainFile_.length() > 0) {
    fileName = trainFile_;
  } else {
    emit errorBox("Need to load a collection of audio files first!");
    return;
  }

  realvec som_in;
  realvec som_res;
  realvec som_res1;

  realvec som_fmatrix;
  QDir dir;

  ifstream iss1;

  iss1.open("som.mpl");
  som_ = mng.getMarSystem(iss1);
  iss1.close();

  ifstream niss1;
  niss1.open("norm.mpl");
  norm_ = mng.getMarSystem(niss1);
  niss1.close();



  cout << "Starting prediction" << endl;
  som_->updControl("mrs_string/mode", "predict");

  Collection l1;
  l1.read(fileName);

  cout << "Read collection" << endl;

  total_->updControl("mrs_string/filename", fileName);

  total_->updControl("mrs_natural/pos", 0);

  som_->updControl("mrs_natural/inSamples", 1);


  realvec predict_res(som_->getctrl("mrs_natural/onObservations")->to<mrs_natural>(),
                      som_->getctrl("mrs_natural/onSamples")->to<mrs_natural>());
  norm_->updControl("mrs_natural/inSamples", 1);



  realvec norm_som_res;


  mrs_natural inObs = total_->getctrl("mrs_natural/inObservations")->to<mrs_natural>();
  mrs_natural inSms = total_->getctrl("mrs_natural/inSamples")->to<mrs_natural>();

  mrs_natural onObs = total_->getctrl("mrs_natural/onObservations")->to<mrs_natural>();
  mrs_natural onSms = total_->getctrl("mrs_natural/onSamples")->to<mrs_natural>();

  som_in.create(inObs, inSms);
  som_res.create(onObs, onSms);
  som_res1.create(onObs+2, onSms);
  norm_som_res.create(onObs+2, onSms);



  ofstream oss1;
  oss1.open("som4.mpl");
  oss1 << *som_ << endl;
  oss1.close();


  realvec norm_som_fmatrix;
  ifstream iss;
  iss.open("norm_som_fmatrix.txt");
  iss >> norm_som_fmatrix;


  cout << l1.size() <<endl;

  QString tempString;
  for (int index = 0; index < l1.size(); index++)
  {
    if(cancel_)
    {
      cancel_ = false;
      break;
    }

    total_->updControl("mrs_natural/label", index);
    total_->updControl("mrs_bool/memReset", true);
    total_->updControl("mrs_natural/cindex", index);


    total_->process(som_in, som_res);
    for (int o=0; o < onObs; o++)
      som_res1(o, 0) = som_res(o, 0);
    QString current = total_->getctrl("mrs_string/currentlyPlaying")->to<mrs_string>().c_str();
    cout << total_->getctrl("mrs_string/currentlyPlaying")->to<mrs_string>() << endl;

    norm_->process(som_res1, norm_som_res);

    realvec foobar;
    foobar.create(som_->getctrl("mrs_natural/inObservations")->to<mrs_natural>(), som_->getctrl("mrs_natural/inSamples")->to<mrs_natural>());

    norm_som_fmatrix.getCol(index, foobar);

    som_->process(foobar, predict_res);

    grid_x = predict_res(0);
    grid_y = predict_res(1);
    addTrack(grid_x, grid_y, current);
    emit repaintSignal();


    total_->updControl("mrs_natural/advance", 1);
  }


  cout << "end_prediction" << endl;
}


/*
Init works by extracting the dropped files, then trains the grid with them.
The other files are then extracted and prediction is started.
*/
void Grid::init()
{
  cout << "starting init" << endl;
  if (initFileLocations.size() != 0)
  {

    realvec* init_train_fmatrix = new realvec();


    // make music.mf file of dropped files
    for(int i = 0; i < initFileLocations.size(); i++)
    {
      for (int l = 0; l < initFileLocations[i].size(); l++)
      {
        realvec temp;
        multimap<std::string, realvec>::iterator temp2 = normFeatureHash->find( initFileLocations[i].at(l)->getFileName() );

        //check that temp has something, if it doesn't display error and abort
        if(temp2 ==  normFeatureHash->end())
        {
          std::stringstream strStm;
          strStm << initFileLocations[i].at(l)->getFileName() << " was not extracted, please run Extract again";
          emit errorBox(strStm.str().c_str());
          return;
        }
        temp = temp2->second;
        //62 rows and cols equal to number of files...
        init_train_fmatrix->stretch( temp.getRows() + 2, init_train_fmatrix->getCols() + temp.getCols() );

        for(int j = 0; j < temp.getRows(); j++)
        {
          //copy features over from temp into init_train_matrix cell by cell
          (*init_train_fmatrix)(j, init_train_fmatrix->getCols() -1  ) = temp(j,0);
        }

        // add X and Y position info to the last two rows of the vector
        (*init_train_fmatrix)(temp.getRows(), init_train_fmatrix->getCols() -1 ) = initFileLocations[i].at(l)->getX();
        (*init_train_fmatrix)(temp.getRows() + 1, init_train_fmatrix->getCols() -1 ) = initFileLocations[i].at(l)->getY();
      }


    }

    ofstream oss1;
    oss1.open("init_train_fmatrix.mpl");
    oss1 << *init_train_fmatrix << endl;
    oss1.close();

    init_ = true;

    som_ = mng.create("SOM", "som");
    som_->updControl("mrs_natural/grid_width", som_width);
    som_->updControl("mrs_natural/grid_height", som_height);
    som_->updControl("mrs_natural/inSamples", init_train_fmatrix->getCols());//number of files

    som_->updControl("mrs_natural/inObservations", init_train_fmatrix->getRows()); //calls update() in SOM.cpp
    som_->updControl("mrs_string/mode", "init");
    som_->updControl("mrs_real/alpha_decay_init", init_alpha_);
    som_->updControl("mrs_real/neighbourhood_decay_init", init_neighbourhood_);
    som_->updControl("mrs_real/std_factor_init", init_std_factor_);



    realvec som_fmatrixres;

    som_fmatrixres.create(som_->getctrl("mrs_natural/onObservations")->to<mrs_natural>(),
                          som_->getctrl("mrs_natural/onSamples")->to<mrs_natural>());

    // loop this for more init runs

    for (int i = 0; i < init_iterations_; i++)
    {
      init_train_fmatrix->shuffle();
      som_->process(*init_train_fmatrix, som_fmatrixres);
    }
    //som_->tick();
    som_->updControl("mrs_bool/done", true);

    // write the trained som network and the feature normalization networks
    oss1.open("som.mpl");
    oss1 << *som_;
    oss1.close();
    oss1.open("som1.mpl");
    oss1 << *som_;
    oss1.close();

    cout << "end of init" << endl;
  } else {
    emit errorBox("Need to drop files for initilization!");
  }
}

void
Grid::openPredictionGrid(QString fname)
{
  fstream file_ip((fname.toStdString()).c_str(), ios::in);
  if (!file_ip) {
    cerr << "Can't open input file " << endl;
    return;
  }

  int x_size = 0;
  int y_size = 0;
  int index = 0;
  size_t splitIndex = 0;
  QString qFname = "";
  string intBufferStr = "";
  string line = "";
  string listFname = "";
  string vecIndex = "";

  cout << "doing file stuff" << endl;

  // get the stored size of the vector
  getline(file_ip,intBufferStr);
  istringstream intBuffer(intBufferStr);
  intBuffer >> x_size;

  getline(file_ip, intBufferStr);
  istringstream intBufferTwo(intBufferStr);
  intBufferTwo >> y_size;

  cout << "x_size = " << x_size << endl;
  cout << "y_size = " << y_size << endl;

  cout << "Before reset grid" << endl;
  resetGrid();
  cout << "After reset grid" << endl;

  while(!file_ip.eof())
  {
    getline(file_ip,line);
    cout << "Line is " << line << endl;

    splitIndex = line.find_first_of(',');
    vecIndex = line.substr(0,splitIndex);
    listFname = line.substr(splitIndex + 1);
    qFname =  listFname.c_str();
    istringstream buffer(vecIndex);
    buffer >> index;
    cout << "index = " << index << endl;
    cout << "filename = " << qFname << endl;

    addTrack(index % som_height, index / som_height, qFname);
  }
}

void
Grid::savePredictionGrid(QString fname)
{
  fstream file_op((fname.toStdString()).c_str(), ios::out);

  //first line is the width of the grid
  file_op << som_width << endl;

  // second line is the height of the grid
  file_op << som_height << endl;

  for (int i = 0; i < files_.size(); i++)
  {
    // output the vector index and filename
    QList<std::string> temp = files_[i];
    for(int j = 0; j < files_[i].size(); j++ )
    {
      file_op << i;
      file_op << "," + temp.takeFirst() << endl;
      emit repaintSignal(); // tell the GUI to redraw itself
    }
  }
  file_op.close();

}

void Grid::openHash()
{
  cout << "Opening hash" << endl;
  // if dir exists
  QDir dir;
  QFile *keysFile;
  QString currentHashKey("");
  realvec currentFeature;
  int currentHashFileNumber = 0;

  if(dir.exists("extractFiles") )
  {
    // open keys
    dir.setCurrent("extractFiles");
    keysFile = new QFile(dir.filePath("keys.txt"));
    if(keysFile->open(QFile::ReadWrite))
    {
      QTextStream keysInput(keysFile);
      while(!keysInput.atEnd())
      {
        keysInput >> currentHashKey;
        keysInput >> currentHashFileNumber;
        if(!currentHashKey.isNull() )
        {
          std::stringstream stringStream;
          stringStream << "featureVec" << currentHashFileNumber << ".hsh";
          cout << dir.filePath(stringStream.str().c_str()).toStdString() << endl;
          currentFeature.read(dir.cleanPath(stringStream.str().c_str()).toStdString());
          if(normFeatureHash->find(currentHashKey.toStdString()) == normFeatureHash->end())
          {
            normFeatureHash->insert( pair<std::string, realvec>(currentHashKey.toStdString(), currentFeature) );
          }

        }
        //TODO:: Clean up pointers
      }

    }
  }
  dir.setCurrent("..");
  cout << "Hash opened" << endl;

}

/*
 * removeInitFile always acts on the current grid square
*/
void Grid::removeInitFile()
{
  int k = getCurrentIndex();
  while(initFileLocations[k].size() > 0)
    initFileLocations[k].removeFirst();
}


void
Grid::setXGridSize(QString size)
{
  cerr << "0" << endl;
  oldWidth_ = som_width;
  som_width = size.toInt();
  cerr << "0.1" << endl;
  resetGrid();
  emit repaintSignal();
}

void
Grid::setYGridSize(QString size)
{
  oldHeight_ = som_height;
  som_height = size.toInt();
  resetGrid();
  emit repaintSignal();
}
void
Grid::resetGridSlot()
{
  resetGrid();
}

/*
* ---------------------------------------------------
*  Mode Setters
* ---------------------------------------------------
*/
void Grid::setExtractMode()
{
  cout << "grid extract" << endl;
  state_ = 1;
}
void Grid::setTrainMode()
{
  state_ = 2;
}
void Grid::setPredictMode()
{
  state_ = 3;
}
void Grid::setInitMode()
{
  state_ = 4;
}
void Grid::clearMode()
{
  state_ = 0;
}
void Grid::cancelPressed()
{
  cancel_ = true;
}

/*
* ---------------------------------------------------
* Functions
* ---------------------------------------------------
*/
void Grid::resetGrid()
{


  for(int i = 0; i < oldHeight_ * oldWidth_; i++)
    free(genreDensity[i]);
  free(genreDensity);
  oldHeight_ = som_height;
  oldWidth_ = som_width;


  genreDensity = (int **)malloc(som_height * som_width * sizeof(int *));

  for (int i = 0; i < som_height*som_width; i++)
  {
    genreDensity[i] = (int *)malloc(10 * sizeof(int));
    for(int j = 0; j < 10; j++)
    {
      genreDensity[i][j] = 0;
    }
  }


  initFileLocations.clear();
  initFileLocations.resize(som_height * som_width);
  _gridX = 0;
  _gridY = 0;
  continuous_ = false;
  cancel_ = false;
  state_ = 0;
  init_ = false;



  delete featureHash;
  delete normFeatureHash;
  featureHash = new multimap<string, realvec>();
  normFeatureHash = new multimap<string, realvec>();
  numFeatures = 0;
  counters.clear();
  counterSizes.clear();
  labels.clear();
  files_.clear();


  delete m_playSystem; m_playSystem = 0;
  delete m_playMarSystem; m_playMarSystem = 0;
  delete som_;
  delete total_;
  delete norm_;

  setup();
}

int Grid::getCurrentIndex()
{
  return _gridY * som_height + _gridX;
}

void Grid::setGridX(int x) {
  if ( 0 <= x && x <= som_width ) {
    _gridX = x;
  }
}

void Grid::setGridY(int y) {
  if ( 0 <= y && y <= som_height ) {
    _gridY = y;
  }
}

void Grid::setGridX1(int x) {
  if ( 0 <= x && x <= som_width ) {
    _gridX1 = x;
  }
}

void Grid::setGridY1(int y) {
  if ( 0 <= y && y <= som_height ) {
    _gridY1 = y;
  }
}

void Grid::setPlaylist(std::string playlist)
{
  playlist_ = playlist;
}

void Grid::addTrack(int x, int y, QString track) {

  int index = y * som_height + x;
  /* For use with "genres" only
  * The 2nd dimenstion of density uses the following values
  * 0 - blues
  * 1 - classical
  * 2 - country
  * 3 - disco
  * 4 - hiphop
  * 5 - jazz
  * 6 - metal
  * 7 - reggae
  * 8 - rock
  * 9 - pop
  */
  if(track.contains("blues", Qt::CaseInsensitive))
  {
    genreDensity[index][0]++;
  }
  else if(track.contains("classical", Qt::CaseInsensitive))
  {
    genreDensity[index][1]++;
  }
  else if(track.contains("country", Qt::CaseInsensitive))
  {
    genreDensity[index][2]++;
  }
  else if(track.contains("disco", Qt::CaseInsensitive))
  {
    genreDensity[index][3]++;
  }
  else if(track.contains("hiphop", Qt::CaseInsensitive))
  {
    genreDensity[index][4]++;
  }
  else if(track.contains("jazz", Qt::CaseInsensitive))
  {
    genreDensity[index][5]++;
  }
  else if(track.contains("metal", Qt::CaseInsensitive))
  {
    genreDensity[index][6]++;
  }
  else if(track.contains("reggae", Qt::CaseInsensitive))
  {
    genreDensity[index][7]++;
  }
  else if(track.contains("rock", Qt::CaseInsensitive))
  {
    genreDensity[index][8]++;
  }
  else if(track.contains("pop", Qt::CaseInsensitive))
  {
    genreDensity[index][9]++;
  }

  files_[index].push_back(track.toStdString());
  counterSizes[index]++;
}

int * Grid::getDensity(int index)
{
  return genreDensity[index];
}

int Grid::getCellSize(int winSize)
{
  return winSize / std::max(som_height, som_width);
}

void Grid::playTrack(int index)
{
  QList<std::string> playlist = getCurrentFiles();
  // Super safety abort
  if(playlist.size() <= index)
  {
    cout << "ABORT" << endl;
    return;
  }

  static const bool NO_UPDATE = false;
  m_fileNameControl->setValue( QString::fromStdString(playlist[index]), NO_UPDATE );
  m_initAudioControl->setValue(true, NO_UPDATE);
  m_playSystem->update();
  m_playSystem->start();
}

void Grid::stopPlaying()
{
  m_playSystem->stop();
}

QList<std::string> Grid::getInitFiles()
{
  QList<GridTriplet *> triplets =  initFileLocations[getCurrentIndex()];
  QList<std::string> fileLocations;
  for(int i = 0; i < triplets.size(); i++ )
    fileLocations.append(triplets.at(i)->getFileName());
  return fileLocations;
}
void Grid::addInitFile(QString fileName, int x, int y)
{
  init_ = true;
  GridTriplet* gt = new GridTriplet(fileName.toStdString(), x,y);
  initFileLocations[y * som_height + x].append(gt);
}
QList<std::string> Grid::getCurrentFiles()
{
  return files_[getCurrentIndex()];
}
QList<std::string> Grid::getFilesAt(int index)
{
  if(index >= files_.size())
  {
    cout << "out of range" << endl;
  }
  return files_[index];
}
int Grid::getGridCounterSizes(int index)
{
  return counterSizes[index];
}
int Grid::getGridCounter(int index)
{
  return counters[index];
}
void Grid::setGridCounter(int index, int value)
{
  counters[index] = value;
}

void Grid::setXPos(int value)
{
  _gridX = value;
}

void Grid::setYPos(int value)
{
  _gridY = value;
}

void Grid::setX1Pos(int value)
{
  _gridX1 = value;
}

void Grid::setY1Pos(int value)
{
  _gridY1 = value;
}

void Grid::setContinuous(bool value)
{
  continuous_ = value;

}
GridTriplet::GridTriplet()
{
  x = 0;
  y = 0;
  fileName = "";
}
GridTriplet::GridTriplet(std::string fileName, int x, int y)
{
  this->x = x;
  this->y = y;
  this->fileName = fileName;
}
GridTriplet::GridTriplet(int x, int y)
{
  this->x = x;
  this->y = y;
  fileName = "";
}
void GridTriplet::setX(int value)
{
  x = value;
}
void GridTriplet::setY(int value)
{
  y = value;
}
void GridTriplet::setFileName(std::string value)
{
  fileName = value;
}


void
Grid::setTrainFile(QString fname)
{
  trainFile_ = fname.toStdString();
  cout << "Grid::openPredictionGrid trainFile=" << trainFile_ << endl;
}


// This is called when we receive an OSC message to the "/update"
// address.  It means that we need to get the values of all of our
// x1_,y1_,x2_, etc. members and update the grid
void
Grid::setValue(int i)
{
  cout << "i=" << i << "x0_=" << x0_->value << " y0_=" << y0_->value;
  cout << " x1_=" << x1_->value << " y1_=" << y1_->value << endl;

  _gridX = x0_->value;
  _gridY = y0_->value;

  _gridX1 = x1_->value;
  _gridY1 = y1_->value;

  emit repaintSignal();

}
