//  Graham Percival  <gperciva@uvic.ca>
#ifndef CLICKFRAME_H
#define CLICKFRAME_H

#include <QFrame>
#include <QTime>

#include "MarSystemManager.h"
using namespace Marsyas;

//namespace MarsyasQt
//{

/*
	\brief Adds click (and double-click) detection to a QFrame.
	\ingroup MarsyasQt

	Adds click (and double-click) detection to a QFrame.
*/
class QClickFrame : public QFrame
{
	Q_OBJECT
public:
	QClickFrame(QWidget *parent = 0);

protected:
	void mousePressEvent(QMouseEvent *event);

signals:
	void clicked();
	void doubleclicked();

private:
	QTime timer;
};

//} //namespace
#endif

