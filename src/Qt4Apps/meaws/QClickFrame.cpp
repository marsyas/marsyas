#include "QClickFrame.h"

//namespace MarsyasQt
//{
QClickFrame::QClickFrame(QWidget *parent)
	: QFrame(parent)
{
	setMouseTracking(true);
	timer.start();
}

void
QClickFrame::mousePressEvent(QMouseEvent *event)
{
	// number is milliseconds since last click
	if (timer.restart() < 500)
		emit doubleclicked();
	else
		emit clicked();
	QWidget::mousePressEvent(event);
}



//} //namespace

