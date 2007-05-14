#include "mainwindow.h"

// C++ stuff for testing
#include <iostream>
using namespace std;

// basic application functions
MainWindow::MainWindow() {
	createObjects();
	createMain();
	createActions();
	createMenus();
	createToolBars();
	createStatusBar();
	readSettings();

	enableActions(MEAWS_READY_NOTHING);
}

MainWindow::~MainWindow() {
}

void MainWindow::closeEvent(QCloseEvent *event) {
	if (1) {
	//if (maybeSave()) {
		writeSettings();
		event->accept();
	} else {
		event->ignore();
	}
}

void MainWindow::about() {
	QMessageBox::about(this, tr("About Meaws"),
		tr("Meaws (Musician Evaulation and Audition for Winds and Strings) "
		"is a learning tool for musicians.  It listens to a musician and "
		"displays the music with notes coloured based on their intonation."
		));
}

void MainWindow::readSettings() {
	QSettings settings("Meaws", "Meaws");
	QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
	QSize size = settings.value("size", QSize(700, 400)).toSize();
	resize(size);
	move(pos);
}

void MainWindow::writeSettings() {
	QSettings settings("Meaws", "Meaws");
	settings.setValue("pos", pos());
	settings.setValue("size", size());
}

void MainWindow::createStatusBar() {
	statusBar()->showMessage(tr("Ready"));
}

// main window area
void MainWindow::createMain() {
	QFrame* centralFrame = new QFrame;
	setCentralWidget(centralFrame);

	// this is what displays the picture of the exercise.
	imageLabel = new QLabel;
	imageLabel->setBackgroundRole(QPalette::Base);
	imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	imageLabel->setScaledContents(false);
	imageLabel->setMaximumHeight(100);

/*
	// this is what displays our testing text.  Later on we would
	// remove textLabel and make a QT painting area or make it a picture.
	displayResults = new QtMarPlot();
	displayResults->setPlotName("Pitch");
	displayResults->setBackgroundColor(QColor(255,255,255));
	displayResults->setPixelWidth(2);
	displayAmplitude = new QtMarPlot();
	displayAmplitude->setPlotName("Amplitude");
	displayAmplitude->setBackgroundColor(QColor(255,255,255));
	displayAmplitude->setPixelWidth(2);
*/
	// we want to display the above two QLabels within our main window.
	mainLayout = new QVBoxLayout;
//	mainLayout->addWidget(imageLabel,0,Qt::AlignTop);
	mainLayout->addWidget(imageLabel,0,0);
/*
	displayLayout = new QHBoxLayout;
	displayLayout->addWidget(displayResults,0,0);
	displayLayout->addWidget(displayAmplitude,0,0);
	mainLayout->addLayout(displayLayout);
*/

/*
	cout<<"making display"<<endl;
	resultsDisplay = new MeawsDisplay();
	mainLayout->addLayout(resultsDisplay);
	centralFrame->setLayout(mainLayout);
	cout<<"... done"<<endl;

	updateTestingMethod();
*/
	//displayResults->makeupData();
//zz
}

void MainWindow::createMenus()
{
	// file menu
	fileMenu = menuBar()->addMenu(tr("&File"));
	fileMenu->addAction(newUserAct);
	fileMenu->addAction(openUserAct);
	fileMenu->addAction(saveUserAct);
	fileMenu->addAction(saveAsUserAct);
	fileMenu->addSeparator();
	fileMenu->addAction(setUserInfoAct);
	fileMenu->addSeparator();
	fileMenu->addAction(closeUserAct);
	fileMenu->addAction(exitAct);

	// exercise menu
	exerMenu = menuBar()->addMenu(tr("Exercise"));
	exerMenu->addAction(openExerciseAct);
	exerMenu->setEnabled(false);

	// testing menu
	testingMenu = menuBar()->addMenu(tr("Testing"));
	testingMenu->addAction(testingFileAct);
	testingMenu->addAction(calcExerciseAct);
//	testingMenu->addAction(playFileAct);


	menuBar()->addSeparator();
	helpMenu = menuBar()->addMenu(tr("&Help"));
	helpMenu->addAction(aboutAct);
	helpMenu->addAction(aboutQtAct);
}

