#include "MainWindow.h"

MainWindow::MainWindow(Grid* grid_) {
	_library = MusicCollection::getInstance();
	_dataGrid = grid_;

	createWindow();
	createActions();
	createMenus();
	createToolbars();

	_preferencesDialog = NULL;

	// _midi = new MidiListener();
	//openDefaultiTunes();
	isFullScreenMouse = false;
	
}

MainWindow::~MainWindow() {
	_library->empty();
	delete _library;
	// delete _midi;
}

/*
 * ---------------------------------------------------------
 * Slots
 * ---------------------------------------------------------
 */
void MainWindow::openiTunesLibrary() {
	QString fileName =
		QFileDialog::getOpenFileName(this, tr("Open iTunes Library File"),
					 QDir::currentPath(),
					 tr("iTunes XML Library File(*.xml)"));
	if (fileName.isEmpty())
		return;


	QFile file(fileName);
	if (!file.open(QFile::ReadOnly | QFile::Text)) {
		QMessageBox::warning(this, tr("Music Collection"),
					 tr("Cannot read file %1:\n%2.")
						 .arg(fileName)
						 .arg(file.errorString()));
		return;
	}

	statusBar()->showMessage(tr("Parsing iTunes Library......"));

	//if ( Parser::parse( Parser::ParserTypes.ITUNES, file, _library ) ) {
	_library->empty();
	if ( Parser::parse( file, _library ) ) {
		statusBar()->showMessage(tr("File loaded"), 2000);
		qDebug() << "Library updated";
		emit libraryUpdated();
	}
}
/*
** TODO: Remove me, otherwise pain and suffering are abound.
*/

void MainWindow::openDefaultiTunes()
{
	QString fileName = "C:\\Documents and Settings\\NJL\\My Documents\\My Music\\iTunes\\iTunes Music Library.xml";
	QFile file(fileName);
	if (!file.open(QFile::ReadOnly | QFile::Text)) {
		QMessageBox::warning(this, tr("Music Collection"),
					 tr("Cannot read file %1:\n%2.")
						 .arg(fileName)
						 .arg(file.errorString()));
		return;
	}

	statusBar()->showMessage(tr("Parsing iTunes Library......"));

	//if ( Parser::parse( Parser::ParserTypes.ITUNES, file, _library ) ) {
	_library->empty();
	if ( Parser::parse( file, _library ) ) {
		statusBar()->showMessage(tr("File loaded"), 2000);
		qDebug() << "Library updated";
		emit libraryUpdated();
	}

}
void MainWindow::openMarXmlLibrary() {

}

void MainWindow::openMarCsvLibrary() {

}

void MainWindow::cancelButton() {
	emit cancelButtonSignal();
}

void MainWindow::openSavedGrid()
{
  QString fileName = QFileDialog::getOpenFileName(this);
  cout << "Emit" << endl;
  emit openPredictGridFile(fileName);
}

void MainWindow::saveCurrentGrid()
{
	QString fileName = QFileDialog::getSaveFileName(this);
	cout << "Save" << endl;
	emit savePredictGridFile(fileName);
}

void MainWindow::updateCurrentlyPlaying(MusicTrack *track) {
	qDebug() << "Update play box";
	//_playBox->updateCurrentlyPlaying(track);
}

void MainWindow::openPreferences() {
	if ( !_preferencesDialog ) {
		_preferencesDialog = new PreferencesDialog(this);
		//connect(prefDialog, SIGNAL(prefAction), this, SLOT(preferences);
		_preferencesDialog->addPage(new iTunesPage);
	}
	_preferencesDialog->show();
	_preferencesDialog->activateWindow();
}

void MainWindow::display() {
	_library->display();
}

void MainWindow::about() {
	QMessageBox::about(this, tr("About Music Collection"),
		tr("The <b>Music Collection</b> parses and displaysiTunes Xml Library files."));
}

