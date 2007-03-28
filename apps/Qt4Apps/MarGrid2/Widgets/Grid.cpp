#include "Grid.h"

Grid::Grid(int winSize, Tracklist *tracklist, QWidget *parent)
	: Display(tracklist, parent), _winSize(winSize)
{
	_collection = MusicCollection::getInstance();

	//setDragEnabled(true);
	//setDropIndicatorShown(true);
	setAcceptDrops(true);
	setMouseTracking(true);
	setMinimumSize(winSize, winSize);

	_width = 20;
	_gridX = 0;
	_gridY = 0;

	//_marsyas = new MarsyasECP(_collection, this, _width);
	//_extractor = new Extractor();
	//_classifier = new Classifier();

	setup();
}

Grid::~Grid() {
	//delete _marsyas;
	//delete _extractor;
	//delete _classifier;

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
	_cellSize = _winSize / _width;	//size of Classifier

	GridSquare *gs;
	for(int i=0; i < _width; i++) {
		for(int j=0; j < _width; j++) {
			gs = new GridSquare(i, j);
			_squares.push_back(gs);
		}
	}
}

/*
 * ---------------------------------------------------
 * Slots
 * ---------------------------------------------------
 */
void Grid::extract() {
	if ( _collection->getNumTracks() > 0 ) {
		/*
		int size  = 0;

		//Write training set out to file
		ofstream out;
		out.open(GRID_TRAINING_FILE);
		size = _collection->generateTrainingList(out);
		out.close(); 
		*/
		//_marsyas->extract();	
	} else {
		QMessageBox::information(this, tr("MarGrid"),
                                 tr("Need to load a collection of audio files first!"));
	}
}

void Grid::predict() {
	if ( _collection->getNumTracks() > 0 ) {
		resetGrid();

		/*	
		int size = 0;

		//Extract Tracks from collection
        	cout << "Read collection" << endl;
        	ofstream out;
        	out.open( GRID_PREDICTION_FILE );
        	size = _collection->generateTrackList(out);
        	out.close();

		_marsyas->classify();
		repaint();
		*/

		int x = 0;
		int y = 0;

		srand(time(0));

		MusicTrackIterator iter = _collection->getTracks();
		while ( iter.hasNext() ) {
			MusicTrack *track = iter.next();
		
			x = (rand() % _width);
			y = (rand() % _width);
			std::cout << "Placing: " << track->getTitle() << " (" << x << "," << y << ")\n";
	
			addTrack( x, y, track);
		}
	} else {
		QMessageBox::information(this, tr("MarGrid"),
                                 tr("Need to load a collection of audio files first!"));
	}
}

void Grid::train() {
	if ( _collection->getNumTracks() > 0 ) {
		//_marsyas->train();
	} else {
		QMessageBox::information(this, tr("MarGrid"),
                                 tr("Need to load a collection of audio files first!"));
	}
}

void Grid::midiXYEvent(unsigned char xaxis, unsigned char yaxis) {
	int x = (int)(xaxis / 128.0 * _width);
	int y = _width - 1 - (int)(yaxis / 128.0 * _width);

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
	MusicTrackIterator it = _collection->getTracks();
	while ( it.hasNext() ) {
		MusicTrack *track = it.next();
		if ( -1 != track->getX() && -1 != track->getY() ) {
			qDebug() << "Initializing Grid with: " << track->getTitle() 
				 << " (" << track->getX() << "," << track->getY() << ")";
			addTrack( track->getX(), track->getY(), track);
		}
	}
}

/*
 * ---------------------------------------------------
 * Functions
 * ---------------------------------------------------
 */
GridSquare* Grid::getCurrentSquare() {
	int k = _gridX * _width + _gridY;
	return _squares[k];
}

void Grid::setGridX(int x) {
	if ( 0 <= x && x <= _width ) {
		_gridX = x;
	}
}

void Grid::setGridY(int y) {
	if ( 0 <= y && y <= _width ) {
		_gridY = y;
	}
}

void Grid::updateXYPosition(int x, int y) {
	setGridX(x);
	setGridY(y);


	std::cout << "(" << _gridX << "," << _gridY 
		  << ") tracks: " << getCurrentSquare()->getCount() << std::endl;
	repaint();
}
 
