#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// Qt stuff
#include <QMainWindow>
#include <QPrinter>
#include <QGridLayout>
#include <QFrame>
#include <QtGui>

class QAction;
class QMenu;
class QTextEdit;
class QLabel;

// Meaws stuff
#include "defs.h"
#include "dispatcher.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow();
	~MainWindow();

protected:
	void closeEvent(QCloseEvent *event);

public slots:
	void updateMain(int state = 0);

private slots:
	void about();
//	bool close();

private:
// basic application functions
	void createActions();
	void createMain();
	void createMenus();
	void createToolBars();
	void createStatusBar();
	void connectObjects();
	void readSettings();
	void writeSettings();

// something
	void displayMessages();



// main interface objects
	QFrame* centralFrame_;

// menu and toolbar objects
	QMenu *fileMenu_;
	QToolBar *fileToolBar_;
	QAction *newUserAct_;
	QAction *openUserAct_;
	QAction *saveUserAct_;
	QAction *saveAsUserAct_;
	QAction *setUserInfoAct_;
	QAction *closeUserAct_;
	QAction *exitAct_;

	QMenu *exerciseMenu_;
	QToolBar *exerciseToolBar_;
	QAction *openExerciseAct_;
	QAction *toggleAttemptAct;
	QAction *closeExerciseAct_;


	QToolBar *tempoToolBar_;
	QAction *calcExerciseAct_;
	QAction *visualMetroBeatAct_;

	QToolBar *otherToolBar_;
	QAction *testingFileAct_;
	QAction *playFileAct_;
	QSlider *tempoSlider_;
	QSpinBox *tempoBox_;
	QAction *addTryAct_;
	QAction *delTryAct_;

	QMenu *testingMenu_;

	QMenu *helpMenu_;
	QAction *aboutAct_;
	QAction *aboutQtAct_;

	// status bar
	QLabel *normalStatusMessage_;
	QLabel *permanentStatusMessage_;


// other stuff that appears to be necessary... for now
	QLabel *exerciseTitle_;
	bool exerciseRunning_;

	Dispatcher *dispatcher_;
};

#endif
