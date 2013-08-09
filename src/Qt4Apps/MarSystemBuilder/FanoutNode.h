/**
 * FanoutNode Implementation File
 * Implements functions from FanMarSystemNode.h
 * Author: Peter Minter
 */
#ifndef FANOUTNODE_H
#define FANOUTNODE_H

#include "CompositeNode.h"
#include "MarSystemNodeFactory.h"

class FanoutNode:public CompositeNode
{
  Q_OBJECT

public:
  FanoutNode(QString name,QWidget* parent);
  FanoutNode(MarSystem* msys,QWidget* parent);

  bool append(MarSystemNode* newTail);
  bool insert(int index,MarSystemNode* newWidget);
  bool insert(MarSystemNode* before,MarSystemNode* newWidget);
  virtual QWidget* getChildrenCanvas();
  void placeAllWidgets();

public slots:
  void handleChildResize(int x,int y,int w,int h);

signals:
  void resized(int x,int y,int w,int h);

protected:
  virtual void resizeEvent(QResizeEvent* event);
  virtual void paintEvent(QPaintEvent* event);
};

#endif //FANOUTNODE_H
