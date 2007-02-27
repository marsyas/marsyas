#include "mainwindow.h"

MainWindow::MainWindow() {
	marBackend=NULL;
	metro=NULL;
	testingMethod=0;

	exercise = new Exercise();

	createMain();
	createActions();
	createMenus();
	createToolBars();
	readSettings();

	enableActions(1);
}

MainWindow::~MainWindow() {
	if (marBackend != NULL) {
		delete marBackend;
	}
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

void MainWindow::createActions() {
	newUserAct = new QAction(QIcon(":/images/new.png"), tr("&New user"), this);
	newUserAct->setShortcut(tr("Ctrl+N"));
	newUserAct->setStatusTip(tr("Create a new session"));
	connect(newUserAct, SIGNAL(triggered()), this, SLOT(newUser()));

	openAct = new QAction(QIcon(":/images/open.png"), tr("&Open user..."), this);
	openAct->setShortcut(tr("Ctrl+O"));
	openAct->setStatusTip(tr("Open an existing session"));
//	connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

	saveAct = new QAction(QIcon(":/images/save.png"), tr("&Save user"), this);
	saveAct->setShortcut(tr("Ctrl+S"));
	saveAct->setStatusTip(tr("Save the session to disk"));
//	connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

	saveAsAct = new QAction(tr("Save user &As..."), this);
	saveAsAct->setStatusTip(tr("Save the session under a new name"));
//	connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

	closeAct = new QAction(QIcon(":/images/save.png"), tr("&Close user"), this);
	closeAct = new QAction(tr("&Close user"), this);
	closeAct->setShortcut(tr("Ctrl+W"));
	closeAct->setStatusTip(tr("Close user"));
	connect(closeAct, SIGNAL(triggered()), this, SLOT(closeUser()));

	setUserInfoAct = new QAction(QIcon(":/images/new.png"), tr("&User info"), this);
	setUserInfoAct->setShortcut(tr("Ctrl+U"));

	exitAct = new QAction(tr("E&xit"), this);
	exitAct->setShortcut(tr("Ctrl+Q"));
	exitAct->setStatusTip(tr("Exit the application"));
	connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

	aboutAct = new QAction(tr("&About Meaws"), this);
	connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

	aboutQtAct = new QAction(tr("About &Qt"), this);
	aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
	connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));


	openExerciseAct = new QAction(QIcon(":/images/open.png"), tr("Open &Exercise..."), this);
	openExerciseAct->setShortcut(tr("Ctrl+R"));
	openExerciseAct->setStatusTip(tr("Open a new exercise"));
	connect(openExerciseAct, SIGNAL(triggered()), this, SLOT(openExercise()));

	setMetroIntroAct = new QAction(QIcon(":/images/triangle.png"), tr("Set metronome introduction"), this);
	connect(setMetroIntroAct, SIGNAL(triggered()), this, SLOT(setMetroIntro()));

	visualMetroBeat = new QAction(QIcon(":/images/circle.png"), tr("Visual metronome"), this);
	visualMetroBeat->setStatusTip(tr("Shows the beat"));

	calcExerciseAct = new QAction(QIcon(":/images/square.png"), tr("Calculate exercise results"), this);
	connect(calcExerciseAct, SIGNAL(triggered()), this, SLOT(calcExercise()));

	toggleMetroAct = new QAction(this);
	toggleMetroAct->setShortcut(tr("Space"));
	toggleMetroAct->setStatusTip(tr("Start"));
	toggleMetroAct->setIcon(QIcon(":/images/player_play.png"));

	testingFileAct = new QAction(QIcon(":/images/open.png"), tr("&Open test audio file..."), this);
	testingFileAct->setShortcut(tr("Ctrl+T"));
	testingFileAct->setStatusTip(tr("Open test audio file"));
	connect(testingFileAct, SIGNAL(triggered()), this, SLOT(testingFile()));

	playFileAct = new QAction(QIcon(":/images/open.png"), tr("&Play test audio file..."), this);
	playFileAct->setIcon(QIcon(":/images/play.png"));
	playFileAct->setStatusTip(tr("Play test audio file"));
	connect(playFileAct, SIGNAL(triggered()), this, SLOT(playFile()));
}

void MainWindow::playFile() {
	marBackend->playFile();
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

	// this is what displays our testing text.  Later on we would
	// remove textLable and make a QT painting area or make it a picture.
	displayResults = new QLabel;
	updateTestingMethod();

	// we want to display the above two QLabels within our main window.
	mainLayout = new QVBoxLayout;
	mainLayout->addWidget(imageLabel,0,Qt::AlignTop);
	mainLayout->addWidget(displayResults,0,Qt::AlignTop);
	centralFrame->setLayout(mainLayout);
}