void Grid::playNextTrack() {
	GridSquare *square = getCurrentSquare();
	if ( !square->isEmpty() ) {
		MusicTrack* track = square->getCurrent();
		std::cout << "*****************************" << std::endl
			  << "Playing:" << track->getTitle() << std::endl
			  << track->getLocation() << std::endl
			  << "*****************************" << std::endl;
		emit playingTrack(track);
		//_marsyas->play(track);
		//mwr_->updctrl(filePtr_, posFiles[counter]);
		//mwr_->play();
	}

	std::cout << "Playlist:\n";
	MusicTrackIterator iter = square->getTracks();
	while ( iter.hasNext() ) {
		MusicTrack *track = iter.next();
		std::cout << track->getTitle() << std::endl;
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
	getCurrentSquare()->nextTrack();
	_tracklist->listTracks(&getCurrentSquare()->getTracks());
	playNextTrack();
}

void Grid::mouseMoveEvent(QMouseEvent *event) {
	if ( (event->pos().x() >= _winSize) || (event->pos().y() >= _winSize) ) {
		return;
	}

	//updateXYPosition(event->pos().x() / _cellSize, event->pos().y() / _cellSize);
	//playNextTrack();
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
					addTrack(x, y, track);
				}
			}
		} else if ( data->hasFormat("application/playlist-id") ) {
			QString playlistId = data->data("application/playlist-id").data();

			MusicPlaylist *playlist = _collection->getPlaylistByName(playlistId);
			if ( playlist ) {
				MusicTrackIterator ip = playlist->getTracks();
				while ( ip.hasNext() ) {
					MusicTrack *track = ip.next();
					addTrack(x, y, track);
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
	for (int i=0; i < _squares.size(); i++) {
		GridSquare *gs = _squares[i];
		gs->refresh();

		if ( gs->getCount() > maxDensity ) {
			maxDensity = gs->getCount();
		}
		if ( gs->getCount() < minDensity ) {
			minDensity = gs->getCount();
		}
	}

	Colormap *map = Colormap::factory(Colormap::Spectra);
	for (int i=0; i < _width; i++) {
		for (int j=0; j < _width; j++) {

			int k = i * _width + j;

			QRect	 myr(i*_cellSize,j*_cellSize,_cellSize,_cellSize);
			QLine	 myl1(i*_cellSize,j*_cellSize, i*_cellSize, j*_cellSize + _cellSize);
			QLine	 myl2(i*_cellSize,j*_cellSize, i*_cellSize+_cellSize, j*_cellSize );

			GridSquare *gs = _squares[k];
			/**
			 * For grey scale colouring
			if ( gs->isEmpty() ) {
				painter.setBrush(QColor("#ffffff"));
			} else {
				int color = 255 - int(gs->getCount() / float(maxDensity) * 255);
				painter.setBrush( QColor(color, color, color) );
			}
			 */

			/**
			 * For island scale colouring
			 */
			if ( gs->isEmpty() ) {
				QColor color(map->getRed(0), map->getGreen(0), map->getBlue(0));
				painter.setBrush(color);
			} else {
				int c = int(gs->getCount() / float(maxDensity) * (map->getDepth()-1));
				QColor color(map->getRed(c), map->getGreen(c), map->getBlue(c));
				painter.setBrush(color);
			}

			painter.setPen(Qt::NoPen);
			painter.drawRect(myr);

        		painter.setPen(Qt::red);
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

void Grid::addTrack(int x, int y, MusicTrack* track) {
	std::cout << "adding track: " << track->getTitle() 
		  << "(" << x << "," << y << ")\n"
		  << track->getLocation() << std::endl;

	int index = x * _width + y;
	_squares[index]->addTrack(track);
	repaint();
}

void Grid::resetGrid() {
	std::cout << "Resetting Grid...." << std::endl;
  
	for (int i=0; i < _squares.size(); i++) {
        	_squares[i]->clear();
	}
  	repaint();
}

/*
 * ---------------------------------------------------
 * Setters
 * ---------------------------------------------------
void Grid::setExtractor(Extractor *extractor) {
	_extractor = extractor;
}

Extractor* Grid::getExtractor() const { 
	return _extractor; 
}

void Grid::setClassifier(Classifier *classifier) {
	_classifier = classifier;
}

Classifier* Grid::getClassifier() const { 
	return _classifier; 
}
 */

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

/*  
  MarControlPtr filePtr_;
  
  Marsyas::MarSystemManager mng;  
  QVector<QList <std::string> > files;
  QVector<int> counters;
  QVector<int> counterSizes;
  QVector<int> labels;
  
  
  QList<QPixmap> piecePixmaps;
  QList<QRect> pieceRects;
  QList<QPoint> pieceLocations;

  QRect highlightedRect;
  QRect metalRec;
  QRect classicalRec;
  int inPlace;

  MarSystemWrapper*  _mwr;
  Marsyas::MarSystem* _pnet;

  int _somHeight;
  int _somWidth;
  int _cellSize;
  int _gridX;
  int _gridY;
  
  MusicCollection *_collection; 

  Marsyas::realvec _normSomFmatrix;
  Marsyas::MarSystem* _som;
  Marsyas::MarSystem* _total;
  Marsyas::MarSystem* _norm;

painter
	QRegExp qrp("classical+");
	QRegExp qrp1("metal+");
	QRegExp qrp2("hiphop+");
	QRegExp qrp3("blues+");
	QRegExp qrp4("jazz+");

			QVector<int> labelvotes;
			labelvotes << 0 << 0 << 0 << 0 << 0;

			for (int i = 0; i < posFiles.size(); ++i) {
				QString curFile(posFiles.at(i).c_str());

				if (qrp.indexIn(curFile) >= 0) {
					labelvotes[0]++;
				}
				if (qrp1.indexIn(curFile) >= 0) {
					labelvotes[1]++;
				}

				if (qrp2.indexIn(curFile) >= 0) {
					labelvotes[2]++;
				}
				if (qrp3.indexIn(curFile) >= 0) {
					labelvotes[3]++;
				}

				if (qrp4.indexIn(curFile) >= 0) {
					labelvotes[4]++;
				}

			}

			int majority_label = 0;
			int max =0;

			for (int l=0; l < 5; l++) {
				if (labelvotes[l] > max) {
					max = labelvotes[l];
					majority_label = l;
				}
			}

			if (max > 0)
				labels[k] = majority_label +1;


			* if (labels[k] == 0)
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
			*

*/ 

