/**
 * \class MainWindow
 * Main Window of the MarSystemBuilder
 */

#ifndef MAINWINDOWMSB_H
#define MAINWINDOWMSB_H

#include <qvariant.h>
#include <qmainwindow.h>
#include <QGroupBox>
#include <iostream>
#include "MarCanvas.h"
#include "WidgetButton.h"
#include "borderlayout.h"
#include <QtGui>

class MainWindow:public QMainWindow
{
  Q_OBJECT

public:
  MainWindow();
  //

public slots:
  virtual void fileNew();
  virtual void helpAbout();
  virtual void exitApp();
  virtual void closeEvent( QCloseEvent * );
  virtual void openFile();
  virtual void saveFile();

private:
  MarCanvas* theCanvas;//keep a pointer to the canvas inorder to
  //invoke signals and member functions
  QMenuBar *MenuBar;//Main menu bar
  QMenu *fileMenu; // File Menu...new,load, save, quit...
  QMenu *helpMenu;//Help Menu - about...
  QAction* fileNewAction;//new Network
  QAction* helpAboutAction;//About project and me
  QAction* ExitApplicationAction;//quit
  QAction* openAction;//load
  QAction* saveAction;//save
  QLineEdit* widgetNameEdit;
  QComboBox* collectionNameEdit;
private:
  void init();
};


#endif //MAINWINDOWMSB_H
