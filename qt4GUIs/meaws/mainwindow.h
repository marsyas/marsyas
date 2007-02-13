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
#include "user.h"

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
	void closeUser();
/*
	void open();
	bool save();
	bool saveAs();
	void documentWasModified();
*/
	void openExercise();

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

	QString userName;
	User *user;
	bool chooseUserInfo();

	QMenu *fileMenu;
	QToolBar *userToolBar;
	QAction *newUserAct;

	QString exerciseName;

	void enableActions(int state);

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
	QLabel *textLabel;
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
	QAction *startMetroAct;
	QAction *stopMetroAct;

	QLabel *exerciseTitle;

	QSlider *slider;
	QSpinBox *tempoBox;


  MarBackend *marBackend;
	void setupMarBackend();
};

#endif
