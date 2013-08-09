/**
 * MainWindow
 */

#include "MainWindow.h"

MainWindow::MainWindow()
  :QMainWindow()
{
  // actions
  fileNewAction = new QAction( this );
  helpAboutAction = new QAction( this);
  ExitApplicationAction = new QAction( this);
  openAction = new QAction(this);
  saveAction = new QAction(this);

  //MenuBars
  MenuBar = new QMenuBar(this);
  setMenuBar(MenuBar);

  fileMenu = new QMenu( this );
  fileMenu->addAction(fileNewAction);;
  fileMenu->addAction(openAction);
  fileMenu->addAction(saveAction);
  fileMenu->addAction(ExitApplicationAction);
  fileMenu->insertSeparator(ExitApplicationAction);
  menuBar()->addMenu( fileMenu);
  helpMenu = new QMenu( this );
  helpMenu->addAction(helpAboutAction);
  menuBar()->addMenu(helpMenu);

  //Add Text
  fileNewAction->setText(tr("&New MarSystem") );
  helpAboutAction->setText(tr( "&About") );
  ExitApplicationAction->setText( tr("&Quit") );
  openAction->setText( tr("&Open") );
  saveAction->setText( tr("&Save") );
  fileMenu->setTitle( tr("&File") );
  helpMenu->setTitle( tr("&Help") );

  // signals and slots connections
  connect( fileNewAction, SIGNAL( activated() ), this, SLOT( fileNew() ) );
  connect( helpAboutAction, SIGNAL( activated() ), this, SLOT( helpAbout() ) );
  connect( ExitApplicationAction, SIGNAL( activated() ), this, SLOT( exitApp() ) );
  connect( openAction, SIGNAL( activated() ), this, SLOT( openFile() ) );
  connect( saveAction, SIGNAL( activated() ), this, SLOT( saveFile())) ;

  init();


}

// called by the constructor from the QT generated .ui code...
void
MainWindow::init()
{
  cout<<"init()"<<endl;

  QWidget* mainBox = new QWidget;
  QGridLayout* mainLayout = new QGridLayout;

  theCanvas = new MarCanvas(this);
  theCanvas->setMinimumSize(800,600);



  QGroupBox* westPanel = new QGroupBox;
  QVBoxLayout* westLayout = new QVBoxLayout;

  QComboBox* collectionNameEdit = new QComboBox(westPanel);

  QLineEdit* widgetNameEdit = new QLineEdit(QString(""),westPanel);
  westLayout->addWidget(collectionNameEdit);
  westLayout->addWidget(widgetNameEdit);

  QGroupBox* collectionBox = new QGroupBox("Composites");
  QVBoxLayout* collections = new QVBoxLayout;

  QGroupBox* regularBox = new QGroupBox("MarSystems");
  QVBoxLayout* regulars = new QVBoxLayout;


  //Setup all the buttons that will add widgets
  WidgetButton* addMoreGain = new WidgetButton(QString("Add Gain"),
      QString(MarSystemNodeFactory::GAIN_TYPE.c_str()),
      collectionBox);
  WidgetButton* addSeries = new WidgetButton(QString("Add Series"),
      QString(MarSystemNodeFactory::SERIES_TYPE.c_str()),
      collectionBox);
  WidgetButton* addFanout = new WidgetButton(QString("Add Fanout"),
      QString(MarSystemNodeFactory::FANOUT_TYPE.c_str()),
      collectionBox);



  connect(addSeries, SIGNAL(reemitClick(QString)),
          theCanvas, SLOT(addNewMarSystemNode(QString)));
  connect(addFanout, SIGNAL(reemitClick(QString)),
          theCanvas, SLOT(addNewMarSystemNode(QString)));
  connect(addMoreGain, SIGNAL(reemitClick(QString)),
          theCanvas, SLOT(addNewMarSystemNode(QString)));

  collections->addWidget(addSeries);
  collections->addWidget(addFanout);
  //collections->addWidget(addMoreGain);
  collectionBox->setLayout(collections);

  regulars->addWidget(addMoreGain);
  regularBox->setLayout(regulars);

  westLayout->addWidget(collectionBox);
  westLayout->addWidget(regularBox);
  westLayout->addStretch(1);
  westPanel->setLayout(westLayout);


  //seup the scroll area over theCanvas
  QScrollArea* scrollArea = new QScrollArea(this);
  scrollArea->setWidget(theCanvas);

  mainLayout->addWidget(westPanel,0,0);
  mainLayout->addWidget(scrollArea,0,1);
  mainLayout->setColumnStretch(1, 10);
  mainBox->setLayout(mainLayout);
  setCentralWidget(mainBox);
}


// create a new PatchContainer, with a blank MSView and ControlsView...
void
MainWindow::fileNew()
{
  cout << "fileNew()" << endl;
  //if (myWorkSpace == NULL) {
  //  cout << "workspace is null" << endl;
  //}
  //PatchContainer* p = new PatchContainer( myWorkSpace, "New Patch",
  //				  Qt::WDestructiveClose | Qt::WGroupLeader );
  //p->setCaption("New Patcher");
  //p->setIcon( QPixmap("document.xpm") );
  //p->show();
}


void
MainWindow::helpAbout()
{
  QMessageBox::about( this, "About Marsyas",
                      "Marsyas is a collection of objects that can be used for various\n"
                      "sound analysis and synthesis tasks.\n\n"

                      "George Tzanetakis is the main designer and programmer\n"
                      "behind Marsyas. See the file AUTHORS for a list of \n"
                      "additional developers.\n\n"

                      "Copyright (C) 2000 George Tzanetakis <gtzan@cs.princeton.edu>" );
}

void MainWindow::exitApp()
{
  QApplication::exit(0);
}


void MainWindow::closeEvent( QCloseEvent * )
{
  exitApp();
}


// This function opens a typical dialog box and asks the user to specify a Marsystem file to open.
void MainWindow::openFile()
{
  cout << "mainform(): open file" << endl;
  QString s, dirPath;

  QFileDialog fd( this, QString("file dialog"),QString(""),
                  QString("Marsyas Plugin File (*.mpl *.txt)"));

  QStringList files =
    QFileDialog::getOpenFileNames( this,
                                   QString("Choose a File to Load"),
                                   QString(""),
                                   QString("Marsyas Plugin File (*.mpl *.txt)"));
  QStringList mlist = files;

  //Only load the first one
  if(mlist.size()>0) {
    QString loadString = mlist.front();
    theCanvas->loadFromFile(loadString.toStdString());
    theCanvas->update();
  }
}


// saves the current marsystem to the filename in use...
void MainWindow::saveFile()
{
  cout<<"Save File"<<endl;
  QString filename =
    QFileDialog::getSaveFileName(this,
                                 QString("Choose a file to Save to"),
                                 QString(""),
                                 QString("Marsyas Plugin File (*.mpl *.txt)"));
  if(filename.length()>0) {
    theCanvas->saveMarSystem(filename.toStdString());
  }
}


