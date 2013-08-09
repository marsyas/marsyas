/**
 * CanvasWidget.h
 * This will either be a pure virtual or semi veritual class that will
 * represent all classes that Allow MarSystemNodes to be drawn on them.
 */

#ifndef CANVASWIDGET_H
#define CANVASWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QSizePolicy>
#include <string>
#include <iostream>
using namespace std;

#include "MarSystemNode.h"

#define MAXINT 10000000
#define MININT -10000000

class CanvasWidget:public QWidget
{
  Q_OBJECT
public:
  CanvasWidget();
  CanvasWidget(QWidget* parent);
  virtual void drawAttachmentsFor(MarSystemNode* widget);
  int maximum(int a,int b, int c=MININT);
  int minimum(int a,int b, int c=MAXINT);

protected:
  virtual void paintEvent(QPaintEvent * event);
  virtual void dragEnterEvent(QDragEnterEvent *event);
  virtual void dragMoveEvent(QDragMoveEvent *event);
  virtual void dropEvent(QDropEvent *event);
  virtual QRect  getDimensionsFor(MarSystemNode* widget);
  MarSystemNode* paintWidget;

};

#endif //CANVASWIDGET_H
