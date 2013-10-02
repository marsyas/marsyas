/*
 * MarWidget.h-Header File
 * Class Will be extended/implmented by all MarSystems to meet their
 * requirements.  These MarWidgets will know where they lie and how to
 * draw themselves. As well as their parent and their children.
 *
 * Author: Peter Minter
 * Date Created: October 24 2005
 */
#ifndef MARSYSTEMNODE_H
#define MARSYSTEMNODE_H

#include <QtGui>
#include <marsyas/system/MarSystemManager.h>
#include <marsyas/system/MarSystem.h>
#include <string>
using namespace Marsyas;

using std::string;

class MarSystemNode:public QWidget
{
  Q_OBJECT

public:
  MarSystemNode(string type,QString name,QWidget* parent=0);
  MarSystemNode(MarSystem* ms,QWidget* parent=0);
  ~MarSystemNode();

  //Size Member Functions
  virtual int getBottom();
  virtual int getCenter() const;

  //Handle relationships
  void setPrev(MarSystemNode* parent);
  MarSystemNode* getPrev();
  virtual void setNext(MarSystemNode* child);
  MarSystemNode* getNext();

  //Handle Name issues
  void setName(const QString&);
  QString getName() const;

  MarSystem* getSystem();
  //Should return whether this is a composite MarSystemNode
  virtual bool isCollection();

  QPixmap* getPixmap();
  void setPixmap(QPixmap pixmap);
  //Most Marwidgets will return themselves.  I think All now!!!
  //TODO make sure we don't need this
  //virtual MarSystemNode* getParentForChild();

protected:
  void mousePressEvent(QMouseEvent *event);
  QString type_;
  QString name_;
  MarSystemNode* prev_;
  MarSystemNode* next_;
  QPixmap* pixmap_;
  MarSystem* represents_;//The MarSystem this object is representing

};

#endif //MARWIDGET_H 
