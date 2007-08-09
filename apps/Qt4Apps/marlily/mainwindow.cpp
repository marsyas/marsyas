// C++ stuff for testing
#include <iostream>
using namespace std;

#include "mainwindow.h"

MainWindow::MainWindow() {
	createMain();
	createActions();
	createMenus();
	createToolBars();
	createStatusBar();
	readSettings();
}

MainWindow::~MainWindow() {
}

void MainWindow::closeEvent(QCloseEvent *event) {
	event->accept();
}

void MainWindow::about() {
	QMessageBox::about(this, tr("About MarLily"),
		tr("MarLily is a tool to aid investigation of music"
"transcription algorithms"));
}

void MainWindow::readSettings() {
	QSettings settings("MarLily", "MarLily");
	QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
	QSize size = settings.value("size", QSize(700, 400)).toSize();
	resize(size);
	move(pos);
}

void MainWindow::writeSettings() {
	QSettings settings("MarLily", "MarLily");
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
	QFrame* centralFrame = new QFrame;
	setCentralWidget(centralFrame);

	instructionArea = new QGridLayout;
	resultArea = new QGridLayout;

	mainLayout = new QVBoxLayout;
	mainLayout->addLayout(instructionArea);
	mainLayout->addLayout(resultArea);
	centralFrame->setLayout(mainLayout);
}

void MainWindow::createMenus()
{
	fileMenu = menuBar()->addMenu(tr("&File"));
	fileMenu->addAction(openUserAct);

	menuBar()->addSeparator();
	helpMenu = menuBar()->addMenu(tr("&Help"));
	helpMenu->addAction(aboutAct);
	helpMenu->addAction(aboutQtAct);
}

void MainWindow::createToolBars() {
}

void MainWindow::createActions() {
	openUserAct = new QAction(QIcon(":/../meaws/icons/open.png"), tr("&Open user..."), this);
//	openUserAct = new QAction(tr("&Open user..."), this);
	openUserAct->setShortcut(tr("Ctrl+O"));
	openUserAct->setStatusTip(tr("Open an existing session"));
	connect(openUserAct, SIGNAL(triggered()), this, SLOT(open()));

	exitAct = new QAction(tr("E&xit"), this);
	exitAct->setShortcut(tr("Ctrl+Q"));
	exitAct->setStatusTip(tr("Exit the application"));
	connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

	aboutAct = new QAction(tr("&About MarLily"), this);
	connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

	aboutQtAct = new QAction(tr("About &Qt"), this);
	aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
	connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

}


void MainWindow::open() {
	QString openFilename = QFileDialog::getOpenFileName(0,
		tr("Open File"));
	if (!openFilename.isEmpty()) {
		cout<<qPrintable(openFilename)<<endl;

	    Transcriber::getAllFromAudio(openFilename.toStdString(), pitchList, ampList);
//    	Transcriber::toMidi(pitchList);

		cout<<pitchList;
		cout<<ampList;
	}
}

