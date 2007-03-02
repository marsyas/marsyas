#ifndef TOPPANELNEW_H
#define TOPPANELNEW_H 

#include "MarxGLColorGraph.h"
#include "MarSystemManager.h" 
#include <QPushButton> 
#include <QSpinBox>
#include <QSlider>

class TopPanelNew : public QWidget
{
   Q_OBJECT

public:
  TopPanelNew(std::string au = "", QWidget *parent = 0);

public slots: 
  void tick();
  void setTicks(int);
  void setContrast( int );
  


private: 
  MarxGLColorGraph* graph;
  Marsyas::MarSystem* pnet;
  
  int nTicks;

  std::string audio_file;
  
};

	
#endif 
