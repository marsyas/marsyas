#include "Grid.h"

Grid::Grid(int winSize, Tracklist *tracklist, QWidget *parent)
	: MyDisplay(tracklist, parent), _winSize(winSize)
{
	_collection = MusicCollection::getInstance();

	setAcceptDrops(true);
	setMouseTracking(true);
	setMinimumSize(winSize, winSize);

	som_height = 12;
	som_width = 12;
	_gridX = 0;
	_gridY = 0;
	 initAudio_ = false;
	 continuous_ = false;

	setup();
}

Grid::~Grid() {

	GridSquare *gs;
	for(int i=0; i < _squares.size(); i++) {
		gs = _squares[i];
		delete gs;
	}
}

void Grid::clear() {
	update();
}

void Grid::setup() {
	_cellSize = 50;	//size of Classifier

	GridSquare *gs;
	QList<std::string> *ql;
	for(int i=0; i < som_height; i++) {
		for(int j=0; j < som_width; j++) {
			gs = new GridSquare(i, j);
			ql = new QList<std::string>();
			_squares.push_back(gs);
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
		QMessageBox::information(this, tr("MarGrid"),
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

			repaint();

			total_->updctrl("mrs_bool/advance", true);            
		}



		cout << "end_prediction" << endl;
	} else {
		QMessageBox::information(this, tr("MarGrid"),
			tr("Need to load a collection of audio files first!"));
	}
}

void Grid::train() {
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
		QMessageBox::information(this, tr("MarGrid"),
			tr("Need to load a collection of audio files first!"));
	}
}

void Grid::midiXYEvent(unsigned char xaxis, unsigned char yaxis) {
	int x = (int)(xaxis / 128.0 * som_width);
	int y = som_height - 1 - (int)(yaxis / 128.0 * som_height);

	std::cout << "midi xy event (" << x << "," << y << ")\n";
	updateXYPosition(x, y);
	playNextTrack();
}

void Grid::midiPlaylistEvent(bool next) {
	if ( next ) {
		std::cout << "midi playlist event\n";
		getCurrentSquare()->nextTrack();
		playNextTrack();	
	}
}