void MainWindow::createMenus()
{
	fileMenu = menuBar()->addMenu(tr("&File"));
	fileMenu->addAction(newUserAct);
	fileMenu->addAction(openAct);
	fileMenu->addAction(saveAct);
	fileMenu->addAction(saveAsAct);
	fileMenu->addSeparator();
	fileMenu->addAction(setUserInfoAct);
	fileMenu->addSeparator();
	fileMenu->addAction(closeAct);
	fileMenu->addAction(exitAct);

	// exercise menu
	exerMenu = menuBar()->addMenu(tr("Exercise"));
	exerMenu->addAction(openExerciseAct);
	exerMenu->setEnabled(false);

	menuBar()->addSeparator();
	helpMenu = menuBar()->addMenu(tr("&Help"));
	helpMenu->addAction(aboutAct);
	helpMenu->addAction(aboutQtAct);
}

void MainWindow::createToolBars() {
	userToolBar = addToolBar(tr("User"));
	userToolBar->addAction(newUserAct);
	userToolBar->addAction(openAct);
	userToolBar->addAction(saveAct);
	userToolBar->addAction(closeAct);
	userToolBar->addAction(openExerciseAct);

	tempoToolBar = addToolBar(tr("Tempo"));
	tempoToolBar->addAction(setMetroIntroAct);

	tempoBox = new QSpinBox();
	tempoBox->setRange(30,240);
	tempoBox->setValue(60);
	tempoToolBar->addWidget(tempoBox);

	slider = new QSlider(Qt::Horizontal);
	slider->setRange(30, 240);
	slider->setValue(60);
//		slider->setMinimumWidth(60);
	tempoToolBar->addWidget(slider);

	tempoToolBar->addAction(toggleMetroAct);
	tempoToolBar->addAction(calcExerciseAct);
	tempoToolBar->addAction(visualMetroBeat);

	connect(slider, SIGNAL(valueChanged(int)),
		tempoBox, SLOT(setValue(int)));
	connect(slider, SIGNAL(valueChanged(int)),
		this, SLOT(setMetroTempo(int)));
	connect(tempoBox, SIGNAL(valueChanged(int)),
		slider, SLOT(setValue(int)));
	connect(tempoBox, SIGNAL(valueChanged(int)),
		this, SLOT(setMetroTempo(int)));

	infoBar = addToolBar(tr("Info"));
	exerciseTitle = new QLabel();
	exerciseTitle->setText("");
	infoBar->addAction(testingFileAct);
	infoBar->addAction(playFileAct);
	infoBar->addWidget(exerciseTitle);

//	QLabel *userNameLabel = new QLabel();
//	userNameLabel->setText(user->getName());
//	infoBar->addWidget(userNameLabel);
}

void MainWindow::closeExercise() {
	cout<<"closeExercise()"<<endl;
	if (marBackend != NULL) {
		delete marBackend;
		marBackend = NULL;
	}
	if (metro != NULL) {
		delete metro;
		metro = NULL;
	}
	
}

void MainWindow::openExercise() {
//	QString exerciseName = QFileDialog::getOpenFileName(this,
//		tr("Open Exercise"),"music/",tr("Exercises (*.png)"));
	QString exerciseName("music/scale.png");
	if (!exerciseName.isEmpty()) {
		QImage image(exerciseName);
		imageLabel->setPixmap(QPixmap::fromImage(image));
		exerciseTitle->setText( tr("Exercise: %1").arg(QFileInfo(exerciseName).baseName()) );
		enableActions(3);
	}
}

void MainWindow::testingFile() {
	audioFileName = QFileDialog::getOpenFileName(this,
		tr("Open test audio file"),"~/",tr("Audio files (*.wav)"));
}
	
bool MainWindow::maybeTestingMethod() {
	if (testingMethod>0) {
		return true;
	} else {
		return chooseTestingMethod();
	}
}

bool MainWindow::chooseTestingMethod() {
	QStringList items;
	items << tr("Intonation test") << tr("Sound control test");
	bool ok;
	QString item = QInputDialog::getItem(this, tr("Choose testing method"),
		tr("TestingMethod:"), items, 0, false, &ok);
	if (ok && !item.isEmpty()) {
		if (item=="Intonation test") testingMethod=1;
		if (item=="Sound control test") testingMethod=2;
		updateTestingMethod();
		return true;
	} else {
		return false;
	}
}

void MainWindow::updateTestingMethod() {
/*
	if (testingMethod==0) textLabel->setText("No testing method selected");
	if (testingMethod==1) {
		textLabel->setText("Intonation test");
	}
	if (testingMethod==2) {
		textLabel->setText("Sound control test");
	}
*/
}

