#include "mainwindow.h"

MainWindow::MainWindow() {
  marBackend = new MarBackend();

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
	newAct = new QAction(QIcon(":/images/new.png"), tr("&New"), this);
	newAct->setShortcut(tr("Ctrl+N"));
	newAct->setStatusTip(tr("Create a new session"));
//	connect(newAct, SIGNAL(triggered()), this, SLOT(newFile()));

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
	textLabel->setText("Initial");

	// we want to display the above two QLabels within our main window.
	mainLayout = new QVBoxLayout;
	mainLayout->addWidget(imageLabel,0,Qt::AlignTop);
	mainLayout->addWidget(textLabel);
	centralFrame->setLayout(mainLayout);
}

void MainWindow::createMenus()
{
	fileMenu = menuBar()->addMenu(tr("&File"));
	fileMenu->addAction(newAct);
	fileMenu->addAction(openAct);
	fileMenu->addAction(saveAct);
	fileMenu->addAction(saveAsAct);
	fileMenu->addSeparator();
	fileMenu->addAction(exitAct);

	helpMenu = menuBar()->addMenu(tr("&Help"));
	helpMenu->addAction(aboutAct);
	helpMenu->addAction(aboutQtAct);

	// exercise menu
	exerMenu = menuBar()->addMenu(tr("Exercise"));
	exerMenu->addAction(openExerciseAct);
}

void MainWindow::createToolBars()
{
	fileToolBar = addToolBar(tr("File"));
	fileToolBar->addAction(newAct);
	fileToolBar->addAction(openAct);
	fileToolBar->addAction(saveAct);

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
	// eventually we probably want a file Open dialogue box here
	// and we would set some exercise data (expected pitches, what kind of
	// audio analysis to perform, etc) here instead of just loading a png.
	QImage image("exercises/scale.png");
	imageLabel->setPixmap(QPixmap::fromImage(image));
}

