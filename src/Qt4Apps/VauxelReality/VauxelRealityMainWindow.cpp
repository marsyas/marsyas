
#include "VauxelRealityMainWindow.h"

#include "MusicCollectionIO.h"
#include "iTunesPage.h"
#include "GlobeQGLViewer.h"
#include "MarExtractor.h"

VauxelRealityMainWindow::VauxelRealityMainWindow() {
  _library = MusicCollection::getInstance();

  createWindow();
  createActions();
  createMenus();
  createToolbars();

  _preferencesDialog = NULL;

  // _midi = new MidiListener();
}

VauxelRealityMainWindow::~VauxelRealityMainWindow() {
  _library->empty();
  delete _library;
  // delete _midi;
}

/*
 * ---------------------------------------------------------
 * Setups
 * ---------------------------------------------------------
 */
void VauxelRealityMainWindow::createWindow() {
  //setWindowTitle(tr("Music Collection"));
  //resize(480, 320);

  QWidget *w = new QWidget;
  setCentralWidget(w);

  /*
    QPushButton *extract	= new QPushButton(tr("Extract"));
    QPushButton *train = new QPushButton(tr("Train"));
    QPushButton *predict = new QPushButton(tr("Predict"));
    _playLabel = new QLabel("Hello");
    gridLayout->addWidget(extract, 0, 0);
    gridLayout->addWidget(train, 0, 1);
    gridLayout->addWidget(predict, 0, 2);
    gridLayout->addWidget(playLabel, 1, 0, 1, 3);
    */

  _playBox = new PlayGroupBoxW(this);

  _tracklist = new TrackListW(this);
  _playlist = new PlayListW(_tracklist, this);

  _display = new GlobeQGLViewer(this);
  // _display = new Grid(600, _tracklist, this);
  //_display = new Keypad(600, _tracklist, this);

  _extractor = new MarExtractor();

  QGridLayout *gridLayout = new QGridLayout;
  //fromRow, fromCol, rowSpan, colSpan
  gridLayout->addWidget(	_display,			0, 0, 4, 1);
  gridLayout->addWidget(new QLabel(tr("Playlists")),	0, 1, 1, 1);
  gridLayout->addWidget(	_playlist,			1, 1, 1, 1);
  gridLayout->addWidget(new QLabel(tr("Track list")), 	2, 1, 1, 1);
  gridLayout->addWidget(	_tracklist,			3, 1, 1, 1);
  gridLayout->addWidget(	_playBox,			4, 0, 1, 2);

  //connect(extract, SIGNAL(clicked()), margrid, SLOT(extract()));
  //connect(train, SIGNAL(clicked()), margrid, SLOT(train()));
  //connect(predict, SIGNAL(clicked()), margrid, SLOT(predict()));
  //connect(margrid, SIGNAL(playingFile(QString)), this, SLOT(playingFile(MusicTrack*)));

////Jen   connect( _display, SIGNAL(qglObjectSelected(QGLObject *)),
////Jen            this, SLOT(updateCurrentlyPlaying(QGLObject *)));

  ////Jen connect(_display, SIGNAL(playingTrack(MusicTrack*)),
  ////Jen        this, SLOT(updateCurrentlyPlaying(MusicTrack*)));
  /* connect(_midi, SIGNAL(kaossXYEvent(unsigned char, unsigned char)),
      _display, SLOT(midiXYEvent(unsigned char, unsigned char)));
  connect(_midi, SIGNAL(kaossMuteEvent(bool)),
        _display, SLOT(midiPlaylistEvent(bool)));
  */

  connect(this, SIGNAL(libraryUpdated()), _playlist, SLOT(updatePlaylist()));
  ////Jen connect(this, SIGNAL(libraryUpdated()), _display, SLOT(reload()));

  w->setLayout(gridLayout);
  statusBar()->showMessage(tr("Ready"));
}

void VauxelRealityMainWindow::createActions() {

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
  ////Jen connect(_predictAction, SIGNAL(triggered()), _display, SLOT(predict()));

  _extractAction = new QAction(QIcon(":/images/extract.png"),
                               tr("&Extract"), this);
  _extractAction->setShortcut(tr("Ctrl+1"));
  _extractAction->setStatusTip(tr("Extract features from the defined training tracks"));
  ////Jen connect(_extractAction, SIGNAL(triggered()), _display, SLOT(extract()));

  _trainingAction = new QAction(QIcon(":/images/train.png"),
                                tr("&Train"), this);
  _trainingAction->setShortcut(tr("Ctrl+2"));
  _trainingAction->setStatusTip(tr("Train the grid using the defined training tracks"));
  ////Jen connect(_trainingAction, SIGNAL(triggered()), _display, SLOT(train()));

  _aboutAction = new QAction(tr("&About"), this);
  connect(_aboutAction, SIGNAL(triggered()), this, SLOT(about()));

  _saveAction = new QAction(tr("&Save"), this);
  _coutAction->setShortcut(tr("Ctrl+S"));
  connect(_saveAction, SIGNAL(triggered()), this, SLOT(saveiTunesLibrary()));
}

