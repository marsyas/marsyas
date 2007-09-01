/******
 WidgetButton - IMplementation FIle
*********/
#include "WidgetButton.h"

WidgetButton::WidgetButton(QString name,QString widgetType,QWidget* parent)
  :QPushButton(name,parent)
{
  myWidgetType=QString(widgetType);
  connect(this, SIGNAL(clicked()), this, SLOT(catchClick()));
}

void
WidgetButton::catchClick()
{
  emit reemitClick(myWidgetType);
}


