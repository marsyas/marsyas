#include "../Widgets/GridDisplay.h"

GridDisplay::GridDisplay(int winSize, Tracklist *tracklist, Grid* grid_, QWidget *parent)
	: MyDisplay(tracklist, parent), _winSize(winSize)
{
	this->grid_ = grid_;
	_cellSize = grid_->getCellSize();
	setMinimumSize(winSize, winSize);
	setMouseTracking(true);
	connect(this, SIGNAL(clearMode()), grid_, SLOT(clearMode()));
    connect(this, SIGNAL(extractMode()), grid_, SLOT(setExtractMode()));
	connect(this, SIGNAL(trainMode()), grid_, SLOT(setTrainMode()));
	connect(this, SIGNAL(predictMode()), grid_, SLOT(setPredictMode()));
}

GridDisplay::~GridDisplay()
{
	// TODO: DELETE STUFF
}
// ******************************************************************
//
// SLOTS

void GridDisplay::clear()
{

	emit clearMode();

}
void GridDisplay::train()
{
	emit trainMode();
	grid_->buttonPressed.wakeAll();
}
void GridDisplay::extract()
{
	cout << "extract" <<endl;
	emit extractMode();
	grid_->buttonPressed.wakeAll();
}
void GridDisplay::predict()
{
	emit predictMode();
	grid_->buttonPressed.wakeAll();
}
void GridDisplay::savePredictionGrid(QString fname)
{
	//TODO: connect save to grid
}
void GridDisplay::openPredictionGrid(QString fname)
{
	//TODO: connect open to grid
}
void GridDisplay::playModeChanged()
{
	grid_->setContinuous( !grid_->isContinuous() );
}



// ******************************************************************
//
// FUNCTIONS
void GridDisplay::midiXYEvent(unsigned char xaxis, unsigned char yaxis) {
	int x = (int)(xaxis / 128.0 * grid_->getWidth());
	int y = grid_->getHeight() - 1 - (int)(yaxis / 128.0 * grid_->getHeight());

	std::cout << "midi xy event (" << x << "," << y << ")\n";
	updateXYPosition(x, y);
	playNextTrack();
}
void GridDisplay::updateXYPosition(int x, int y)
{
	grid_->setXPos(x);
	grid_->setYPos(y);
}

void GridDisplay::midiPlaylistEvent(bool next) {
	if ( next ) {
		std::cout << "midi playlist event\n";
		//getCurrentSquare()->nextTrack();
		playNextTrack();	
	}
}

void GridDisplay::reload() {

	//TODO:  Figure out if reload is needed
}


void GridDisplay::playNextTrack() {
	if( !grid_->getCurrentFiles().isEmpty() ) 
	{
  

        int counterSize = grid_->getGridCounterSizes(grid_->getCurrentIndex());
        if (counterSize > 0) 
			grid_->setGridCounter( grid_->getCurrentIndex() , (grid_->getGridCounter(grid_->getCurrentIndex()) + 1) % counterSize );  
		int counter = grid_->getGridCounter(grid_->getCurrentIndex());

		QList<std::string> playlist = grid_->getCurrentFiles();
		cout << "Currently Playing: " + playlist[counter] << endl;
		cout << "Playlist: " << endl;
		for(int i = 0; i < counterSize; i++ ) {
			cout << playlist[i] << endl;
		}
		grid_->playTrack(counter);

		//TODO: REMOVE OR KEEP?
		/*mwr_->updctrl( filePtr_, playlist[counter].c_str() );
		mwr_->play();
				  
		  if (initAudio_ == false) 
		  {
			  mwr_->updctrl("AudioSink/dest/mrs_bool/initAudio", true);
			  initAudio_ = true;
		  } else {
			  mwr_->pause();
		  }*/
	}
}


/*
 * -----------------------------------------------------------------------------
 * Mouse Events
 * -----------------------------------------------------------------------------
 */
void GridDisplay::mousePressEvent(QMouseEvent *event) {
	std::cout << "mouse Press Event" << std::endl;

	updateXYPosition(event->pos().x() / _cellSize, event->pos().y() / _cellSize);

	//TODO: REMOVE OR KEEP?
	//getCurrentSquare()->nextTrack();
	//_tracklist->listTracks(&getCurrentSquare()->getTracks());
	playNextTrack();
}

void GridDisplay::mouseMoveEvent(QMouseEvent *event) {
	if ( (event->pos().x() >= _winSize) || (event->pos().y() >= _winSize) ) {
		return;
	}
	if(grid_->isContinuous()) 
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
void GridDisplay::dragMoveEvent(QDragMoveEvent* /* event */) {
	

	//TODO: NEED TO ADD IN FOR INIT

	//qDebug() << "Drag Move";
}

void GridDisplay::dragEnterEvent(QDragEnterEvent* event) {
	if ( event->proposedAction() == Qt::CopyAction ) {
		event->acceptProposedAction();
	}
}

void GridDisplay::dropEvent(QDropEvent *event) {
		//TODO: NEED TO ADD IN FOR INIT
/*
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
	*/
}

void GridDisplay::paintEvent(QPaintEvent* /* event */) {

	QPainter painter;
	painter.begin(this);

	//Find density
	int maxDensity = 0;
	int minDensity = 100;
	for (int i=0; i < grid_->getFiles().size(); i++) {
		if(grid_->getFilesAt(i).size() > maxDensity)
		{
			maxDensity = grid_->getFilesAt(i).size();
		}
		else if (grid_->getFilesAt(i).size() < minDensity) 
		{
			minDensity = grid_->getFilesAt(i).size();
		}
	}

	Colormap *map = Colormap::factory(Colormap::GreyScale);
	for (int i=0; i < grid_->getHeight(); i++) {
		for (int j=0; j < grid_->getWidth(); j++) {
			int k = i * grid_->getHeight() + j;

			QRect	 myr(i*_cellSize,j*_cellSize,_cellSize,_cellSize);
			QLine	 myl1(i*_cellSize,j*_cellSize, i*_cellSize, j*_cellSize + _cellSize);
			QLine	 myl2(i*_cellSize,j*_cellSize, i*_cellSize+_cellSize, j*_cellSize );

			if ( grid_->getFilesAt(k).size() == 0 ) {
				QColor color(map->getRed(125), map->getGreen(0), map->getBlue(0));
				painter.setBrush(color);
			} else {
				int c = int(grid_->getFilesAt(k).size() / float(maxDensity) * (map->getDepth()-1));
				QColor color(map->getRed(c), map->getGreen(c), map->getBlue(c));
				painter.setBrush(color);
			}

			painter.setPen(Qt::NoPen);
			painter.drawRect(myr);

			painter.setPen(Qt::black);
			painter.drawLine(myl1);
			painter.drawLine(myl2);

			painter.setBrush(Qt::red);
			QRect newr( grid_->getXPos() * _cellSize + _cellSize / 4,
				grid_->getYPos() * _cellSize + _cellSize / 4,
				_cellSize - _cellSize / 2,
				_cellSize-_cellSize / 2);
			painter.drawRect(newr);
		}
	}
	delete map;
	painter.end();
}