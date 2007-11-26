// C++ stuff for testing
//#include <iostream>
//using namespace std;

#include "mainwindow.h"

MainWindow::MainWindow()
{
	// create objects
	createMain();
	createActions();
	createMenus();
	createToolBars();
	createStatusBar();
	// connect objects
	dispatcher_ = new Dispatcher(centralFrame_);
	connectObjects();
	// begin operation
	readSettings();
	updateMain(MEAWS_READY_NOTHING);
}

MainWindow::~MainWindow()
{
	delete dispatcher_;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	if (!dispatcher_->close())
		event->ignore();
	event->accept();
	writeSettings();
}

void MainWindow::about()
{
	QMessageBox::about(this, tr("About Meaws"),
	                   tr("Meaws (Musician Evaulation and "
	                      "Audition for Winds and Strings) "
	                      "is a learning tool for "
	                      "musicians.  It listens to a "
	                      "musician and provides feedback on "
	                      "their performance."
	                     ));
}

void MainWindow::readSettings()
{
	QSettings settings("Meaws", "Meaws");
	QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
	QSize size = settings.value("size", QSize(700, 400)).toSize();
	resize(size);
	move(pos);
}

void MainWindow::writeSettings()
{
	QSettings settings("Meaws", "Meaws");
	settings.setValue("pos", pos());
	settings.setValue("size", size());
}

void MainWindow::createStatusBar()
{
	normalStatusMessage_ = new QLabel;
	permanentStatusMessage_ = new QLabel;
	statusBar()->addWidget(normalStatusMessage_);
	statusBar()->addPermanentWidget(permanentStatusMessage_);
	statusBar()->showMessage(tr("Ready"));
}

void MainWindow::createMain()
{
	centralFrame_ = new QFrame;
	centralFrame_->setFrameStyle(QFrame::NoFrame);
	setCentralWidget(centralFrame_);
}

void MainWindow::createMenus()
{
	fileMenu_ = menuBar()->addMenu(tr("&File"));
	fileMenu_->addAction(newUserAct_);
	fileMenu_->addAction(openUserAct_);
	fileMenu_->addAction(saveUserAct_);
	fileMenu_->addAction(saveAsUserAct_);
	fileMenu_->addSeparator();
	fileMenu_->addAction(setUserInfoAct_);
	fileMenu_->addSeparator();
	fileMenu_->addAction(closeUserAct_);
	fileMenu_->addAction(exitAct_);

	exerciseMenu_ = menuBar()->addMenu(tr("Exercise"));
	exerciseMenu_->addAction(openExerciseAct_);
	exerciseMenu_->addAction(openCampaignAct_);
	exerciseMenu_->addAction(closeExerciseAct_);

	testingMenu_ = menuBar()->addMenu(tr("Testing"));
	testingMenu_->addAction(testingFileAct_);
	testingMenu_->addAction(calcExerciseAct_);
	testingMenu_->addAction(playFileAct_);
	testingMenu_->addAction(addTryAct_);
	testingMenu_->addAction(delTryAct_);
	testingMenu_->addAction(resetTryAct_);


	menuBar()->addSeparator();
	helpMenu_ = menuBar()->addMenu(tr("&Help"));
	helpMenu_->addAction(aboutAct_);
	helpMenu_->addAction(aboutQtAct_);
}

void MainWindow::createToolBars()
{
	fileToolBar_ = addToolBar(tr("User"));
	fileToolBar_->addAction(newUserAct_);
	fileToolBar_->addAction(openUserAct_);
	fileToolBar_->addAction(saveUserAct_);
	fileToolBar_->addAction(closeUserAct_);

	exerciseToolBar_ = addToolBar(tr("Exercise"));
	exerciseToolBar_->addAction(openExerciseAct_);
	exerciseToolBar_->addAction(openCampaignAct_);
	exerciseToolBar_->addAction(toggleAttemptAct);
	exerciseToolBar_->addAction(closeExerciseAct_);

	tempoToolBar_ = addToolBar(tr("Tempo"));

	tempoSlider_ = new QSlider(Qt::Horizontal);
	tempoSlider_->setRange(30, 240);
	tempoSlider_->setValue(60);
	tempoSlider_->setMinimumWidth(100);
	//tempoSlider_->setMaximumWidth(200);

	tempoToolBar_->addWidget(tempoSlider_);
	tempoToolBar_->addAction(visualMetroBeatAct_);

	otherToolBar_ = addToolBar(tr("Other"));
	otherToolBar_->addAction(addTryAct_);
	otherToolBar_->addAction(delTryAct_);
	otherToolBar_->addAction(resetTryAct_);
}