void Grid::reload() {
/*	MusicTrackIterator it = _collection->getTracks();
	while ( it.hasNext() ) {
		MusicTrack *track = it.next();
		if ( -1 != track->getX() && -1 != track->getY() ) {
			qDebug() << "Initializing Grid with: " << track->getTitle() 
				 << " (" << track->getX() << "," << track->getY() << ")";
			addTrack( track->getX(), track->getY(), track);
		}
	}*/
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


/*
 * ---------------------------------------------------
 * Functions
 * ---------------------------------------------------
 */
GridSquare* Grid::getCurrentSquare() {
	int k = _gridX * som_height + _gridY;
	return _squares[k];
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

void Grid::updateXYPosition(int x, int y) {
	setGridX(x);
	setGridY(y);


	std::cout << "(" << _gridX << "," << _gridY 
		  << ") tracks: " << files_[_gridX * som_height + _gridY].size() << std::endl;
	repaint();
}
 
void Grid::playNextTrack() {
	if( !files_[getCurrentIndex()].isEmpty() ) 
	{
  

        int counterSize = counterSizes[getCurrentIndex()];
        if (counterSize > 0) 
           counters[getCurrentIndex()] = (counters[getCurrentIndex()] + 1) % counterSize;  
		int counter = counters[getCurrentIndex()];

		QList<std::string> playlist = files_[getCurrentIndex()];
		cout << "Currently Playing: " + playlist[counter] << endl;
		cout << "Playlist: " << endl;
		for(int i = 0; i < counterSize; i++ ) {
			cout << playlist[i] << endl;
		}
		mwr_->updctrl( filePtr_, playlist[counter].c_str() );
		mwr_->play();
				  
		  if (initAudio_ == false) 
		  {
			  mwr_->updctrl("AudioSink/dest/mrs_bool/initAudio", true);
			  initAudio_ = true;
		  } else {
			  mwr_->pause();
		  }
	}
}


/*
 * -----------------------------------------------------------------------------
 * Mouse Events
 * -----------------------------------------------------------------------------
 */
void Grid::mousePressEvent(QMouseEvent *event) {
	std::cout << "mouse Press Event" << std::endl;

	updateXYPosition(event->pos().x() / _cellSize, event->pos().y() / _cellSize);
	//getCurrentSquare()->nextTrack();
	//_tracklist->listTracks(&getCurrentSquare()->getTracks());
	playNextTrack();
}

void Grid::mouseMoveEvent(QMouseEvent *event) {
	if ( (event->pos().x() >= _winSize) || (event->pos().y() >= _winSize) ) {
		return;
	}
	if(continuous_) 
	{
	updateXYPosition(event->pos().x() / _cellSize, event->pos().y() / _cellSize);
	playNextTrack();
	}
}

/*
 * -----------------------------------------------------------------------------
 * Mouse Events
 * -----------------------------------------------------------------------------
 */
void Grid::dragMoveEvent(QDragMoveEvent* /* event */) {
	//qDebug() << "Drag Move";
}

void Grid::dragEnterEvent(QDragEnterEvent* event) {
	if ( event->proposedAction() == Qt::CopyAction ) {
		event->acceptProposedAction();
	}
}

void Grid::dropEvent(QDropEvent *event) {

	if ( event->proposedAction() == Qt::CopyAction ) {
		//Position of drop event
		int x = event->pos().x() / _cellSize;
		int y = event->pos().y() / _cellSize;

		bool ok = false;
		const QMimeData *data = event->mimeData();
		
		QString trackName = data->text();

		if ( data->hasFormat("application/track-id") ) {
			int trackId = data->data("application/track-id").toInt(&ok);
		
			qDebug() << "Track Drop Recv: " << trackName << " " << trackId;
			if ( ok ) {
				MusicTrack *track = _collection->getTrackById(trackId);
				if ( track ) {
					addTrack(x, y, track->getLocation().toStdString());
				}
			}
		} else if ( data->hasFormat("application/playlist-id") ) {
			QString playlistId = data->data("application/playlist-id").data();

			MusicPlaylist *playlist = _collection->getPlaylistByName(playlistId);
			if ( playlist ) {
				MusicTrackIterator ip = playlist->getTracks();
				while ( ip.hasNext() ) {
					MusicTrack *track = ip.next();
					addTrack(x, y, track->getLocation().toStdString());
				}
			}
		}	
	}
}

void Grid::paintEvent(QPaintEvent* /* event */) {
	QPainter painter;
	painter.begin(this);

	//Find density
	int maxDensity = 0;
	int minDensity = 100;
	for (int i=0; i < files_.size(); i++) {
		if(files_[i].size() > maxDensity)
		{
			maxDensity = files_[i].size();
		}
		else if (files_[i].size() < minDensity) 
		{
			minDensity = files_[i].size();
		}
	}

	Colormap *map = Colormap::factory(Colormap::GreyScale);
	for (int i=0; i < som_height; i++) {
		for (int j=0; j < som_width; j++) {

			int k = i * som_height + j;

			QRect	 myr(i*_cellSize,j*_cellSize,_cellSize,_cellSize);
			QLine	 myl1(i*_cellSize,j*_cellSize, i*_cellSize, j*_cellSize + _cellSize);
			QLine	 myl2(i*_cellSize,j*_cellSize, i*_cellSize+_cellSize, j*_cellSize );

			if ( files_[k].size() == 0 ) {
				QColor color(map->getRed(125), map->getGreen(0), map->getBlue(0));
				painter.setBrush(color);
			} else {
				int c = int(files_[k].size() / float(maxDensity) * (map->getDepth()-1));
				QColor color(map->getRed(c), map->getGreen(c), map->getBlue(c));
				painter.setBrush(color);
			}

			painter.setPen(Qt::NoPen);
			painter.drawRect(myr);

			painter.setPen(Qt::black);
			painter.drawLine(myl1);
			painter.drawLine(myl2);

			painter.setBrush(Qt::red);
			QRect newr( _gridX * _cellSize + _cellSize / 4,
				_gridY * _cellSize + _cellSize / 4,
				_cellSize - _cellSize / 2,
				_cellSize-_cellSize / 2);
			painter.drawRect(newr);
		}
	}
	delete map;
	painter.end();
}

void Grid::addTrack(int x, int y, std::string track) {

	int index = x * som_height + y;
	files_[index].push_back(track);
	counterSizes[index]++;
	repaint();
}

void Grid::resetGrid() {
	std::cout << "Resetting Grid...." << std::endl;
  
	for (int i=0; i < _squares.size(); i++) {
		cout << i << endl;
        _squares[i]->clear();
	}
  	repaint();
}


/*
 * ---------------------------------------------------
 * GridSquares
 * ---------------------------------------------------
 */
GridSquare::GridSquare(int x, int y) 
	: _x(x), _y(y) 
{
	_current = 0;
}

bool GridSquare::isEmpty() const {
	return _list.isEmpty();
}

int GridSquare::getCount() const {
	return _list.size();
}

int GridSquare::getX() const {
	return _x;
}

int GridSquare::getY() const {
	return _y;
}

void GridSquare::addTrack(MusicTrack* track) {
	if ( track ) {
		track->setX(_x);
		track->setY(_y);
		_list.push_back(track);
	}
}

void GridSquare::nextTrack() {
	_current++;
	if ( _current >= _list.size() ) {
		_current = 0;
	}
}

MusicTrack* GridSquare::getCurrent() {
	if ( getCount() > 0 ) {
		MusicTrack* track = _list[_current];
		return track;
	} else {
		return NULL;
	}
}

MusicTrackIterator GridSquare::getTracks() {
	return MusicTrackIterator(_list);
}

void GridSquare::clear() {
	_list.clear();
}

/*
 * A track can be added to the grid twice, this function removes 
 * tracks that don't currently have x/y corresponding to GridSquare
 */
void GridSquare::refresh() {
	MusicTrack *track;
	for(int i=0; i < _list.size(); i++) {
		track = _list[i];
		if ( track->getX() != _x || track->getY() != _y ) {
			_list.remove(i);
			if ( _current == i ) {
				nextTrack();
			}
		}
	}
}