void VauxelRealityMainWindow::createMenus() {
  _fileMenu = menuBar()->addMenu(tr("&File"));

  _openMenu = new QMenu(tr("Open..."));
  _openMenu->addAction(_openiTunesAction);
  _openMenu->addAction(_openXmlAction);
  _openMenu->addAction(_openCsvAction);


  QAction *openAction = _fileMenu->addAction(tr("&Open..."));
  openAction->setMenu(_openMenu);

  _fileMenu->addAction(_saveAction);
  _fileMenu->addAction(_exitAction);

  _debugMenu = menuBar()->addMenu(tr("&Debug"));
  _debugMenu->addAction(_coutAction);

  menuBar()->addSeparator();

  _helpMenu = menuBar()->addMenu(tr("&Help"));
  _helpMenu->addAction(_aboutAction);
}

void VauxelRealityMainWindow::createToolbars() {
  _toolbar = addToolBar(tr("Tools"));
  _toolbar->addAction(_openiTunesAction);
  _toolbar->addSeparator();
  _toolbar->addAction(_openPreferencesAction);
  _toolbar->addSeparator();
  _toolbar->addAction(_extractAction);
  _toolbar->addAction(_trainingAction);
  _toolbar->addAction(_predictAction);
}


/*
 * ---------------------------------------------------------
 * Slots
 * ---------------------------------------------------------
 */
void
VauxelRealityMainWindow::openiTunesLibrary()
{
  QString fileName =
    QFileDialog::getOpenFileName(this, tr("Open iTunes Library File"),
                                 QDir::currentPath(),
                                 tr("iTunes XML Library File(*.xml)"));
  if (fileName.isEmpty())
    return;


  QFile file(fileName);
  if (!file.open(QFile::ReadOnly | QFile::Text))
  {
    QMessageBox::warning(this, tr("Music Collection"),
                         tr("Cannot read file %1:\n%2.")
                         .arg(fileName)
                         .arg(file.errorString()));
    return;
  }

  statusBar()->showMessage(tr("Parsing iTunes Library......"));

  //if ( Parser::parse( Parser::ParserTypes.ITUNES, file, _library ) ) {
  _library->empty();
  if ( MusicCollectionIO::parse( file, _library ) )
  {
    statusBar()->showMessage(tr("File loaded"), 2000);
    qDebug() << "Library updated";
    emit libraryUpdated();
  }
}

void
VauxelRealityMainWindow::openMarXmlLibrary()
{

}

void
VauxelRealityMainWindow::openMarCsvLibrary()
{

}

void
VauxelRealityMainWindow::updateCurrentlyPlaying(MusicTrack *track)
{
  qDebug() << "Update play box";
  _playBox->updateCurrentlyPlaying(track);
}

void
VauxelRealityMainWindow::openPreferences()
{
  if ( !_preferencesDialog ) {
    _preferencesDialog = new PreferencesDialog(this);
    //connect(prefDialog, SIGNAL(prefAction), this, SLOT(preferences);
    _preferencesDialog->addPage(new iTunesPage);
  }
  _preferencesDialog->show();
  _preferencesDialog->activateWindow();
}

void
VauxelRealityMainWindow::display()
{
  _library->display();
}

void
VauxelRealityMainWindow::about()
{
  QMessageBox::about(this, tr("About Music Collection"),
                     tr("The <b>Music Collection</b> parses and displaysiTunes Xml Library files."));
}

void
VauxelRealityMainWindow::saveiTunesLibrary()
{
  QString fileName =
    QFileDialog::getSaveFileName(this, tr("Save iTunes Library File"),
                                 QDir::currentPath(),
                                 tr("iTunes XML Library File(*.xml)"));
  if (fileName.isEmpty())
    return;


  QFile file(fileName);
  if (!file.open(QFile::WriteOnly | QFile::Text))
  {
    QMessageBox::warning(this, tr("Music Collection"),
                         tr("Cannot write to file %1:\n%2.")
                         .arg(fileName)
                         .arg(file.errorString()));
    return;
  }

  statusBar()->showMessage(tr("Saving iTunes Library......"));


  //iTunesXmlWriter writer(file);
  //writer << _library;
  //	statusBar()->showMessage(tr("File Saved"), 2000);
  //	qDebug() << "Library save";

  if ( MusicCollectionIO::write( file, _library ) )
  {
    statusBar()->showMessage(tr("File Saved"), 2000);
    qDebug() << "Library save";
  }

}