void MainWindow::createToolBars() {
	fileToolBar = addToolBar(tr("User"));
	fileToolBar->addAction(newUserAct);
	fileToolBar->addAction(openUserAct);
	fileToolBar->addAction(saveUserAct);
	fileToolBar->addAction(closeUserAct);


	tempoToolBar = addToolBar(tr("Tempo"));
	tempoToolBar->addAction(openExerciseAct);
	tempoToolBar->addAction(setMetroIntroAct);

	tempoBox = new QSpinBox();
	tempoBox->setRange(30,240);
	tempoBox->setValue(60);
	tempoToolBar->addWidget(tempoBox);

	tempoSlider = new QSlider(Qt::Horizontal);
	tempoSlider->setRange(30, 240);
	tempoSlider->setValue(60);
//	tempoSlider->setMinimumWidth(150);
//	tempoSlider->setMaximumWidth(250);
	tempoToolBar->addWidget(tempoSlider);

	tempoToolBar->addAction(toggleMetroAct);
	tempoToolBar->addAction(visualMetroBeat);

	connect(tempoSlider, SIGNAL(valueChanged(int)),
		tempoBox, SLOT(setValue(int)));
	connect(tempoBox, SIGNAL(valueChanged(int)),
		tempoSlider, SLOT(setValue(int)));
/*
	connect(slider, SIGNAL(valueChanged(int)),
		this, SLOT(setMetroTempo(int)));
	connect(tempoBox, SIGNAL(valueChanged(int)),
		this, SLOT(setMetroTempo(int)));
*/
	exerciseTitle = new QLabel();
	exerciseTitle->setText("");
	otherToolBar = addToolBar(tr("Other"));
	otherToolBar->addWidget(exerciseTitle);
}