bool MainWindow::chooseUserInfo() {
/*
	bool ok;
	QString text = QInputDialog::getText(this, tr("User info"),
		tr("User name:"), QLineEdit::Normal,
		QDir::home().dirName(), &ok);
	if (ok && !text.isEmpty()) {
		userName = text;
		return true;
	} else {
		return false;
	}
*/
	return true;
}



void MainWindow::newUser() {
	try {
		user = new User();
	}
	catch (bool failed) {
		return;
	}
	if ( chooseTestingMethod() ) {
		connect(setUserInfoAct, SIGNAL(triggered()), user, SLOT(setUserInfo()));
		enableActions(2);
	}
	openExercise();
}

void MainWindow::enableActions(int state) {
	if (state==1) {   // just opened app
		setWindowTitle(tr("Meaws"));

		saveAct   ->setEnabled(false);
		saveAsAct ->setEnabled(false);
		closeAct  ->setEnabled(false);
		setUserInfoAct->setEnabled(false);

		infoBar     ->setEnabled(false);
		exerMenu    ->setEnabled(false);
		openExerciseAct ->setEnabled(false);

		//tempoToolBar ->setEnabled(false);
		tempoToolBar ->setEnabled(true);
		closeExercise();
	}
	if (state==2) {   // user created or loaded
		setWindowTitle(tr("Meaws - %1").arg(user->getName()));

		saveAct   ->setEnabled(true);
		saveAsAct ->setEnabled(true);
		closeAct  ->setEnabled(true);
		setUserInfoAct->setEnabled(true);

		infoBar     ->setEnabled(true);
		exerMenu    ->setEnabled(true);
		openExerciseAct ->setEnabled(true);

		tempoToolBar ->setEnabled(false);

		closeExercise();
	}
	if (state==3) {   // exercise picked
		setupMarBackend();
		exerciseRunning=false;
		metro = new Metro(visualMetroBeat, this);

		connect(toggleMetroAct, SIGNAL(triggered()), this, SLOT(toggleExercise()));

		tempoToolBar ->setEnabled(true);
	}
	if (state==4) {   // exercise results

	}
}

void MainWindow::toggleExercise() {
	if (exerciseRunning) {   // stop it
		marBackend->stop();
		metro->stopMetro();
		toggleMetroAct->setStatusTip(tr("Start"));
		toggleMetroAct->setIcon(QIcon(":/images/player_play.png"));
		exerciseRunning = !exerciseRunning;
	} else {   // start it
		marBackend->start();
		metro->startMetro();
		toggleMetroAct->setStatusTip(tr("Stop"));
		toggleMetroAct->setIcon(QIcon(":/images/player_pause.png"));
		exerciseRunning = !exerciseRunning;
	}
}

void MainWindow::setupMarBackend() {
	if (marBackend != NULL) {
		delete marBackend;
		marBackend = NULL;
	}
	marBackend = new MarBackend(testingMethod);
	QString filename = user->getName();
	filename.append("-");
	filename.append( QDateTime::currentDateTime().toString("yyyyMMdd") );
	filename.append("-");
	filename.append( QDateTime::currentDateTime().toString("hhmmss") );
	filename.append(".wav");
	audioFileName = filename;
	marBackend->setFileName( qPrintable(audioFileName) );

	// communication with Marsyas backend
	connect(marBackend, SIGNAL(nextNoteError(float, int)), exercise, SLOT(nextNoteError(float, int)));
}

void MainWindow::beat() {

}

void MainWindow::closeUser() {
	if (marBackend != NULL) {
		delete marBackend;
		marBackend = NULL;
	}

	delete user;
	imageLabel->clear();
	testingMethod=0;
	updateTestingMethod();
	enableActions(1);
}

void MainWindow::setMetroIntro() {
	bool ok;
	int i = QInputDialog::getInteger(this, tr("Extra beats"),
		tr("How many beats do you want \nto hear before beginning?"),
		4, 0, 32, 1, &ok);
	if (ok) {
		metroIntroBeats=i;
		metro->setIntro(metroIntroBeats);
	}
}

void MainWindow::setMetroTempo(int tempo) {
	metro->setTempo(tempo);
}

void MainWindow::calcExercise() {
	marBackend->calculate(qPrintable(audioFileName));

	exercise->getLily();
	string command = "cd /Users/gperciva/tmp/ ; lilypond -dpreview out.ly";
	system(command.c_str());
	QString imageFileName="/Users/gperciva/tmp/out.preview.png";
	QImage image(imageFileName);
	displayResults->setPixmap(QPixmap::fromImage(image));

}

