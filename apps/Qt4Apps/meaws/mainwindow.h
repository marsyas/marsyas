#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPrinter>
#include <QGridLayout>
#include <QFrame>
#include <QtGui>

#include <iostream>
using namespace std;

#include "backend.h"
#include "metro.h"
#include "user.h"
#include "exercise.h"
#include "analyze.h"

class QAction;
class QMenu;
class QTextEdit;
class QLabel;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow();
	~MainWindow();

protected:
	void closeEvent(QCloseEvent *event);

private slots:
	void about();
	void newUser();
	void openUser();
	void closeUser();
/*
	void open();
	bool save();
	bool saveAs();
	void documentWasModified();
*/
	void openExercise();
	void closeExercise();
	void setMetroIntro();
	void setMetroTempo(int tempo);
	void beat();
	void toggleExercise();
	void calcExercise();
	void testingFile();
	void playFile();
	void enableActions(int state);

private:
	void createActions();
	void createMain();
	void createMenus();
	void createToolBars();
	void readSettings();
	void writeSettings();

// testingMethod = 0  not chosen
//               = 1  Intonation
//               = 2  Sound control
	int testingMethod;
	bool maybeTestingMethod();
	bool chooseTestingMethod();
	void updateTestingMethod();

	User *user;
	bool chooseUserInfo();

	QMenu *fileMenu;
	QToolBar *userToolBar;
	QAction *newUserAct;

	QString exerciseName;


/*
	void createStatusBar();
	bool maybeSave();
	void loadFile(const QString &userName);
	bool saveFile(const QString &userName);
	void setCurrentFile(const QString &userName);
	QString strippedName(const QString &fullFileName);
*/

	QTextEdit *textEdit;
	QLabel *imageLabel;
	QLabel *displayResults;
	QGridLayout *mainGrid;
	QVBoxLayout *mainLayout;
	QFrame* centralFrame;

	QString curFile;

	QMenu *helpMenu;
	QMenu *exerMenu;
	QToolBar *tempoToolBar;
	QToolBar *infoBar;
	QAction *openAct;
	QAction *saveAct;
	QAction *saveAsAct;
	QAction *closeAct;
	QAction *exitAct;
	QAction *aboutAct;
	QAction *aboutQtAct;

	QAction *openExerciseAct;

	QAction *toggleMetroAct;
	QAction *setMetroIntroAct;
	QAction *setUserInfoAct;
	QAction *calcExerciseAct;
	QAction *visualMetroBeat;

	QAction *testingFileAct;
	QAction *playFileAct;

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
};

#endif
