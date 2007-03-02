#ifndef TOPPANELNEW_H
#define TOPPANELNEW_H 

#include <QPushButton> 
#include <QSpinBox>
#include <QSlider>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QLabel>

#include "MarxGL2Din3DSpaceGraph.h"
#include "MarSystemManager.h" 

class TopPanelNew : public QWidget
{
   Q_OBJECT

public:
  TopPanelNew(std::string au = "", QWidget *parent = 0);

public slots: 
  void tick();
  void setTicks(int);
  void setYRotation(int);
  void setXTranslation(int);
  void setYTranslation(int);
  void setZTranslation(int);
  void setXAxisStretch(int);
  void setYAxisStretch(int);


private: 
  MarxGL2Din3DSpaceGraph* graph;
  Marsyas::MarSystem* pnet;
  
  int nTicks;

  std::string audio_file;
  
};

	
#endif 
