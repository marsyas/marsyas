/*********************************
  Most Basic MarWidget.  First Attempt at a child class to inherhit
  from MarWidget.
**********************************/
#ifndef GAINNODE_H
#define GAINNODE_H

#include "MarSystemNode.h"
#include "MarSystemNodeFactory.h"

class GainNode:public MarSystemNode
{
  Q_OBJECT
public:
  GainNode(QString name,QWidget* parent);
  GainNode(MarSystem* msys,QWidget* parent);

protected:
  void paintEvent(QPaintEvent* event);
  void resizeEvent(QResizeEvent* event);
};

#endif