void MainWindow::saveiTunesLibrary() {
	QString fileName =
		QFileDialog::getSaveFileName(this, tr("Save iTunes Library File"),
					 QDir::currentPath(),
					 tr("iTunes XML Library File(*.xml)"));
	if (fileName.isEmpty())
		return;


	QFile file(fileName);
	if (!file.open(QFile::WriteOnly | QFile::Text)) {
		QMessageBox::warning(this, tr("Music Collection"),
					 tr("Cannot write to file %1:\n%2.")
						 .arg(fileName)
						 .arg(file.errorString()));
		return;
	}

	statusBar()->showMessage(tr("Saving iTunes Library......"));

	iTunesXmlWriter writer(file);
	writer << _library;
	statusBar()->showMessage(tr("File Saved"), 2000);
	qDebug() << "Library save";
}
void MainWindow::changedPlayMode()
{
	emit playModeChanged();
}

void MainWindow::fullScreenMode(bool mode)
{
	isFullScreenMouse = mode;
	if(mode) 
	{
		showMaximized();
		_toolbar->hide();
		menuBar()->hide();
		setWindowFlags(Qt::FramelessWindowHint);
		//#ifdef Q_WS_MACX
		//	HideMenuBar();
		//#endif
		grabKeyboard();

	}
	else
	{
		showNormal();
		_toolbar->show();
		menuBar()->show();
		setWindowFlags((Qt::WindowFlags)(~Qt::FramelessWindowHint));
	//	#ifdef Q_WS_MACX
	//		ShowMenuBar();
	//	#endif
		releaseKeyboard();
	}
	show();
}


/*
 * ---------------------------------------------------------
 * Setups 
 * ---------------------------------------------------------
 */
void MainWindow::createWindow() {


	QWidget *w = new QWidget;
	setCentralWidget(w);

	_playBox = new PlayBox(this);	

	_tracklist = new Tracklist(this);
	_playlist = new Playlist(_tracklist, this);
	

	_display = new GridDisplay(600, _tracklist, _dataGrid, this);

	QGridLayout *gridLayout = new QGridLayout;
				//fromRow, fromCol, rowSpan, colSpan
	gridLayout->addWidget(	_display,			0, 0, 4, 1);
    gridLayout->addWidget(new QLabel(tr("Playlists")),	0, 1, 1, 1);
	gridLayout->addWidget(	_playlist,			1, 1, 1, 1);
    gridLayout->addWidget(new QLabel(tr("Track list")), 	2, 1, 1, 1);
	gridLayout->addWidget(	_tracklist,			3, 1, 1, 1);
	gridLayout->addWidget(	_playBox,			4, 0, 1, 2);

	connect(_display, SIGNAL(playingTrack(MusicTrack*)), 
		 this, SLOT(updateCurrentlyPlaying(MusicTrack*)));

	connect(this, SIGNAL(libraryUpdated()), _playlist, SLOT(updatePlaylist()));
	connect(this, SIGNAL(libraryUpdated()), _display, SLOT(reload()));
	connect(this, SIGNAL(openPredictGridFile(QString)), _display, SLOT(openPredictionGrid(QString)));
	connect(this, SIGNAL(savePredictGridFile(QString)), _display, SLOT(savePredictionGrid(QString)));
	connect(this, SIGNAL(playModeChanged()), _display, SLOT(playModeChanged()));
	connect(this, SIGNAL(cancelButtonSignal()), _display, SLOT(cancelButton()));
	connect(_display, SIGNAL(fullScreenMode(bool)), this, SLOT(fullScreenMode(bool)));
	connect(this, SIGNAL(fullScreenModeOff()), _display, SLOT(fullScreenMouse()));
	connect(_playlist, SIGNAL(SelectedPlaylist(QString)), _display, SLOT(playlistSelected(QString)));

	w->setLayout(gridLayout);
	statusBar()->showMessage(tr("Ready"));
}

