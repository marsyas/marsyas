#include "mainwindow.h"

MainWindow::MainWindow() {
  marBackend = new MarBackend();
	testingMethod=0;

	createMain();
	createActions();
	createMenus();
	createToolBars();
	readSettings();
}

MainWindow::~MainWindow() {
	delete marBackend;
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
	QMessageBox::about(this, tr("About AudMetro"),
		tr("Metrosyas is a metronome that listens to a musician and "
		"displays the music with notes coloured based on their intonation."
		));
}

void MainWindow::readSettings() {
	QSettings settings("Metrosyas", "Metrosyas");
	QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
	QSize size = settings.value("size", QSize(700, 400)).toSize();
	resize(size);
	move(pos);
}

void MainWindow::writeSettings() {
	QSettings settings("Metrosyas", "Metrosyas");
	settings.setValue("pos", pos());
	settings.setValue("size", size());
}

void MainWindow::createActions() {
	newUserAct = new QAction(QIcon(":/images/new.png"), tr("&New"), this);
	newUserAct->setShortcut(tr("Ctrl+N"));
	newUserAct->setStatusTip(tr("Create a new session"));
	connect(newUserAct, SIGNAL(triggered()), this, SLOT(newUser()));

	openAct = new QAction(QIcon(":/images/open.png"), tr("&Open..."), this);
	openAct->setShortcut(tr("Ctrl+O"));
	openAct->setStatusTip(tr("Open an existing session"));
//	connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

	saveAct = new QAction(QIcon(":/images/save.png"), tr("&Save"), this);
	saveAct->setShortcut(tr("Ctrl+S"));
	saveAct->setStatusTip(tr("Save the session to disk"));
//	connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

	saveAsAct = new QAction(tr("Save &As..."), this);
	saveAsAct->setStatusTip(tr("Save the session under a new name"));
//	connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

	exitAct = new QAction(tr("E&xit"), this);
	exitAct->setShortcut(tr("Ctrl+Q"));
	exitAct->setStatusTip(tr("Exit the application"));
	connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

	aboutAct = new QAction(tr("&About"), this);
	aboutAct->setStatusTip(tr("Show the application's About box"));
	connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

	aboutQtAct = new QAction(tr("About &Qt"), this);
	aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
	connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));


	openExerciseAct = new QAction(QIcon(":/images/open.png"), tr("Open &Exercise..."), this);
	openExerciseAct->setShortcut(tr("Ctrl+R"));
	openExerciseAct->setStatusTip(tr("Open a new exercise"));
	connect(openExerciseAct, SIGNAL(triggered()), this, SLOT(openExercise()));

	startMetroAct = new QAction(QIcon(":/images/player_play.png"), tr("Start..."), this);
	connect(startMetroAct, SIGNAL(triggered()), marBackend, SLOT(startMetro()));

	stopMetroAct = new QAction(QIcon(":/images/player_stop.png"), tr("Stop..."), this);
	connect(stopMetroAct, SIGNAL(triggered()), marBackend, SLOT(stopMetro()));
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
	textLabel = new QLabel;
	updateTestingMethod();

	// we want to display the above two QLabels within our main window.
	mainLayout = new QVBoxLayout;
	mainLayout->addWidget(imageLabel,0,Qt::AlignTop);
	mainLayout->addWidget(textLabel);
	centralFrame->setLayout(mainLayout);
}

void MainWindow::createMenus()
{
	userMenu = menuBar()->addMenu(tr("&User"));
	userMenu->addAction(newUserAct);
	userMenu->addAction(openAct);
	userMenu->addAction(saveAct);
	userMenu->addAction(saveAsAct);
	userMenu->addSeparator();
	userMenu->addAction(exitAct);

	helpMenu = menuBar()->addMenu(tr("&Help"));
	helpMenu->addAction(aboutAct);
	helpMenu->addAction(aboutQtAct);

	// exercise menu
	exerMenu = menuBar()->addMenu(tr("Exercise"));
	exerMenu->addAction(openExerciseAct);
}

void MainWindow::createToolBars() {
	userToolBar = addToolBar(tr("User"));
	userToolBar->addAction(newUserAct);
	userToolBar->addAction(openAct);
	userToolBar->addAction(saveAct);
}

void MainWindow::createExtraToolBars() {
	tempoToolBar = addToolBar(tr("Tempo"));
	QSpinBox *tempoBox = new QSpinBox();
	tempoBox->setRange(30,240);
	tempoBox->setValue(60);
	tempoToolBar->addWidget(tempoBox);

	QSlider *slider = new QSlider(Qt::Horizontal);
	slider->setRange(30, 240);
	slider->setValue(60);
//		slider->setMinimumWidth(60);
	tempoToolBar->addWidget(slider);


	infoBar = addToolBar(tr("Info"));
	QLabel *pieceTitle = new QLabel();
	pieceTitle->setText("Title of piece");
	infoBar->addWidget(pieceTitle);

	QLabel *userNameLabel = new QLabel();
	userNameLabel->setText(userName);
	infoBar->addWidget(userNameLabel);


	tempoToolBar->addAction(openExerciseAct);
	tempoToolBar->addAction(startMetroAct);
	tempoToolBar->addAction(stopMetroAct);

	// communication with Marsyas backend
	connect(slider, SIGNAL(valueChanged(int)),
		tempoBox, SLOT(setValue(int)));
	connect(slider, SIGNAL(valueChanged(int)),
		marBackend, SLOT(setTempo(int)));
	connect(tempoBox, SIGNAL(valueChanged(int)),
		slider, SLOT(setValue(int)));
	connect(tempoBox, SIGNAL(valueChanged(int)),
		marBackend, SLOT(setTempo(int)));
}

void MainWindow::openExercise() {
	// this is how you would show your own exercises
	// eventually we probably want a user Open dialogue box here
	// and we would set some exercise data (expected pitches, what kind of
	// audio analysis to perform, etc) here instead of just loading a png.
	if (maybeTestingMethod()) {
		QImage image("exercises/scale.png");
		imageLabel->setPixmap(QPixmap::fromImage(image));
	}
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
	items << tr("String intonation test") << tr("Wind air control test");
	bool ok;
	QString item = QInputDialog::getItem(this, tr("Choose testing method"),
		tr("TestingMethod:"), items, 0, false, &ok);
	if (ok && !item.isEmpty()) {
		if (item=="String intonation test") testingMethod=1;
		if (item=="Wind air control test") testingMethod=2;
		updateTestingMethod();
		return true;
	} else {
		return false;
	}
}

void MainWindow::updateTestingMethod() {
	if (testingMethod==0) textLabel->setText("No testing method selected");
	if (testingMethod==1) {
		textLabel->setText("String intonation test");
		marBackend->startGraham();
	}
	if (testingMethod==2) {
		textLabel->setText("Wind air control test");
		marBackend->startMathieu();
	}
}

bool MainWindow::chooseUserInfo() {
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
}

void MainWindow::newUser() {
	if (chooseTestingMethod()) {  // force a choice here; no `maybe'
		if (chooseUserInfo()) {
			createExtraToolBars();
		}
	}
}