void MainWindow::createActions()
{
	// main stuff
	exitAct_ = new QAction(tr("E&xit"), this);
	exitAct_->setShortcut(tr("Ctrl+Q"));
	exitAct_->setStatusTip(tr("Exit the application"));

	aboutAct_ = new QAction(tr("&About Meaws"), this);

	aboutQtAct_ = new QAction(tr("About &Qt"), this);
	aboutQtAct_->setStatusTip(tr("Show the Qt library's About box"));

	// user
	newUserAct_ = new QAction(QIcon(":/icons/new.png"), tr("&New user"), this);
	newUserAct_->setShortcut(tr("Ctrl+N"));
	newUserAct_->setStatusTip(tr("Create a new session"));

	openUserAct_ = new QAction(QIcon(":/icons/open.png"),
	                           tr("&Open user..."), this);
	openUserAct_->setShortcut(tr("Ctrl+O"));
	openUserAct_->setStatusTip(tr("Open an existing session"));

	saveUserAct_ = new QAction(QIcon(":/icons/save.png"),
	                           tr("&Save user"), this);
	saveUserAct_->setShortcut(tr("Ctrl+S"));
	saveUserAct_->setStatusTip(tr("Save the session to disk"));

	saveAsUserAct_ = new QAction(tr("Save user &As..."), this);
	saveAsUserAct_->setStatusTip(tr("Save the session under a new name"));

	closeUserAct_ = new QAction(QIcon(":/icons/quit.png"),
	                            tr("&Close user"), this);
	closeUserAct_->setShortcut(tr("Ctrl+W"));
	closeUserAct_->setStatusTip(tr("Close user"));

	setUserInfoAct_ = new QAction(QIcon(":/icons/new.png"),
	                              tr("&User info"), this);
	setUserInfoAct_->setShortcut(tr("Ctrl+U"));

	// exercise
	openExerciseAct_ = new QAction(QIcon(":/icons/open.png"), tr("Open Exe&rcise..."), this);
	openExerciseAct_->setShortcut(tr("Ctrl+R"));
	openExerciseAct_->setStatusTip(tr("Open a new exercise"));

	openCampaignAct_ = new QAction(QIcon(":/icons/open.png"), tr("Open Campaign... &E"), this);
	openCampaignAct_->setShortcut(tr("Ctrl+E"));
	openCampaignAct_->setStatusTip(tr("Open a new campaign"));

	toggleAttemptAct = new QAction(this);
	toggleAttemptAct->setShortcut(tr("Space"));
	toggleAttemptAct->setStatusTip(tr("Start"));
	toggleAttemptAct->setIcon(QIcon(":/icons/player_play.png"));

	closeExerciseAct_ = new QAction(QIcon(":/icons/quit.png"), tr("&Close dispatcher_"), this);
	closeExerciseAct_->setShortcut(tr("Ctrl+D"));
	closeExerciseAct_->setStatusTip(tr("Close dispatcher"));

	// metronome + other
	visualMetroBeatAct_ = new QAction(QIcon(":/icons/circle.png"), tr("Visual metro_nome"), this);
	visualMetroBeatAct_->setStatusTip(tr("Shows the beat"));

	calcExerciseAct_ = new QAction(QIcon(":/icons/square.png"), tr("Calculate exercise results"), this);

	testingFileAct_ = new QAction(QIcon(":/icons/open.png"), tr("&Open test audio file..."), this);
	testingFileAct_->setShortcut(tr("Ctrl+T"));
	testingFileAct_->setStatusTip(tr("Open test audio file"));

	playFileAct_ = new QAction(QIcon(":/icons/open.png"), tr("&Play test audio file..."), this);
	playFileAct_->setIcon(QIcon(":/icons/play.png"));
	playFileAct_->setStatusTip(tr("Play test audio file"));

	addTryAct_ = new QAction(tr("Add a try"), this);
	delTryAct_ = new QAction(tr("Delete a try"), this);
	resetTryAct_ = new QAction(tr("Reset try"), this);
}

