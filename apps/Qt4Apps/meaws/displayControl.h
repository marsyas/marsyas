#ifndef DISPLAYCONTROL_H
#define DISPLAYCONTROL_H

#include <QWidget>
#include "MarSystemManager.h"
using namespace Marsyas;

class DisplayControl : public QWidget
{
	Q_OBJECT

public:
	DisplayControl(QWidget *parent = 0);
	~DisplayControl();
	void setData(realvec* getData);

	// for testing
	void makeupData();

protected:
	void paintEvent(QPaintEvent *event);

private:
	realvec *data;
};
#endif

