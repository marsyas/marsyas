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
/*
	void newFile();
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
/*
	void createStatusBar();
	bool maybeSave();
	void loadFile(const QString &fileName);
	bool saveFile(const QString &fileName);
	void setCurrentFile(const QString &fileName);
	QString strippedName(const QString &fullFileName);
*/

	QTextEdit *textEdit;
	QLabel *imageLabel;
	QLabel *textLabel;
	QGridLayout *mainGrid;
	QVBoxLayout *mainLayout;
	QFrame* centralFrame;

	QString curFile;

	QMenu *fileMenu;
	QMenu *helpMenu;
	QMenu *exerMenu;
	QToolBar *fileToolBar;
	QToolBar *tempoToolBar;
	QToolBar *infoBar;
	QAction *newAct;
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
