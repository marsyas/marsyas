/*
  MarCanvas.h - header File
  MarCanvas is a surface upon which to draw MarSystemNodes.
  It will act as a slot for  drag and drop signals.
  It will be a point of reference for all MarSystemNodes.

  Author: Peter Minter
  Date Created: Oct 24 2005
*/
#ifndef MARCANVAS_H
#define MARCANVAS_H

#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QSizePolicy>
#include <string>
#include <iostream>
#include <marsyas/system/MarSystem.h>
#include <MarSystemManager.h>
#include "CanvasWidget.h"
#include "MarSystemNodeFactory.h"

using namespace std;

class MarCanvas : public CanvasWidget
{
  Q_OBJECT

public:
  MarCanvas(QWidget * parent=0);
  void loadFromFile(string filename);
  MarSystemNode* loadMarSystem(istream& is,QWidget* canvas=0);
  void saveMarSystem(string filename);
  //virtual void drawAttachmentsFor(MarSystemNode* widget);


public slots:
  void addNewMarSystemNode(QString widgetType);
  void handleChildResize(int x,int y,int w,int h);

signals:
  void canvasChanged();


protected:
//virtual void paintEvent(QPaintEvent * event);
//virtual void dragEnterEvent(QDragEnterEvent *event);
//virtual void dragMoveEvent(QDragMoveEvent *event);
//virtual void dropEvent(QDropEvent *event);
//virtual QRect getDimensionsFor(MarSystemNode* widget);
//MarSystemNode * paintWidget;


  void dragEnterEvent(QDragEnterEvent *event);
  void dragMoveEvent(QDragMoveEvent *event);
  void dropEvent(QDropEvent *event);
  void paintEvent(QPaintEvent* event);


private:
  MarSystemNode * myMar;
  MarSystemNode * lastWidget;


};

#endif //MARCANVAS_H
