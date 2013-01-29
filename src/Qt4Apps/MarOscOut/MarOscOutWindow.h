
#ifndef MAROSCINWINDOW_H
#define MAROSCINWINDOW_H

#include <QApplication>
#include <QFont>
#include <QGridLayout>
#include <QPushButton>
#include <QMainWindow>
#include <QtCore>
#include <QtGui>

#include "common_source.h"
#include "MarSystem.h"
#include "MarSystemManager.h" 
#include "MarControl.h"
#include "MarSystemQtWrapper.h" 

using namespace MarsyasQt;


class MarOscOutWindow : public QMainWindow
{
Q_OBJECT

public:
  QSlider* gainSlider_;

  MarOscOutWindow(string);
  MarSystemQtWrapper* getMarSystemQtWrapper(){return mwr_;};

public slots: 
  void gainChanged(int value);
  void play(string fileName);
  void ctrlChanged(MarControlPtr cname);

private:

  mrs_real gain_;
  mrs_real frequencyPole_;

  void createNetwork();
  void startNetwork();

  MarSystem* net_;
  MarSystemQtWrapper* mwr_;
 
  MarControlPtr gainPtr_;
  MarControlPtr initPtr_;
  MarControlPtr fnamePtr_;

};

#endif

	
