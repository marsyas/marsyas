/**
 Default MarSYstemNode for all regular Mar Systems
*/

#ifndef DEFAULTMARSYSNODE_H
#define DEFAULTMARSYSNODE_H
#include <string>
#include "MarSystemNode.h"
#include "MarSystemNodeFactory.h"
using namespace std;
class DefaultMarSystemNode:public MarSystemNode
{
  Q_OBJECT
public:
  DefaultMarSystemNode(string type,QString name,QWidget* parent);
  DefaultMarSystemNode(MarSystem* msys, QWidget* parent);
protected:
  //void paintEvent(QPaintEvent* event);
  void resizeEvent(QResizeEvent*);
};

#endif //DEFAULTMARSYSNODE

