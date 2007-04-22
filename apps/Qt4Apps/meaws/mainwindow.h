#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// Qt stuff
#include <QMainWindow>
#include <QPrinter>
#include <QGridLayout>
#include <QFrame>
#include <QtGui>

#include <QTemporaryFile>
class QAction;
class QMenu;
class QTextEdit;
class QLabel;

// C++ stuff
#include <iostream>
using namespace std;

// Meaws stuff
#include "user.h"
#include "backend.h"
#include "metro.h"
#include "exercise.h"
#include "analyze.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow();
	~MainWindow();

protected:
	void closeEvent(QCloseEvent *event);

private slots:
// basic application slots
	void about();
	void enableActions(int state);

// user slots
	void newUser();
	void openUser();
	void closeUser();
/*
	void open();
	bool save();
	bool saveAs();
*/

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

private:
// basic application functions
	void createActions();
	void createMain();
	void createMenus();
	void createToolBars();
	void createStatusBar();
	void readSettings();
	void writeSettings();

// basic application variables
	QMenu *fileMenu;
	QToolBar *userToolBar;
	QAction *newUserAct;
	QAction *openAct;
	QAction *saveAct;
	QAction *saveAsAct;
	QAction *setUserInfoAct;
	QAction *closeAct;
	QAction *exitAct;


	QMenu *exerMenu;
	QToolBar *tempoToolBar;
	QAction *openExerciseAct;
	QAction *toggleMetroAct;
	QAction *setMetroIntroAct;
	QAction *calcExerciseAct;
	QAction *visualMetroBeat;

	QAction *testingFileAct;
	QAction *playFileAct;


	QToolBar *infoBar;

	QMenu *helpMenu;
	QAction *aboutAct;
	QAction *aboutQtAct;

	QGridLayout *mainGrid;
	QVBoxLayout *mainLayout;
	QFrame* centralFrame;



// user functions
	bool chooseUserInfo();

// user variables
	User *user;

// testingMethod = 0  not chosen
//               = 1  Intonation
//               = 2  Sound control
	int testingMethod;
	bool maybeTestingMethod();
	bool chooseTestingMethod();
	void updateTestingMethod();


	QString exerciseName;


/*
	bool maybeSave();
	void loadFile(const QString &userName);
	bool saveFile(const QString &userName);
	void setCurrentFile(const QString &userName);
	QString strippedName(const QString &fullFileName);
*/

	QTextEdit *textEdit;
	QLabel *imageLabel;
	QLabel *displayResults;
	QString curFile;

	QLabel *exerciseTitle;

	QSlider *slider;
	QSpinBox *tempoBox;
	int metroIntroBeats;

	QString audioFileName;

  MarBackend *marBackend;
	void setupMarBackend();

	bool exerciseRunning;

	Metro *metro;
	Exercise *exercise;
	Analyze *analyze;

	string dataDir;
};

#endif