void MainWindow::createActions() {

	_openiTunesAction = new QAction(QIcon(":/images/iTunes.png"),
					tr("Open i&Tunes Library..."), this);
	_openiTunesAction->setShortcut(tr("Ctrl+T"));
	_openiTunesAction->setStatusTip(tr("Open iTunes Library file"));
	connect(_openiTunesAction, SIGNAL(triggered()), this, SLOT(openiTunesLibrary()));

	_openPreferencesAction = new QAction(QIcon(":/images/Preferences.png"),
					tr("&Preferences..."), this);
	_openPreferencesAction->setShortcut(tr("Ctrl+P"));
	_openPreferencesAction->setStatusTip(tr("Open Preferences"));
	connect(_openPreferencesAction, SIGNAL(triggered()), this, SLOT(openPreferences()));

	_openXmlAction = new QAction(tr("Open Mar&Xml Library..."), this);
	_openXmlAction->setShortcut(tr("Ctrl+X"));
	connect(_openXmlAction, SIGNAL(triggered()), this, SLOT(openMarXmlLibrary()));
	
	_openCsvAction = new QAction(tr("&Open MarC&sv Library..."), this);
	_openCsvAction->setShortcut(tr("Ctrl+s"));
	connect(_openCsvAction, SIGNAL(triggered()), this, SLOT(openMarCsvLibrary()));

	_exitAction = new QAction(tr("E&xit"), this);
	_exitAction->setShortcut(tr("Ctrl+Q"));
	connect(_exitAction, SIGNAL(triggered()), this, SLOT(close()));

	_coutAction = new QAction(tr("&Cout Library"), this);
	_coutAction->setShortcut(tr("Ctrl+E"));
	connect(_coutAction, SIGNAL(triggered()), this, SLOT(display()));

	_predictAction = new QAction(QIcon(":/images/predict.png"),
				     tr("&Predict"), this);
	_predictAction->setShortcut(tr("Ctrl+3"));
	_predictAction->setStatusTip(tr("Predict the the placement of the prediction tracks"));
	connect(_predictAction, SIGNAL(triggered()), _display, SLOT(predict()));

	_extractAction = new QAction(QIcon(":/images/extract.png"),
				     tr("&Extract"), this);
	_extractAction->setShortcut(tr("Ctrl+1"));
	_extractAction->setStatusTip(tr("Extract features from the defined training tracks"));
	connect(_extractAction, SIGNAL(triggered()), _display, SLOT(extract()));

	_trainingAction = new QAction(QIcon(":/images/train.png"),
				      tr("&Train"), this);
	_trainingAction->setShortcut(tr("Ctrl+2"));
	_trainingAction->setStatusTip(tr("Train the grid using the defined training tracks"));
	connect(_trainingAction, SIGNAL(triggered()), _display, SLOT(train()));

	_aboutAction = new QAction(tr("&About"), this);
	connect(_aboutAction, SIGNAL(triggered()), this, SLOT(about()));

	_saveAction = new QAction(tr("&Save"), this);
	_saveAction->setShortcut(tr("Ctrl+S"));
	connect(_saveAction, SIGNAL(triggered()), this, SLOT(saveiTunesLibrary()));

	_saveGridAction = new QAction(tr("&Save Grid"), this);
	connect(_saveGridAction, SIGNAL(triggered()), this, SLOT(saveCurrentGrid()) );

	_loadGridAction = new QAction(tr("&Load Saved Grid"),this);
	connect(_loadGridAction, SIGNAL(triggered()), this, SLOT(openSavedGrid()));
	_playModeAction = new QAction(tr("&Continuous"), this);
	connect(_playModeAction, SIGNAL(triggered()), this, SLOT(changedPlayMode()));

	_cancelAction = new QAction(tr("&Cancel Action"), this);
	connect(_cancelAction, SIGNAL(triggered()), this, SLOT(cancelButton()));

	_initAction = new QAction(tr("&Initlize Grid"), this);
	connect(_initAction, SIGNAL(triggered()), _display, SLOT(init()));
	
	_normHashAction = new QAction(tr("Open Saved Hash"), this);
	connect(_normHashAction, SIGNAL(triggered()), _display, SLOT(normHashLoad()));

	_fullScreenAction = new QAction (tr("&Full Screen Mouse Mode"), this);
	connect(_fullScreenAction, SIGNAL(triggered()), _display, SLOT(fullScreenMouse()));

	_colourMapMode = new QAction (tr("&Colour Mapping Mode"),this);
	connect(_colourMapMode, SIGNAL(triggered()), _display, SLOT(colourMapMode()));
}

