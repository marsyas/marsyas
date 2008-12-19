#include "Grid.h"

Grid::Grid()
{
	_collection = MusicCollection::getInstance();

	som_height = 12;
	som_width = 12;
	oldWidth_ = som_width;
	oldHeight_ = som_height;
	_gridX = 0;
	_gridY = 0;
	initAudio_ = false;
	continuous_ = false;
	cancel_ = false;
	state_ = 0;
	init_ = false;
	featureHash = new multimap<string, realvec>();
	normFeatureHash = new multimap<string, realvec>();
	numFeatures = 0;
	initFileLocations.resize(som_height * som_width);
	playlist_ = "Library"; // by default select the full iTunes library

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
		switch(state_){
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
	_cellSize = 50;	//size of Classifier

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
	total_->updctrl("mrs_real/repetitions", 1.0);

	// Playback network
	pnet_ = mng.create("Series", "pnet_");
	pnet_->addMarSystem(mng.create("SoundFileSource", "src"));
	// pnet_->addMarSystem(mng.create("Stereo2Mono", "s2m"));
	pnet_->addMarSystem(mng.create("Gain", "gain"));
	pnet_->addMarSystem(mng.create("AudioSink", "dest"));
	pnet_->linkctrl("mrs_bool/notEmpty","SoundFileSource/src/mrs_bool/notEmpty");


	mwr_ = new MarSystemQtWrapper(pnet_);
	filePtr_ = mwr_->getctrl("SoundFileSource/src/mrs_string/filename");

	mwr_->start();

}

/*
* ---------------------------------------------------
* Slots
* ---------------------------------------------------
*/
void Grid::extract() {
	if ( _collection->getNumTracks() > 0 ) {


		int index = 0;

		// !!! use itunes to generate the collection file rather then using a file
		ofstream featureFile;
		featureFile.open("music.mf");
		_collection->generatePlayList(featureFile, playlist_.c_str());
		extractAction("music.mf");
	}	
	else 
	{
		emit errorBox("Need to load a collection of audio files first!");
	}
}
void Grid::extractAction(std::string filename)
{
	total_->updctrl("mrs_string/filename", filename);


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

	som_fmatrix.create(total_onObservations, 
		numFiles);

	// calculate features 
	cout << "Calculating features" << endl;
	for (int index=0; index < numFiles; index++)
	{
		if(cancel_)
		{
			cancel_ = false;
			break;
		}
		total_->updctrl("mrs_natural/label", index);
		total_->updctrl("mrs_bool/memReset", true);
		total_->updctrl("mrs_natural/cindex", index);

		string current = total_->getctrl("mrs_string/currentlyPlaying")->to<mrs_string>();
		cout << "Processed " << index << " files " << endl; 
		cout << current  << " - ";
		total_->process(som_in,som_res);
		
		// if current file isn't in out master hash, extract the file and load it into the hash
		it = featureHash->find(current);

		if(it == featureHash->end() )
		{
			// hash the resulting feature on file name
			featureHash->insert(pair<string,realvec>(current,som_res));

		}
		else
		{
			cout << "found!" << endl;
			som_res = featureHash->find(current)->second;
		}

		for (int o=0; o < total_onObservations; o++) 
			som_fmatrix(o, index) = som_res(o, 0);
		total_->updctrl("mrs_bool/advance", true);     

	}

	ofstream oss;
	oss.open("som_fmatrix.txt");
	oss << som_fmatrix << endl;	
	oss.close();

	// normalize and create hashmap of normalized feature vectors on their file name
	
	MarSystem*  norm_;
	realvec norm_som_fmatrix;

	// Normalize the feature matrix so that all features are between 0 and 1
	norm_som_fmatrix.create(som_fmatrix.getRows(),
		som_fmatrix.getCols());
	norm_ = mng.create("NormMaxMin", "norm");
	norm_->updctrl("mrs_natural/inSamples", som_fmatrix.getCols());
	norm_->updctrl("mrs_natural/inObservations", 
		total_->getctrl("mrs_natural/onObservations")->to<mrs_natural>());

	norm_->updctrl("mrs_string/mode", "train");
	norm_->process(som_fmatrix, norm_som_fmatrix);
	norm_->updctrl("mrs_string/mode", "predict");  
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
		for ( it=featureHash->begin() ; it != featureHash->end(); it++ )
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
	if ( _collection->getNumTracks() > 0 ) {

		// Read the feature matrix from file som_fmatrix.txt 
		realvec norm_som_fmatrix;
		ifstream iss;
		iss.open("norm_som_fmatrix.txt");
		iss >> norm_som_fmatrix;


		MarSystem*  som_;

		// Skip the SOM manipulation.
		if(!skipTraining)
		{
			if(init_) 
			{
				ifstream iss1;
				iss1.open("som.mpl");
				som_ = mng.getMarSystem(iss1);
				iss1.close();
				som_->updctrl("mrs_natural/inSamples", norm_som_fmatrix.getCols());
			}
			else
			{
				cout << "not init" << endl;
				// Create netork for training the self-organizing map 
				som_ = mng.create("SOM", "som");  
				som_->updctrl("mrs_natural/grid_width", som_width);
				som_->updctrl("mrs_natural/grid_height", som_height);
				som_->updctrl("mrs_natural/inSamples", norm_som_fmatrix.getCols());
				som_->updctrl("mrs_natural/inObservations", norm_som_fmatrix.getRows());  
			}

			som_->updctrl("mrs_string/mode", "train");


			realvec som_fmatrixres;
			som_fmatrixres.create(som_->getctrl("mrs_natural/onObservations")->to<mrs_natural>(), 
				som_->getctrl("mrs_natural/onSamples")->to<mrs_natural>());

				ofstream oss1;
				oss1.open("som2.mpl");
				oss1 << *som_;
				oss1.close();


			cout << "Starting training" << endl;

			bool done_ = false;
			for (int i=0; i < 500; i ++) 
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
				done_ = true;
			}

			oss1.open("som3.mpl");
			oss1 << *som_ << endl;
			oss1.close();

			som_->updctrl("mrs_bool/done", done_);
			som_->tick();
			cout << "Training done" << endl;

			// write the trained som network and the feature normalization networks 
			oss1.open("som.mpl");
			oss1 << *som_ << endl;
			delete som_;
		}




	} else {
		emit errorBox("Need to load a collection of audio files first!");
	}
}
void Grid::predict() {
	if ( _collection->getNumTracks() > 0 ) {

		realvec som_in;
		realvec som_res;
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
		som_->updctrl("mrs_string/mode", "predict");  



		Collection l1;
		l1.read("music.mf");

		cout << "Read collection" << endl;

		total_->updctrl("mrs_string/filename", "music.mf"); 

		total_->updctrl("mrs_natural/pos", 0);

		som_->updctrl("mrs_natural/inSamples", 1);


		realvec predict_res(som_->getctrl("mrs_natural/onObservations")->to<mrs_natural>(), 
			som_->getctrl("mrs_natural/onSamples")->to<mrs_natural>());
		norm_->updctrl("mrs_natural/inSamples", 1);



		realvec norm_som_res;

		som_in.create(total_->getctrl("mrs_natural/inObservations")->to<mrs_natural>(), 
			total_->getctrl("mrs_natural/inSamples")->to<mrs_natural>()); 

		som_res.create(total_->getctrl("mrs_natural/onObservations")->to<mrs_natural>(), 
			total_->getctrl("mrs_natural/onSamples")->to<mrs_natural>());

		norm_som_res.create(total_->getctrl("mrs_natural/onObservations")->to<mrs_natural>(), 
			total_->getctrl("mrs_natural/onSamples")->to<mrs_natural>());

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

			total_->updctrl("mrs_natural/label", index);
			total_->updctrl("mrs_bool/memReset", true);
			total_->updctrl("mrs_natural/cindex", index);


			total_->process(som_in, som_res);
			QString current = total_->getctrl("mrs_string/currentlyPlaying")->to<mrs_string>().c_str();

			norm_->process(som_res, norm_som_res);

			realvec foobar;
			foobar.create(som_->getctrl("mrs_natural/inObservations")->to<mrs_natural>(), som_->getctrl("mrs_natural/inSamples")->to<mrs_natural>());

			norm_som_fmatrix.getCol(index, foobar);

			som_->process(foobar, predict_res); 

			grid_x = predict_res(0);
			grid_y = predict_res(1);
			addTrack(grid_x, grid_y, current);
			emit repaintSignal();

	
			total_->updctrl("mrs_bool/advance", true);  
		}


		cout << "end_prediction" << endl;
	} else {
		emit errorBox("Need to load a collection of audio files first!");
	}
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
					emit errorBox("One of the dropped files was not extracted, please run Extract again");
					return;
				}
				temp = temp2->second;
				init_train_fmatrix->stretch( temp.getRows() + 2, init_train_fmatrix->getCols() + temp.getCols() );

				for(int j = 0; j < temp.getRows(); j++)
				{
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
		som_->updctrl("mrs_natural/grid_width", som_width);
		som_->updctrl("mrs_natural/grid_height", som_height);
		som_->updctrl("mrs_natural/inSamples", init_train_fmatrix->getCols());


		som_->updctrl("mrs_natural/inObservations", init_train_fmatrix->getRows());  
		som_->updctrl("mrs_string/mode", "init");



		realvec som_fmatrixres;

		som_fmatrixres.create(som_->getctrl("mrs_natural/onObservations")->to<mrs_natural>(), 
			som_->getctrl("mrs_natural/onSamples")->to<mrs_natural>());

		// loop this for more init runs

		for (int i = 0; i < 10; i++)
		{
			init_train_fmatrix->shuffle();
			som_->process(*init_train_fmatrix, som_fmatrixres);
		}
		//som_->tick();
		som_->updctrl("mrs_bool/done", true);

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
	int splitIndex = 0;
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

	resetGrid();
	int i = 1;
	while(!file_ip.eof())
	{
		getline(file_ip,line);
		splitIndex = line.find_first_of(',');
		vecIndex = line.substr(0,splitIndex);
		listFname = line.substr(splitIndex + 1);
		qFname =  listFname.c_str();
		istringstream buffer(vecIndex);	
		buffer >> index;
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
					featureHash->insert( pair<std::string, realvec>(currentHashKey.toStdString(), currentFeature) );

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

	delete featureHash;
	delete normFeatureHash;

	genreDensity = (int **)malloc(som_height * som_width * sizeof(int *));
	
	for (int i = 0; i < som_height * som_width; i++) 
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
	initAudio_ = false;
	continuous_ = false;
	cancel_ = false;
	state_ = 0;
	init_ = false;
	featureHash = new multimap<string, realvec>();
	normFeatureHash = new multimap<string, realvec>();
	numFeatures = 0;
	counters.clear();
	counterSizes.clear();
	labels.clear();
	files_.clear();
	delete mwr_;
	delete pnet_;
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
	mwr_->updctrl( filePtr_, playlist[index].c_str() );
	if (initAudio_ == false) 
	{
		mwr_->updctrl("AudioSink/dest/mrs_bool/initAudio", true);
		initAudio_ = true;
		oldPlayingIndex = index;
		mwr_->play();
	}
}

void Grid::stopPlaying()
{
	mwr_->updctrl("AudioSink/dest/mrs_bool/initAudio", false);
	mwr_->pause();
	initAudio_ = false;

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
