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

// Meaws stuff
#include "defs.h"
#include "user.h"
#include "exerciseDispatcher.h"
/*
#include "backend.h"
#include "metro.h"
#include "exercise.h"
#include "analyze.h"
#include "display.h"
*/

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

private:
// basic application functions
	void createActions();
	void createMain();
	void createMenus();
	void createToolBars();
	void createStatusBar();
	void readSettings();
	void writeSettings();
	void createObjects();

// main interface objects
	QFrame* centralFrame;
	QVBoxLayout *mainLayout;
	QFrame *instructionArea;
	QFrame *resultArea;

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
	QAction *setMetroIntroAct;
	QAction *calcExerciseAct;
	QAction *visualMetroBeat;

	QToolBar *otherToolBar;
	QAction *testingFileAct;
	QAction *playFileAct;
	QSlider *tempoSlider;
	QSpinBox *tempoBox;

	QMenu *testingMenu;

	QMenu *helpMenu;
	QAction *aboutAct;
	QAction *aboutQtAct;

	// status bar
	QLabel *normalStatusMessage;
	QLabel *permanentStatusMessage;


// other stuff that appears to be necessary... for now
	QLabel *exerciseTitle;
	std::string dataDir;
	QLabel *imageLabel;
	bool exerciseRunning;

// user functions
//	bool chooseUserInfo();

// main object variables
	User *user;
	ExerciseDispatcher *exercise;


// testingMethod = 0  not chosen
//               = 1  Intonation
//               = 2  Sound control

/*
	int testingMethod;
	bool maybeTestingMethod();
	bool chooseTestingMethod();
	void updateTestingMethod();


	QString exerciseName;
*/

/*
	bool maybeSave();
	void loadFile(const QString &userName);
	bool saveFile(const QString &userName);
	void setCurrentFile(const QString &userName);
	QString strippedName(const QString &fullFileName);
*/

/*
	QTextEdit *textEdit;
	//MeawsDisplay *resultsDisplay;
//	QtMarPlot *displayResults;
//	QtMarPlot *displayAmplitude;
	QHBoxLayout *displayLayout;

	QString curFile;


	int metroIntroBeats;

	QString audioFileName;
*/
  //MarBackend *marBackend;
//	void setupMarBackend();


/*
	Metro *metro;
	Exercise *exercise;
	Analyze *analyze;
*/

};

#endif
