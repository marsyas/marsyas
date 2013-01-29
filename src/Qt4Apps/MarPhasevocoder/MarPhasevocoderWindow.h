
#ifndef MARPHASEVOCODERWINDOW_H
#define MARPHASEVOCODERWINDOW_H

#include <QApplication>
#include <QFont>
#include <QGridLayout>
#include <QPushButton>
#include <QMainWindow>

#include "common_source.h"
#include "MarSystem.h"
#include "MarSystemManager.h" 
#include "MarControlGUI.h"
#include "MarSystemQtWrapper.h" 

class MarPhasevocoderWindow : public QMainWindow
{
    Q_OBJECT

public:
    MarPhasevocoderWindow(string inAudioFileName);

public slots: 
   void about();
   void timeChanged(int value);
   void volumeChanged(int value);
   void freqChanged(int value);
   void sinusoidsChanged(int value);
   void posChanged();
   void open();  
   void ctrlChanged(MarControlPtr cname);

private:
  void createMenus();
  void createActions();
  void createNetwork(string inAudioFileName);
  void startNetwork();
   
  MarSystem* pvoc_;
  MarSystemQtWrapper* mwr_;
  
  MarControlPtr freqPtr_;
  MarControlPtr posPtr_;
  MarControlPtr initPtr_;
  MarControlPtr fnamePtr_;
  MarControlGUI* freqControl_;
  MarControlGUI* posControl_;
  MarControlGUI* sinesControl_;
		
		
  
  QSlider* freqSlider_;
  QSlider* posSlider_;
  
  QMenu*   fileMenu;  
  QMenu*   helpMenu;
  QAction* openAct;
  QAction* aboutAct;
};

#endif

	
