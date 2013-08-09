/********
 An Extention of QPush Button to allow for a string to be sent in the
 clicked signal.
*********/

#ifndef WIDGETBUTTON_H
#define WIDGETBUTTON_H

#include <QPushButton>
#include <QWidget>
#include <QString>

class WidgetButton:public QPushButton
{
  Q_OBJECT
public:
  WidgetButton(QString name,QString widgetType,QWidget* parent =0);

public slots:
  void catchClick();
signals:
  void reemitClick(QString widgetType);

private:
  QString myWidgetType;
};

#endif//WIDGETBUTTON_H
