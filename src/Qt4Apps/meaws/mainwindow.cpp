// C++ stuff for testing
#include <iostream>
using namespace std;

#include "mainwindow.h"

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
	if (user != NULL) {
		if ( !user->close() )
			event->ignore();
		else {
			if (exercise != NULL) {
				delete exercise;
				exercise = NULL;
			}
			writeSettings();
			event->accept();
		}
	}
}

void MainWindow::about() {
	QMessageBox::about(this, tr("About Meaws"),
		tr("Meaws (Musician Evaulation and Audition for Winds and Strings) "
		"is a learning tool for musicians.	It listens to a musician and "
		"provides feedback on their performance."
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
	normalStatusMessage = new QLabel;
	permanentStatusMessage = new QLabel;
	statusBar()->addWidget(normalStatusMessage);
	statusBar()->addPermanentWidget(permanentStatusMessage);
	statusBar()->showMessage(tr("Ready"));
}

void MainWindow::createMain() {
	mainLayout = new QVBoxLayout;
	mainLayout->addWidget( exercise->getInstructionArea() );
	mainLayout->addWidget( exercise->getResultArea() );
	mainLayout->setContentsMargins(1,1,1,1);

	QFrame* centralFrame = new QFrame;
	centralFrame->setFrameStyle(QFrame::NoFrame);
	centralFrame->setLayout(mainLayout);
	setCentralWidget(centralFrame);
}

void MainWindow::createMenus()
{
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

	exerciseMenu = menuBar()->addMenu(tr("Exercise"));
	exerciseMenu->addAction(openExerciseAct);
	exerciseMenu->addAction(closeExerciseAct);

	testingMenu = menuBar()->addMenu(tr("Testing"));
	testingMenu->addAction(testingFileAct);
	testingMenu->addAction(calcExerciseAct);
	testingMenu->addAction(playFileAct);
	testingMenu->addAction(addTryAct);
	testingMenu->addAction(delTryAct);


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

	exerciseToolBar = addToolBar(tr("Exercise"));
	exerciseToolBar->addAction(openExerciseAct);
	exerciseToolBar->addAction(toggleAttemptAct);
	exerciseToolBar->addAction(closeExerciseAct);

	tempoToolBar = addToolBar(tr("Tempo"));
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
	connect(openUserAct, SIGNAL(triggered()), user, SLOT(open()));

	saveUserAct = new QAction(QIcon(":/icons/save.png"), tr("&Save user"), this);
	saveUserAct->setShortcut(tr("Ctrl+S"));
	saveUserAct->setStatusTip(tr("Save the session to disk"));
	connect(saveUserAct, SIGNAL(triggered()), user, SLOT(save()));

	saveAsUserAct = new QAction(tr("Save user &As..."), this);
	saveAsUserAct->setStatusTip(tr("Save the session under a new name"));
	connect(saveAsUserAct, SIGNAL(triggered()), user, SLOT(saveAs()));

	closeUserAct = new QAction(QIcon(":/icons/quit.png"), tr("&Close user"), this);
	closeUserAct->setShortcut(tr("Ctrl+W"));
	closeUserAct->setStatusTip(tr("Close user"));
	connect(closeUserAct, SIGNAL(triggered()), user, SLOT(close()));

	setUserInfoAct = new QAction(QIcon(":/icons/new.png"), tr("&User info"), this);
	setUserInfoAct->setShortcut(tr("Ctrl+U"));
	connect(setUserInfoAct, SIGNAL(triggered()), user, SLOT(setUserInfo()));


	exitAct = new QAction(tr("E&xit"), this);
	exitAct->setShortcut(tr("Ctrl+Q"));
	exitAct->setStatusTip(tr("Exit the application"));
	connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

	aboutAct = new QAction(tr("&About Meaws"), this);
	connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

	aboutQtAct = new QAction(tr("About &Qt"), this);
	aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
	connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));


	openExerciseAct = new QAction(QIcon(":/icons/open.png"), tr("Open &Exercise..."), this);
	openExerciseAct->setShortcut(tr("Ctrl+R"));
	openExerciseAct->setStatusTip(tr("Open a new exercise"));
	connect(openExerciseAct, SIGNAL(triggered()), exercise, SLOT(open()));

	toggleAttemptAct = new QAction(this);
	toggleAttemptAct->setShortcut(tr("Space"));
	toggleAttemptAct->setStatusTip(tr("Start"));
	toggleAttemptAct->setIcon(QIcon(":/icons/player_play.png"));
	connect(toggleAttemptAct, SIGNAL(triggered()), exercise, SLOT(toggleAttempt()));

	closeExerciseAct = new QAction(QIcon(":/icons/quit.png"), tr("&Close exercise"), this);
	closeExerciseAct->setShortcut(tr("Ctrl+E"));
	closeExerciseAct->setStatusTip(tr("Close exercise"));
	connect(closeExerciseAct, SIGNAL(triggered()), exercise, SLOT(close()));




	setMetroIntroAct = new QAction(QIcon(":/icons/triangle.png"), tr("Set metronome introduction"), this);
//	connect(setMetroIntroAct, SIGNAL(triggered()), this, SLOT(setMetroIntro()));

	visualMetroBeat = new QAction(QIcon(":/icons/circle.png"), tr("Visual metronome"), this);
	visualMetroBeat->setStatusTip(tr("Shows the beat"));

	calcExerciseAct = new QAction(QIcon(":/icons/square.png"), tr("Calculate exercise results"), this);
	connect(calcExerciseAct, SIGNAL(triggered()), exercise, SLOT(analyze()));

	testingFileAct = new QAction(QIcon(":/icons/open.png"), tr("&Open test audio file..."), this);
	testingFileAct->setShortcut(tr("Ctrl+T"));
	testingFileAct->setStatusTip(tr("Open test audio file"));
	connect(testingFileAct, SIGNAL(triggered()), exercise, SLOT(openAttempt()));

	playFileAct = new QAction(QIcon(":/icons/open.png"), tr("&Play test audio file..."), this);
	playFileAct->setIcon(QIcon(":/icons/play.png"));
	playFileAct->setStatusTip(tr("Play test audio file"));
	connect(playFileAct, SIGNAL(triggered()), exercise, SLOT(playFile()));

	addTryAct = new QAction(tr("Add a try"), this);
	connect(addTryAct, SIGNAL(triggered()), exercise, SLOT(addTry()));

	delTryAct = new QAction(tr("Delete a try"), this);
	connect(delTryAct, SIGNAL(triggered()), exercise, SLOT(delTry()));
}

