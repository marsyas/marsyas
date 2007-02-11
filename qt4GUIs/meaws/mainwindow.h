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
	void createExtraToolBars();

// testingMethod = 0  not chosen
//               = 1  strings (Graham's stuff)
//               = 2  winds (Mathieu's stuff)
	int testingMethod;
	bool maybeTestingMethod();
	bool chooseTestingMethod();
	void updateTestingMethod();

	QString userName;
	bool chooseUserInfo();

	QMenu *userMenu;
	QToolBar *userToolBar;
	QAction *newUserAct;


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
	QAction *exitAct;
	QAction *aboutAct;
	QAction *aboutQtAct;

	QAction *openExerciseAct;
	QAction *startMetroAct;
	QAction *stopMetroAct;

  MarBackend *marBackend;
};

#endif
