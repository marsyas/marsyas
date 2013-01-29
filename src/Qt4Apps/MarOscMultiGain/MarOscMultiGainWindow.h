
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


class MarOscMultiGainWindow : public QMainWindow
{
Q_OBJECT

public:
  QSlider* gain1Slider_;
  QSlider* gain2Slider_;
  QSlider* gain3Slider_;

  MarOscMultiGainWindow(string,string,string);
  MarSystemQtWrapper* getMarSystemQtWrapper(){return mwr_;};

public slots: 
  void gain1Changed(int value);
  void gain2Changed(int value);
  void gain3Changed(int value);
  void play(string,string,string);
  void ctrlChanged(MarControlPtr cname);

private:

  mrs_real gain1_;
  mrs_real gain2_;
  mrs_real gain3_;

  void createNetwork(string,string,string);
  void startNetwork();

  MarSystem* net_;
  MarSystemQtWrapper* mwr_;
 
  MarControlPtr gain1Ptr_;
  MarControlPtr gain2Ptr_;
  MarControlPtr gain3Ptr_;

  MarControlPtr initPtr_;

  MarControlPtr fname1Ptr_;
  MarControlPtr fname2Ptr_;
  MarControlPtr fname3Ptr_;

};

#endif

	
