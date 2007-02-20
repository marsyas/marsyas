
#ifndef MARPHASEVOCODERWINDOW_H
#define MARPHASEVOCODERWINDOW_H


#include <QApplication>
#include <QFont>
#include <QGridLayout>
#include <QPushButton>
#include <QMainWindow>

#include "common.h"
#include "MarSystem.h"
#include "MarSystemManager.h" 
#include "MarControlGUI.h"
#include "../MarSystemQtWrapper.h" 


class MarPhasevocoderWindow : public QMainWindow
{
    Q_OBJECT

public:
    MarPhasevocoderWindow();

public slots: 
   void about();
   void timeChanged(int value);
   void volumeChanged(int value);
   void freqChanged(int value);
   void sinusoidsChanged(int value);
   void open();  
   void ctrlChanged(MarControlPtr cname, MarControlPtr cvalue);

private:
  void createMenus();
  void createActions();
  void createNetwork();
  void startNetwork();
  
  
  
  MarSystem* pvoc_;
  MarSystemQtWrapper* mwr_;
  
  MarControlPtr freqPtr_;
  MarControlPtr initPtr_;
  MarControlPtr fnamePtr_;
  MarControlGUI* freqControl_;
  
  QSlider* freqSlider_;
  

  QMenu*   fileMenu;  
  QMenu*   helpMenu;
  QAction* openAct;
  QAction* aboutAct;


  
  
};

#endif

	
