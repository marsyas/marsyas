#ifndef TOPPANELNEW_H
#define TOPPANELNEW_H 

#include "MarxGL2Din3DSpaceGraph.h"
#include "MarSystemManager.h" 
#include <QPushButton> 
#include <QSpinBox>
#include <QSlider>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QLabel>

class TopPanelNew : public QWidget
{
   Q_OBJECT

public:
  TopPanelNew(string au = "", QWidget *parent = 0);

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
  MarSystem* pnet;
  
  int nTicks;

  string audio_file;
  
};

	
#endif 
