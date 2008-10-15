#include "Grid.h"

Grid::Grid()
{
	_collection = MusicCollection::getInstance();

	som_height = 12;
	som_width = 12;
	_gridX = 0;
	_gridY = 0;
	 initAudio_ = false;
	 continuous_ = false;
	 state_ = 0;

	setup();
}

Grid::~Grid() {

	//TODO: DROP files_ POINTER
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
		
		ofstream featureFile;
		featureFile.open("music.mf");
		_collection->generateTrackList(featureFile);
		featureFile.close();

		total_->updctrl("mrs_string/filename", "music.mf");

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
			total_->updctrl("mrs_bool/advance", true);      

		}

		ofstream oss;
		oss.open("som_fmatrix.txt");
		oss << som_fmatrix << endl;	 
	} else {
		QMessageBox::information(0, tr("MarGrid"),
			tr("Need to load a collection of audio files first!"));
	}
}

void Grid::predict() {
	if ( _collection->getNumTracks() > 0 ) {
		
		resetGrid();
		realvec som_in;
		realvec som_res;
		realvec som_fmatrix;
		ifstream iss1;
		iss1.open("som.mpl");
		som_ = mng.getMarSystem(iss1);

		ifstream niss1;
		niss1.open("norm.mpl");
		norm_ = mng.getMarSystem(niss1);

		cout << "Starting prediction" << endl;
		som_->updctrl("mrs_string/mode", "predict");  

		Collection l1;
		l1.read("music.mf");
		cout << "Read collection" << endl;

		total_->updctrl("mrs_natural/pos", 0);

		total_->updctrl("mrs_string/filename", "music.mf"); 

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

		QString tempString;
		int TEMP_loc = 0;
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
			cout << grid_x;
			cout << ",";
			cout << grid_y  << endl;
			addTrack(grid_x, grid_y, current);


			total_->updctrl("mrs_bool/advance", true);            
		}



		cout << "end_prediction" << endl;
	} else {
		QMessageBox::information(0, tr("MarGrid"),
			tr("Need to load a collection of audio files first!"));
	}
}

void Grid::train() {
	cout << "yo" << endl;
	if ( _collection->getNumTracks() > 0 ) {
		
		// Read the feature matrix from file som_fmatrix.txt 
		realvec train_som_fmatrix;
		ifstream iss;
		iss.open("som_fmatrix.txt");
		iss >> train_som_fmatrix;

		MarSystem*  norm_;
		MarSystem*  som_;
		realvec norm_som_fmatrix;



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
		ofstream oss1;
		oss1.open("som.mpl");
		oss1 << *som_ << endl;
		delete som_;

		ofstream noss;
		noss.open("norm.mpl");
		noss << *norm_ << endl;
		delete norm_;
		
	} else {
		QMessageBox::information(0, tr("MarGrid"),
			tr("Need to load a collection of audio files first!"));
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
		
		cout << index;
		cout << ",";
		cout << listFname << endl; 
		files_[index].push_back(qFname.toStdString());
		counterSizes[index]++;
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

		}
	}
	file_op.close();
}
void Grid::setExtractMode()
{
	cout << "grid extract" << endl;
	state_ = 1;
	//extract();
}
void Grid::setTrainMode()
{
	state_ = 2;
	//train();
}
void Grid::setPredictMode()
{
	state_ = 3;
	//predict();
}
void Grid::clearMode()
{
	state_ = 0;
}


/*
 * ---------------------------------------------------
 * Functions
 * ---------------------------------------------------
 */
void Grid::resetGrid()
{
	//TODO:: RESET!
}

int Grid::getCurrentIndex()
{
	return _gridX * som_height + _gridY;
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

 
void Grid::addTrack(int x, int y, std::string track) {

	int index = x * som_height + y;
	files_[index].push_back(track);
	counterSizes[index]++;
}
void Grid::playTrack(int index)
{
	QList<std::string> playlist = getCurrentFiles();

	mwr_->updctrl( filePtr_, playlist[index].c_str() );
	mwr_->play();

	if (initAudio_ == false) 
	{
		mwr_->updctrl("AudioSink/dest/mrs_bool/initAudio", true);
		initAudio_ = true;
	} else {
		mwr_->pause();
	}
}
QList<std::string> Grid::getCurrentFiles()
{
	return files_[getCurrentIndex()];
}
QList<std::string> Grid::getFilesAt(int index)
{
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
