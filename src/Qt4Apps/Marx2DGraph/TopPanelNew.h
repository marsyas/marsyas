#ifndef TOPPANELNEW_H
#define TOPPANELNEW_H 

#include "Marx2DGraph.h"
#include "MarSystemManager.h" 
#include "MarSystemWrapper.h"
#include "MarSystem.h"
#include "MarControl.h" 
#include <QPushButton> 
#include <QSpinBox>
#include <QDialog>
#include <QLabel> 
#include<QListWidget> 

class TopPanelNew : public QWidget
{
   Q_OBJECT

public:
  TopPanelNew(std::string au = "", QWidget *parent = 0);

public slots: 
  void tick();
  void graph();
  void setup();
  void dialogDone();
  


private: 
      
      
  realvec out_;
  QVector<MarsyasQt::Marx2DGraph*> graphs;
  MarsyasQt::Marx2DGraph* graph3;
  Marsyas::MarSystem* pnet_;
  MarSystemWrapper* mwr_;
  QGridLayout *gridLayout_;

  QVector<string> probes_;
  QListWidget *listWidget;
  QSpinBox* graphNum;
  

  int nTicks;
  int nGraphs_;
  
  std::string audio_file;
  
};

	
#endif 