void MainWindow::createMenus() {

	gridHeightLabel = new QLabel("Grid Height: ");
	gridWidthLabel = new QLabel("Grid Width: ");
	gridWidth = new QLineEdit(this);
	gridWidth->setMinimumWidth(30);
	gridWidth->setMaximumWidth(30);
	gridWidth->setInputMask("99");
	gridWidth->setText("12");
	gridHeight = new QLineEdit(this);
	gridHeight->setInputMask("99");
	gridHeight->setMinimumWidth(30);
	gridHeight->setMaximumWidth(30);
	gridHeight->setText("12");

	_fileMenu = menuBar()->addMenu(tr("&File"));

	_openMenu = new QMenu(tr("Open..."));
	_openMenu->addAction(_openiTunesAction);
	_openMenu->addAction(_openXmlAction);
	_openMenu->addAction(_openCsvAction);
	_openMenu->addAction(_loadGridAction);
	_openMenu->addAction(_normHashAction);


	QAction *openAction = _fileMenu->addAction(tr("&Open..."));
	openAction->setMenu(_openMenu);	
	
	_fileMenu->addAction(_saveAction);
	_fileMenu->addAction(_saveGridAction);
	_fileMenu->addAction(_playModeAction);
	_fileMenu->addAction(_exitAction);
	
	_debugMenu = menuBar()->addMenu(tr("&Debug"));
	_debugMenu->addAction(_coutAction);

	menuBar()->addSeparator();

	_helpMenu = menuBar()->addMenu(tr("&Help"));
	_helpMenu->addAction(_aboutAction);
}

void MainWindow::createToolbars() {

	QWidget *gridWidthWidget = new QWidget();
  QHBoxLayout *gridWidthLayout = new  QHBoxLayout;
  gridWidthLayout->addWidget(gridWidthLabel);
  gridWidthLayout->addWidget(gridWidth);
  gridWidthWidget->setLayout(gridWidthLayout);

  QWidget *gridHeightWidget = new QWidget();
  QHBoxLayout *gridHeightLayout = new  QHBoxLayout;
  gridHeightLayout->addWidget(gridHeightLabel);
  gridHeightLayout->addWidget(gridHeight);
  gridHeightWidget->setLayout(gridHeightLayout);


	_toolbar = addToolBar(tr("Tools"));
	_toolbar->addAction(_openiTunesAction);
	_toolbar->addSeparator();
	_toolbar->addAction(_openPreferencesAction);
	_toolbar->addSeparator();
	_toolbar->addAction(_extractAction);
	_toolbar->addAction(_trainingAction);
	_toolbar->addAction(_predictAction);
	_toolbar->addSeparator();
	_toolbar->addAction(_cancelAction);
	_toolbar->addSeparator();
	_toolbar->addAction(_initAction);
	_toolbar->addSeparator();
	_toolbar->addAction(_fullScreenAction);
	_toolbar->addSeparator();
	_toolbar->addAction(_colourMapMode);
	_toolbar->addSeparator();
	_toolbar->addWidget(gridHeightWidget);
	_toolbar->addWidget(gridWidthWidget);

	//TODO: CLEANUP
	connect(gridWidth, SIGNAL(textChanged(QString)), _display, SLOT(setXGridSize(QString)));
    connect(gridHeight, SIGNAL(textChanged(QString)), _display, SLOT(setYGridSize(QString)));

}

void MainWindow::keyReleaseEvent(QKeyEvent *keyEvent)
{
	if(isFullScreenMouse) 
	{
	cout << "in" << endl;
	emit fullScreenModeOff();
	fullScreenMode(false);
	}
}