void MainWindow::connectObjects()
{
	// MainWindow objects
	connect(exitAct_, SIGNAL(triggered()), this, SLOT(close()));
	connect(aboutAct_, SIGNAL(triggered()), this, SLOT(about()));
	connect(aboutQtAct_, SIGNAL(triggered()), qApp, SLOT(aboutQt()));


	// Dispatcher objects
	connect(dispatcher_, SIGNAL(updateMain(int)),
	        this, SLOT(updateMain(int)));
	connect(openExerciseAct_, SIGNAL(triggered()),
	        dispatcher_, SLOT(openExercise()));
	connect(openCampaignAct_, SIGNAL(triggered()),
	        dispatcher_, SLOT(openCampaign()));

	connect(testingFileAct_, SIGNAL(triggered()),
	        dispatcher_, SLOT(openAttempt()));
	connect(toggleAttemptAct, SIGNAL(triggered()),
	        dispatcher_, SLOT(toggleAttempt()));

	/*
	connect(closeExerciseAct_, SIGNAL(triggered()),
	dispatcher_, SLOT(close()));

	connect(calcExerciseAct_, SIGNAL(triggered()),
	dispatcher_, SLOT(analyze()));
	connect(playFileAct_, SIGNAL(triggered()),
	dispatcher_, SLOT(playFile()));

	*/



	// Sub-dispatcher objects
	QObject* user = dispatcher_->getUserPointer();
	connect(user, SIGNAL(updateMain(int)), this, SLOT(updateMain(int)));

	connect(closeUserAct_, SIGNAL(triggered()), user, SLOT(close()));
	connect(newUserAct_, SIGNAL(triggered()), user, SLOT(newUser()));
	connect(openUserAct_, SIGNAL(triggered()), user, SLOT(open()));
	connect(saveUserAct_, SIGNAL(triggered()), user, SLOT(save()));
	connect(saveAsUserAct_, SIGNAL(triggered()), user, SLOT(saveAs()));
	connect(setUserInfoAct_, SIGNAL(triggered()), user, SLOT(setUserInfo()));

	// metro
	QObject* metro = dispatcher_->getMetroPointer();

	connect(visualMetroBeatAct_, SIGNAL(triggered()), metro,
	        SLOT(toggleBigMetro()));
	// TODO: icky
	((Metro*) metro)->setIcon(visualMetroBeatAct_);

}

void MainWindow::connectExercise()
{
	QObject* exercise = dispatcher_->getExercisePointer();
	connect(exercise, SIGNAL(updateMain(int)), this, SLOT(updateMain(int)));

	connect(addTryAct_, SIGNAL(triggered()), exercise, SLOT(addTry()));
	connect(delTryAct_, SIGNAL(triggered()), exercise, SLOT(delTry()));
	connect(resetTryAct_, SIGNAL(triggered()), exercise, SLOT(resetTry()));

}

void MainWindow::displayMessages()
{
	setWindowTitle(dispatcher_->getTitle());
	permanentStatusMessage_->setText(dispatcher_->getStatus());
}

void MainWindow::updateMain(int state)
{
	switch (state)
	{
	case 0:
		displayMessages();
		break;
	case MEAWS_READY_NOTHING:
		saveUserAct_   ->setEnabled(false);
		saveAsUserAct_ ->setEnabled(false);
		closeUserAct_  ->setEnabled(false);
		setUserInfoAct_->setEnabled(false);

		exerciseMenu_   ->setEnabled(false);
		exerciseToolBar_->setEnabled(false);
		tempoToolBar_   ->setEnabled(false);
		testingMenu_    ->setEnabled(false);

		displayMessages();
		break;
	case MEAWS_READY_USER:
		saveUserAct_   ->setEnabled(true);
		saveAsUserAct_ ->setEnabled(true);
		closeUserAct_  ->setEnabled(true);
		setUserInfoAct_->setEnabled(true);

		exerciseMenu_	->setEnabled(true);
		exerciseToolBar_ ->setEnabled(true);
		toggleAttemptAct->setEnabled(false);
		closeExerciseAct_->setEnabled(false);

		tempoToolBar_   ->setEnabled(false);
		testingMenu_    ->setEnabled(false);

		displayMessages();
		break;
	case MEAWS_READY_EXERCISE:
		connectExercise();
		toggleAttemptAct->setEnabled(true);
		closeExerciseAct_->setEnabled(true);

		tempoToolBar_->setEnabled(true);
		testingMenu_ ->setEnabled(true);

		displayMessages();
		break;
	case MEAWS_TRY_PAUSED:
		toggleAttemptAct->setStatusTip(tr("Start"));
		toggleAttemptAct->setIcon(QIcon(":/icons/player_play.png"));

		displayMessages();
		break;

	case MEAWS_TRY_RUNNING:
		toggleAttemptAct->setStatusTip(tr("Stop"));
		toggleAttemptAct->setIcon(QIcon(":/icons/player_pause.png"));

		displayMessages();
		break;
	}
}

