#ifndef TOPPANELNEW_H
#define TOPPANELNEW_H 

#include "Marx2DGraph.h"
#include "MarSystemManager.h" 
#include "MarSystemWrapper.h"
#include "MarSystem.h"
#include "MarControl.h" 
#include <QPushButton> 
#include <QSpinBox>

class TopPanelNew : public QWidget
{
   Q_OBJECT

public:
  TopPanelNew(std::string au = "", QWidget *parent = 0);

public slots: 
  void tick();
  void setTicks(int);
  void graph();
  


private: 
  QVector<Marx2DGraph*> graphs;
  Marx2DGraph* graph3;
  Marsyas::MarSystem* pnet_;
  MarSystemWrapper* mwr_;
  QGridLayout *gridLayout_;
  

  int nTicks;
  int nGraphs_;
  
  std::string audio_file;
  
};

	
#endif 
