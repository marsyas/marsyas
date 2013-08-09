#ifndef RECEIVE_H
#define RECEIVE_H

#include <QWidget>
#include <cstdlib>

//using namespace MarsyasQt;
using namespace std;

class OSCReceiver : public QWidget
{
  Q_OBJECT

public:
  void setName(string s);
  int value;

  OSCReceiver(QWidget *parent = 0);



public slots:
  void setValue(int);

private:
  string name;
};

#endif
