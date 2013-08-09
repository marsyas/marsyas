#include "MainWindow.h"

#include <QFileDialog>
#include <QToolBar>
#include <QMenuBar>
#include <QMenu>
#include <QStatusBar>

MainWindow::MainWindow(Grid* grid_) {
  _library = MusicCollection::getInstance();
  _dataGrid = grid_;

  createWindow();
  createActions();
  createMenus();
  createToolbars();

  // _midi = new MidiListener();
  openDefaultiTunes();
  isFullScreenMouse = false;

}

MainWindow::~MainWindow() {
  _library->empty();
  delete _library;
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

  _library->empty();
  if ( Parser::parse( file, _library ) ) {
    statusBar()->showMessage(tr("File loaded"), 2000);
    qDebug() << "Library updated";

    //output the iTunes library location for easy loading later
    QFile nameFile("libraryName.mln");
    nameFile.open(QFile::ReadWrite | QFile::Truncate);
    nameFile.write(fileName.toStdString().c_str());

    emit libraryUpdated();
  }
}

//
// Open a Marsyas .mf collection file
//
void MainWindow::openCollectionFile() {

  QString fileName =
    QFileDialog::getOpenFileName(this, tr("Open Collection File"),
                                 QDir::currentPath(),
                                 tr("Marsyas Collection File(*.mf)"));
  if (fileName.isEmpty())
    return;

  _dataGrid->setTrainFile(fileName);
}

/*
** Load the default iTunes library if it exists
*/

void MainWindow::openDefaultiTunes()
{
  QString fileName;
  QFile nameFile("libraryName.mln");
  if(nameFile.open(QFile::ReadOnly | QFile::Text))
  {
    fileName = nameFile.readLine();
    nameFile.close();
  }
  else
  {
    nameFile.close();
    return;
  }

  QFile file(fileName);
  if (!file.open(QFile::ReadOnly | QFile::Text)) {
    QMessageBox::warning(this, tr("Music Collection"),
                         tr("Cannot read file %1:\n%2.")
                         .arg(fileName)
                         .arg(file.errorString()));
    return;
  }

  statusBar()->showMessage(tr("Parsing iTunes Library......"));

  _library->empty();
  if ( Parser::parse( file, _library ) ) {
    statusBar()->showMessage(tr("File loaded"), 2000);
    qDebug() << "Library updated";
    emit libraryUpdated();
  }

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

void MainWindow::display() {
  _library->display();
}

void MainWindow::about() {
  QMessageBox::about(this, tr("MarGrid2"),
                     tr(" A music player based on a self organized map."));
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

    _toolbar->hide();
    menuBar()->hide();
    //setWindowFlags(Qt::FramelessWindowHint);
    //showMaximized();
    showFullScreen();
    grabKeyboard();

  }
  else
  {

    _toolbar->show();
    menuBar()->show();
    //	setWindowFlags((Qt::WindowFlags)(~Qt::FramelessWindowHint));
    showNormal();
    releaseKeyboard();
  }
  show();
}

void MainWindow::optionsDialogTriggered()
{
  OptionsDialog *dialog = new OptionsDialog(_dataGrid);
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
  _colourMapDisplay = new ColourMapDisplay(this);


  _display = new GridDisplay(600, _tracklist, _dataGrid, this);

  QGridLayout *gridLayout = new QGridLayout;
  //fromRow, fromCol, rowSpan, colSpan
  gridLayout->addWidget(	_display,			0, 0, 4, 1);

  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->addWidget(new QLabel(tr("Playlists")));
  vbox->addWidget(_playlist);
  vbox->addWidget(new QLabel(tr("Track list")));
  vbox->addWidget(_tracklist);
  vbox->addWidget(_colourMapDisplay);
  gridLayout->addLayout(vbox,1,1,1,1);

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
  connect(_display, SIGNAL(updateColourMap(int *, int)), _colourMapDisplay, SLOT(updateSquare(int *, int)));
  connect(this, SIGNAL(resetGrid()), _display, SLOT(resetGrid()));

  w->setLayout(gridLayout);
  statusBar()->showMessage(tr("Ready"));
}

