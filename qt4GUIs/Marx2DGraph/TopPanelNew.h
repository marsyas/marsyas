#ifndef TOPPANELNEW_H
#define TOPPANELNEW_H 

#include "Marx2DGraph.h"
#include "MarSystemManager.h" 
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
  


private: 
  Marx2DGraph* graph2;
  Marx2DGraph* graph3;
  Marsyas::MarSystem* pnet;
  
  int nTicks;

  std::string audio_file;
  
};

	
#endif 
