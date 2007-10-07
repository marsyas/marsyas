#include "QClickFrame.h"

//namespace MarsyasQt
//{
QClickFrame::QClickFrame(QWidget *parent)
	: QFrame(parent)
{
	setMouseTracking(true);
}

void
QClickFrame::mousePressEvent(QMouseEvent *event)
{
	emit clicked();
	QWidget::mousePressEvent(event);
}

//} //namespace

