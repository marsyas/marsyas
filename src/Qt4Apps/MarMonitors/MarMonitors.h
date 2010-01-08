#ifndef MARMONITORS_H
#define MARMONITORS_H 

#include "Marx2DGraph.h"
#include "MarSystemManager.h" 
#include "MarSystemQtWrapper.h"
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
using namespace MarsyasQt;

class MarMonitors : public QMainWindow
{
   Q_OBJECT

public:
  MarMonitors(string pluginName);

public slots: 
  void tick();
  void graph(int size);
  void setup();
  void dialogDone();
  void open();
  void about();  


private: 
  void createMenus();
  void createActions();
  void initNetwork(QString pluginName);

  
  string pluginName_;
  realvec out_;
  QVector<Marx2DGraph*> graphs;
  Marx2DGraph* graph3;
  Marsyas::MarSystem* pnet_;
  MarSystemQtWrapper* mwr_;
  QGridLayout *gridLayout_;
  QWidget* centralWidget_;
  

  QVector<string> probes_;
  QListWidget *listWidget;
  QSpinBox* graphNum;
  std::map<std::string, MarControlPtr> mycontrols_;

  int nTicks;
  int nGraphs_;
  

  QMenu*   fileMenu;  
  QMenu*   helpMenu;
  QAction* openAct;
  QAction* aboutAct;

  
};

	
#endif 