void MainWindow::createActions() {
	newUserAct = new QAction(QIcon(":/icons/new.png"), tr("&New user"), this);
	newUserAct->setShortcut(tr("Ctrl+N"));
	newUserAct->setStatusTip(tr("Create a new session"));
	connect(newUserAct, SIGNAL(triggered()), user, SLOT(newUser()));

	openUserAct = new QAction(QIcon(":/icons/open.png"), tr("&Open user..."), this);
	openUserAct->setShortcut(tr("Ctrl+O"));
	openUserAct->setStatusTip(tr("Open an existing session"));
//	connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

	saveUserAct = new QAction(QIcon(":/icons/save.png"), tr("&Save user"), this);
	saveUserAct->setShortcut(tr("Ctrl+S"));
	saveUserAct->setStatusTip(tr("Save the session to disk"));
//	connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

	saveAsUserAct = new QAction(tr("Save user &As..."), this);
	saveAsUserAct->setStatusTip(tr("Save the session under a new name"));
//	connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

	closeUserAct = new QAction(QIcon(":/icons/quit.png"), tr("&Close user"), this);
	//closeUserAct = new QAction(tr("&Close user"), this);
	closeUserAct->setShortcut(tr("Ctrl+W"));
	closeUserAct->setStatusTip(tr("Close user"));
//	connect(closeAct, SIGNAL(triggered()), this, SLOT(closeUser()));

	setUserInfoAct = new QAction(QIcon(":/icons/new.png"), tr("&User info"), this);
	setUserInfoAct->setShortcut(tr("Ctrl+U"));

	exitAct = new QAction(tr("E&xit"), this);
	exitAct->setShortcut(tr("Ctrl+Q"));
	exitAct->setStatusTip(tr("Exit the application"));
	connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

	aboutAct = new QAction(tr("&About Meaws"), this);
//	connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

	aboutQtAct = new QAction(tr("About &Qt"), this);
	aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
//	connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));


	openExerciseAct = new QAction(QIcon(":/icons/open.png"), tr("Open &Exercise..."), this);
	openExerciseAct->setShortcut(tr("Ctrl+R"));
	openExerciseAct->setStatusTip(tr("Open a new exercise"));
//	connect(openExerciseAct, SIGNAL(triggered()), this, SLOT(openExercise()));

	setMetroIntroAct = new QAction(QIcon(":/icons/triangle.png"), tr("Set metronome introduction"), this);
//	connect(setMetroIntroAct, SIGNAL(triggered()), this, SLOT(setMetroIntro()));

	visualMetroBeat = new QAction(QIcon(":/icons/circle.png"), tr("Visual metronome"), this);
	visualMetroBeat->setStatusTip(tr("Shows the beat"));

	calcExerciseAct = new QAction(QIcon(":/icons/square.png"), tr("Calculate exercise results"), this);
//	connect(calcExerciseAct, SIGNAL(triggered()), this, SLOT(calcExercise()));

	toggleMetroAct = new QAction(this);
	toggleMetroAct->setShortcut(tr("Space"));
	toggleMetroAct->setStatusTip(tr("Start"));
	toggleMetroAct->setIcon(QIcon(":/icons/player_play.png"));

	testingFileAct = new QAction(QIcon(":/icons/open.png"), tr("&Open test audio file..."), this);
	testingFileAct->setShortcut(tr("Ctrl+T"));
	testingFileAct->setStatusTip(tr("Open test audio file"));
//	connect(testingFileAct, SIGNAL(triggered()), this, SLOT(testingFile()));

	playFileAct = new QAction(QIcon(":/icons/open.png"), tr("&Play test audio file..."), this);
	playFileAct->setIcon(QIcon(":/icons/play.png"));
	playFileAct->setStatusTip(tr("Play test audio file"));
//	connect(playFileAct, SIGNAL(triggered()), this, SLOT(playFile()));
}

void MainWindow::createObjects() {
	user = new User();
	connect(user, SIGNAL(enableActions(int)), this, SLOT(enableActions(int)));

}


void MainWindow::enableActions(int state) {
	switch (state) {
	case MEAWS_READY_NOTHING:   // just opened app
		setWindowTitle(tr("Meaws"));

		saveUserAct   ->setEnabled(false);
		saveAsUserAct ->setEnabled(false);
		closeUserAct  ->setEnabled(false);
		setUserInfoAct->setEnabled(false);

		exerMenu     ->setEnabled(false);
		tempoToolBar ->setEnabled(false);
		testingMenu  ->setEnabled(false);
//		openExerciseAct ->setEnabled(false);
		break;
	case MEAWS_READY_USER:   // user selected
//		setWindowTitle(tr("Meaws - %1").arg(user->getName()));

		saveUserAct   ->setEnabled(true);
		saveAsUserAct ->setEnabled(true);
		closeUserAct  ->setEnabled(true);
		setUserInfoAct->setEnabled(true);

		testingMenu     ->setEnabled(true);
		exerMenu    ->setEnabled(true);
		openExerciseAct ->setEnabled(true);

		tempoToolBar ->setEnabled(false);

//		closeExercise();
		break;
	case MEAWS_READY_EXERCISE:   // exercise loaded
//		setupMarBackend();
//		exerciseRunning=false;

//		string audioFile = dataDir;
//		audioFile.append("sd.wav");
//zzz
/*
		metro = new Metro(visualMetroBeat, this, audioFile);
		connect(toggleMetroAct, SIGNAL(triggered()), this, SLOT(toggleExercise()));
*/
		tempoToolBar ->setEnabled(true);

		testingMenu ->setEnabled(true);
		break;
	case MEAWS_READY_AUDIO:    // ready to analyze
		break;
	}
}