void MainWindow::createObjects() {
	user = new User();
	connect(user, SIGNAL(enableActions(int)), this, SLOT(enableActions(int)));

	exercise = new ExerciseDispatcher();
	connect(exercise, SIGNAL(enableActions(int)), this, SLOT(enableActions(int)));
	connect(exercise, SIGNAL(attemptRunning(bool)), this, SLOT(attemptRunning(bool)));
}

void MainWindow::attemptRunning(bool running) {
	if (running) { // start try
		toggleAttemptAct->setStatusTip(tr("Stop"));
		toggleAttemptAct->setIcon(QIcon(":/icons/player_pause.png"));
		metro->startMetro();
	} else { // stop try
		toggleAttemptAct->setStatusTip(tr("Start"));
		toggleAttemptAct->setIcon(QIcon(":/icons/player_play.png"));
		metro->stopMetro();
	}
}


void MainWindow::enableActions(int state) {
	switch (state) {
	case MEAWS_READY_NOTHING:	// just opened app
		setWindowTitle(tr("Meaws"));

		saveUserAct   ->setEnabled(false);
		saveAsUserAct ->setEnabled(false);
		closeUserAct  ->setEnabled(false);
		setUserInfoAct->setEnabled(false);

		exerciseMenu   ->setEnabled(false);
		exerciseToolBar->setEnabled(false);
		tempoToolBar   ->setEnabled(false);
		testingMenu    ->setEnabled(false);
		break;
	case MEAWS_READY_USER:	 // user selected
		setWindowTitle(tr("Meaws - %1").arg(user->getName()));

		saveUserAct   ->setEnabled(true);
		saveAsUserAct ->setEnabled(true);
		closeUserAct  ->setEnabled(true);
		setUserInfoAct->setEnabled(true);

		exerciseMenu	->setEnabled(true);
		exerciseToolBar ->setEnabled(true);
		toggleAttemptAct->setEnabled(false);
		closeExerciseAct->setEnabled(false);

		tempoToolBar   ->setEnabled(false);
		testingMenu    ->setEnabled(false);
		break;
	case MEAWS_READY_EXERCISE:	 // exercise loaded
	{
		toggleAttemptAct->setEnabled(true);
		closeExerciseAct->setEnabled(true);


		tempoToolBar->setEnabled(true);
		testingMenu ->setEnabled(true);

		string audioFile = "data/sd.wav";
		metro = new Metro(visualMetroBeat, this, audioFile);
//		connect(toggleAttemptAct, SIGNAL(triggered()), metro, SLOT(toggleExercise()));
		break;
	}
	case MEAWS_READY_AUDIO:    // ready to analyze

		//statusBar()->showMessage(exercise->getMessage(),100000);
		permanentStatusMessage->setText(exercise->getMessage());
		break;
	}
}


