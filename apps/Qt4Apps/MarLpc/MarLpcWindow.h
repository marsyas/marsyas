
#ifndef MARLPCWINDOW_H
#define MARLPCWINDOW_H

#include <QApplication>
#include <QFont>
#include <QGridLayout>
#include <QPushButton>
#include <QMainWindow>

#include "common.h"
#include "MarSystem.h"
#include "MarSystemManager.h" 
#include "MarControlGUI.h"
#include "MarSystemQtWrapper.h" 

class MarLpcWindow : public QMainWindow
{
    Q_OBJECT

public:
    MarLpcWindow();

public slots: 
   void about();
  
   void tiltChanged(int value);
   void breathinessChanged(int value);
   void frequencyPoleChanged(int value);
   void amplitudePoleChanged(int value);
   void cutOffChanged(int value);
  
   void posChanged();
   void open();  
   void ctrlChanged(MarControlPtr cname);

private:
  void createMenus();
  void createActions();
  void createNetwork();
  void startNetwork();
   
  MarSystem* lpc_;
  MarSystemQtWrapper* mwr_;
 
  MarControlPtr posPtr_;
  MarControlPtr frequencyPolePtr_;
  MarControlPtr amplitudePolePtr_;
  MarControlPtr initPtr_;
  MarControlPtr fnamePtr_;

  MarControlGUI* posControl_;
  MarControlGUI* frequencyPoleControl_;
  MarControlGUI* amplitudePoleControl_;

  QSlider* posSlider_;
  
  QMenu*   fileMenu;  
  QMenu*   helpMenu;
  QAction* openAct;
  QAction* aboutAct;
};

#endif

	
