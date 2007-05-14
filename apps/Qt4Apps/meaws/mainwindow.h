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

private slots:
// basic application slots
	void about();

/*
// user slots
	void newUser();
	void openUser();
	void closeUser();
	void open();
	bool save();
	bool saveAs();

// exercise slots
	void openExercise();
	void closeExercise();

// other slots
	void setMetroIntro();
	void setMetroTempo(int tempo);
	void beat();
	void toggleExercise();
	void calcExercise();
	void testingFile();
	void playFile();
*/

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
	QGridLayout *mainGrid;
	QVBoxLayout *mainLayout;

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

	QMenu *exerMenu;
	QToolBar *tempoToolBar;
	QAction *openExerciseAct;
	QAction *toggleMetroAct;
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



// other stuff that appears to be necessary... for now
	QLabel *exerciseTitle;
	std::string dataDir;
	QLabel *imageLabel;
	bool exerciseRunning;

// user functions
//	bool chooseUserInfo();

// user variables
	User *user;


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