void MainWindow::createActions() {

  _openiTunesAction = new QAction(QIcon(":/images/iTunes.png"),
                                  tr("Open i&Tunes Library..."), this);
  _openiTunesAction->setShortcut(tr("Ctrl+T"));
  _openiTunesAction->setStatusTip(tr("Open iTunes Library file"));
  connect(_openiTunesAction, SIGNAL(triggered()), this, SLOT(openiTunesLibrary()));

  _openCollectionAction = new QAction(QIcon(":/images/openCollection.png"),
                                      tr("Open Collection File"), this);
  _openCollectionAction->setShortcut(tr("Ctrl+T"));
  _openCollectionAction->setStatusTip(tr("Open Collection file"));
  connect(_openCollectionAction, SIGNAL(triggered()), this, SLOT(openCollectionFile()));

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

  _initAction = new QAction(QIcon(":/images/init.png"), tr("&Initlize"), this);
  connect(_initAction, SIGNAL(triggered()), _display, SLOT(init()));

  _aboutAction = new QAction(tr("&About"), this);
  connect(_aboutAction, SIGNAL(triggered()), this, SLOT(about()));

  _saveGridAction = new QAction(tr("&Save Grid"), this);
  connect(_saveGridAction, SIGNAL(triggered()), this, SLOT(saveCurrentGrid()) );

  _loadGridAction = new QAction(tr("&Load Saved Grid"),this);
  connect(_loadGridAction, SIGNAL(triggered()), this, SLOT(openSavedGrid()));
  _playModeAction = new QAction(tr("&Continuous"), this);
  connect(_playModeAction, SIGNAL(triggered()), this, SLOT(changedPlayMode()));

  _cancelAction = new QAction(tr("&Cancel Action"), this);
  connect(_cancelAction, SIGNAL(triggered()), this, SLOT(cancelButton()));

  _normHashAction = new QAction(tr("Open Saved Hash"), this);
  connect(_normHashAction, SIGNAL(triggered()), _display, SLOT(hashLoad()));

  _fullScreenAction = new QAction (tr("&Full Screen Mouse Mode"), this);
  connect(_fullScreenAction, SIGNAL(triggered()), _display, SLOT(fullScreenMouse()));

  _colourMapMode = new QAction (tr("&Colour Mapping Mode"),this);
  connect(_colourMapMode, SIGNAL(triggered()), _display, SLOT(colourMapMode()));

  _resetButtonAction = new QAction (tr("&Reset"), this);
  connect (_resetButtonAction, SIGNAL(triggered()), this, SLOT(resetButtonPressed()));

  _optionsDialogAction = new QAction(tr("&Options"), this);
  connect(_optionsDialogAction, SIGNAL(triggered()), this, SLOT(optionsDialogTriggered()));
}

void MainWindow::createMenus() {

  gridHeightLabel = new QLabel("Grid Height: ");
  gridWidthLabel = new QLabel("Grid Width: ");
  gridWidth = new QLineEdit(this);
  gridWidth->setMinimumWidth(30);
  gridWidth->setMaximumWidth(30);
  gridWidth->setInputMask("999");
  gridWidth->setText("12");
  gridHeight = new QLineEdit(this);
  gridHeight->setInputMask("999");
  gridHeight->setMinimumWidth(30);
  gridHeight->setMaximumWidth(30);
  gridHeight->setText("12");

  _fileMenu = menuBar()->addMenu(tr("&File"));

  _openMenu = new QMenu(tr("Open..."));
  _openMenu->addAction(_openiTunesAction);
  _openMenu->addAction(_openCollectionAction);
  _openMenu->addAction(_loadGridAction);
  _openMenu->addAction(_normHashAction);


  QAction *openAction = _fileMenu->addAction(tr("&Open..."));
  openAction->setMenu(_openMenu);

  _fileMenu->addAction(_saveGridAction);
  _fileMenu->addSeparator();
  _fileMenu->addAction(_exitAction);

  _viewMenu = menuBar()->addMenu(tr("&View"));
  _viewMenu->addAction(_playModeAction);
  _viewMenu->addAction(_fullScreenAction);
  _viewMenu->addAction(_colourMapMode);

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
  _toolbar->addAction(_openCollectionAction);
  _toolbar->addSeparator();
  _toolbar->addAction(_extractAction);
  _toolbar->addAction(_trainingAction);
  _toolbar->addAction(_predictAction);
  _toolbar->addAction(_initAction);
  _toolbar->addSeparator();
  _toolbar->addAction(_resetButtonAction);
  _toolbar->addSeparator();
  _toolbar->addAction(_cancelAction);
  _toolbar->addSeparator();
  _toolbar->addWidget(gridHeightWidget);
  _toolbar->addWidget(gridWidthWidget);
  _toolbar->addSeparator();
  _toolbar->addAction(_optionsDialogAction);


  //TODO: CLEANUP
  connect(gridWidth, SIGNAL(textChanged(QString)), _display, SLOT(setXGridSize(QString)));
  connect(gridHeight, SIGNAL(textChanged(QString)), _display, SLOT(setYGridSize(QString)));

}

void MainWindow::keyPressEvent(QKeyEvent *keyEvent)
{

  if (keyEvent->key() == Qt::Key_Return) {
    _display->fullScreenMouse();

  } else if (keyEvent->key() == Qt::Key_Control) {
    _dataGrid->stopPlaying();

  } else _display->keyMove(keyEvent);

}

void MainWindow::keyReleaseEvent(QKeyEvent *keyEvent)
{
}

void MainWindow::resetButtonPressed()
{
  QMessageBox msgBox;
  msgBox.setText("Are you sure you want to reset the grid?");
  msgBox.setInformativeText("You will need to extract and train your collection again.");
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
  msgBox.setDefaultButton(QMessageBox::No);
  int ret = msgBox.exec();

  if(ret == QMessageBox::Yes)
  {
    emit resetGrid();
  }
}
