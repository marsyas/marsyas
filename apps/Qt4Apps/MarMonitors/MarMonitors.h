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
#include<QListWidget> 

class MarMonitors : public QWidget
{
   Q_OBJECT

public:
  MarMonitors(std::string au = "", QWidget *parent = 0);

public slots: 
  void tick();
  void graph();
  void setup();
  void dialogDone();
  


private: 
      
      
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
  
  std::string audio_file;
  
};

	
#endif 
