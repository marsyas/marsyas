#include <QtGui>

#include "OSCReceiver.h"
#include <sstream>

#include <marsyas/system/MarSystemManager.h>

using namespace std;
using namespace Marsyas;

OSCReceiver::OSCReceiver(QWidget *parent)
  : QWidget(parent)
{
  value = -1;
}

void
OSCReceiver::setValue(int i)
{
//   cout << "OSCReceiver::setValue i=(" << i << ")" << endl;
  value = i;
}

void
OSCReceiver::setName(string s)
{
//   cout << "name=" << s << endl;
}
