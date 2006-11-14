
#ifndef MARPHASEVOCODERWINDOW_H
#define MARPHASEVOCODERWINDOW_H


#include <QApplication>
#include <QFont>
#include <QGridLayout>
#include <QPushButton>
#include <QMainWindow>

#include "MarSystem.h"
#include "MarSystemManager.h" 
#include "MarSystemWrapper.h" 
#include "common.h"

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


private:
  void createMenus();
  void createActions();
  void createNetwork();

  
  
  MarSystem* pvoc_;
  MarSystemWrapper* mwr_;
  
  QMenu*   fileMenu;  
  QMenu*   helpMenu;
  QAction* openAct;
  QAction* aboutAct;


  
  
};

#endif

	
