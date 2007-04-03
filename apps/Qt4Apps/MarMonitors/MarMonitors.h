#ifndef MARMONITORS_H
#define MARMONITORS_H 

#include "Marx2DGraph.h"
#include "MarSystemManager.h" 
#include "../MarSystemQtWrapper.h"
#include "MarSystem.h"
#include "MarControl.h" 
#include <QPushButton> 
#include <QSpinBox>
#include <QDialog>
#include <QLabel> 
#include <QMenuBar>
#include <QMenu>
#include <QListWidget> 
#include <QMainWindow>
#include <QApplication> 
#include <QFileDialog> 

class MarMonitors : public QMainWindow
{
   Q_OBJECT

public:
  MarMonitors();

public slots: 
  void tick();
  void graph();
  void setup();
  void dialogDone();
  void open();
  void about();  


private: 
  void createMenus();
  void createActions();
  void initNetwork(QString pluginName);

  
      
  realvec out_;
  QVector<Marx2DGraph*> graphs;
  Marx2DGraph* graph3;
  Marsyas::MarSystem* pnet_;
  MarSystemQtWrapper* mwr_;
  QGridLayout *gridLayout_;

  QVector<string> probes_;
  QListWidget *listWidget;
  QSpinBox* graphNum;
  

  int nTicks;
  int nGraphs_;
  

  QMenu*   fileMenu;  
  QMenu*   helpMenu;
  QAction* openAct;
  QAction* aboutAct;

  
};

	
#endif 
