#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// Qt stuff
#include <QMainWindow>
#include <QPrinter>
#include <QGridLayout>
#include <QFrame>
#include <QtGui>

// #include <QPainter>
// #include <QTemporaryFile>
class QAction;
class QMenu;
class QTextEdit;
class QLabel;

#include "Transcriber.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow();
	~MainWindow();

protected:
	void closeEvent(QCloseEvent *event);

public slots:

private slots:
	void about();
	void open();

private:
// basic application functions
	void createActions();
	void createMain();
	void createMenus();
	void createToolBars();
	void createStatusBar();
	void readSettings();
	void writeSettings();

// main interface objects
	QFrame* centralFrame;
	QVBoxLayout *mainLayout;
	QGridLayout *instructionArea;
	QGridLayout *resultArea;

// menu and toolbar objects
	QMenu *fileMenu;
	QToolBar *fileToolBar;
	QAction *openUserAct;
	QAction *exitAct;

	QMenu *helpMenu;
	QAction *aboutAct;
	QAction *aboutQtAct;

	// status bar
	QLabel *normalStatusMessage;
	QLabel *permanentStatusMessage;


	realvec pitchList;
	realvec ampList;
};

#endif
