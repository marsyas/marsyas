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
#include "user.h"
#include "dispatcher-exercise.h"
#include "metro.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow();
	~MainWindow();

protected:
	void closeEvent(QCloseEvent *event);

public slots:
	void enableActions(int state);
	void attemptRunning(bool running);

private slots:
	void about();
	bool closeUser();

private:
// basic application functions
	void createActions();
	void createMain();
	void createMenus();
	void createToolBars();
	void createStatusBar();
	void readSettings();
	void writeSettings();

	void createUser();
	void createExercise();

// main interface objects
	QFrame* centralFrame;

// menu and toolbar objects
	QMenu *fileMenu;
	QToolBar *fileToolBar;
	QAction *newUserAct;
	QAction *openUserAct;
	QAction *saveUserAct;
	QAction *saveAsUserAct;
	QAction *setUserInfoAct;
	QAction *closeUserAct;
	QAction *exitAct;

	QMenu *exerciseMenu;
	QToolBar *exerciseToolBar;
	QAction *openExerciseAct;
	QAction *toggleAttemptAct;
	QAction *closeExerciseAct;


	QToolBar *tempoToolBar;
	QAction *calcExerciseAct;
	QAction *visualMetroBeatAct;

	QToolBar *otherToolBar;
	QAction *testingFileAct;
	QAction *playFileAct;
	QSlider *tempoSlider;
	QSpinBox *tempoBox;
	QAction *addTryAct;
	QAction *delTryAct;

	QMenu *testingMenu;

	QMenu *helpMenu;
	QAction *aboutAct;
	QAction *aboutQtAct;

	// status bar
	QLabel *normalStatusMessage;
	QLabel *permanentStatusMessage;


// other stuff that appears to be necessary... for now
	QLabel *exerciseTitle;
	bool exerciseRunning;

// main object variables
	User *user;
	ExerciseDispatcher *exercise;
	Metro *metro;

};

#endif
