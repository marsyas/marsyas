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
#include "../QtMarPlot.h"

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

  void showMusic();
  void segment();
  void nextNote();
  void prevNote();

private:
// basic application functions
  void createActions();
  void createMain();
  void createMenus();
  void createToolBars();
  void createStatusBar();
  void readSettings();
  void writeSettings();

  void display();
  void message();

// main interface objects
  QFrame* centralFrame;
  QVBoxLayout *mainLayout;
  QtMarPlot *pitchPlot;
  QtMarPlot *ampPlot;

// menu and toolbar objects
  QMenu *fileMenu;
  QAction *openUserAct;
  QMenu *audioMenu;
  QAction *segmentAct;
  QAction *nextNoteAct;
  QAction *prevNoteAct;
  QAction *showMusicAct;
  QAction *exitAct;

  QMenu *helpMenu;
  QAction *aboutAct;
  QAction *aboutQtAct;

  // status bar
  QLabel *normalStatusMessage;
  QLabel *permanentStatusMessage;


  int currNote;
  realvec *pitchList;
  realvec *ampList;
  realvec *boundaries;
  realvec *pitchSplit;
  realvec *ampSplit;
  realvec *tempPitch;
  realvec *tempAmp;
};

#endif
