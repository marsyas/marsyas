#ifndef TOPPANELNEW_H
#define TOPPANELNEW_H 

#include "MarxGLMultiBufferGraph.h"
#include "Marx3dSlider.h"
#include "MarSystemManager.h" 
#include <QPushButton> 
#include <QSpinBox>
#include <QGroupBox>
#include <QHBoxLayout>
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
  void xyzModelTranslation(float, float, float);
  void xyzModelRotation(float, float, float);
  void xyzProjectionTranslation(float, float, float);
  void xyzProjectionRotation(float, float, float);


private: 
  MarxGLMultiBufferGraph* graph;
  MarSystem* pnet;
  
  int nTicks;
  string audio_file;
  
};

	
#endif 
